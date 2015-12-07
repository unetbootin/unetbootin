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
#include <QString>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include <QProcess>
#include <QDir>
#include <QRegExp>
#include <QSet>
#include <QDirIterator>
#include "shared.h"

using std::cout;
using std::endl;

bool operator==(const FrameworkInfo &a, const FrameworkInfo &b)
{
    return ((a.frameworkPath == b.frameworkPath) && (a.binaryPath == b.binaryPath));
}

QDebug operator<<(QDebug debug, const FrameworkInfo &info)
{
    debug << "Framework directory" << info.frameworkDirectory << "\n";
    debug << "Framework name" << info.frameworkName << "\n";
    debug << "Framework path" << info.frameworkPath << "\n";
    debug << "Binary directory" << info.binaryDirectory << "\n";
    debug << "Binary name" << info.binaryName << "\n";
    debug << "Binary path" << info.binaryPath << "\n";
    debug << "Version" << info.version << "\n";
    debug << "Install name" << info.installName << "\n";
    debug << "Deployed install name" << info.deployedInstallName << "\n";
    debug << "Source file Path" << info.sourceFilePath << "\n";
    debug << "Deployed Directtory (relative to bundle)" << info.destinationDirectory << "\n";
    
    return debug;
}

const QString bundleFrameworkDirectory = "Contents/Frameworks";
const QString bundleBinaryDirectory = "Contents/MacOS";

inline QDebug operator<<(QDebug debug, const ApplicationBundleInfo &info)
{
    debug << "Application bundle path" << info.path << "\n";
    debug << "Binary path" << info.binaryPath << "\n";
    return debug;
}

bool copyFilePrintStatus(const QString &from, const QString &to)
{
    if (QFile::copy(from, to)) {
        qDebug() << "copied" << from << "to" << to;
        return true;
    } else {
        qDebug() << "ERROR: file copy failed from" << from << "to" << to;
        return false;
    }
}


FrameworkInfo parseOtoolLibraryLine(const QString &line)
{
    FrameworkInfo info;
    QString trimmed = line.trimmed();

    if (trimmed.isEmpty())
        return info;
        
    // Don't deploy system libraries.
    if (trimmed.startsWith("/System/Library/") ||
        (trimmed.startsWith("/usr/lib/") && trimmed.contains("libQt") == false) // exception for libQtuitools and libQtlucene
        || trimmed.startsWith("@executable_path"))
        return info;
        
    enum State {QtPath, FrameworkName, DylibName, Version, End};
    State state = QtPath;
    int part = 0;
    QString name;
    QString qtPath;

    // Split the line into [Qt-path]/lib/qt[Module].framework/Versions/[Version]/
    QStringList parts = trimmed.split("/");
    while (part < parts.count()) {
        const QString currentPart = parts.at(part).simplified() ;
//        qDebug() << "currentPart" << currentPart;
        ++part;
        if (currentPart == "")
            continue;

        if (state == QtPath) {
            // Check for library name part
            if (part < parts.count() && parts.at(part).contains(".dylib ")) {
                state = DylibName;
                info.installName += "/" + (qtPath + "lib/").simplified();
                info.frameworkDirectory = info.installName;
                state = DylibName;
                continue;
            } else if (part < parts.count() && parts.at(part).endsWith(".framework")) {
                info.installName += "/" + (qtPath + "lib/").simplified();
                info.frameworkDirectory = info.installName;
                state = FrameworkName;
                continue;
            } else if (trimmed.startsWith("/") == false) {      // If the line does not contain a full path, the app is using a binary Qt package.
                if (currentPart.contains(".framework")) {
                    info.frameworkDirectory = "/Library/Frameworks/";
                    state = FrameworkName;
                } else {
                    info.frameworkDirectory = "/usr/lib/";
                    state = DylibName;
                }
                
                --part;
                continue;
            }
            qtPath += (currentPart + "/");
        
        } if (state == FrameworkName) { 
            // remove ".framework"
            name = currentPart;
            name.chop(QString(".framework").length());
            info.frameworkName = currentPart;
            state = Version;
            ++part;
            continue;
        } if (state == DylibName) {
            name =  currentPart.split(" (compatibility").at(0);
            info.frameworkName = name;
            info.installName += info.frameworkName;
            info.deployedInstallName = "@executable_path/../Frameworks/" + info.frameworkName;
            info.binaryName = name;
            info.frameworkPath = info.frameworkDirectory + info.frameworkName;
            info.sourceFilePath = info.frameworkPath;
            info.destinationDirectory = bundleFrameworkDirectory + "/";
            state = End;
            ++part;
            continue;
        } else if (state == Version) {
            info.version = currentPart;
            info.binaryDirectory = "Versions/" + info.version; 
            info.binaryName = name;
            info.binaryPath = "/" + info.binaryDirectory + "/" + info.binaryName;
            info.installName += info.frameworkName + info.binaryPath;
            info.deployedInstallName = "@executable_path/../Frameworks/" + info.frameworkName + info.binaryPath;
            info.frameworkPath = info.frameworkDirectory + info.frameworkName;
            info.sourceFilePath = info.frameworkPath + info.binaryPath;
            info.destinationDirectory = bundleFrameworkDirectory + "/" + info.frameworkName + "/" + info.binaryDirectory;

            state = End;
        } else if (state == End) {
            break;
        }
    }

    return info;
}

