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
#ifndef MAC_DEPLOMYMENT_SHARED_H
#define MAC_DEPLOMYMENT_SHARED_H

#include <QString>
#include <QStringList>
#include <QDebug>

class FrameworkInfo
{
public:
    QString frameworkDirectory;
    QString frameworkName;
    QString frameworkPath;
    QString binaryDirectory;
    QString binaryName;
    QString binaryPath;
    QString version;
    QString installName;
    QString deployedInstallName;
    QString sourceFilePath;
    QString destinationDirectory;
};

bool operator==(const FrameworkInfo &a, const FrameworkInfo &b);
QDebug operator<<(QDebug debug, const FrameworkInfo &info);

class ApplicationBundleInfo
{
public:    
    QString path;
    QString binaryPath;
};

class DeploymentInfo
{
public:
    QString qtPath;
    QString pluginPath;
    QStringList deployedFrameworks;
};


inline QDebug operator<<(QDebug debug, const ApplicationBundleInfo &info);

void changeQtFrameworks(const QString appPath, const QString &qtPath);
void changeQtFrameworks(const QList<FrameworkInfo> frameworks, const QString &appBinaryPath, const QString &qtPath);

FrameworkInfo parseOtoolLibraryLine(const QString &line);
QString findAppBinary(const QString &appBundlePath);
QList<FrameworkInfo> getQtFrameworks(const QString &path);
QList<FrameworkInfo> getQtFrameworks(const QStringList &otoolLines);
QString copyFramework(const FrameworkInfo &framework, const QString path);
DeploymentInfo deployQtFrameworks(const QString &appBundlePath);
DeploymentInfo deployQtFrameworks(QList<FrameworkInfo> frameworks, const QString &bundlePath, const QString &binaryPath);
void createQtConf(const QString &appBundlePath);
void deployPlugins(const QString &appBundlePath, DeploymentInfo deploymentInfo);
void changeIdentification(const QString &id, const QString &binaryPath);
void changeInstallName(const QString &oldName, const QString &newName, const QString &binaryPath);
QString findAppBinary(const QString &appBundlePath);
void createDiskImage(const QString &appBundlePath);

#endif
