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
#include "qhelp_global.h"
#include "fulltextsearch/qhits_p.h"
#include "fulltextsearch/qquery_p.h"
#include "fulltextsearch/qanalyzer_p.h"
#include "fulltextsearch/qdocument_p.h"
#include "fulltextsearch/qsearchable_p.h"
#include "fulltextsearch/qindexreader_p.h"
#include "fulltextsearch/qindexwriter_p.h"
#include "qhelpsearchindexwriter_clucene_p.h"

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

QT_BEGIN_NAMESPACE

namespace qt {
    namespace fulltextsearch {
        namespace clucene {

class DocumentHelper
{
public:
    DocumentHelper(const QString& fileName, const QByteArray &data)
        : fileName(fileName) , data(readData(data)) {}
    ~DocumentHelper() {}

    bool addFieldsToDocument(QCLuceneDocument *document,
        const QString &namespaceName, const QString &attributes = QString())
    {
        if (!document)
            return false;

        if(!data.isEmpty()) {
            QString parsedData = parseData();
            QString parsedTitle = QHelpGlobal::documentTitle(data);

            if(!parsedData.isEmpty()) {
                document->add(new QCLuceneField(QLatin1String("content"),
                    parsedData,QCLuceneField::INDEX_TOKENIZED));
                document->add(new QCLuceneField(QLatin1String("path"), fileName,
                    QCLuceneField::STORE_YES | QCLuceneField::INDEX_UNTOKENIZED));
                document->add(new QCLuceneField(QLatin1String("title"), parsedTitle,
                    QCLuceneField::STORE_YES | QCLuceneField::INDEX_UNTOKENIZED));
                document->add(new QCLuceneField(QLatin1String("titleTokenized"), parsedTitle,
                    QCLuceneField::STORE_YES | QCLuceneField::INDEX_TOKENIZED));
                document->add(new QCLuceneField(QLatin1String("namespace"), namespaceName,
                    QCLuceneField::STORE_YES | QCLuceneField::INDEX_UNTOKENIZED));
                document->add(new QCLuceneField(QLatin1String("attribute"), attributes,
                    QCLuceneField::STORE_YES | QCLuceneField::INDEX_TOKENIZED));
                return true;
            }
        }

        return false;
    }

private:
    QString readData(const QByteArray &data)
    {
        QTextStream textStream(data);
        QByteArray charSet = QHelpGlobal::charsetFromData(data).toLatin1();
        textStream.setCodec(QTextCodec::codecForName(charSet.constData()));

        QString stream = textStream.readAll();
        if (stream.isNull() || stream.isEmpty())
            return QString();

        return stream;
    }

    QString parseData() const
    {
        const int length = data.length();
        const QChar *buf = data.unicode();

        QString parsedContent;
        parsedContent.reserve(length);

        bool valid = true;
        int j = 0, count = 0;

        QChar c;
        while (j < length) {
            c = buf[j++];
            if (c == QLatin1Char('<') || c == QLatin1Char('&')) {
                if (count > 1)
                    parsedContent.append(QLatin1Char(' '));
                count = 0;
                valid = false;
                continue;
            }
            if ((c == QLatin1Char('>') || c == QLatin1Char(';')) && !valid) {
                valid = true;
                continue;
            }
            if (!valid)
                continue;

            if (c.isLetterOrNumber() || c.isPrint()) {
                ++count;
                parsedContent.append(c.toLower());
            } else {
                if (count > 1)
                    parsedContent.append(QLatin1Char(' '));
                count = 0;
            }
        }

        return parsedContent;
    }

private:
    QString fileName;
    QString data;
};


QHelpSearchIndexWriter::QHelpSearchIndexWriter()
    : QThread(0)
    , m_cancel(false)
{
    // nothing todo
}

QHelpSearchIndexWriter::~QHelpSearchIndexWriter()
{
    mutex.lock();
    this->m_cancel = true;
    waitCondition.wakeOne();
    mutex.unlock();

    wait();
}

void QHelpSearchIndexWriter::cancelIndexing()
{
    mutex.lock();
    this->m_cancel = true;
    mutex.unlock();
}

void QHelpSearchIndexWriter::updateIndex(const QString &collectionFile,
    const QString &indexFilesFolder, bool reindex)
{
    mutex.lock();
    this->m_cancel = false;
    this->m_reindex = reindex;
    this->m_collectionFile = collectionFile;
    this->m_indexFilesFolder = indexFilesFolder;
    mutex.unlock();

    start(QThread::NormalPriority);
}

void QHelpSearchIndexWriter::optimizeIndex()
{
    if (QCLuceneIndexReader::indexExists(m_indexFilesFolder)) {
        if (QCLuceneIndexReader::isLocked(m_indexFilesFolder))
            return;

        QCLuceneStandardAnalyzer analyzer;
        QCLuceneIndexWriter writer(m_indexFilesFolder, analyzer, false);
        writer.optimize();
        writer.close();
    }
}

void QHelpSearchIndexWriter::run()
{
    QMutexLocker mutexLocker(&mutex);

    if (m_cancel)
        return;

    const bool reindex = this->m_reindex;
    const QString collectionFile(this->m_collectionFile);

    mutexLocker.unlock();

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    const QLatin1String key("CluceneIndexedNamespaces");
    if (reindex)
        engine.setCustomValue(key, QLatin1String(""));

    QMap<QString, QDateTime> indexMap;
    const QLatin1String oldKey("CluceneSearchNamespaces");
    if (!engine.customValue(oldKey, QString()).isNull()) {
        // old style qhc file < 4.4.2, need to convert...
        const QStringList indexedNamespaces = engine.customValue(oldKey).
            toString().split(QLatin1String("|"), QString::SkipEmptyParts);
        foreach (const QString& nameSpace, indexedNamespaces)
            indexMap.insert(nameSpace, QDateTime());
        engine.removeCustomValue(oldKey);
    } else {
        QDataStream dataStream(engine.customValue(key).toByteArray());
        dataStream >> indexMap;
    }

    QString indexPath = m_indexFilesFolder;

    QFileInfo fInfo(indexPath);
    if (fInfo.exists() && !fInfo.isWritable()) {
        qWarning("Full Text Search, could not create index (missing permissions).");
        return;
    }

    emit indexingStarted();

    QCLuceneIndexWriter *writer = 0;
    QCLuceneStandardAnalyzer analyzer;
    const QStringList registeredDocs = engine.registeredDocumentations();

    QLocalSocket localSocket;
    localSocket.connectToServer(QString(QLatin1String("QtAssistant%1"))
        .arg(QLatin1String(QT_VERSION_STR)));

    QLocalServer localServer;
    bool otherInstancesRunning = true;
    if (!localSocket.waitForConnected()) {
        otherInstancesRunning = false;
        localServer.listen(QString(QLatin1String("QtAssistant%1"))
            .arg(QLatin1String(QT_VERSION_STR)));
    }

#if !defined(QT_NO_EXCEPTIONS)
    try {
#endif
        // check if it's locked, and if the other instance is running
        if (!otherInstancesRunning && QCLuceneIndexReader::isLocked(indexPath))
            QCLuceneIndexReader::unlock(indexPath);

        if (QCLuceneIndexReader::isLocked(indexPath)) {
            // poll unless indexing finished to fake progress
            while (QCLuceneIndexReader::isLocked(indexPath)) {
                mutexLocker.relock();
                if (m_cancel)
                    break;
                mutexLocker.unlock();
                this->sleep(1);
            }
            emit indexingFinished();
            return;
        }

        if (QCLuceneIndexReader::indexExists(indexPath) && !reindex) {
            foreach(const QString& namespaceName, registeredDocs) {
                mutexLocker.relock();
                if (m_cancel) {
                    emit indexingFinished();
                    return;
                }
                mutexLocker.unlock();

                if (!indexMap.contains(namespaceName)) {
                    // make sure we remove some partly indexed stuff
                    removeDocuments(indexPath, namespaceName);
                } else {
                    QString path = engine.documentationFileName(namespaceName);
                    if (indexMap.value(namespaceName) < QFileInfo(path).lastModified()) {
                        // make sure we remove some outdated indexed stuff
                        indexMap.remove(namespaceName);
                        removeDocuments(indexPath, namespaceName);
                    }

                    if (indexMap.contains(namespaceName)) {
                        // make sure we really have content indexed for namespace
                        // NOTE: Extra variable just for GCC 3.3.5
                        QLatin1String key("namespace");
                        QCLuceneTermQuery query(QCLuceneTerm(key, namespaceName));
                        QCLuceneIndexSearcher indexSearcher(indexPath);
                        QCLuceneHits hits = indexSearcher.search(query);
                        if (hits.length() <= 0)
                            indexMap.remove(namespaceName);
                    }
                }
            }
            writer = new QCLuceneIndexWriter(indexPath, analyzer, false);
        } else {
            indexMap.clear();
            writer = new QCLuceneIndexWriter(indexPath, analyzer, true);
        }
#if !defined(QT_NO_EXCEPTIONS)
    } catch (...) {
        qWarning("Full Text Search, could not create index writer.");
        return;
    }
#endif

    writer->setMergeFactor(100);
    writer->setMinMergeDocs(1000);
    writer->setMaxFieldLength(QCLuceneIndexWriter::DEFAULT_MAX_FIELD_LENGTH);

    QStringList namespaces;
    foreach(const QString& namespaceName, registeredDocs) {
        mutexLocker.relock();
        if (m_cancel) {
            writer->close();
            delete writer;
            emit indexingFinished();
            return;
        }
        mutexLocker.unlock();

        namespaces.append(namespaceName);
        if (indexMap.contains(namespaceName))
            continue;

        const QList<QStringList> attributeSets =
            engine.filterAttributeSets(namespaceName);

        if (attributeSets.isEmpty()) {
            const QList<QUrl> docFiles = indexableFiles(&engine, namespaceName,
                QStringList());
            if (!addDocuments(docFiles, engine, QStringList(), namespaceName,
                writer, analyzer))
                break;
        } else {
            bool bail = false;
            foreach (const QStringList& attributes, attributeSets) {
                const QList<QUrl> docFiles = indexableFiles(&engine,
                    namespaceName, attributes);
                if (!addDocuments(docFiles, engine, attributes, namespaceName,
                    writer, analyzer)) {
                    bail = true;
                    break;
                }
            }
            if (bail)
                break;
        }

        mutexLocker.relock();
        if (!m_cancel) {
            QString path(engine.documentationFileName(namespaceName));
            indexMap.insert(namespaceName, QFileInfo(path).lastModified());
            writeIndexMap(engine, indexMap);
        }
        mutexLocker.unlock();
    }

    writer->close();
    delete writer;

    mutexLocker.relock();
    if (!m_cancel) {
        mutexLocker.unlock();

        QStringList indexedNamespaces = indexMap.keys();
        foreach(const QString& namespaceName, indexedNamespaces) {
            mutexLocker.relock();
            if (m_cancel)
                break;
            mutexLocker.unlock();

            if (!namespaces.contains(namespaceName)) {
                indexMap.remove(namespaceName);
                writeIndexMap(engine, indexMap);
                removeDocuments(indexPath, namespaceName);
            }
        }
    }
    emit indexingFinished();
}

bool QHelpSearchIndexWriter::addDocuments(const QList<QUrl> docFiles,
    const QHelpEngineCore &engine, const QStringList &attributes,
    const QString &namespaceName, QCLuceneIndexWriter *writer,
    QCLuceneAnalyzer &analyzer)
{
    QMutexLocker locker(&mutex);
    const QString attrList = attributes.join(QLatin1String(" "));

    locker.unlock();
    foreach(const QUrl& url, docFiles) {
        QCLuceneDocument document;
        DocumentHelper helper(url.toString(), engine.fileData(url));
        if (helper.addFieldsToDocument(&document, namespaceName, attrList))
            writer->addDocument(document, analyzer);

        locker.relock();
        if (m_cancel)
            return false;
        locker.unlock();
    }

    return true;
}

void QHelpSearchIndexWriter::removeDocuments(const QString &indexPath,
    const QString &namespaceName)
{
    if (namespaceName.isEmpty() || QCLuceneIndexReader::isLocked(indexPath))
        return;

    QCLuceneIndexReader reader = QCLuceneIndexReader::open(indexPath);
    reader.deleteDocuments(QCLuceneTerm(QLatin1String("namespace"),
        namespaceName));

    reader.close();
}

bool QHelpSearchIndexWriter::writeIndexMap(QHelpEngineCore& engine,
    const QMap<QString, QDateTime>& indexMap)
{
    QByteArray bArray;

    QDataStream data(&bArray, QIODevice::ReadWrite);
    data << indexMap;

    return engine.setCustomValue(QLatin1String("CluceneIndexedNamespaces"),
        bArray);
}

QList<QUrl> QHelpSearchIndexWriter::indexableFiles(QHelpEngineCore *helpEngine,
    const QString &namespaceName, const QStringList &attributes) const
{
    QList<QUrl> docFiles = helpEngine->files(namespaceName, attributes,
        QLatin1String("html"));
    docFiles += helpEngine->files(namespaceName, attributes, QLatin1String("htm"));
    docFiles += helpEngine->files(namespaceName, attributes, QLatin1String("txt"));

    return docFiles;
}


        }   // namespace clucene
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE
