/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "config.h"
#include "profile.h"
#include "docuparser.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLibraryInfo>
#include <QFont>
#include <QFontInfo>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QList>

QT_BEGIN_NAMESPACE

static Config *static_configuration = 0;

inline QString getVersionString()
{
    return QString::number( (QT_VERSION >> 16) & 0xff )
        + QLatin1String(".") + QString::number( (QT_VERSION >> 8) & 0xff );
}

Config::Config()
    : profil( 0 ), hideSidebar( false ), rebuildDocs(true)
{
    if( !static_configuration ) {
        static_configuration = this;
    } else {
        qWarning( "Multiple configurations not allowed!" );
    }
}

Config *Config::loadConfig(const QString &profileFileName)
{
    Config *config = new Config();

    if (profileFileName.isEmpty()) { // no profile
        if (!config->defaultProfileExists()) {
            config->profil = Profile::createDefaultProfile();
            config->saveProfile(config->profil);
        } else {
            config->profil = new Profile();
        }
        config->loadDefaultProfile();
        config->load();
        return config;
    }

    QFile file(profileFileName);
    if (!file.exists()) {
        qWarning( "File does not exist: %s", qPrintable(profileFileName) );
        return 0;
    }
    DocuParser *parser = DocuParser::createParser( profileFileName );
    if (!parser) {
        qWarning( "Failed to create parser for file: %s", qPrintable(profileFileName) );
        return 0;
    }
    if (parser->parserVersion() < DocuParser::Qt320) {
        qWarning( "File does not contain profile information" );
        return 0;
    }
    DocuParser320 *profileParser = static_cast<DocuParser320*>(parser);
    parser->parse(&file);
    config->profil = profileParser->profile();
    if (!config->profil) {
        qWarning( "Config::loadConfig(), no profile in: %s", qPrintable(profileFileName) );
        return 0;
    }
    config->profil->setProfileType(Profile::UserProfile);
    config->profil->setDocuParser(profileParser);
    config->load();
    return config;
}

Config *Config::configuration()
{
    Q_ASSERT( static_configuration );
    return static_configuration;
}

void Config::load()
{
    const QString key = getVersionString() + QLatin1String("/");

    bool isDefaultProfile = profil->props[QLatin1String("name")] == QLatin1String("default");
    const QString pKey = isDefaultProfile ? QString::fromLatin1(QT_VERSION_STR)
        : getVersionString();

    const QString profkey = pKey + QLatin1String("/Profile/") + profil->props[QLatin1String("name")] + QLatin1String("/");

    QSettings settings;

    home = profil->props[QLatin1String("startpage")];;
    if (home.isEmpty() && isDefaultProfile)
        home = QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/html/index.html");
    src = settings.value( profkey + QLatin1String("Source") ).toStringList();
    sideBar = settings.value( key + QLatin1String("SideBarPage") ).toInt();
    if (qApp->type() != QApplication::Tty)
        winGeometry = settings.value(key + QLatin1String("windowGeometry")).toByteArray();

    mainWinState = settings.value(key + QLatin1String("MainWindowState")).toByteArray();
    pointFntSize = settings.value(key + QLatin1String("FontSize"), qApp->font().pointSizeF()).toDouble();
    rebuildDocs = settings.value( key + QLatin1String("RebuildDocDB"), true ).toBool();

    profileNames = settings.value( key + QLatin1String("Profile") ).toStringList();
    
    m_fontSettings.windowFont = qVariantValue<QFont>(settings.value(key + QLatin1String("windowfont"), qApp->font()));
    m_fontSettings.browserFont = qVariantValue<QFont>(settings.value(key + QLatin1String("browserfont"), qApp->font()));
    m_fontSettings.useWindowFont = settings.value(key + QLatin1String("usewindowfont"), false).toBool();
    m_fontSettings.useBrowserFont = settings.value(key + QLatin1String("usebrowserfont"), false).toBool();
    m_fontSettings.windowWritingSystem = static_cast<QFontDatabase::WritingSystem>(
        settings.value(key + QLatin1String("windowwritingsystem"), QFontDatabase::Latin).toInt());
    m_fontSettings.browserWritingSystem = static_cast<QFontDatabase::WritingSystem>(
        settings.value(key + QLatin1String("browserwritingsystem"), QFontDatabase::Latin).toInt());

    m_fontSettings.browserFont.setPointSizeF(pointFntSize);
}

void Config::save()
{
    saveSettings();
    saveProfile( profil );
}

void Config::saveSettings()
{
    const QString key = getVersionString() + QLatin1String("/");

    const QString pKey = (profil->props[QLatin1String("name")] == QLatin1String("default"))
        ? QString::fromLatin1(QT_VERSION_STR)
        : getVersionString();

    const QString profkey = pKey + QLatin1String("/Profile/") + profil->props[QLatin1String("name")] + QLatin1String("/");

    QSettings settings;

    settings.setValue( profkey + QLatin1String("Source"), src );
    settings.setValue( key + QLatin1String("SideBarPage"), sideBarPage() );
    if (qApp->type() != QApplication::Tty)
        settings.setValue(key + QLatin1String("windowGeometry"), winGeometry);

    settings.setValue( key + QLatin1String("MainWindowState"), mainWinState );
    settings.setValue( key + QLatin1String("FontSize"), pointFntSize);
    settings.setValue( key + QLatin1String("RebuildDocDB"), rebuildDocs );

    settings.setValue(key + QLatin1String("windowfont"), m_fontSettings.windowFont);
    settings.setValue(key + QLatin1String("browserfont"), m_fontSettings.browserFont);
    settings.setValue(key + QLatin1String("usewindowfont"), m_fontSettings.useWindowFont);
    settings.setValue(key + QLatin1String("usebrowserfont"), m_fontSettings.useBrowserFont);
    settings.setValue(key + QLatin1String("windowwritingsystem"), m_fontSettings.windowWritingSystem);
    settings.setValue(key + QLatin1String("browserwritingsystem"), m_fontSettings.browserWritingSystem);
}

#ifdef ASSISTANT_DEBUG
static void dumpmap( const QMap<QString,QString> &m, const QString &header )
{
    qDebug( header );
    QMap<QString,QString>::ConstIterator it = m.begin();
    while (it != m.end()) {
        qDebug( "  " + it.key() + ":\t\t" + *it );
        ++it;
    }
}
#endif

bool Config::defaultProfileExists()
{
    QSettings settings;
    const QString profKey = QLatin1String(QT_VERSION_STR) + QLatin1String("/Profile/default/");

    if (settings.contains(profKey + QLatin1String("DocFiles"))
        && settings.contains(profKey + QLatin1String("Titles"))
        && settings.contains(profKey + QLatin1String("ImageDirs"))) {
        QStringList dcfs = settings.value(profKey + QLatin1String("DocFiles") ).toStringList();
        foreach (QString file, dcfs) {
            if (file == Profile::storableFilePath(file))
                return true;
        }
    }
    return false;
}

void Config::loadDefaultProfile()
{
    QSettings settings;
    const QString profKey = QLatin1String(QT_VERSION_STR) + QLatin1String("/Profile/default/");

    if (!defaultProfileExists())
        return;

    // Override the defaults with settings in registry.
    profil->icons.clear();
    profil->indexPages.clear();
    profil->imageDirs.clear();
    profil->docs.clear();
    profil->dcfTitles.clear();

    QStringList titles = settings.value( profKey + QLatin1String("Titles") ).toStringList();
    QStringList iconLst = settings.value( profKey + QLatin1String("DocIcons") ).toStringList();
    QStringList indexLst = settings.value( profKey + QLatin1String("IndexPages") ).toStringList();
    QStringList imgDirLst = settings.value( profKey + QLatin1String("ImageDirs") ).toStringList();
    QStringList dcfs = settings.value( profKey + QLatin1String("DocFiles") ).toStringList();
    profil->props[QLatin1String("name")] = QLatin1String("default");

    QString filePath;
    QStringList::ConstIterator it = titles.constBegin();
    QStringList::ConstIterator iconIt = iconLst.constBegin();
    QStringList::ConstIterator indexIt = indexLst.constBegin();
    QStringList::ConstIterator imageIt = imgDirLst.constBegin();
    QStringList::ConstIterator dcfIt = dcfs.constBegin();
    while((it != titles.constEnd())
          && (iconIt != iconLst.constEnd())
          && (indexIt != indexLst.constEnd())
          && (imageIt != imgDirLst.constEnd())
          && (dcfIt != dcfs.constEnd())) {
        profil->addDCFIcon( *it, *iconIt );
        profil->addDCFIndexPage(*it, Profile::loadableFilePath(*indexIt));
        profil->addDCFImageDir( *it, *imageIt );
        profil->addDCFTitle(Profile::loadableFilePath(*dcfIt), *it);
        ++it, ++iconIt, ++indexIt, ++imageIt, ++dcfIt;
    }
#if ASSISTANT_DEBUG
    dumpmap( profil->icons, QLatin1String("Icons") );
    dumpmap( profil->indexPages, QLatin1String("IndexPages") );
    dumpmap( profil->imageDirs, QLatin1String("ImageDirs") );
    dumpmap( profil->dcfTitles, QLatin1String("dcfTitles") );
    qDebug( "Docfiles: \n  " + profil->docs.join( "\n  " ) );
#endif
}

void Config::saveProfile( Profile *profile )
{
    if (profil->profileType() == Profile::UserProfile)
        return;

    const QString key = (profile->props[QLatin1String("name")] == QLatin1String("default"))
        ? QString::fromLatin1(QT_VERSION_STR)
        : getVersionString();

    const QString profKey = key + QLatin1String("/Profile/") + profile->props[QLatin1String("name")] + QLatin1String("/");

    QString path = QLibraryInfo::location(QLibraryInfo::DocumentationPath).replace(QLatin1String("\\"), QLatin1String("/"));
    QStringList indexes, icons, imgDirs, dcfs;
    QStringList titles = profile->dcfTitles.keys();
    QStringList::ConstIterator it = titles.constBegin();
    QString filePath;
    for ( ; it != titles.constEnd(); ++it ) {

        indexes << Profile::storableFilePath(profile->indexPages[*it]);
        icons << profile->icons[*it];
        imgDirs << profile->imageDirs[*it];
        dcfs << Profile::storableFilePath(profile->dcfTitles[*it]);
    }

    QSettings settings;
    settings.setValue( profKey + QLatin1String("Titles"), titles );
    settings.setValue( profKey + QLatin1String("DocFiles"), dcfs );
    settings.setValue( profKey + QLatin1String("IndexPages"), indexes );
    settings.setValue( profKey + QLatin1String("DocIcons"), icons );
    settings.setValue( profKey + QLatin1String("ImageDirs"), imgDirs );

#if ASSISTANT_DEBUG
    qDebug() << "Titles:\n  - " << ((QStringList*)&titles)->join("\n  - ");
    qDebug() << "Docfiles:\n  - " << dcfs.join("\n  - " );
    qDebug() << "IndexPages:\n  - " << indexes.join("\n  - ");
    qDebug() << "DocIcons:\n  - " << icons.join("\n  - " );
    qDebug() << "ImageDirs:\n  - " << imgDirs.join("\n  - " );
#endif
}

QStringList Config::mimePaths()
{
    static QStringList lst;

    if( lst.count() > 0 )
        return lst;

    for (QMap<QString,QString>::ConstIterator it = profil->dcfTitles.constBegin();
         it != profil->dcfTitles.constEnd(); ++it ) {

        // Mime source for .dcf file path
        QFileInfo info( *it );
        QString dcfPath = info.absolutePath();
        if (!lst.contains(dcfPath))
            lst << dcfPath;

        // Image dir for .dcf
        QString imgDir = QDir::toNativeSeparators( dcfPath + QDir::separator()
                                                  + profil->imageDirs[it.key()] );
        if (!lst.contains(imgDir))
            lst << imgDir;
    }
    return lst;
}

QStringList Config::profiles() const
{
    return profileNames;
}

QString Config::title() const
{
    QString s = profil->props[QLatin1String("title")];
    if (s.isEmpty())
        s = QObject::tr("Qt Assistant by Nokia");
    return s;
}

QString Config::aboutApplicationMenuText() const
{
    return profil->props[QLatin1String("aboutmenutext")];
}

QString Config::aboutURL() const
{
    return profil->props[QLatin1String("abouturl")];
}

QString Config::homePage() const
{
    return home.isEmpty() ? profil->props[QLatin1String("startpage")] : home;
}

QStringList Config::source() const
{
    return src.size() == 0 ? QStringList(profil->props[QLatin1String("startpage")]) : src;
}

QStringList Config::docFiles() const
{
    return profil->docs;
}

QPixmap Config::docIcon( const QString &title ) const
{
    // ### To allow qdoc generated dcf files to reference the doc icons from qmake_image_col
    QString name = profil->icons[title];
    QString resName = QLatin1String(":/trolltech/assistant/images/") + name;

    if (QFile::exists(resName))
        return QPixmap(resName);

    if (name.startsWith(QLatin1String("file:")))
        name = name.mid(5);
    return QPixmap(name);
}

QPixmap Config::applicationIcon() const
{
    QString name = profil->props[QLatin1String("applicationicon")];
    QString resName = QLatin1String(":/trolltech/assistant/images/") + name;

    if (QFile::exists(resName))
        return QPixmap(resName);

    if (name.startsWith(QLatin1String("file:")))
        name = name.mid(5);
    return QPixmap(name);
}

QStringList Config::docTitles() const
{
    return QStringList(profil->indexPages.keys());
}

QString Config::docImageDir( const QString &docfile ) const
{
    return profil->imageDirs[docfile];
}

QString Config::indexPage( const QString &title ) const
{
    return profil->indexPages[title];
}

void Config::hideSideBar( bool b )
{
    hideSidebar = b;
}

bool Config::sideBarHidden() const
{
    return hideSidebar;
}

QString Config::assistantDocPath() const
{
    return profil->props[QLatin1String("assistantdocs")].isEmpty()
        ? QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/html")
        : profil->props[QLatin1String("assistantdocs")];
}

QT_END_NAMESPACE
