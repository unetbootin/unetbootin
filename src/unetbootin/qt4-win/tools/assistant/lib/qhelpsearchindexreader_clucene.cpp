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

#include "qhelpenginecore.h"
#include "fulltextsearch/qsearchable_p.h"
#include "fulltextsearch/qqueryparser_p.h"
#include "fulltextsearch/qindexreader_p.h"
#include "qhelpsearchindexreader_clucene_p.h"

#include <QtCore/QDir>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QMutexLocker>

QT_BEGIN_NAMESPACE

namespace qt {
    namespace fulltextsearch {
        namespace clucene {

QHelpSearchIndexReader::QHelpSearchIndexReader()
    : QThread()
    , m_cancel(false)
    , m_collectionFile(QString())
{
    // nothing todo
}

QHelpSearchIndexReader::~QHelpSearchIndexReader()
{
    mutex.lock();
    this->m_cancel = true;
    waitCondition.wakeOne();
    mutex.unlock();

    wait();
}

void QHelpSearchIndexReader::cancelSearching()
{
    mutex.lock();
    this->m_cancel = true;
    mutex.unlock();
}

void QHelpSearchIndexReader::search(const QString &collectionFile, 
                                    const QList<QHelpSearchQuery> &queryList)
{
    QMutexLocker lock(&mutex);
    
    this->hitList.clear();
    this->m_cancel = false;
    this->m_query = queryList;
    this->m_collectionFile = collectionFile;

    start(QThread::NormalPriority);
}

int QHelpSearchIndexReader::hitsCount() const
{
    return hitList.count();
}

QHelpSearchEngine::SearchHit QHelpSearchIndexReader::hit(int index) const
{
    return hitList.at(index);
}

void QHelpSearchIndexReader::run()
{
    mutex.lock();
    
    if (m_cancel) {
        mutex.unlock();
        return;
    }

    const QString collectionFile(this->m_collectionFile);
    const QList<QHelpSearchQuery> &queryList = this->m_query;

    mutex.unlock();

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    QString indexPath = engine.customValue(QLatin1String("indexFilesFolder")).toString();
    QDir dir(indexPath);
    if (!dir.isAbsolute()) {
        indexPath = QFileInfo(engine.collectionFile()).path() +
            dir.separator() + dir.path();
    }
    
    if(QCLuceneIndexReader::indexExists(indexPath)) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            return;
        }
        mutex.unlock();

