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

#include "qhelpenginecore.h"
#include "qhelpsearchindexreader_default_p.h"

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QtCore/QFileInfo>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

namespace qt {
    namespace fulltextsearch {
        namespace std {

namespace {
    QStringList split( const QString &str )
    {
        QStringList lst;
        int j = 0;
        int i = str.indexOf(QLatin1Char('*'), j );

        if (str.startsWith(QLatin1String("*")))
            lst << QLatin1String("*");

        while ( i != -1 ) {
            if ( i > j && i <= (int)str.length() ) {
                lst << str.mid( j, i - j );
                lst << QLatin1String("*");
            }
            j = i + 1;
            i = str.indexOf(QLatin1Char('*'), j );
        }

        int l = str.length() - 1;
        if ( str.mid( j, l - j + 1 ).length() > 0 )
            lst << str.mid( j, l - j + 1 );

        return lst;
    }
}


Reader::Reader()
    : indexPath(QString())
    , indexFile(QString())
    , documentFile(QString())
{
    termList.clear();
    indexTable.clear();
    searchIndexTable.clear();
}

Reader::~Reader()
{
    reset();
    searchIndexTable.clear();
}

bool Reader::readIndex()
{
    if (indexTable.contains(indexFile))
        return true;

    QFile idxFile(indexFile);
    if (!idxFile.open(QFile::ReadOnly))
        return false;

    QString key;
    int numOfDocs;
    EntryTable entryTable;
    QVector<Document> docs;
    QDataStream dictStream(&idxFile);
    while (!dictStream.atEnd()) {
        dictStream >> key;
        dictStream >> numOfDocs;
        docs.resize(numOfDocs);
        dictStream >> docs;
        entryTable.insert(key, new Entry(docs));
    }
    idxFile.close();

    if (entryTable.isEmpty())
        return false;

    QFile docFile(documentFile);
    if (!docFile.open(QFile::ReadOnly))
        return false;

    QString title, url;
    DocumentList documentList;
    QDataStream docStream(&docFile);
    while (!docStream.atEnd()) {
        docStream >> title;
        docStream >> url;
        documentList.append(QStringList(title) << url);
    }
    docFile.close();

    if (documentList.isEmpty()) {
        cleanupIndex(entryTable);
        return false;
    }

    indexTable.insert(indexFile, Index(entryTable, documentList));
    return true;
}

bool Reader::initCheck() const
{
    return !searchIndexTable.isEmpty();
}

void Reader::setIndexPath(const QString &path)
{
    indexPath = path;
}

void Reader::filterFilesForAttributes(const QStringList &attributes)
{
    searchIndexTable.clear();
    for(IndexTable::ConstIterator it = indexTable.begin(); it != indexTable.end(); ++it) {
        const QString fileName = it.key();
        bool containsAll = true;
        QStringList split = fileName.split(QLatin1String("@"));
        foreach (const QString attribute, attributes) {
            if (!split.contains(attribute, Qt::CaseInsensitive)) {
                containsAll = false;
                break;
            }
        }

        if (containsAll)
            searchIndexTable.insert(fileName, it.value());
    }
}

void Reader::setIndexFile(const QString &namespaceName, const QString &attributes)
{
    QString extention = namespaceName + QLatin1String("@") + attributes;
    indexFile = indexPath + QLatin1String("/indexdb40.") + extention;
    documentFile = indexPath + QLatin1String("/indexdoc40.") + extention;
}

bool Reader::splitSearchTerm(const QString &searchTerm, QStringList *terms,
                                  QStringList *termSeq, QStringList *seqWords)
{
    QString term = searchTerm;

    term = term.simplified();
    term = term.replace(QLatin1String("\'"), QLatin1String("\""));
    term = term.replace(QLatin1String("`"), QLatin1String("\""));
    term = term.replace(QLatin1String("-"), QLatin1String(" "));
    term = term.replace(QRegExp(QLatin1String("\\s[\\S]?\\s")), QLatin1String(" "));

    *terms = term.split(QLatin1Char(' '));
    QStringList::iterator it = terms->begin();
    for (; it != terms->end(); ++it) {
        (*it) = (*it).simplified();
        (*it) = (*it).toLower();
        (*it) = (*it).replace(QLatin1String("\""), QLatin1String(""));
    }

    if (term.contains(QLatin1Char('\"'))) {
        if ((term.count(QLatin1Char('\"')))%2 == 0) {
            int beg = 0;
            int end = 0;
            QString s;
            beg = term.indexOf(QLatin1Char('\"'), beg);
            while (beg != -1) {
                beg++;
                end = term.indexOf(QLatin1Char('\"'), beg);
                s = term.mid(beg, end - beg);
                s = s.toLower();
                s = s.simplified();
                if (s.contains(QLatin1Char('*'))) {
                    qWarning("Full Text Search, using a wildcard within phrases is not allowed.");
                    return false;
                }
                *seqWords += s.split(QLatin1Char(' '));
                *termSeq << s;
                beg = term.indexOf(QLatin1Char('\"'), end + 1);
            }
        } else {
            qWarning("Full Text Search, the closing quotation mark is missing.");
            return false;
        }
    }

    return true;
}

void Reader::searchInIndex(const QStringList &terms)
{
    foreach (const QString term, terms) {
        QVector<Document> documents;

        for(IndexTable::ConstIterator it = searchIndexTable.begin();
            it != searchIndexTable.end(); ++it) {
            EntryTable entryTable = it.value().first;
            DocumentList documentList = it.value().second;

            if (term.contains(QLatin1Char('*')))
                documents = setupDummyTerm(getWildcardTerms(term, entryTable), entryTable);
            else if (entryTable.value(term))
                documents = entryTable.value(term)->documents;
            else
                continue;

            if (!documents.isEmpty()) {
                DocumentInfo info;
                QString title, url;
                QVector<DocumentInfo> documentsInfo;
                foreach(const Document doc, documents) {
                    info.docNumber = doc.docNumber;
                    info.frequency = doc.frequency;
                    info.documentUrl = documentList.at(doc.docNumber).at(1);
                    info.documentTitle = documentList.at(doc.docNumber).at(0);
                    documentsInfo.append(info);
                }

                bool found = false;
                for(QList<TermInfo>::Iterator tit = termList.begin();
                    tit != termList.end(); ++tit) {
                    TermInfo *t = &(*tit);
                    if(t->term == term) {
                        t->documents += documentsInfo;
                        t->frequency += documentsInfo.count();
                        found = true; break;
                    }
                }
                if (!found)
                    termList.append(TermInfo(term, documentsInfo.count(), documentsInfo));
            }
        }
    }
    qSort(termList);
}

QVector<DocumentInfo> Reader::hits()
{
    QVector<DocumentInfo> documents;
    if (!termList.count())
        return documents;

    documents = termList.takeFirst().documents;
    for(QList<TermInfo>::Iterator it = termList.begin(); it != termList.end(); ++it) {
        TermInfo *t = &(*it);
        QVector<DocumentInfo> docs = t->documents;
        for(QVector<DocumentInfo>::Iterator minDoc_it = documents.begin();
            minDoc_it != documents.end(); ) {
            bool found = false;
            for (QVector<DocumentInfo>::ConstIterator doc_it = docs.constBegin();
                doc_it != docs.constEnd(); ++doc_it ) {
                if ( (*minDoc_it).docNumber == (*doc_it).docNumber ) {
                    (*minDoc_it).frequency += (*doc_it).frequency;
                    found = true;
                    break;
                }
            }
            if (!found)
                minDoc_it = documents.erase(minDoc_it);
            else
                ++minDoc_it;
        }
    }

    qSort(documents);
    return documents;
}

bool Reader::searchForPattern(const QStringList &patterns, const QStringList &words,
                                   const QByteArray &data)
{
    if (data.isEmpty())
        return false;

    for(QHash<QString, PosEntry*>::ConstIterator mit =
        miniIndex.begin(); mit != miniIndex.end(); ++mit) {
            delete mit.value();
    }
    miniIndex.clear();

    wordNum = 3;
    QStringList::ConstIterator cIt = words.begin();
    for ( ; cIt != words.end(); ++cIt )
        miniIndex.insert(*cIt, new PosEntry(0));

    QTextStream s(data);
    QString text = s.readAll();
    bool valid = true;
    const QChar *buf = text.unicode();
    QChar str[64];
    QChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( j < text.length() ) {
        if ( c == QLatin1Char('<') || c == QLatin1Char('&') ) {
            valid = false;
            if ( i > 1 )
                buildMiniIndex( QString(str,i) );
            i = 0;
            c = buf[++j];
            continue;
        }
        if ( ( c == QLatin1Char('>') || c == QLatin1Char(';') ) && !valid ) {
            valid = true;
            c = buf[++j];
            continue;
        }
        if ( !valid ) {
            c = buf[++j];
            continue;
        }
        if ( ( c.isLetterOrNumber() || c == QLatin1Char('_') ) && i < 63 ) {
            str[i] = c.toLower();
            ++i;
        } else {
            if ( i > 1 )
                buildMiniIndex( QString(str,i) );
            i = 0;
        }
        c = buf[++j];
    }
    if ( i > 1 )
        buildMiniIndex( QString(str,i) );

    QStringList::ConstIterator patIt = patterns.begin();
    QStringList wordLst;
    QList<uint> a, b;
    QList<uint>::iterator aIt;
    for ( ; patIt != patterns.end(); ++patIt ) {
        wordLst = (*patIt).split(QLatin1Char(' '));
        a = miniIndex[ wordLst[0] ]->positions;
        for ( int j = 1; j < (int)wordLst.count(); ++j ) {
            b = miniIndex[ wordLst[j] ]->positions;
            aIt = a.begin();
            while ( aIt != a.end() ) {
                if ( b.contains( *aIt + 1 )) {
                    (*aIt)++;
                    ++aIt;
                } else {
                    aIt = a.erase( aIt );
                }
            }
        }
    }
    if ( a.count() )
        return true;
    return false;
}

QVector<Document> Reader::setupDummyTerm(const QStringList &terms,
                                              const EntryTable &entryTable)
{
    QList<Term> termList;
    for (QStringList::ConstIterator it = terms.begin(); it != terms.end(); ++it) {
        if (entryTable.value(*it)) {
            Entry *e = entryTable.value(*it);
            termList.append(Term(*it, e->documents.count(), e->documents ) );
        }
    }
    QVector<Document> maxList(0);
    if ( !termList.count() )
        return maxList;
    qSort(termList);

    maxList = termList.takeLast().documents;
    for(QList<Term>::Iterator it = termList.begin(); it != termList.end(); ++it) {
        Term *t = &(*it);
        QVector<Document> docs = t->documents;
        for (QVector<Document>::iterator docIt = docs.begin(); docIt != docs.end(); ++docIt ) {
            if ( maxList.indexOf( *docIt ) == -1 )
                maxList.append( *docIt );
        }
    }
    return maxList;
}

QStringList Reader::getWildcardTerms(const QString &term,
                                          const EntryTable &entryTable)
{
    QStringList lst;
    QStringList terms = split(term);
    QStringList::Iterator iter;

    for(EntryTable::ConstIterator it = entryTable.begin();
        it != entryTable.end(); ++it) {
        int index = 0;
        bool found = false;
        QString text( it.key() );
        for ( iter = terms.begin(); iter != terms.end(); ++iter ) {
            if ( *iter == QLatin1String("*") ) {
                found = true;
                continue;
            }
            if ( iter == terms.begin() && (*iter)[0] != text[0] ) {
                found = false;
                break;
            }
            index = text.indexOf( *iter, index );
            if ( *iter == terms.last() && index != (int)text.length()-1 ) {
                index = text.lastIndexOf( *iter );
                if ( index != (int)text.length() - (int)(*iter).length() ) {
                    found = false;
                    break;
                }
            }
            if ( index != -1 ) {
                found = true;
                index += (*iter).length();
                continue;
            } else {
                found = false;
                break;
            }
        }
        if (found)
            lst << text;
    }

    return lst;
}

void Reader::buildMiniIndex(const QString &string)
{
    if (miniIndex[string])
        miniIndex[string]->positions.append(wordNum);
    ++wordNum;
}

void Reader::reset()
{
    for(IndexTable::Iterator it = indexTable.begin();
        it != indexTable.end(); ++it) {
        cleanupIndex(it.value().first);
        it.value().second.clear();
    }
}

void Reader::cleanupIndex(EntryTable &entryTable)
{
    for(EntryTable::ConstIterator it =
        entryTable.begin(); it != entryTable.end(); ++it) {
            delete it.value();
    }

    entryTable.clear();
}


QHelpSearchIndexReader::QHelpSearchIndexReader()
    : QThread()
    , m_cancel(false)
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
                                    const QString &indexFilesFolder,
                                    const QList<QHelpSearchQuery> &queryList)
{
    QMutexLocker lock(&mutex);

    this->hitList.clear();
    this->m_cancel = false;
    this->m_query = queryList;
    this->m_collectionFile = collectionFile;
    this->m_indexFilesFolder = indexFilesFolder;

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

    const QList<QHelpSearchQuery> &queryList = this->m_query;
    const QLatin1String key("DefaultSearchNamespaces");
    const QString collectionFile(this->m_collectionFile);
    const QString indexPath = m_indexFilesFolder;

    mutex.unlock();

    QString queryTerm;
    foreach (const QHelpSearchQuery query, queryList) {
        if (query.fieldName == QHelpSearchQuery::DEFAULT) {
            queryTerm = query.wordList.at(0);
            break;
        }
    }

    if (queryTerm.isEmpty())
        return;

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;
    
    const QStringList registeredDocs = engine.registeredDocumentations();
    const QStringList indexedNamespaces = engine.customValue(key).toString().
        split(QLatin1String("|"), QString::SkipEmptyParts);

    emit searchingStarted();

    // setup the reader
    m_reader.setIndexPath(indexPath);
    foreach(const QString namespaceName, registeredDocs) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            searchingFinished(0);   // TODO: check this ???
            return;
        }
        mutex.unlock();

        const QList<QStringList> attributeSets =
            engine.filterAttributeSets(namespaceName);

        foreach (QStringList attributes, attributeSets) {
            // read all index files
            m_reader.setIndexFile(namespaceName, attributes.join(QLatin1String("@")));
            if (!m_reader.readIndex()) {
                qWarning("Full Text Search, could not read file for namespace: %s.",
                    namespaceName.toUtf8().constData());
            }
        }
    }

    // get the current filter attributes and minimize the index files table
    m_reader.filterFilesForAttributes(engine.filterAttributes(engine.currentFilter()));

    hitList.clear();
    QStringList terms, termSeq, seqWords;
    if (m_reader.initCheck() && // check if we could read anything
        m_reader.splitSearchTerm(queryTerm, &terms, &termSeq, &seqWords) ) {

        // search for term(s)
        m_reader.searchInIndex(terms);    // TODO: should this be interruptible as well ???

        QVector<DocumentInfo> hits = m_reader.hits();
        if (!hits.isEmpty()) {
            if (termSeq.isEmpty()) {
                foreach (const DocumentInfo docInfo, hits) {
                    mutex.lock();
                    if (m_cancel) {
                        mutex.unlock();
                        searchingFinished(0);   // TODO: check this, speed issue while locking???
                        return;
                    }
                    mutex.unlock();
                    hitList.append(qMakePair(docInfo.documentTitle, docInfo.documentUrl));
                }
            } else {
                foreach (const DocumentInfo docInfo, hits) {
                    mutex.lock();
                    if (m_cancel) {
                        mutex.unlock();
                        searchingFinished(0);   // TODO: check this, speed issue while locking???
                        return;
                    }
                    mutex.unlock();

                    if (m_reader.searchForPattern(termSeq, seqWords, engine.fileData(docInfo.documentUrl))) // TODO: should this be interruptible as well ???
                        hitList.append(qMakePair(docInfo.documentTitle, docInfo.documentUrl));
                }
            }
        }
    }

    emit searchingFinished(hitList.count());
}

        }   // namespace std
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE
