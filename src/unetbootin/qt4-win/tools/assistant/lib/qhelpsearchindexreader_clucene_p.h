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

#ifndef QHELPSEARCHINDEXREADERCLUCENE_H
#define QHELPSEARCHINDEXREADERCLUCENE_H

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

#include "qhelpsearchengine.h"
#include "fulltextsearch/qquery_p.h"

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

QT_BEGIN_NAMESPACE

namespace qt {
    namespace fulltextsearch {
        namespace clucene {

class QHelpSearchIndexReader : public QThread
{
    Q_OBJECT

public:
    QHelpSearchIndexReader();
    ~QHelpSearchIndexReader();

    void cancelSearching();
    void search(const QString &collectionFile, 
        const QList<QHelpSearchQuery> &queryList);

    int hitsCount() const;
    QHelpSearchEngine::SearchHit hit(int index) const;

signals:
    void searchingStarted();
    void searchingFinished(int hits);

private:
    void run();
    bool defaultQuery(const QString &term,
        QCLuceneBooleanQuery &booleanQuery);
    bool buildQuery(QCLuceneBooleanQuery &booleanQuery,
        const QList<QHelpSearchQuery> &queryList);
    
private:
    QMutex mutex;
    QList<QHelpSearchEngine::SearchHit> hitList;
    QWaitCondition waitCondition;

    bool m_cancel;
    QString m_collectionFile;
    QList<QHelpSearchQuery> m_query;
};

        }   // namespace clucene
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE

#endif  // QHELPSEARCHINDEXREADERCLUCENE_H