        emit searchingStarted();

#if !defined(QT_NO_EXCEPTIONS)
        try {
#endif
            QCLuceneBooleanQuery booleanQuery;
            if (!buildQuery(booleanQuery, queryList)) {
                emit searchingFinished(0);
                return;
            }

            const QStringList attribList = engine.filterAttributes(engine.currentFilter());
            if (!attribList.isEmpty()) {
                QCLuceneStandardAnalyzer analyzer;
                QCLuceneQuery* query = QCLuceneQueryParser::parse(QLatin1String("+")
                    + attribList.join(QLatin1String(" +")), QLatin1String("attribute"),
                    analyzer);

                if (!query) {
                    emit searchingFinished(0);
                    return;
                }
                booleanQuery.add(query, true, true, false);
            }

            QCLuceneIndexSearcher indexSearcher(indexPath);
            QCLuceneHits hits = indexSearcher.search(booleanQuery);
            const QStringList namespaceList = engine.registeredDocumentations();

            QSet<QString> pathSet;
            QCLuceneDocument document;
            for (qint32 i = 0; i < hits.length(); i++) {
                document = hits.document(i);
                const QString path = document.get(QLatin1String("path"));
                if (!pathSet.contains(path) && namespaceList.contains(
                    document.get(QLatin1String("namespace")), Qt::CaseInsensitive)) {
                    pathSet.insert(path);
                    hitList.append(qMakePair(path, document.get(QLatin1String("title"))));
                }
                document.clear();

                mutex.lock();
                if (m_cancel) {
                    mutex.unlock();
                    emit searchingFinished(0);
                    return;
                }
                mutex.unlock();
            }

            indexSearcher.close();
            emit searchingFinished(hitList.count());

#if !defined(QT_NO_EXCEPTIONS)
        } catch(...) {
            hitList.clear();
            emit searchingFinished(0);
        }
#endif
    }
}

bool QHelpSearchIndexReader::defaultQuery(const QString &term,
                                          QCLuceneBooleanQuery &booleanQuery)
{
    QCLuceneStandardAnalyzer analyzer;
    
    const QLatin1String c("content");
    const QLatin1String t("titleTokenized");

    QCLuceneQuery *query = QCLuceneQueryParser::parse(term, c, analyzer);
    QCLuceneQuery *query2 = QCLuceneQueryParser::parse(term, t, analyzer);
    if (query && query2) {
        booleanQuery.add(query, true, false, false);
        booleanQuery.add(query2, true, false, false);
        return true;
    }

    return false;
}

bool QHelpSearchIndexReader::buildQuery(QCLuceneBooleanQuery &booleanQuery, 
                                        const QList<QHelpSearchQuery> &queryList)
{
    foreach (const QHelpSearchQuery query, queryList) {
        switch (query.fieldName) {
            case QHelpSearchQuery::FUZZY: {
                const QLatin1String fuzzy("~");
                foreach (const QString term, query.wordList) {
                    if (term.isEmpty() || !defaultQuery(term + fuzzy, booleanQuery))
                        return false;
                }
            }   break;

            case QHelpSearchQuery::WITHOUT: {
                foreach (const QString term, query.wordList) {
                    QCLuceneQuery *query = new QCLuceneTermQuery(QCLuceneTerm(
                        QLatin1String("content"), term.toLower()));
                    QCLuceneQuery *query2 = new QCLuceneTermQuery(QCLuceneTerm(
                        QLatin1String("titleTokenized"), term.toLower()));

                    if (query && query2) {
                        booleanQuery.add(query, true, false, true);
                        booleanQuery.add(query2, true, false, true);
                    } else {
                        return false;
                    }
                }
            }   break;

            case QHelpSearchQuery::PHRASE: {
                const QString term = query.wordList.at(0).toLower();
                if (term.contains(QLatin1Char(' '))) {
                    QStringList termList = term.split(QLatin1String(" "));
                    QCLucenePhraseQuery *q = new QCLucenePhraseQuery();
                    foreach (const QString t, termList)
                        q->addTerm(QCLuceneTerm(QLatin1String("content"), t));

                    booleanQuery.add(q, true, true, false);
                } else {
                    QCLuceneQuery *query = new QCLuceneTermQuery(QCLuceneTerm(
                        QLatin1String("content"), term));
                    QCLuceneQuery *query2 = new QCLuceneTermQuery(QCLuceneTerm(
                        QLatin1String("titleTokenized"), term));

                    if (query && query2) {
                        booleanQuery.add(query, true, true, false);
                        booleanQuery.add(query2, true, false, false);
                    } else {
                        return false;
                    }
                }
            }   break;

            case QHelpSearchQuery::ALL: {
                foreach (const QString term, query.wordList) {
                    QCLuceneQuery *query = new QCLuceneTermQuery(QCLuceneTerm(
                        QLatin1String("content"), term));

                    if (query) {
                        booleanQuery.add(query, true, true, false);
                    } else {
                        return false;
                    }
                }
            }   break;
            
            case QHelpSearchQuery::DEFAULT: {
                QCLuceneStandardAnalyzer analyzer;
                foreach (const QString t, query.wordList) {
                    QCLuceneQuery *query = QCLuceneQueryParser::parse(t,
                        QLatin1String("content"), analyzer);
            
                    if (query)
                        booleanQuery.add(query, true, true, false);
                }
            }   break;

            case QHelpSearchQuery::ATLEAST: {
                foreach (const QString term, query.wordList) {
                    if (term.isEmpty() || !defaultQuery(term, booleanQuery))
                        return false;
                }
            }
        }
    }

    return true;
}

        }   // namespace clucene
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE
