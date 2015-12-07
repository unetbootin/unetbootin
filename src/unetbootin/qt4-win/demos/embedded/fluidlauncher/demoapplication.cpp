/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include <QDebug>
#include <QDir>
 
#include "demoapplication.h"



DemoApplication::DemoApplication(QString executableName, QString caption, QString imageName, QStringList args)
{
    imagePath = imageName;
    appCaption = caption;

    if (executableName[0] == QLatin1Char('/'))
        executablePath = executableName;
    else
        executablePath = QDir::cleanPath(QDir::currentPath() + QLatin1Char('/') + executableName);
  
    arguments = args;

    process.setProcessChannelMode(QProcess::ForwardedChannels);

    QObject::connect( &process, SIGNAL(finished(int, QProcess::ExitStatus)), 
                      this, SLOT(processFinished(int, QProcess::ExitStatus)));

    QObject::connect( &process, SIGNAL(error(QProcess::ProcessError)), 
                      this, SLOT(processError(QProcess::ProcessError)));

    QObject::connect( &process, SIGNAL(started()), this, SLOT(processStarted()));
}


void DemoApplication::launch()
{
    process.start(executablePath, arguments);
}

QImage* DemoApplication::getImage()
{
    return new QImage(imagePath);
}

QString DemoApplication::getCaption()
{
    return appCaption;
}

void DemoApplication::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    emit demoFinished();

    QObject::disconnect(this, SIGNAL(demoStarted()), 0, 0);
    QObject::disconnect(this, SIGNAL(demoFinished()), 0, 0);
}

void DemoApplication::processError(QProcess::ProcessError err)
{
    qDebug() << "Process error: " << err;
    if (err == QProcess::Crashed)
        emit demoFinished();
}


void DemoApplication::processStarted()
{
    emit demoStarted();
}






