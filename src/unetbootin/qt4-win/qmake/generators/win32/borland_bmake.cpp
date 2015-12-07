/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qmake application of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "borland_bmake.h"
#include "option.h"
#include <qdir.h>
#include <qregexp.h>
#include <time.h>

QT_BEGIN_NAMESPACE

BorlandMakefileGenerator::BorlandMakefileGenerator() : Win32MakefileGenerator(), init_flag(false)
{

}

bool
BorlandMakefileGenerator::writeMakefile(QTextStream &t)
{
    writeHeader(t);
    if(!project->values("QMAKE_FAILED_REQUIREMENTS").isEmpty()) {
        QStringList &qut = project->values("QMAKE_EXTRA_TARGETS");
        for(QStringList::ConstIterator it = qut.begin(); it != qut.end(); ++it)
            t << *it << " ";
        t << "all first clean:" << "\n\t"
          << "@echo \"Some of the required modules ("
          << var("QMAKE_FAILED_REQUIREMENTS") << ") are not available.\"" << "\n\t"
          << "@echo \"Skipped.\"" << endl << endl;
        return true;
    }

    if(project->first("TEMPLATE") == "app" ||
       project->first("TEMPLATE") == "lib") {
        writeBorlandParts(t);
        return MakefileGenerator::writeMakefile(t);
    }
    else if(project->first("TEMPLATE") == "subdirs") {
        writeSubDirs(t);
        return true;
    }
    return false;
}

void
BorlandMakefileGenerator::writeBorlandParts(QTextStream &t)
{
    t << "!if !$d(BCB)" << endl;
    t << "BCB = $(MAKEDIR)\\.." << endl;
    t << "!endif" << endl << endl;

    writeStandardParts(t);
}

void
BorlandMakefileGenerator::init()
{
    if(init_flag)
        return;
    init_flag = true;

    /* this should probably not be here, but I'm using it to wrap the .t files */
    if (project->first("TEMPLATE") == "app") {
        project->values("QMAKE_APP_FLAG").append("1");
    } else if(project->first("TEMPLATE") == "lib"){
        project->values("QMAKE_LIB_FLAG").append("1");
    } else if(project->first("TEMPLATE") == "subdirs") {
        MakefileGenerator::init();
        if(project->isEmpty("QMAKE_COPY_FILE"))
            project->values("QMAKE_COPY_FILE").append("$(COPY)");
        if(project->isEmpty("QMAKE_COPY_DIR"))
            project->values("QMAKE_COPY_DIR").append("xcopy /s /q /y /i");
        if(project->isEmpty("QMAKE_INSTALL_FILE"))
            project->values("QMAKE_INSTALL_FILE").append("$(COPY_FILE)");
        if(project->isEmpty("QMAKE_INSTALL_PROGRAM"))
            project->values("QMAKE_INSTALL_PROGRAM").append("$(COPY_FILE)");
        if(project->isEmpty("QMAKE_INSTALL_DIR"))
            project->values("QMAKE_INSTALL_DIR").append("$(COPY_DIR)");
        if(project->values("MAKEFILE").isEmpty())
            project->values("MAKEFILE").append("Makefile");
        if(project->values("QMAKE_QMAKE").isEmpty())
            project->values("QMAKE_QMAKE").append("qmake");
        return;
    }

    processVars();

    project->values("QMAKE_LIBS") += project->values("LIBS");

    MakefileGenerator::init();

    if (project->isActiveConfig("dll") || !project->values("QMAKE_APP_FLAG").isEmpty()) {
        // bcc does not generate a .tds file for static libs
        QString tdsPostfix;
        if (!project->values("VERSION").isEmpty())
            tdsPostfix = project->first("TARGET_VERSION_EXT");
        tdsPostfix += ".tds";
        project->values("QMAKE_CLEAN").append(project->first("DESTDIR") + project->first("TARGET") + tdsPostfix);
    }
}

void BorlandMakefileGenerator::writeBuildRulesPart(QTextStream &t)
{
    t << "first: all" << endl;
    t << "all: " << fileFixify(Option::output.fileName()) << " " << varGlue("ALL_DEPS"," "," "," ") << " $(DESTDIR_TARGET)" << endl << endl;
    t << "$(DESTDIR_TARGET): " << var("PRE_TARGETDEPS") << " $(OBJECTS) " << var("POST_TARGETDEPS");
    if(!project->isEmpty("QMAKE_PRE_LINK"))
        t << "\n\t" <<var("QMAKE_PRE_LINK");
    if(project->isActiveConfig("staticlib")) {
        t << "\n\t-$(DEL_FILE) $(DESTDIR_TARGET)"
	      << "\n\t" << "$(LIB) $(DESTDIR_TARGET) @&&|" << " \n+"
	      << project->values("OBJECTS").join(" \\\n+") << " \\\n+"
	      << project->values("OBJMOC").join(" \\\n+");
    } else {
        t << "\n\t" << "$(LINK) @&&|" << "\n\t"
	      << "$(LFLAGS) $(OBJECTS) $(OBJMOC),$(DESTDIR_TARGET),,$(LIBS),$(DEF_FILE),$(RES_FILE)";
    }
    t << endl << "|";
    if(!project->isEmpty("QMAKE_POST_LINK"))
        t << "\n\t" <<var("QMAKE_POST_LINK");
    t << endl;
}

void BorlandMakefileGenerator::writeCleanParts(QTextStream &t)
{
    t << "clean: "
      << varGlue("OBJECTS","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","")
      << varGlue("QMAKE_CLEAN","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","\n")
      << varGlue("CLEAN_FILES","\n\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ","\n");

    if(!project->isEmpty("IMAGES"))
        t << varGlue("QMAKE_IMAGE_COLLECTION", "\n\t-$(DEL_FILE) ", "\n\t-$(DEL_FILE) ", "");
    t << endl;

    t << "distclean: clean"
      << "\n\t-$(DEL_FILE) $(DESTDIR_TARGET)"
      << endl << endl;
}

QT_END_NAMESPACE
