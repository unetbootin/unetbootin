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

#ifndef QHELPDBREADER_H
#define QHELPDBREADER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QByteArray>
#include <QtCore/QSet>

QT_BEGIN_NAMESPACE

class QSqlQuery;

class QHelpDBReader : public QObject
{
    Q_OBJECT

public:
    QHelpDBReader(const QString &dbName, const QString &uniqueId,
        QObject *parent);
    ~QHelpDBReader();

    bool init();
    
    QString errorMessage() const;

    QString databaseName() const;
    QString namespaceName() const;
    QString virtualFolder() const;
    QList<QStringList> filterAttributeSets() const;
    QStringList files(const QStringList &filterAttributes,
        const QString &extensionFilter = QString()) const;
    bool fileExists(const QString &virtualFolder, const QString &filePath,
        const QStringList &filterAttributes = QStringList()) const;
    QByteArray fileData(const QString &virtualFolder,
        const QString &filePath) const;

    QStringList customFilters() const;
    QStringList filterAttributes(const QString &filterName = QString()) const;
    QStringList indicesForFilter(const QStringList &filterAttributes) const;
    void linksForKeyword(const QString &keyword, const QStringList &filterAttributes,
        QMap<QString, QUrl> &linkMap) const;

    void linksForIdentifier(const QString &id, const QStringList &filterAttributes,
        QMap<QString, QUrl> &linkMap) const;

	QList<QByteArray> contentsForFilter(const QStringList &filterAttributes) const;
    QUrl urlOfPath(const QString &relativePath) const; 

    bool createAttributesCache(const QStringList &attributes);
    QVariant metaData(const QString &name) const;

private:
    QUrl buildQUrl(const QString &ns, const QString &folder,
        const QString &relFileName, const QString &anchor) const;
    QString mergeList(const QStringList &list) const;
    QString quote(const QString &string) const;
    
    bool m_initDone;
    QString m_dbName;
    QString m_uniqueId;
    QString m_error;
    QSqlQuery *m_query;
    mutable QString m_namespace;
    QSet<QString> m_viewAttributes;
    bool m_useAttributesCache;
};

QT_END_NAMESPACE

#endif
