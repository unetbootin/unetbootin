/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qindexreader_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/index/IndexReader.h>

QT_BEGIN_NAMESPACE

QCLuceneIndexReaderPrivate::QCLuceneIndexReaderPrivate()
    : QSharedData()
{
    reader = 0;
    deleteCLuceneIndexReader = true;
}

QCLuceneIndexReaderPrivate::QCLuceneIndexReaderPrivate(const QCLuceneIndexReaderPrivate &other)
    : QSharedData()
{
    reader = _CL_POINTER(other.reader);
}

QCLuceneIndexReaderPrivate::~QCLuceneIndexReaderPrivate()
{
    if (deleteCLuceneIndexReader)
        _CLDECDELETE(reader);
}


QCLuceneIndexReader::QCLuceneIndexReader()
    : d(new QCLuceneIndexReaderPrivate())
{
    // nothing todo, private
}

QCLuceneIndexReader::~QCLuceneIndexReader()
{
    // nothing todo
}

bool QCLuceneIndexReader::isLuceneFile(const QString &filename)
{
    using namespace lucene::index;

    const QByteArray tmpPath = filename.toLocal8Bit();
    return IndexReader::isLuceneFile(tmpPath.constData());
}

bool QCLuceneIndexReader::indexExists(const QString &directory)
{
    using namespace lucene::index;

    const QByteArray tmpPath = directory.toLocal8Bit();
    return IndexReader::indexExists(tmpPath.constData());
}

QCLuceneIndexReader QCLuceneIndexReader::open(const QString &path)
{
    using namespace lucene::index;

    QCLuceneIndexReader indexReader;
    const QByteArray tmpPath = path.toLocal8Bit();
    indexReader.d->reader = IndexReader::open(tmpPath.constData());
    
    return indexReader;
}

void QCLuceneIndexReader::unlock(const QString &path)
{
    using namespace lucene::index;

    const QByteArray tmpPath = path.toLocal8Bit();
    IndexReader::unlock(tmpPath.constData());
}

bool QCLuceneIndexReader::isLocked(const QString &directory)
{
    using namespace lucene::index;

    const QByteArray tmpPath = directory.toLocal8Bit();
    return IndexReader::isLocked(tmpPath.constData());
}

quint64 QCLuceneIndexReader::lastModified(const QString &directory)
{
    using namespace lucene::index;

    const QByteArray tmpPath = directory.toLocal8Bit();
    return quint64(IndexReader::lastModified(tmpPath.constData()));
}

qint64 QCLuceneIndexReader::getCurrentVersion(const QString &directory)
{
    using namespace lucene::index;

    const QByteArray tmpPath = directory.toLocal8Bit();
    return qint64(IndexReader::getCurrentVersion(tmpPath.constData()));
}

void QCLuceneIndexReader::close()
{
    d->reader->close();
}

bool QCLuceneIndexReader::isCurrent()
{
    return d->reader->isCurrent();
}

void QCLuceneIndexReader::undeleteAll()
{
    d->reader->undeleteAll();
}

qint64 QCLuceneIndexReader::getVersion()
{
    return qint64(d->reader->getVersion());
}

void QCLuceneIndexReader::deleteDocument(qint32 docNum)
{
    d->reader->deleteDocument(int32_t(docNum));
}

bool QCLuceneIndexReader::hasNorms(const QString &field)
{
    TCHAR *fieldName = QStringToTChar(field);
    bool retValue = d->reader->hasNorms(fieldName);
    delete [] fieldName;

    return retValue;
}

qint32 QCLuceneIndexReader::deleteDocuments(const QCLuceneTerm &term)
{
    return d->reader->deleteDocuments(term.d->term);
}

bool QCLuceneIndexReader::document(qint32 index, QCLuceneDocument &document)
{
    if (!document.d->document)
        document.d->document = new lucene::document::Document();

    if (d->reader->document(int32_t(index), document.d->document))
        return true;

    return false;
}

void QCLuceneIndexReader::setNorm(qint32 doc, const QString &field, qreal value)
{
    TCHAR *fieldName = QStringToTChar(field);
    d->reader->setNorm(int32_t(doc), fieldName, float_t(value));
    delete [] fieldName;
}

void QCLuceneIndexReader::setNorm(qint32 doc, const QString &field, quint8 value)
{
    TCHAR *fieldName = QStringToTChar(field);
    d->reader->setNorm(int32_t(doc), fieldName, uint8_t(value));
    delete [] fieldName;
}

QT_END_NAMESPACE
