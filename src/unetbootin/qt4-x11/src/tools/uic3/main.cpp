/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ui3reader.h"
#include "domtool.h"
#include "globaldefs.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QSettings>
#include <stdio.h>
#include <stdlib.h>

#if defined Q_WS_WIN
#include <qt_windows.h>
#endif

QT_BEGIN_NAMESPACE

int runUic3(int argc, char * argv[])
{
    bool impl = false;
    bool wrap = false;
    bool subcl = false;
    bool extract = false;
    bool imagecollection = false;
    bool imagecollection_tmpfile = false;
    bool convert = false;
    QStringList images;
    const char *error = 0;
    const char* fileName = 0;
    const char* className = 0;
    const char* headerFile = 0;
    const char* convertedUiFile = 0;
    QByteArray outputFile;
    QString  qrcOutputFile;
    QByteArray image_tmpfile;
    const char* projectName = 0;
    const char* trmacro = 0;
    bool nofwd = false;
    bool fix = false;
    bool deps = false;
    bool implicitIncludes = true;
    QByteArray pchFile;


    QApplication app(argc, argv, false);

    for (int n = 1; n < argc && error == 0; n++) {
        QByteArray arg = argv[n];
        if (arg[0] == '-') {                        // option
            QByteArray opt = arg.data() + 1;
            if (opt[0] == 'o') {                // output redirection
                if (opt[1] == '\0') {
                    if (!(n < argc-1)) {
                        error = "Missing output-file name";
                        break;
                    }
                    outputFile = argv[++n];
                } else
                    outputFile = opt.data() + 1;
            } else if (opt[0] == 'i' || opt == "impl") {
                impl = true;
                if (opt == "impl" || opt[1] == '\0') {
                    if (!(n < argc-1)) {
                        error = "Missing name of header file";
                        break;
                    }
                    headerFile = argv[++n];
                } else
                    headerFile = opt.data() + 1;
            } else if (opt[0] == 'w' || opt == "wrap") {
                wrap = true;
                if (opt == "wrap" || opt[1] == '\0') {
                    if (!(n < argc-1)) {
                        error = "Missing name of converted ui file";
                        break;
                    }
                    convertedUiFile = argv[++n];
                } else
                    convertedUiFile = opt.data() + 1;
            } else if (opt == "extract") {                // output redirection
                extract = true;
                if (!(n < argc-1)) {
                    error = "Missing output qrc-file name";
                    break;
                }
                qrcOutputFile = QFile::decodeName(argv[++n]);
            } else if ( opt[0] == 'e' || opt == "embed" ) {
                imagecollection = true;
                if ( opt == "embed" || opt[1] == '\0' ) {
                    if ( !(n < argc-1) ) {
                        error = "Missing name of project";
                        break;
                    }
                    projectName = argv[++n];
                } else {
                    projectName = opt.data() + 1;
                }
                if ( argc > n+1 && qstrcmp( argv[n+1], "-f" ) == 0 ) {
                    imagecollection_tmpfile = true;
                    image_tmpfile = argv[n+2];
                    n += 2;
                }
            } else if (opt == "d") {
                deps = true;
            } else if (opt == "no-implicit-includes") {
                implicitIncludes = false;
            } else if (opt == "nofwd") {
                nofwd = true;
            } else if (opt == "nounload") {
                // skip
            } else if (opt == "convert") {
                convert = true;
            } else if (opt == "subdecl") {
                subcl = true;
                if (!(n < argc-2)) {
                    error = "Missing arguments";
                    break;
                }
                className = argv[++n];
                headerFile = argv[++n];
            } else if (opt == "subimpl") {
                subcl = true;
                impl = true;
                if (!(n < argc-2)) {
                    error = "Missing arguments";
                    break;
                }
                className = argv[++n];
                headerFile = argv[++n];
            } else if (opt == "tr") {
                if (opt == "tr" || opt[1] == '\0') {
                    if (!(n < argc-1)) {
                        error = "Missing tr macro.";
                        break;
                    }
                    trmacro = argv[++n];
                } else {
                    trmacro = opt.data() + 1;
                }
            } else if (opt == "L") {
                if (!(n < argc-1)) {
                    error = "Missing plugin path.";
                    break;
                }
                ++n; // ignore the next argument
            } else if (opt == "version") {
                fprintf(stderr,
                         "Qt User Interface Compiler version %s\n",
                         QT_VERSION_STR);
                return 1;
            } else if (opt == "help") {
                break;
            } else if (opt == "fix") {
                fix = true;
            } else if (opt == "pch") {
                if (!(n < argc-1)) {
                    error = "Missing name of PCH file";
                    break;
                }
                pchFile = argv[++n];
            } else {
                error = "Unrecognized option";
            }
        } else {
            if (imagecollection && !imagecollection_tmpfile)
                images << QLatin1String(argv[n]);
            else if (fileName)                // can handle only one file
                error = "Too many input files specified";
            else
                fileName = argv[n];
        }
    }

    if (argc < 2 || error || (!fileName && !imagecollection)) {
        fprintf(stderr,
                 "Qt User Interface Compiler version %s\n",
                 QT_VERSION_STR);
        if (error)
            fprintf(stderr, "uic: %s\n", error);

        fprintf(stderr, "Usage: %s  [options] [mode] <uifile>\n\n"
                 "Convert a UI file to version 4:\n"
                 "   %s  [options] -convert <uifile>\n"
                 "Generate declaration:\n"
                 "   %s  [options] <uifile>\n"
                 "\t<uiheaderfile>  name of the data file\n"
                 "   %s  [options] -decl <uiheaderfile> <uifile>\n"
                 "\t<uiheaderfile>  name of the data file\n"
                 "   %s  [options] -wrap <converteduifile> <uifile>\n"
                 "\t<converteduifile>  name of the converted ui file\n"
                 "Generate implementation:\n"
                 "   %s  [options] -impl <headerfile> <uifile>\n"
                 "\t<headerfile>    name of the declaration file\n"
                 "Generate image collection:\n"
                 "   %s  [options] -embed <project> <image1> <image2> <image3> ...\n"
                 "or\n"
                 "   %s  [options] -embed <project> -f <temporary file containing image names>\n"
                 "\t<project>       project name\n"
                 "\t<image[1-N]>    image files\n"
                 "Generate subclass declaration:\n"
                 "   %s  [options] -subdecl <subclassname> <baseclassheaderfile> <uifile>\n"
                 "\t<subclassname>     name of the subclass to generate\n"
                 "\t<baseclassheaderfile>    declaration file of the baseclass\n"
                 "Generate subclass implementation:\n"
                 "   %s  [options] -subimpl <subclassname> <subclassheaderfile> <uifile>\n"
                 "\t<subclassname>     name of the subclass to generate\n"
                 "\t<subclassheaderfile>    declaration file of the subclass\n"
                 "Options:\n"
                 "\t-o file            Write output to file rather than stdout\n"
                 "\t-extract qrcFile   Create resource file and extract embedded images into \"image\" dir\n"
                 "\t-pch file          Add #include \"file\" as the first statement in implementation\n"
                 "\t-nofwd             Omit forward declarations of custom classes\n"
                 "\t-no-implicit-includes Do not generate #include-directives for custom classes\n"
                 "\t-nounload          Don't unload plugins after processing\n"
                 "\t-tr func           Use func() instead of tr() for i18n\n"
                 "\t-L path            Additional plugin search path\n"
                 "\t-version           Display version of uic\n"
                 "\t-help              Display this information\n"
                 , argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]
           );
        return 1;
    }

    if (imagecollection_tmpfile) {
        QFile ifile(QFile::decodeName(image_tmpfile));
        if (ifile.open(QIODevice::ReadOnly)) {
            QTextStream ts(&ifile);
            QString s = ts.read();
            s = s.simplified();
            images = s.split(QLatin1Char(' '));
            for (QStringList::Iterator it = images.begin(); it != images.end(); ++it)
                *it = (*it).simplified();
        }
    }

    QFile fileOut;
    if (!outputFile.isEmpty()) {
        fileOut.setFileName(QFile::decodeName(outputFile));
        if (!fileOut.open(QIODevice::WriteOnly)) {
            fprintf(stderr, "%s: Could not open output file '%s'\n", argv[0], outputFile.data());
            return 1;
        }
    } else {
        fileOut.open(QIODevice::WriteOnly, stdout);
    }

    QTextStream out(&fileOut);

    Ui3Reader ui3(out);
    ui3.setExtractImages(extract, qrcOutputFile);

    if (projectName && imagecollection) {
        out.setEncoding(QTextStream::Latin1);
        ui3.embed(projectName, images);
        return 0;
    }

    out.setEncoding(QTextStream::UnicodeUTF8);

    QFile file(QFile::decodeName(fileName));
    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "%s: Could not open file '%s'\n", argv[0], fileName);
        return 1;
    }

    QDomDocument doc;
    QString errMsg;
    int errLine;
    if (!doc.setContent(&file, &errMsg, &errLine)) {
        fprintf(stderr, "%s: Failed to parse %s: %s in line %d\n", argv[0], fileName, errMsg.latin1(), errLine);
        return 1;
    }

    QDomElement e = doc.firstChild().toElement();
    double version = e.attribute(QLatin1String("version"), QLatin1String("3.0")).toDouble();

    if (version > 3.3) {
        fprintf(stderr, "%s: File generated with too recent version of Qt Designer (%s vs. %s)\n",
                  argv[0], e.attribute(QLatin1String("version")).latin1(), "3.3");
        return 1;
    }

    DomTool::fixDocument(doc);

    if (fix) {
        out << doc.toString();
        return 0;
    }

    if (imagecollection) {
        out.setEncoding(QTextStream::Latin1);
        ui3.embed(projectName, images);
        return 0;
    } else if (deps) {
        QStringList globalIncludes, localIncludes;
        ui3.computeDeps(e, globalIncludes, localIncludes, impl);

        foreach (QString i, globalIncludes)
            printf("%s\n", i.toLatin1().constData());

        foreach (QString i, localIncludes)
            printf("%s\n", i.toLatin1().constData());

        if (impl)
            printf("%s\n", headerFile);

        return 0;
    } else if (convert) {
        ui3.generateUi4(QFile::decodeName(fileName), QFile::decodeName(outputFile), doc, implicitIncludes);
        return 0;
    }

    QString protector;
    if (subcl && className && !impl)
        protector = QString::fromUtf8(className).toUpper() + QLatin1String("_H");

    if (!protector.isEmpty()) {
        out << "#ifndef " << protector << endl;
        out << "#define " << protector << endl;
    }

    if (!pchFile.isEmpty() && impl) {
        out << "#include \"" << pchFile << "\" // PCH include" << endl;
    }

    if (headerFile) {
        out << "#include \"" << headerFile << "\"" << endl << endl;
    }

    QString convertedUi;
    if (wrap) {
        convertedUi = QFile::decodeName(convertedUiFile);
        int pos = convertedUi.lastIndexOf(QLatin1String(".ui"));
        if (pos > 0) {
            convertedUi = convertedUi.mid(0, pos);
            convertedUi += QLatin1String(".h");
        }
        convertedUi = QLatin1String("ui_") + convertedUi;
    }

    ui3.generate(QFile::decodeName(fileName),
        QFile::decodeName(outputFile),
        doc,
        !impl,
        subcl,
        QString::fromUtf8(trmacro),
        QString::fromUtf8(className),
        nofwd,
        implicitIncludes, 
        convertedUi);

    if (!protector.isEmpty()) {
        out << endl;
        out << "#endif // " << protector << endl;
    }

    if (fileOut.error() != QFile::NoError) {
        fprintf(stderr, "%s: Error writing to file\n", argv[0]);
        if (!outputFile.isEmpty())
            remove(outputFile);
    }

    return 0;
}

QT_END_NAMESPACE

int main(int argc, char * argv[])
{
    return QT_PREPEND_NAMESPACE(runUic3(argc, argv));
}
