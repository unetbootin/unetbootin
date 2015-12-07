/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "proparser.h"
#include "profileevaluator.h"
#include <stdio.h>
#include <proreader.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

// assumes that the list is sorted (or more correctly, that all equal
// elements are grouped together).
void removeDuplicates(QStringList *strings, bool alreadySorted /*= true*/)
{
    if (!alreadySorted) {
        strings->sort();
    }
    QString prev;
    QStringList::iterator it = strings->begin();
    while (it != strings->end()) {
        if (*it == prev) {
            it = strings->erase(it);
        }else{
            prev = *it;
            ++it;
        }
    }
}

bool evaluateProFile(const QString &fileName, bool verbose,QMap<QByteArray, QStringList> *varMap)
{
    bool ok = true;

    QStringList sourceFiles;
    QString codecForTr;
    QString codecForSource;
    QStringList tsFileNames;

    ProFileTranslationsScanner *visitor = new ProFileTranslationsScanner(verbose);
    QFileInfo fi(fileName);
    QDir rootPath;
    ok = fi.exists();
    if (ok) {
        rootPath.setPath(fi.absolutePath());
        ProFile *pro = visitor->queryProFile(fi.absoluteFilePath());
        if (!pro)
            ok = false;
        else
            ok = pro->Accept(visitor);
    }
    if (ok) {
        // app/lib template
        sourceFiles += visitor->values(QLatin1String("SOURCES"));
        sourceFiles += visitor->values(QLatin1String("HEADERS"));
        tsFileNames = visitor->values(QLatin1String("TRANSLATIONS"));
        QStringList trcodec = visitor->values(QLatin1String("CODEC"))
            + visitor->values(QLatin1String("DEFAULTCODEC"))
            + visitor->values(QLatin1String("CODECFORTR"));

        if (!trcodec.isEmpty())
            codecForTr = trcodec.last();

        QStringList srccodec = visitor->values(QLatin1String("CODECFORSRC"));
        if (!srccodec.isEmpty())
            codecForSource = srccodec.last();

        QStringList forms = visitor->values(QLatin1String("INTERFACES"))
            + visitor->values(QLatin1String("FORMS"))
            + visitor->values(QLatin1String("FORMS3"));
        sourceFiles << forms;
    }
    if (ok) {
        removeDuplicates(&sourceFiles, false);
        removeDuplicates(&tsFileNames, false);

        varMap->insert("SOURCES", sourceFiles);
        varMap->insert("CODECFORTR", QStringList() << codecForTr);
        varMap->insert("CODECFORSRC", QStringList() << codecForSource);
        varMap->insert("TRANSLATIONS", tsFileNames);
    }
    return ok;

}

QStringList getListOfProfiles(const QStringList &proFiles, bool verbose)
{
    QStringList profileList = proFiles;
    bool ok = true;

    int i = 0;
    while( i < profileList.count() ) {
        QFileInfo fi(profileList.at(i));
        ProFileTranslationsScanner *visitor = new ProFileTranslationsScanner(verbose);
        ok = fi.exists();
        if (ok) {
            ProFile *pro = visitor->queryProFile(fi.absoluteFilePath());
            if (!pro)
                ok = false;
            else
                ok = pro->Accept(visitor);

            if (ok && visitor->templateType() == ProFileEvaluator::TT_Subdirs) {
                QStringList subdirs = visitor->values(QLatin1String("SUBDIRS"));
                for (int is = 0; is < subdirs.count(); ++is) {
                    QString subdir = subdirs[is];
                    QDir dir( subdir );
                    QStringList profiles = dir.entryList(QStringList() << QLatin1String("*.pro"));
                    if (profiles.count())
                        profileList << subdir + QLatin1Char('/') + profiles[0];
                }
                profileList.removeAt(i);
            } else {
                ++i;
            }
        }
        delete visitor;
    }
    if (!ok)
        profileList.clear();
    return profileList;
}

QT_END_NAMESPACE
