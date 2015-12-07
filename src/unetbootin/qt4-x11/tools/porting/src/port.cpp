/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "projectporter.h"
#include "fileporter.h"
#include "logger.h"
#include "preprocessorcontrol.h"

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QtDebug>

QT_BEGIN_NAMESPACE

QString rulesFilePath;
QString applicationDirPath;

QString findRulesFile(const QString &fileName)
{
    // Check QLibraryInfo::DataPath/filename
    QString filePath;
    filePath = QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::DataPath) + QLatin1String("/") + fileName)  ;
    if (QFile::exists(filePath))
        return QFileInfo(filePath).canonicalFilePath();

    // Check QLibraryInfo::PrefixPath/tools/porting/src/filename
    filePath = QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::PrefixPath) + QLatin1String("/tools/porting/src/") + fileName);
    if (QFile::exists(filePath))
        return QFileInfo(filePath).canonicalFilePath();

    //no luck
    return QString();
}

/*
    A option contains an argument and its help text.
*/
class Option
{
public:
    Option(const QString &argument, const QString &description)
    :argument(argument), description(description) {}

    /*
        Checks if candidateArgument matches the options argument.
    */
    bool checkArgument(const QString &candidateArgument) const
    {
        return (candidateArgument == argument) ||
               (candidateArgument.toLower() == argument.toLower());
    }

    QString argument;
    QString description;
};

typedef QList<Option> OptionList;

void usage(const OptionList &optionList)
{
    printf("Tool for porting Qt 3 applications to Qt 4, using the compatibility library\n");
    printf("and compatibility functions in the core library.\n");
    printf("Usage: qt3to4 [options] <Infile>, [Infile], ...\n");
    printf("\n");
    printf("Infile can be a source file or a project file.\n");
    printf("If you specify a project file, ending with .pro or .pri,\n");
    printf("qt3to4 will port all files specified in that project.\n");
    printf("\n");
    printf("Options:\n");

    // Find the length of the longest argument.
    int argumentMaxLenght = 0;
    foreach (const Option option, optionList) {
        if (option.argument.count() > argumentMaxLenght)
            argumentMaxLenght = option.argument.count();
    }

    // Print the options, pad with spaces between the argument and description where needed.
    const int extraSpaces = 5;
    foreach (const Option option, optionList) {
        printf("%s", option.argument.toLocal8Bit().constData());
        for (int i = 0; i < argumentMaxLenght - option.argument.count() + extraSpaces; ++i)
            printf(" ");
        puts(option.description.toLocal8Bit().constData());
    }

    printf("\n");
    printf("The porting documentation contains more information on how\n");
    printf("to use qt3to4 as well as general porting information.\n");
}

int runPort(int argc, char**argv)
{
    QCoreApplication app(argc, argv);
    applicationDirPath = app.applicationDirPath();
    QString defaultRulesFileName = QLatin1String("q3porting.xml");
    QStringList inFileNames;
    QStringList includeSearchDirectories;
    bool enableCppParsing = true;
    bool useBuildtinQt3Headers = true;
    bool showMissingFilesWarnings = false;
    bool alwaysOverwrite = false;
    int currentArg = 1;

    const Option helpOption(QLatin1String("-h"), QLatin1String("Display this help."));
    const Option rulesFileOption(QLatin1String("-rulesFile"), QLatin1String("Specify the location for the rules file."));
    const Option includeDirectoryOption(QLatin1String("-I"), QLatin1String("Add directory to the list of directories to be searched for header files."));
    const Option disableCppParsingOption(QLatin1String("-disableCppParsing"), QLatin1String("Disable the C++ parsing component."));
    const Option disableBuiltinQt3HeadersOption(QLatin1String("-disableBuiltinQt3Headers"), QLatin1String("Do not use the built-in Qt 3 headers."));
    const Option missingFileWarningsOption(QLatin1String("-missingFileWarnings"), QLatin1String("Warn about files not found while searching for header files."));
    const Option alwaysOverwriteOption(QLatin1String("-alwaysOverwrite"), QLatin1String("Port all files without prompting."));
    const Option strictOption(QLatin1String("-strict"), QLatin1String("Be stricter when selecting which tokens to replace."));

    const OptionList optionList = OptionList() << helpOption << alwaysOverwriteOption << rulesFileOption
                                               << includeDirectoryOption << disableCppParsingOption
                                               << disableBuiltinQt3HeadersOption << missingFileWarningsOption
                                               << strictOption;

    if (argc == 1) {
        usage(optionList);
        return 0;
    }

    // Read arguments.
    while (currentArg < argc) {
        QString argText = QLatin1String(argv[currentArg]);
        if(argText.isEmpty()) {
            continue;
        } else if (argText == QLatin1String("--help") || argText == QLatin1String("/h") || argText == QLatin1String("-help")
            || argText == QLatin1String("-h")  || argText == QLatin1String("-?") || argText == QLatin1String("/?")) {
            usage(optionList);
            return 0;
        } else if (rulesFileOption.checkArgument(argText)) {
            ++currentArg;
            if (currentArg >= argc) {
                printf("You must specify a file name along with %s \n", argText.toLocal8Bit().constData());
                return 0;
            }
            rulesFilePath = QLatin1String(argv[currentArg]);

            if (!QFile::exists(rulesFilePath)) {
                printf("File not found: %s\n", rulesFilePath.toLocal8Bit().constData());
                return 0;
            }
        } else if (includeDirectoryOption.checkArgument(argText)) {
            ++currentArg;
            if (currentArg >= argc) {
                printf("You must specify a directory name along with %s\n",
                     argText.toLocal8Bit().constData());
                return 0;
            }
            includeSearchDirectories += QLatin1String(argv[currentArg]);
        } else if (disableCppParsingOption.checkArgument(argText)) {
            enableCppParsing = false;
        } else if (strictOption.checkArgument(argText)) {
            // Enable strict mode, this is used by the ScopedTokenReplacement constructor. 
            Logger::instance()->globalState.insert(QLatin1String("strictMode"), QLatin1String(""));
        } else if (disableBuiltinQt3HeadersOption.checkArgument(argText)) {
            useBuildtinQt3Headers = false;
        } else if (missingFileWarningsOption.checkArgument(argText)) {
            showMissingFilesWarnings = true;
        } else if (alwaysOverwriteOption.checkArgument(argText)) {
            alwaysOverwrite = true;
            FileWriter::instance()->setOverwriteFiles(FileWriter::AlwaysOverWrite);
        } else if (argText[0]  == QLatin1Char('-')) {
            printf("Unknown option %s\n", argText.toLocal8Bit().constData());
            return 0;
        } else {
            inFileNames.append(argText);
        }
        ++currentArg;
    }

    if (rulesFilePath.isEmpty())
        rulesFilePath = findRulesFile(defaultRulesFileName);

    // Check if we have a rule file.
    if (!QFile::exists(rulesFilePath)) {
        printf("Error: Could not find the %s rule file: ", defaultRulesFileName.toLocal8Bit().constData());
        printf("Please try specifying the location of the file with the %s option \n", 
            rulesFileOption.argument.toLocal8Bit().constData());
        return 0;
    }

    // Check if we have any infiles
    if (inFileNames.isEmpty()) {
        printf("You must specify a file name. \n");
        return 0;
    }

    // Read rule file and create PortingRules instance.
    printf("Using rules file: ");
    puts(QDir::toNativeSeparators(rulesFilePath).toLocal8Bit().constData());
    PortingRules::createInstance(rulesFilePath);


    // Construct a ProjectPorter object add pass it the options.
    QStringList builtinQtheaders;
    if (useBuildtinQt3Headers) {
        builtinQtheaders += QLatin1String(":qt3headers0.resource");
        builtinQtheaders += QLatin1String(":qt3headers1.resource");
        builtinQtheaders += QLatin1String(":qt3headers2.resource");
        builtinQtheaders += QLatin1String(":qt3headers3.resource");
    }

    ProjectPorter porter(QDir::currentPath(), includeSearchDirectories, builtinQtheaders);
    porter.enableCppParsing(enableCppParsing);
    porter.enableMissingFilesWarnings(showMissingFilesWarnings);

    // Determine mode based on file exstesions and port.
    // (The ProjectPorter class is also used for porting single files :)
    foreach (QString inFileName, inFileNames) {
        const QString canonicalFileName = QFileInfo(inFileName).canonicalFilePath();
        if (QFile::exists(canonicalFileName)) {
            if (canonicalFileName.endsWith(QLatin1String(".pro")) || canonicalFileName.endsWith(QLatin1String(".pri")))
                porter.portProject(canonicalFileName);
            else
                porter.portFile(canonicalFileName);
        } else {
            printf("File not found: %s \n", QDir::toNativeSeparators(inFileName).toLocal8Bit().constData());
        }
    }

    // Write log
    if (Logger::instance()->numEntries() > 0) {
        QStringList report = Logger::instance()->fullReport();
        QString logFileName =  QLatin1String("portinglog.txt");
        printf("Writing log to %s \n", logFileName.toLocal8Bit().constData());
        QByteArray logContents;
        QBuffer logBuffer(&logContents);
        logBuffer.open(QIODevice::Text | QIODevice::WriteOnly);
        QTextStream logStream(&logBuffer);
        foreach (QString logLine, report) {
            logStream << logLine << endl;
        }
        logStream << endl;

        QFile logFile(logFileName);
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
        logFile.write(logContents);
    }
    Logger::deleteInstance();
    PortingRules::deleteInstance();
    return 0;
}

QT_END_NAMESPACE

int main(int argc, char**argv)
{
    return QT_PREPEND_NAMESPACE(runPort)(argc, argv);
}
