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

#ifndef QHELPENGINECORE_H
#define QHELPENGINECORE_H

#include <QtHelp/qhelp_global.h>

#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QVariant>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Help)

class QHelpEngineCorePrivate;

class QHELP_EXPORT QHelpEngineCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoSaveFilter READ autoSaveFilter WRITE setAutoSaveFilter)
    Q_PROPERTY(QString collectionFile READ collectionFile WRITE setCollectionFile)
    Q_PROPERTY(QString currentFilter READ currentFilter WRITE setCurrentFilter)
    
public:
    QHelpEngineCore(const QString &collectionFile, QObject *parent = 0);
    virtual ~QHelpEngineCore();

    bool setupData();

    QString collectionFile() const;
    void setCollectionFile(const QString &fileName);

    bool copyCollectionFile(const QString &fileName);

    static QString namespaceName(const QString &documentationFileName);
    bool registerDocumentation(const QString &documentationFileName);
    bool unregisterDocumentation(const QString &namespaceName);
    QString documentationFileName(const QString &namespaceName);

    QStringList customFilters() const;
    bool removeCustomFilter(const QString &filterName);
    bool addCustomFilter(const QString &filterName,
        const QStringList &attributes);

    QStringList filterAttributes() const;
    QStringList filterAttributes(const QString &filterName) const;

    QString currentFilter() const;
    void setCurrentFilter(const QString &filterName);

    QStringList registeredDocumentations() const;
    QList<QStringList> filterAttributeSets(const QString &namespaceName) const;
    QList<QUrl> files(const QString namespaceName,
        const QStringList &filterAttributes,
        const QString &extensionFilter = QString());
    QUrl findFile(const QUrl &url) const;
    QByteArray fileData(const QUrl &url) const;

    QMap<QString, QUrl> linksForIdentifier(const QString &id) const;

    bool removeCustomValue(const QString &key);
    QVariant customValue(const QString &key,
        const QVariant &defaultValue = QVariant()) const;
    bool setCustomValue(const QString &key, const QVariant &value);

    static QVariant metaData(const QString &documentationFileName,
        const QString &name);

    QString error() const;

    void setAutoSaveFilter(bool save);
    bool autoSaveFilter() const;

Q_SIGNALS:
    void setupStarted();
    void setupFinished();
    void currentFilterChanged(const QString &newFilter);
    void warning(const QString &msg);

protected:
    QHelpEngineCore(QHelpEngineCorePrivate *helpEngineCorePrivate,
        QObject *parent);

private:
    QHelpEngineCorePrivate *d;
    friend class QHelpEngineCorePrivate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QHELPENGINECORE_H
