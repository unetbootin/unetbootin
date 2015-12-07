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

#ifndef QHELPCOLLECTIONHANDLER_H
#define QHELPCOLLECTIONHANDLER_H

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

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QStringList>

#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE

class QHelpCollectionHandler : public QObject
{
    Q_OBJECT

public:
    struct DocInfo
    {
        QString fileName;
        QString folderName;
        QString namespaceName;
    };
    typedef QList<DocInfo> DocInfoList;

    QHelpCollectionHandler(const QString &collectionFile, QObject *parent = 0);
    ~QHelpCollectionHandler();

    QString collectionFile() const;

    bool openCollectionFile();
    bool copyCollectionFile(const QString &fileName);

    QStringList customFilters() const;
    bool removeCustomFilter(const QString &filterName);
    bool addCustomFilter(const QString &filterName,
        const QStringList &attributes);

    DocInfoList registeredDocumentations() const;
    bool registerDocumentation(const QString &fileName);
    bool unregisterDocumentation(const QString &namespaceName);

    bool removeCustomValue(const QString &key);
    QVariant customValue(const QString &key, const QVariant &defaultValue) const;
    bool setCustomValue(const QString &key, const QVariant &value);

    bool addFilterAttributes(const QStringList &attributes);
    QStringList filterAttributes() const;
    QStringList filterAttributes(const QString &filterName) const;
    
    int registerNamespace(const QString &nspace, const QString &fileName);
    bool registerVirtualFolder(const QString &folderName, int namespaceId);
    void optimizeDatabase(const QString &fileName);

signals:
    void error(const QString &msg);

private:
    bool isDBOpened();
    bool createTables(QSqlQuery *query);

    bool m_dbOpened;
    QString m_collectionFile;
    QString m_connectionName;
    mutable QSqlQuery m_query;
};

QT_END_NAMESPACE

#endif  //QHELPCOLLECTIONHANDLER_H
