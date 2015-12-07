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
#include "proparser.h"
#include "textreplacement.h"
#include "fileporter.h"
#include "logger.h"
#include "translationunit.h"
#include "codemodelattributes.h"
#include <QtDebug>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QBuffer>

QT_BEGIN_NAMESPACE

using namespace TokenEngine;

ProjectPorter::ProjectPorter(QString basePath, QStringList includeDirectories, QStringList qt3HeadersFilenames)
:basePath(basePath)
,includeDirectories(includeDirectories)
,defaultDefinitions(defaultMacros(preprocessorCache))
,filePorter(preprocessorCache)
,qt3HeadersFilenames(qt3HeadersFilenames)
,analyze(true)
,warnings(false)
{}

void ProjectPorter::enableCppParsing(bool enable)
{
    analyze = enable;
}

void ProjectPorter::enableMissingFilesWarnings(bool enable)
{
    warnings = enable;
}

void ProjectPorter::portProject(QString fileName)
{
    QFileInfo fileInfo(fileName);
    portProject(fileInfo.path(), fileInfo.fileName());
}

/*
    Port a single file
*/
void ProjectPorter::portFile(QString fileName)
{
    if (analyze) {
        IncludeFiles includeFiles(basePath, includeDirectories);

        PreprocessorController preprocessor(includeFiles, preprocessorCache, qt3HeadersFilenames);
        connect(&preprocessor, SIGNAL(error(QString,QString)), SLOT(error(QString,QString)));

        Rpp::DefineMap definitionsCopy = *defaultDefinitions;
        // Preprocess
        TokenSectionSequence translationUnit = preprocessor.evaluate(fileName, &definitionsCopy);
        // Parse
        TranslationUnit translationUnitData = TranslationUnitAnalyzer().analyze(translationUnit);

        // Enable attribute generation for this file.
        enableAttributes(includeFiles, fileName);
        // Generate attributes.
        CodeModelAttributes().createAttributes(translationUnitData);
    }

    portFiles(QString(), QStringList() << fileName);
}

void ProjectPorter::error(QString type, QString text)
{
   if (warnings && type == QLatin1String("Error"))
        printf("Warning: %s\n", text.toLocal8Bit().constData());
}

void ProjectPorter::portProject(QString basePath, QString proFileName)
{
    QString fullInFileName = basePath + QLatin1String("/") + proFileName;
    QFileInfo infileInfo(fullInFileName);
    if (!infileInfo.exists()) {
        printf("Could not open file: %s\n", QDir::toNativeSeparators(fullInFileName).toLocal8Bit().constData());
        return;
    }

    QString proFileContents = loadFile(fullInFileName);
    QMap<QString, QString> proFileMap = proFileTagMap(proFileContents, QDir(basePath).absolutePath());


    // Check if this is a TEMPLATE = subdirs .pro file, in that case we
    // process each subdir (recursively).

    QString templateTag = proFileMap[QLatin1String("TEMPLATE")];
    if (templateTag == QLatin1String("subdirs")) {
        QStringList subdirs = proFileMap[QLatin1String("SUBDIRS")].split(QLatin1String(" "), QString::SkipEmptyParts);
        foreach(QString subdir, subdirs) {
            QString newBasePath  = basePath + QLatin1String("/") + subdir;
            QStringList dirsInSubdir = subdir.split(QRegExp(QLatin1String("/|\\\\")), QString::SkipEmptyParts);
            QString newProFileName = dirsInSubdir.last() + QLatin1String(".pro");
            portProject(newBasePath, newProFileName);
        }
        return;
    }

    // Get headers and sources file names from .pro file.
    QStringList sources = proFileMap[QLatin1String("SOURCES")].split(QLatin1String(" "), QString::SkipEmptyParts);
    QStringList headers = proFileMap[QLatin1String("HEADERS")].split(QLatin1String(" "), QString::SkipEmptyParts);
    QStringList forms = proFileMap[QLatin1String("FORMS")].split(QLatin1String(" "), QString::SkipEmptyParts);
    QStringList uidoth;
    for (int i = 0; i < forms.size(); ++i) {
        QString ui_h = forms.at(i) + QLatin1String(".h");
        if (QFile::exists(basePath + QLatin1String("/") + ui_h))
            uidoth += ui_h;
    }

    if (analyze) {
        printf("Parsing");
        // Get include paths from the pro file.
        QStringList includeProPaths = proFileMap[QLatin1String("INCLUDEPATH")].split(QLatin1String(" "), QString::SkipEmptyParts);
        QStringList dependProPaths = proFileMap[QLatin1String("DEPENDPATH")].split(QLatin1String(" "), QString::SkipEmptyParts);
        IncludeFiles includeFiles(basePath, includeDirectories + includeProPaths + dependProPaths);

        PreprocessorController preprocessorController(includeFiles, preprocessorCache, qt3HeadersFilenames);
        connect(&preprocessorController, SIGNAL(error(QString,QString)), SLOT(error(QString,QString)));

        TranslationUnitAnalyzer translationUnitAnalyzer;
        CodeModelAttributes codeModelAttributes;

        // Enable attribute generation for header files.
        foreach(QString headerFile, headers)
            enableAttributes(includeFiles, headerFile);

        // Enable attribute generation for ui.h files.
        foreach(QString headerFile, uidoth)
            enableAttributes(includeFiles, headerFile);

        // Analyze each translation unit. (one per cpp file)
        foreach(QString sourceFile, sources) {
            printf(".");
            fflush(stdout);
            Rpp::DefineMap definitionsCopy = *defaultDefinitions;
            TokenSectionSequence translationUnit =
                preprocessorController.evaluate(sourceFile, &definitionsCopy);
            TranslationUnit translationUnitData =
                translationUnitAnalyzer.analyze(translationUnit);

            // Enable attribute generation for this file.
            enableAttributes(includeFiles, sourceFile);

            codeModelAttributes.createAttributes(translationUnitData);
        }
        puts("");
    }


    // Port files.
    portFiles(basePath, sources);
    portFiles(basePath, headers);
    if (!uidoth.isEmpty())
        portFiles(basePath, uidoth);

    Logger::instance()->globalState[QLatin1String("currentFileName")] = proFileName;
    Logger::instance()->beginSection();
    portProFile(fullInFileName, proFileMap);
}

/*
    Port each file given in the fileNames list. If a file name is relative
    it is assumed to be relative to basePath.
*/
void ProjectPorter::portFiles(QString basePath, QStringList fileNames)
{
    foreach(QString fileName, fileNames) {
        QString fullFilePath;
        QFileInfo fileInfo(fileName);
        if (fileInfo.isAbsolute()) {
            fullFilePath = QDir::cleanPath(fileName);
        } else {
            fullFilePath = QDir::cleanPath(basePath + QLatin1String("/") + fileName);
        }

        QFileInfo fullFilePathInfo(fullFilePath);
        if (!fullFilePathInfo.exists()) {
            printf("Could not find file: %s\n", QDir::toNativeSeparators(fullFilePath).toLocal8Bit().constData());
            continue;
        }

        if (!processedFilesSet.contains(fullFilePath)){
            Logger::instance()->globalState[QLatin1String("currentFileName")] = fullFilePath;
            filePorter.port(fullFilePath);
            processedFilesSet.insert(fullFilePath);
        }
    }
}

void ProjectPorter::portProFile(QString fileName, QMap<QString, QString> tagMap)
{
    // Read pro file.
    QFile proFile(fileName);
    if (!proFile.open(QIODevice::ReadOnly))
        return;

    const QByteArray contents = proFile.readAll();
    const QByteArray lineEnding = detectLineEndings(contents);
    proFile.seek(0);

    QTextStream proTextStream(&proFile);
    QStringList lines;
    while (!proTextStream.atEnd())
        lines += proTextStream.readLine();

    proFile.close();

    // Find out what modules we should add to the QT variable.
     QSet<QByteArray> qtModules;

    // Add qt3support to the Qt tag
    qtModules.insert(QByteArray("qt3support"));

    // Read CONFIG and add other modules.
    QStringList config = tagMap[QLatin1String("CONFIG")].split(QLatin1String(" "), QString::SkipEmptyParts);
    if (config.contains(QLatin1String("opengl")))
        qtModules.insert(QByteArray("opengl"));
    if (config.contains(QLatin1String("xml")))
        qtModules.insert(QByteArray("xml"));
    if (config.contains(QLatin1String("sql")))
        qtModules.insert(QByteArray("sql"));
    if (config.contains(QLatin1String("network")))
        qtModules.insert(QByteArray("network"));

    // Get set of used modules from the file porter.
    qtModules += filePorter.usedQtModules();

    // Remove gui and core.
    qtModules.remove(QByteArray("gui"));
    qtModules.remove(QByteArray("core"));

    // Qt3Support is already added.
    qtModules.remove(QByteArray("3support"));

    // Remove modules already present in the QT variable.
    QStringList qt = tagMap[QLatin1String("QT")].split(QLatin1String(" "), QString::SkipEmptyParts);
    foreach(QString name, qt) {
        qtModules.remove(name.toLatin1());
    }

    Logger *logger = Logger::instance();
    bool changesMade = false;

    if (!qtModules.isEmpty()) {
        changesMade = true;
        QString insertText = QLatin1String("QT += ");
        foreach(QByteArray module, qtModules) {
            insertText += QString::fromLatin1(module) + QLatin1Char(' ');
        }
        lines += QString(QLatin1String("#The following line was inserted by qt3to4"));
        lines += insertText;
         QString logText = QLatin1String("In file ")
                        + logger->globalState.value(QLatin1String("currentFileName"))
                        + QLatin1String(": Added entry ")
                        + insertText;
        logger->addEntry(new PlainLogEntry(QLatin1String("Info"), QLatin1String("Porting"), logText));
    }

    // Add uic3 if we have forms, and change FORMS and INTERFACES to FORMS3
    if (!tagMap[QLatin1String("FORMS")].isEmpty() || !tagMap[QLatin1String("INTERFACES")].isEmpty()) {
        changesMade = true;
        lines += QString(QLatin1String("#The following line was inserted by qt3to4"));
        QString insertText = QLatin1String("CONFIG += uic3") + QString::fromLatin1(lineEnding.constData());
        lines += insertText;
        QString logText = QLatin1String("In file ")
                        + logger->globalState.value(QLatin1String("currentFileName"))
                        + QLatin1String(": Added entry ")
                        + insertText;
        logger->addEntry(new PlainLogEntry(QLatin1String("Info"), QLatin1String("Porting"), logText));

        const QString formsToForms3(QLatin1String("#The following line was changed from FORMS to FORMS3 by qt3to4"));
        const QString interfacesToForms3(QLatin1String("#The following line was changed from INTERFACES to FORMS3 by qt3to4"));
        for (int i = 0; i < lines.count(); ++i) {
            QString cLine = lines.at(i);
            cLine = cLine.trimmed();
            if (cLine.startsWith(QLatin1String("FORMS"))) {
                lines[i].replace(QLatin1String("FORMS"), QLatin1String("FORMS3"));
                lines.insert(i, formsToForms3);
                ++i;
                QString logText = QLatin1String("In file ")
                    + logger->globalState.value(QLatin1String("currentFileName"))
                    + QLatin1String(": Renamed FORMS to FORMS3");
                logger->addEntry(new PlainLogEntry(QLatin1String("Info"), QLatin1String("Porting"), logText));
            } else if (cLine.startsWith(QLatin1String("INTERFACES"))) {
                lines[i].replace(QLatin1String("INTERFACES"), QLatin1String("FORMS3"));
                lines.insert(i, interfacesToForms3);
                ++i;
                QString logText = QLatin1String("In file ")
                    + logger->globalState.value(QLatin1String("currentFileName"))
                    + QLatin1String(": Renamed INTERFACES to FORMS3");
                logger->addEntry(new PlainLogEntry(QLatin1String("Info"), QLatin1String("Porting"), logText));
            }
        }
    }

    // Comment out any REQUIRES tag.
    if (!tagMap[QLatin1String("REQUIRES")].isEmpty()) {
        changesMade = true;
        QString insertText(QLatin1String("#The following line was commented out by qt3to4"));
        for (int i = 0; i < lines.count(); ++i) {
            if (lines.at(i).startsWith(QLatin1String("REQUIRES"))) {
                QString lineCopy = lines.at(i);
                lineCopy.prepend(QLatin1Char('#'));
                lines[i] = lineCopy;
                lines.insert(i, insertText);
                ++i; //skip ahead, we just insertet a line at i.
                QString logText = QLatin1String("In file ")
                            + logger->globalState.value(QLatin1String("currentFileName"))
                            + QLatin1String(": Commented out REQUIRES section");
                logger->addEntry(new PlainLogEntry(QLatin1String("Info"), QLatin1String("Porting"), logText));
            }
        }
    }

    // Check if any changes has been made.
    if (!changesMade) {
        Logger::instance()->addEntry(
            new PlainLogEntry(QLatin1String("Info"), QLatin1String("Porting"),  QLatin1String("No changes made to file ") + fileName));
        Logger::instance()->commitSection();
        return;
    }

    // Write lines to array.
    QByteArray bob;
    QTextStream outProFileStream(&bob);
    foreach(QString line, lines)
        outProFileStream << line << lineEnding;
    outProFileStream.flush();

    // Write array to file, commit log if write was successful.
    FileWriter::WriteResult result = FileWriter::instance()->writeFileVerbously(fileName, bob);
    if (result == FileWriter::WriteSucceeded) {
        logger->commitSection();
    } else if (result == FileWriter::WriteFailed) {
        logger->revertSection();
        logger->addEntry(
            new PlainLogEntry(QLatin1String("Error"), QLatin1String("Porting"),  QLatin1String("Error writing to file ") + fileName));
    } else if (result == FileWriter::WriteSkipped) {
        logger->revertSection();
        logger->addEntry(
            new PlainLogEntry(QLatin1String("Error"), QLatin1String("Porting"),  QLatin1String("User skipped file ") + fileName));
    } else {
        // Internal error.
        logger->revertSection();
        const QString errorString = QLatin1String("Internal error in qt3to4 - FileWriter returned invalid result code while writing to ") + fileName;
        logger->addEntry(new PlainLogEntry(QLatin1String("Error"), QLatin1String("Porting"), errorString));
    }
}

/*
    Enables attribute generation for fileName. The file is looked up using the
    provied includeFiles object.
*/
void ProjectPorter::enableAttributes(const IncludeFiles &includeFiles, const QString &fileName)
{
    QString resolvedFilePath = includeFiles.resolve(fileName);
    if (!QFile::exists(resolvedFilePath))
            resolvedFilePath = includeFiles.angleBracketLookup(fileName);
    if (!QFile::exists(resolvedFilePath))
        return;

    TokenContainer tokenContainer = preprocessorCache.sourceTokens(resolvedFilePath);
    TokenAttributes *attributes = tokenContainer.tokenAttributes();
    attributes->addAttribute("CreateAttributes", "True");
}

QT_END_NAMESPACE