QString findAppBinary(const QString &appBundlePath)
{
    QString appName = QFileInfo(appBundlePath).completeBaseName();
    QString binaryPath = appBundlePath  + "/Contents/MacOS/" + appName;
    
    if (QFile::exists(binaryPath))
        return binaryPath;
    qDebug() << "Error: Could not find bundle binary for" << appBundlePath;
    return QString();
}

QList<FrameworkInfo> getQtFrameworks(const QStringList &otoolLines)
{
    QList<FrameworkInfo> libraries; 
    foreach(const QString line, otoolLines) {
        FrameworkInfo info = parseOtoolLibraryLine(line);
        if (info.frameworkName.isEmpty() == false) {
            libraries.append(info);
        }
    }
    return libraries;
}

QList<FrameworkInfo> getQtFrameworks(const QString &path)
{
    QProcess otool;
    otool.start("otool", QStringList() << "-L" << path);
    otool.waitForFinished();
    
    if (otool.exitCode() != 0) {
        qDebug() << otool.readAllStandardError();
    }
    
    QString output = otool.readAllStandardOutput();
    QStringList outputLines = output.split("\n");
    outputLines.removeFirst(); // remove line containing the binary path
    if (path.contains(".framework") || path.contains(".dylib"))
        outputLines.removeFirst(); // frameworks and dylibs lists themselves as a dependency.

    return getQtFrameworks(outputLines);
}

// copies everything _inside_ sourcePath to destinationPath
void recursiveCopy(const QString &sourcePath, const QString &destinationPath)
{
    QDir().mkpath(destinationPath);

    QStringList files = QDir(sourcePath).entryList(QStringList() << "*", QDir::Files | QDir::NoDotAndDotDot);
    foreach (QString file, files) {
        const QString fileSourcePath = sourcePath + "/" + file;
        const QString fileDestinationPath = destinationPath + "/" + file;
        copyFilePrintStatus(fileSourcePath, fileDestinationPath);
    }

    QStringList subdirs = QDir(sourcePath).entryList(QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QString dir, subdirs) {
        recursiveCopy(sourcePath + "/" + dir, destinationPath + "/" + dir);
    }
}

QString copyFramework(const FrameworkInfo &framework, const QString path)
{
    const QString from = framework.sourceFilePath;
    const QString toDir = path + "/" + framework.destinationDirectory;
    const QString to = toDir + "/" + framework.binaryName;

    if (QFile::exists(from) == false) {
        qDebug() << "ERROR: no file at" << from;
        return QString();
    }


    QDir dir;
    if (dir.mkpath(toDir) == false) {
        qDebug() << "ERROR: could not create destination directory" << to;
        return QString();
    }
    

    if (QFile::exists(to)) {
//        qDebug() << framework.frameworkName << "already deployed, skip";
        return QString();
    }
    
    copyFilePrintStatus(from, to);

    const QString resourcesSourcePath = framework.frameworkPath + "/Resources"; 
    const QString resourcesDestianationPath = path + "/Contents/Frameworks/" + framework.frameworkName + "/Resources"; 
    recursiveCopy(resourcesSourcePath, resourcesDestianationPath);

    return to;
}

void runInstallNameTool(QStringList options)
{
    QProcess installNametool;
    installNametool.start("install_name_tool", options);
    installNametool.waitForFinished();
    if (installNametool.exitCode() != 0) {
        qDebug() << installNametool.readAllStandardError();
        qDebug() << installNametool.readAllStandardOutput();
    }
}

void changeIdentification(const QString &id, const QString &binaryPath)
{
//    qDebug() << "change identification on" << binaryPath << id;
    runInstallNameTool(QStringList() << "-id" << id << binaryPath);
}

void changeInstallName(const QString &oldName, const QString &newName, const QString &binaryPath)
{
//    qDebug() << "change install name on" << binaryPath << oldName << newName;
    runInstallNameTool(QStringList() << "-change" << oldName << newName << binaryPath);
}



/*
    Deploys the the listed frameworks listed into an app bundle.
    The frameworks are searched for dependencies, which are also deployed.
    (deploying Qt3Support will also deploy QtNetwork and QtSql for example.)
    Returns a DeploymentInfo structure containing the Qt path used and a
    a list of actually deployed frameworks.
*/
DeploymentInfo deployQtFrameworks(QList<FrameworkInfo> frameworks, const QString &bundlePath, const QString &binaryPath)
{
    QStringList copiedFrameworks;
    DeploymentInfo deploymenInfo;
    
    while (frameworks.isEmpty() == false) {
        const FrameworkInfo framework = frameworks.takeFirst();
        copiedFrameworks.append(framework.frameworkName);
        
        // Get the qt path from one of the Qt frameworks;
        if (deploymenInfo.qtPath == QString() && framework.frameworkName.contains("Qt") 
            && framework.frameworkDirectory.contains("/lib"))
        {
                deploymenInfo.qtPath = framework.frameworkDirectory;
                deploymenInfo.qtPath.chop(5); // remove "/lib/"
        }

//        qDebug() << "";
//        qDebug() << "deploy" << framework.frameworkName;

        if (framework.installName.startsWith("/@executable_path/")) {
            qDebug()  << framework.frameworkName << "already deployed, skipping.";
            continue;
        }
        
        // Install_name_tool the new id into the binary
        changeInstallName(framework.installName, framework.deployedInstallName, binaryPath);

        // Copy farmework to app bundle.
        const QString deployedBinaryPath = copyFramework(framework, bundlePath);
        // Skip the rest if already was deployed.
        if (deployedBinaryPath == QString())
            continue;
        
        // Install_name_tool it a new id.
        changeIdentification(framework.deployedInstallName, deployedBinaryPath);
        // Check for framework dependencies
        QList<FrameworkInfo> dependencies = getQtFrameworks(deployedBinaryPath);

        foreach (FrameworkInfo dependency, dependencies) {
//            qDebug() << "dependent framework" << dependency.installName << deployedBinaryPath;
            changeInstallName(dependency.installName, dependency.deployedInstallName, deployedBinaryPath);
            
            // Deploy framework if neccesary.
            if (copiedFrameworks.contains(dependency.frameworkName) == false && frameworks.contains(dependency) == false) {
                frameworks.append(dependency);
            }
        }
    }
    deploymenInfo.deployedFrameworks = copiedFrameworks;
    return deploymenInfo;
}

DeploymentInfo deployQtFrameworks(const QString &appBundlePath)
{
   ApplicationBundleInfo applicationBundle;
   applicationBundle.path = appBundlePath;
   applicationBundle.binaryPath = findAppBinary(appBundlePath);
   return deployQtFrameworks(getQtFrameworks(applicationBundle.binaryPath), applicationBundle.path, applicationBundle.binaryPath);
}

void deployPlugins(const ApplicationBundleInfo &appBundleInfo, const QString &pluginSourcePath, const QString pluginDestinationPath, DeploymentInfo deploymentInfo)
{
    QStringList plugins = QDir(pluginSourcePath).entryList(QStringList() << "*.dylib");

    if (plugins.isEmpty() == false) {
        QDir dir;
        dir.mkpath(pluginDestinationPath);
    }

    foreach (QString pluginName, plugins) {
        
        // Skip some Qt plugins based on what frameworks were deployed:
        //qDebug() << pluginSourcePath << deploymentInfo.pluginPath;
        
        if (pluginSourcePath.contains(deploymentInfo.pluginPath)) {
            QStringList deployedFrameworks = deploymentInfo.deployedFrameworks;

            // Skip the debug versions of the plugins
            if (pluginName.endsWith("_debug.dylib"))
                continue;

            // Skip the designer plugins
            if (pluginSourcePath.contains("plugins/designer"))
                continue;

#ifndef QT_GRAPHICSSYSTEM_OPENGL
            // SKip the opengl graphicssystem plugin when not in use.
            if (pluginName.contains("libqglgraphicssystem"))
                continue;
#endif            
            // Deploy accessibility for Qt3Support only if the Qt3Support.framework is in use
            if (deployedFrameworks.indexOf("Qt3Support.framework") == -1 && pluginName.contains("accessiblecompatwidgets"))
                continue;

            // Deploy the svg icon plugin if QtSVG.framework is in use.
            if (deployedFrameworks.indexOf("QtSVG.framework") == -1 && pluginName.contains("svg"))
                continue;

            // Deploy the phonon plugins if phonon.framework is in use
            if (deployedFrameworks.indexOf("phonon.framework") == -1 && pluginName.contains("phonon"))
                continue;

            // Deploy the sql plugins if QtSql.framework is in use
            if (deployedFrameworks.indexOf("QtSql.framework") == -1 && pluginName.contains("sql"))
                continue;

            // Deploy the script plugins if QtScript.framework is in use
            if (deployedFrameworks.indexOf("QtScript.framework") == -1 && pluginName.contains("script"))
                continue;
        }

        const QString sourcePath = pluginSourcePath + "/" + pluginName;
        const QString destinationPath = pluginDestinationPath + "/" + pluginName;
        if (copyFilePrintStatus(sourcePath, destinationPath)) {
        
        // Special case for the phonon plugin: CoreVideo is not available as a separate framework
        // on panther, link against the QuartzCore framework instead. (QuartzCore contians CoreVideo.)
        if (pluginName.contains("libphonon_qt7")) {
            changeInstallName("/System/Library/Frameworks/CoreVideo.framework/Versions/A/CoreVideo",
                              "/System/Library/Frameworks/QuartzCore.framework/Versions/A/QuartzCore",
                                destinationPath);
        }

//        qDebug() << "deploy plugin depedencies:";
            QList<FrameworkInfo> frameworks = getQtFrameworks(destinationPath);
//            qDebug() << frameworks;
            deployQtFrameworks(frameworks, appBundleInfo.path, destinationPath);
//        qDebug() << "deploy plugin depedencies done";
        }
    } // foreach plugins

    QStringList subdirs = QDir(pluginSourcePath).entryList(QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (const QString &subdir, subdirs)
        deployPlugins(appBundleInfo, pluginSourcePath + "/" + subdir, pluginDestinationPath + "/" + subdir, deploymentInfo);
}

void createQtConf(const QString &appBundlePath)
{
    QByteArray contents = "[Paths]\nPlugins = plugins\n";
    QString filePath = appBundlePath + "/Contents/Resources/";
    QString fileName = filePath + "qt.conf";

    QDir().mkpath(filePath);
    
    QFile qtconf(fileName);
    if (qtconf.exists()) {
        qDebug() << "";
        qDebug() << "Warning:" << fileName << "already exists, will not overwrite.";
        qDebug() << "To make sure the plugins are loaded from the correct location,";
        qDebug() << "please make sure qt.conf contains the following lines:";
        qDebug() << contents;
        qDebug() << "";
        return;
    }

    qtconf.open(QIODevice::WriteOnly);
    if (qtconf.write(contents) != -1) {
        qDebug() << "";
        qDebug() << "Created configuration file:" << fileName;
        qDebug() << "This file sets the plugin search path to" << appBundlePath + "/Contents/plugins";
        qDebug() << "";
    }
}

void deployPlugins(const QString &appBundlePath, DeploymentInfo deploymentInfo)
{
    ApplicationBundleInfo applicationBundle;
    applicationBundle.path = appBundlePath;
    applicationBundle.binaryPath = findAppBinary(appBundlePath);
   
    const QString pluginDestinationPath = appBundlePath + "/" + "Contents/plugins";
    
//    qDebug() << "";
//    qDebug() << "recursively copying plugins from" << deploymentInfo.pluginPath << "to" << pluginDestinationPath;
    deployPlugins(applicationBundle, deploymentInfo.pluginPath, pluginDestinationPath, deploymentInfo);
}


void changeQtFrameworks(const QList<FrameworkInfo> frameworks, const QString &appBinaryPath, const QString &absoluteQtPath)
{
    qDebug() << "Changing" << appBinaryPath << "to link against Qt in" << absoluteQtPath;
    QString finalQtPath = absoluteQtPath;

    if (absoluteQtPath.startsWith("/Library/Frameworks") == false)
        finalQtPath += "/lib/";

    foreach (FrameworkInfo framework, frameworks) {
        const QString oldBinaryId = framework.installName;
        const QString newBinaryId = finalQtPath + framework.frameworkName +  framework.binaryPath;
        qDebug() << "Changing" << oldBinaryId << "to" << newBinaryId;
        changeInstallName(oldBinaryId, newBinaryId, appBinaryPath);
    }
}

void changeQtFrameworks(const QString appPath, const QString &qtPath)
{
    const QString appBinaryPath = findAppBinary(appPath);
    const QList<FrameworkInfo> qtFrameworks = getQtFrameworks(appBinaryPath);
    const QString absoluteQtPath = QDir(qtPath).absolutePath();
    changeQtFrameworks(qtFrameworks, appBinaryPath, absoluteQtPath);
}


void createDiskImage(const QString &appBundlePath)
{
    QString appBaseName = appBundlePath;
    appBaseName.chop(4); // remove ".app" from end
    
    QString dmgName = appBaseName + ".dmg";

    QFile dmg(dmgName);

    if (dmg.exists()) {
        qDebug() << "Disk image already exists, skipping .dmg creation for" << dmg.fileName();
    } else {
        qDebug() << "Creating disk image (.dmg) for" << appBundlePath;
    }

    // More dmg options can be found in the hdiutil man page.
    QString options = QString("create %1.dmg -srcfolder %1.app -format UDZO -volname %1").arg(appBaseName);

    QProcess hdutil;
    hdutil.start("hdiutil", options.split(' '));
    hdutil.waitForFinished(-1);
}
