/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "../shared/helpgenerator.h"

#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>

#include <private/qhelpgenerator_p.h>
#include <private/qhelpprojectdata_p.h>
#include <QtHelp/QHelpEngineCore>

#include <QtXml/QXmlStreamReader>

QT_USE_NAMESPACE

class CollectionConfigReader : public QXmlStreamReader
{
public:
    void readData(const QByteArray &contents);

    QString title() const { return m_title; }
    QString startPage() const { return m_startPage; }
    QString applicationIcon() const { return m_applicationIcon; }
    QString currentFilter() const { return m_currentFilter; }
    bool enableFilterFunctionality() const
        { return m_enableFilterFunctionality; }
    bool hideFilterFunctionality() const
        { return m_hideFilterFunctionality; }
        bool enableAddressBar() const { return m_enableAddressBar; }
    bool hideAddressBar() const { return m_hideAddressBar; }
    bool enableDocumentationManager() const
        { return m_enableDocumentationManager; }

    QMap<QString, QString> aboutMenuTexts() const
        { return m_aboutMenuTexts; }
    QString aboutIcon() const { return m_aboutIcon; }
    QMap<QString, QString> aboutTextFiles() const
        { return m_aboutTextFiles; }

    QMap<QString, QString> filesToGenerate() const
        { return m_filesToGenerate; }

    QStringList filesToRegister() const { return m_filesToRegister; }

    QString cacheDirectory() const { return m_cacheDirectory; }

private:
    void raiseErrorWithLine();
    void readConfig();
    void readAssistantSettings();
    void readMenuTexts();
    void readAboutDialog();
    void readDocFiles();
    void readGenerate();
    void readFiles();
    void readRegister();

    QString m_title;
    QString m_startPage;
    QString m_applicationIcon;
    QString m_currentFilter;
    bool m_enableFilterFunctionality;
    bool m_hideFilterFunctionality;
    bool m_enableAddressBar;
    bool m_hideAddressBar;
    bool m_enableDocumentationManager;
    QMap<QString, QString> m_aboutMenuTexts;
    QString m_aboutIcon;
    QMap<QString, QString> m_aboutTextFiles;
    QMap<QString, QString> m_filesToGenerate;
    QStringList m_filesToRegister;
    QString m_cacheDirectory;
};

void CollectionConfigReader::raiseErrorWithLine()
{
    raiseError(QObject::tr("Unknown token at line %1.")
                    .arg(lineNumber()));    
}

void CollectionConfigReader::readData(const QByteArray &contents)
{
    m_enableFilterFunctionality = true;
    m_hideFilterFunctionality = true;
    m_enableAddressBar = true;
    m_hideAddressBar = true;
    m_enableDocumentationManager = true;

    addData(contents);
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("QHelpCollectionProject")
                && attributes().value(QLatin1String("version")) == QLatin1String("1.0"))
                readConfig();
            else
                raiseError(QObject::tr("Unknown token at line %1. Expected \"QtHelpCollectionProject\"!")
                .arg(lineNumber()));
        }
    }    
}

void CollectionConfigReader::readConfig()
{
    bool ok = false;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("assistant"))
                readAssistantSettings();
            else if (name() == QLatin1String("docFiles"))
                readDocFiles();
            else
                raiseErrorWithLine();
        } else if (isEndElement() && name() == QLatin1String("QHelpCollectionProject")) {
            ok = true;
        }
    }
    if (!ok && !hasError())
        raiseError(QLatin1String("Missing end tags."));
}

void CollectionConfigReader::readAssistantSettings()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("title")) {
                m_title = readElementText();
            } else if (name() == QLatin1String("startPage")) {
                m_startPage = readElementText();
            } else if (name() == QLatin1String("currentFilter")) {
                m_currentFilter = readElementText();
            } else if (name() == QLatin1String("applicationIcon")) {
                m_applicationIcon = readElementText();
            } else if (name() == QLatin1String("enableFilterFunctionality")) {
                if (attributes().value(QLatin1String("visible")) == QLatin1String("true"))
                    m_hideFilterFunctionality = false;
                if (readElementText() == QLatin1String("false"))
                    m_enableFilterFunctionality = false;
            } else if (name() == QLatin1String("enableDocumentationManager")) {
                if (readElementText() == QLatin1String("false"))
                    m_enableDocumentationManager = false;
            } else if (name() == QLatin1String("enableAddressBar")) {
                if (attributes().value(QLatin1String("visible")) == QLatin1String("true"))
                    m_hideAddressBar = false;
                if (readElementText() == QLatin1String("false"))
                    m_enableAddressBar = false;
            } else if (name() == QLatin1String("aboutMenuText")) {
                readMenuTexts();
            } else if (name() == QLatin1String("aboutDialog")) {
                readAboutDialog();
            } else if (name() == "cacheDirectory") {
                m_cacheDirectory = readElementText();
            } else {
                raiseErrorWithLine();
            }
        } else if (isEndElement() && name() == QLatin1String("assistant")) {
            break;
        }
    }
}

