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

#ifndef QHELPSEARCHINDEXREADERDEFAULT_H
#define QHELPSEARCHINDEXREADERDEFAULT_H

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

#include "qhelpsearchindex_default_p.h"
#include "qhelpsearchengine.h"

#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QByteArray>
#include <QtCore/QStringList>
#include <QtCore/QWaitCondition>

QT_BEGIN_NAMESPACE

struct Entry;
struct PosEntry;

namespace qt {
    namespace fulltextsearch {
        namespace std {

class Reader
{
    typedef QList<QStringList> DocumentList;
    typedef QHash<QString, Entry*> EntryTable;
    typedef QPair<EntryTable, DocumentList> Index;
    typedef QHash<QString, Index> IndexTable;

public:
    Reader();
    ~Reader();

    bool readIndex();
    bool initCheck() const;
    void setIndexPath(const QString &path);
    void filterFilesForAttributes(const QStringList &attributes);
    void setIndexFile(const QString &namespaceName, const QString &attributes);
    bool splitSearchTerm(const QString &searchTerm, QStringList *terms, 
        QStringList *termSeq, QStringList *seqWords);
    
    void searchInIndex(const QStringList &terms);
    QVector<DocumentInfo> hits();
    bool searchForPattern(const QStringList &patterns, 
        const QStringList &words, const QByteArray &data);

private:
    QVector<Document> setupDummyTerm(const QStringList &terms, const EntryTable &entryTable);
    QStringList getWildcardTerms(const QString &term, const EntryTable &entryTable);
    void buildMiniIndex(const QString &string);
    void reset();
    void cleanupIndex(EntryTable &entryTable);

private:
    uint wordNum;
    QString indexPath;
    QString indexFile;
    QString documentFile;

    IndexTable indexTable;
    QList<TermInfo> termList;
    IndexTable searchIndexTable;
    QHash<QString, PosEntry*> miniIndex;
};


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

private:
    QMutex mutex;
    Reader m_reader;
    QWaitCondition waitCondition;
    QList<QHelpSearchEngine::SearchHit> hitList;

    bool m_cancel;
    QList<QHelpSearchQuery> m_query;
    QString m_collectionFile;
};

        }   // namespace std
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE

#endif  // QHELPSEARCHINDEXREADERDEFAULT_H
