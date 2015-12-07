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

#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QDateTime>
#include <QtHelp/QHelpEngineCore>
#include "qtdocinstaller.h"

QT_BEGIN_NAMESPACE

QtDocInstaller::QtDocInstaller(const QString &collectionFile)
{
    m_abort = false;
    m_collectionFile = collectionFile;
}

QtDocInstaller::~QtDocInstaller()
{
    if (!isRunning())
        return;
    m_mutex.lock();
    m_abort = true;
    m_mutex.unlock();
    wait();
}

void QtDocInstaller::installDocs()
{
    start(LowPriority);
}

void QtDocInstaller::run()
{
    QHelpEngineCore *helpEngine = new QHelpEngineCore(m_collectionFile);
    helpEngine->setupData();
    bool changes = false;

    QStringList docs;
    docs << QLatin1String("assistant")
        << QLatin1String("designer")
        << QLatin1String("linguist")
        << QLatin1String("qmake")
        << QLatin1String("qt");

    foreach (QString doc, docs) {
        changes |= installDoc(doc, helpEngine);
        m_mutex.lock();
        if (m_abort) {
            m_mutex.unlock();
            return;
        }
        m_mutex.unlock();
    }
    emit docsInstalled(changes);
}

bool QtDocInstaller::installDoc(const QString &name, QHelpEngineCore *helpEngine)
{
    QString versionKey = QString(QLatin1String("qtVersion%1$$$%2")).
        arg(QLatin1String(QT_VERSION_STR)).arg(name);

    QString info = helpEngine->customValue(versionKey, QString()).toString();
    QStringList lst = info.split(QLatin1String("|"));

    QDateTime dt;
    if (lst.count() && !lst.first().isEmpty())
        dt = QDateTime::fromString(lst.first(), Qt::ISODate);

    QString qchFile;
    if (lst.count() == 2)
        qchFile = lst.last();
    
    QDir dir(QLibraryInfo::location(QLibraryInfo::DocumentationPath)
        + QDir::separator() + QLatin1String("qch"));
    
    QStringList files = dir.entryList(QStringList() << QLatin1String("*.qch"));
    if (files.isEmpty()) {
        helpEngine->setCustomValue(versionKey, QDateTime().toString(Qt::ISODate)
            + QLatin1String("|"));
        return false;
    }
    foreach (QString f, files) {
        if (f.startsWith(name)) {
            QFileInfo fi(dir.absolutePath() + QDir::separator() + f);
            if (dt.isValid() && fi.lastModified().toString(Qt::ISODate) == dt.toString(Qt::ISODate)
                && qchFile == fi.absoluteFilePath())
                return false;

            QString namespaceName = QHelpEngineCore::namespaceName(fi.absoluteFilePath());
            if (namespaceName.isEmpty())
                continue;

            if (helpEngine->registeredDocumentations().contains(namespaceName))
                helpEngine->unregisterDocumentation(namespaceName);

            if (!helpEngine->registerDocumentation(fi.absoluteFilePath())) {
                emit errorMessage(
                    tr("The file %1 could not be registered successfully!\n\nReason: %2")
                    .arg(fi.absoluteFilePath()).arg(helpEngine->error()));
            }

            helpEngine->setCustomValue(versionKey, fi.lastModified().toString(Qt::ISODate)
                + QLatin1String("|") + fi.absoluteFilePath());
            return true;
        }
    }
    return false;
}

QT_END_NAMESPACE
