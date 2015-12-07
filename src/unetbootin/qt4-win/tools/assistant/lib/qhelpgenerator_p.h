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

#ifndef QHELPGENERATOR_H
#define QHELPGENERATOR_H

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

#include "qhelp_global.h"
#include "qhelpdatainterface_p.h"

#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QHelpGeneratorPrivate;

class QHELP_EXPORT QHelpGenerator : public QObject
{
    Q_OBJECT

public:
    QHelpGenerator(QObject *parent = 0);
    ~QHelpGenerator();

    bool generate(QHelpDataInterface *helpData,
        const QString &outputFileName);
    QString error() const;

Q_SIGNALS:
    void statusChanged(const QString &msg);
    void progressChanged(double progress);
    void warning(const QString &msg);

private:
    struct FileNameTableData
    {
        QString name;
        int fileId;
        QString title;
    };

    void writeTree(QDataStream &s, QHelpDataContentItem *item, int depth);
    bool createTables();
    bool insertFileNotFoundFile();
    bool registerCustomFilter(const QString &filterName,
		const QStringList &filterAttribs, bool forceUpdate = false);
    bool registerVirtualFolder(const QString &folderName, const QString &ns);
    bool insertFilterAttributes(const QStringList &attributes);
    bool insertKeywords(const QList<QHelpDataIndexItem> keywords,
        const QStringList &filterAttributes);
    bool insertFiles(const QStringList &files, const QString &rootPath,
        const QStringList &filterAttributes);
    bool insertContents(const QByteArray &ba, 
        const QStringList &filterAttributes);
    bool insertMetaData(const QMap<QString, QVariant> &metaData);
    void cleanupDB();
    void setupProgress(QHelpDataInterface *helpData);
    void addProgress(double step);
    
    QHelpGeneratorPrivate *d;    
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
