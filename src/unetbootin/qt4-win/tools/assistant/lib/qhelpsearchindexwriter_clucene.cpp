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
#include "qhelp_global.h"
#include "fulltextsearch/qanalyzer_p.h"
#include "fulltextsearch/qdocument_p.h"
#include "fulltextsearch/qindexreader_p.h"
#include "fulltextsearch/qindexwriter_p.h"
#include "qhelpsearchindexwriter_clucene_p.h"

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

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

    bool addFieldsToDocument(QCLuceneDocument *document, const QString &namespaceName,
        const QString &attributes = QString())
    {
        if (!document)
            return false;

        if(!data.isEmpty()) {
            QString parsedData = parseData();
            QString parsedTitle = documentTitle(data);	        

            if(!parsedData.isEmpty()) {
                document->add(new QCLuceneField(QLatin1String("content"), parsedData, 
                    QCLuceneField::INDEX_TOKENIZED));
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
        QString charSet = charsetFromData(data);
        textStream.setCodec(QTextCodec::codecForName(charSet.toLatin1().constData()));

        QString stream = textStream.readAll();
        if (stream.isNull() || stream.isEmpty())
            return QString();

        return stream;
    }

    QString parseData() const
    {
        QString parsedContent;
        int length = data.length();
        const QChar *buf = data.unicode();

        QChar str[64];
        QChar c = buf[0];
        bool valid = true;     
        int j = 0, i = 0;

        while (j < length) {
            if ( c == QLatin1Char('<') || c == QLatin1Char('&') ) {
                valid = false;
                if ( i > 1 )
                    parsedContent += QLatin1String(" ") + QString(str,i);

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
                    parsedContent += QLatin1String(" ") + QString(str,i);

                i = 0;
            }
            c = buf[++j];
        }
        
        if ( i > 1 )
            parsedContent += QLatin1String(" ") + QString(str,i);

        return parsedContent;
    }

private:
    QString fileName;
    QString data;
};


QHelpSearchIndexWriter::QHelpSearchIndexWriter()
    : QThread(0)
    , m_cancel(false)
    , m_collectionFile(QString())
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

void QHelpSearchIndexWriter::updateIndex(const QString &collectionFile, bool reindex)
{
    QMutexLocker locker(&mutex);

    this->m_cancel = false;
    this->m_reindex = reindex;
    this->m_collectionFile = collectionFile;

    start(QThread::NormalPriority);
}

void QHelpSearchIndexWriter::optimizeIndex(const QString &indexFilesPath)
{
    if (QCLuceneIndexReader::indexExists(indexFilesPath)) {
        if (QCLuceneIndexReader::isLocked(indexFilesPath))
            QCLuceneIndexReader::unlock(indexFilesPath);

        QCLuceneStandardAnalyzer analyzer;
        QCLuceneIndexWriter writer(indexFilesPath, analyzer, false);
        writer.optimize();
        writer.close();
    }
}

void QHelpSearchIndexWriter::run()
{
    mutex.lock();

    if (m_cancel) {
        mutex.unlock();
        return;
    }
    
    const bool reindex = this->m_reindex;
    const QLatin1String key("CluceneSearchNamespaces");
    const QString collectionFile(this->m_collectionFile);
    
    mutex.unlock();

    QHelpEngineCore engine(collectionFile, 0);
    if (!engine.setupData())
        return;

    if (reindex)
        engine.setCustomValue(key, QLatin1String(""));

    const QStringList indexedNamespaces = engine.customValue(key).toString().
        split(QLatin1String("|"), QString::SkipEmptyParts);

    QCLuceneIndexWriter *writer = 0;
    QCLuceneStandardAnalyzer analyzer;
    QString indexPath;

#if !defined(QT_NO_EXCEPTIONS)
    try {
#endif
        indexPath = engine.customValue(QLatin1String("indexFilesFolder")).toString();
        QDir dir(indexPath);
        if (!dir.isAbsolute()) {
            indexPath = QFileInfo(engine.collectionFile()).path() +
                dir.separator() + dir.path();
        }

        if (QCLuceneIndexReader::indexExists(indexPath) && !reindex) {
            if (QCLuceneIndexReader::isLocked(indexPath))
                QCLuceneIndexReader::unlock(indexPath);
            writer = new QCLuceneIndexWriter(indexPath, analyzer, false);
        } else
            writer = new QCLuceneIndexWriter(indexPath, analyzer, true);
#if !defined(QT_NO_EXCEPTIONS)
    } catch (...) {
        qWarning("Full Text Search, could not create index writer.");
        return;
    }
#endif

    writer->setMaxFieldLength(QCLuceneIndexWriter::DEFAULT_MAX_FIELD_LENGTH);

    QStringList namespaces;
    const QStringList registeredDocs = engine.registeredDocumentations();

    emit indexingStarted();

    foreach(const QString namespaceName, registeredDocs) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            break;
        }
        mutex.unlock();

        namespaces.append(namespaceName);
        if (indexedNamespaces.contains(namespaceName))
            continue;

        // make sure we remove some partly indexed stuff
        removeDocuments(indexPath, namespaceName);
        
        const QList<QStringList> attributeSets = 
            engine.filterAttributeSets(namespaceName);

        if (attributeSets.isEmpty()) {
            const QList<QUrl> docFiles = 
                engine.files(namespaceName, QStringList(), QLatin1String("html"));
            
            if (!addDocuments(docFiles, engine, QStringList(), namespaceName,
                writer, analyzer))
                break;
        } else {
            foreach (const QStringList attributes, attributeSets) {
                const QList<QUrl> docFiles = 
                    engine.files(namespaceName, attributes, QLatin1String("html"));

                if (!addDocuments(docFiles, engine, attributes, namespaceName,
                    writer, analyzer))
                    break;
            }
        }
        mutex.lock();
        if (!m_cancel) {
            engine.setCustomValue(key, addNamespace(
                engine.customValue(key).toString(), namespaceName));
        }
        mutex.unlock();
    }

    writer->close();
    delete writer;

    mutex.lock();
    if (m_cancel) {
        mutex.unlock();
        emit indexingFinished();
        return;
    }
    mutex.unlock();

    QStringListIterator qsli(indexedNamespaces);
    while (qsli.hasNext()) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            break;
        }
        mutex.unlock();

        const QString namespaceName = qsli.next();
        if (namespaces.contains(namespaceName))
            continue;

        engine.setCustomValue(key, removeNamespace(
            engine.customValue(key).toString(), namespaceName));
    }

    emit indexingFinished();
}

bool QHelpSearchIndexWriter::addDocuments(const QList<QUrl> docFiles,
                                          const QHelpEngineCore &engine,
                                          const QStringList &attributes,
                                          const QString &namespaceName,
                                          QCLuceneIndexWriter *writer,
                                          QCLuceneAnalyzer &analyzer)
{
    foreach(const QUrl url, docFiles) {
        mutex.lock();
        if (m_cancel) {
            mutex.unlock();
            return false;
        }
        mutex.unlock();

        QCLuceneDocument document;
        DocumentHelper helper(url.toString(), engine.fileData(url));
        const QString list = attributes.join(QLatin1String(" "));
        if (helper.addFieldsToDocument(&document, namespaceName, list))
            writer->addDocument(document, analyzer);
        document.clear();
    }

    return true;
}

void QHelpSearchIndexWriter::removeDocuments(const QString &indexPath,
                                             const QString &namespaceName)
{
    if (!QCLuceneIndexReader::indexExists(indexPath) || namespaceName.isEmpty())
        return;

    if (QCLuceneIndexReader::isLocked(indexPath))
        QCLuceneIndexReader::unlock(indexPath);

    QCLuceneIndexReader reader = QCLuceneIndexReader::open(indexPath);
    reader.deleteDocuments(QCLuceneTerm(QLatin1String("namespace"), namespaceName));

    reader.close();
}

QString QHelpSearchIndexWriter::addNamespace(const QString namespaces, 
                                             const QString &namespaceName)
{
    QString value = namespaces;
    if (!value.contains(namespaceName))
        value.append(namespaceName).append(QLatin1String("|"));

    return value;
}

QString QHelpSearchIndexWriter::removeNamespace(const QString namespaces, 
                                                 const QString &namespaceName)
{
    QString value = namespaces;
    if (value.contains(namespaceName))
        value.remove(namespaceName + QLatin1String("|"));

    return value;
}

        }   // namespace clucene
    }   // namespace fulltextsearch
}   // namespace qt

QT_END_NAMESPACE