void CollectionConfigReader::readMenuTexts()
{
    while (!atEnd())
    {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("text")) {
                QString lang = attributes().value(QLatin1String("language")).toString();
                if (lang.isEmpty())
                    lang = QLatin1String("default");
                m_aboutMenuTexts.insert(lang, readElementText());
            } else {
                raiseErrorWithLine();
            }
        } else if (isEndElement() && name() == QLatin1String("aboutMenuText")) {
            break;
        }
    }
}

void CollectionConfigReader::readAboutDialog()
{
    while (!atEnd())
    {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("file")) {
                QString lang = attributes().value(QLatin1String("language")).toString();
                if (lang.isEmpty())
                    lang = QLatin1String("default");
                m_aboutTextFiles.insert(lang, readElementText());
            } else if (name() == QLatin1String("icon")) {
                m_aboutIcon = readElementText();
            } else {
                raiseErrorWithLine();
            }
        } else if (isEndElement() && name() == QLatin1String("aboutDialog")) {
            break;
        }
    }
}

void CollectionConfigReader::readDocFiles()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("generate")) {
                readGenerate();
            } else if (name() == QLatin1String("register")) {
                readRegister();
            } else {
                raiseErrorWithLine();
            }
        } else if (isEndElement() && name() == QLatin1String("docFiles")) {
            break;
        }
    }
}

void CollectionConfigReader::readGenerate()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("file"))
                readFiles();
            else
                raiseErrorWithLine();
        } else if (isEndElement() && name() == QLatin1String("generate")) {
            break;
        }
    }
}

void CollectionConfigReader::readFiles()
{
    QString input;
    QString output;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("input"))
                input = readElementText();
            else if (name() == QLatin1String("output"))
                output = readElementText();
            else
                raiseErrorWithLine();
        } else if (isEndElement() && name() == QLatin1String("file")) {
            break;
        }
    }
    if (input.isEmpty() || output.isEmpty()) {
        raiseError(QLatin1String("Missing input or output file for help file generation!"));
        return;
    }
    m_filesToGenerate.insert(input, output);
}

