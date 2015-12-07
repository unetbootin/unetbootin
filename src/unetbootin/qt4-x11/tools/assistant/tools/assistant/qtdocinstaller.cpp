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

    foreach (const QString &doc, docs) {
        changes |= installDoc(doc, helpEngine);
        m_mutex.lock();
        if (m_abort) {
            delete helpEngine;
            m_mutex.unlock();
            return;
        }
        m_mutex.unlock();
    }
    delete helpEngine;
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

    const QStringList files = dir.entryList(QStringList() << QLatin1String("*.qch"));
    if (files.isEmpty()) {
        helpEngine->setCustomValue(versionKey, QDateTime().toString(Qt::ISODate)
            + QLatin1String("|"));
        return false;
    }
    foreach (const QString &f, files) {
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