void CollectionConfigReader::readRegister()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == QLatin1String("file"))
                m_filesToRegister.append(readElementText());
            else
                raiseErrorWithLine();
        } else if (isEndElement() && name() == QLatin1String("register")) {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    QString error;
    QString arg;    
    QString collectionFile;
    QString configFile;
    QString basePath;
    bool showHelp = false;
    bool showVersion = false;

    for (int i=1; i<argc; ++i) {
        arg = QString::fromLocal8Bit(argv[i]);    
        if (arg == QLatin1String("-o")) {
            if (++i < argc) {
                QFileInfo fi(QString::fromLocal8Bit(argv[i]));
                collectionFile = fi.absoluteFilePath();                
            } else {
                error = QObject::tr("Missing output file name!");
            }
        } else if (arg == QLatin1String("-h")) {
            showHelp = true;
        } else if (arg == QLatin1String("-v")) {
            showVersion = true;
        } else {
            QFileInfo fi(arg);
            configFile = fi.absoluteFilePath();
            basePath = fi.absolutePath();
        }
    }

    if (showVersion) {
        fprintf(stdout, "Qt Collection Generator version 1.0 (Qt %s)\n", QT_VERSION_STR);
        return 0;
    }

    if (configFile.isEmpty() && !showHelp)
        error = QObject::tr("Missing collection config file!");

    QString help = QObject::tr("\nUsage:\n\n"
        "qcollectiongenerator <collection-config-file> [options]\n\n"
        "  -o <collection-file>   Generates a collection file\n"
        "                         called <collection-file>. If\n"
        "                         this option is not specified\n"
        "                         a default name will be used.\n"
        "  -v                     Displays the version of\n"
        "                         qcollectiongenerator.\n\n");

    if (showHelp) {
        fprintf(stdout, "%s", qPrintable(help));
        return 0;
    }else if (!error.isEmpty()) {
        fprintf(stderr, "%s\n\n%s", qPrintable(error), qPrintable(help));
        return -1;
    }

    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "Could not open %s!\n", qPrintable(configFile));
        return -1;
    }

    if (collectionFile.isEmpty()) {
        QFileInfo fi(configFile);
        collectionFile = basePath + QDir::separator()
            + fi.baseName() + QLatin1String(".qhc");
    }

    QCoreApplication app(argc, argv);

    fprintf(stdout, "Reading collection config file...\n");
    CollectionConfigReader config;
    config.readData(file.readAll());
    if (config.hasError()) {
        fprintf(stderr, "Collection config file error: %s\n", qPrintable(config.errorString()));
        return -1;
    }
    
    QMap<QString, QString>::const_iterator it = config.filesToGenerate().constBegin();
    while (it != config.filesToGenerate().constEnd()) {
        fprintf(stdout, "Generating help for %s...\n", qPrintable(it.key()));
        QHelpProjectData helpData;
        if (!helpData.readData(basePath + QDir::separator() + it.key())) {
            fprintf(stderr, "%s\n", qPrintable(helpData.errorMessage()));
            return -1;
        }
        
        HelpGenerator helpGenerator;
        if (!helpGenerator.generate(&helpData, basePath + QDir::separator() + it.value())) {
            fprintf(stderr, "%s\n", qPrintable(helpGenerator.error()));
            return -1;
        }
        ++it;
    }
    
    fprintf(stdout, "Creating collection file...\n");

    QFileInfo colFi(collectionFile);
    if (colFi.exists()) {
        if (!colFi.dir().remove(colFi.fileName())) {
            fprintf(stderr, "The file %s cannot be overwritten!\n", qPrintable(collectionFile));
            return -1;
        }
    }

    QHelpEngineCore helpEngine(collectionFile);
    if (!helpEngine.setupData()) {
        fprintf(stderr, "%s\n", qPrintable(helpEngine.error()));
        return -1;
    }    

    foreach (QString file, config.filesToRegister()) {
        if (!helpEngine.registerDocumentation(basePath + QDir::separator() + file)) {
            fprintf(stderr, "%s\n", qPrintable(helpEngine.error()));
            return -1;
        }
    }
    
    if (!config.title().isEmpty())
        helpEngine.setCustomValue(QLatin1String("WindowTitle"), config.title());

    if (!config.startPage().isEmpty())
        helpEngine.setCustomValue(QLatin1String("LastShownPages"), config.startPage());

    if (!config.currentFilter().isEmpty())
        helpEngine.setCustomValue(QLatin1String("CurrentFilter"), config.currentFilter());

    if (!config.cacheDirectory().isEmpty())
        helpEngine.setCustomValue(QLatin1String("CacheDirectory"), config.cacheDirectory());

    helpEngine.setCustomValue(QLatin1String("EnableFilterFunctionality"),
        config.enableFilterFunctionality());
    helpEngine.setCustomValue(QLatin1String("HideFilterFunctionality"),
        config.hideFilterFunctionality());
    helpEngine.setCustomValue(QLatin1String("EnableDocumentationManager"),
        config.enableDocumentationManager());
    helpEngine.setCustomValue(QLatin1String("EnableAddressBar"),
        config.enableAddressBar());
    helpEngine.setCustomValue(QLatin1String("HideAddressBar"),
        config.hideAddressBar());

    if (!config.applicationIcon().isEmpty()) {
        QFile icon(basePath + QDir::separator() + config.applicationIcon());
        if (!icon.open(QIODevice::ReadOnly)) {
            fprintf(stderr, "Cannot open %s!\n", qPrintable(icon.fileName()));
            return -1;
        }
        helpEngine.setCustomValue(QLatin1String("ApplicationIcon"), icon.readAll());
    }

    if (config.aboutMenuTexts().count()) {
        QByteArray ba;
        QDataStream s(&ba, QIODevice::WriteOnly);
        QMap<QString, QString>::const_iterator it = config.aboutMenuTexts().constBegin();
        while (it != config.aboutMenuTexts().constEnd()) {
            s << it.key();
            s << it.value();
            ++it;   
        }
        helpEngine.setCustomValue(QLatin1String("AboutMenuTexts"), ba);
    }

    if (!config.aboutIcon().isEmpty()) {
        QFile icon(basePath + QDir::separator() + config.aboutIcon());
        if (!icon.open(QIODevice::ReadOnly)) {
            fprintf(stderr, "Cannot open %s!\n", qPrintable(icon.fileName()));
            return -1;
        }
        helpEngine.setCustomValue(QLatin1String("AboutIcon"), icon.readAll());
    }

    if (config.aboutTextFiles().count()) {
        QByteArray ba;
        QDataStream s(&ba, QIODevice::WriteOnly);
        QMap<QString, QString>::const_iterator it = config.aboutTextFiles().constBegin();
        while (it != config.aboutTextFiles().constEnd()) {
            s << it.key();
            QFile f(basePath + QDir::separator() + it.value());
            if (!f.open(QIODevice::ReadOnly)) {
                fprintf(stderr, "Cannot open %s!\n", qPrintable(f.fileName()));
                return -1;
            }
            s << f.readAll();
            ++it;
        }
        helpEngine.setCustomValue(QLatin1String("AboutTexts"), ba);
    }
  
    return 0;
}
