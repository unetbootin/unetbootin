/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qreader_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/util/Reader.h>

QT_BEGIN_NAMESPACE

QCLuceneReaderPrivate::QCLuceneReaderPrivate() 
    : QSharedData()
{ 
    reader = 0; 
    deleteCLuceneReader = true;
}

QCLuceneReaderPrivate::QCLuceneReaderPrivate(const QCLuceneReaderPrivate &other)
    : QSharedData()
{ 
    reader = _CL_POINTER(other.reader); 
}

QCLuceneReaderPrivate::~QCLuceneReaderPrivate() 
{ 
    if (deleteCLuceneReader)
        _CLDECDELETE(reader);
}
    

QCLuceneReader::QCLuceneReader()
    : d(new QCLuceneReaderPrivate())
{
    // nothing todo
}

QCLuceneReader::~QCLuceneReader()
{
    // nothing todo
}


QCLuceneStringReader::QCLuceneStringReader(const QString &value)
    : QCLuceneReader()
    , string(QStringToTChar(value))
{
    d->reader = new lucene::util::StringReader(string);
}

QCLuceneStringReader::QCLuceneStringReader(const QString &value, qint32 length)
    : QCLuceneReader()
    , string(QStringToTChar(value))
{
    d->reader = new lucene::util::StringReader(string, int32_t(length));
}

QCLuceneStringReader::QCLuceneStringReader(const QString &value, qint32 length, 
                                           bool copyData)
    : QCLuceneReader()
    , string(QStringToTChar(value))
{
    d->reader = new lucene::util::StringReader(string, int32_t(length), copyData);
}

QCLuceneStringReader::~QCLuceneStringReader()
{
    delete [] string;
}


QCLuceneFileReader::QCLuceneFileReader(const QString &path, const QString &encoding,
                                       qint32 cacheLength, qint32 cacheBuffer)
    : QCLuceneReader()
{
    const QByteArray tmpPath = path.toLocal8Bit();
    const QByteArray tmpEncoding = encoding.toAscii();
    d->reader = new lucene::util::FileReader(tmpPath.constData(),
        tmpEncoding.constData(), int32_t(cacheLength), int32_t(cacheBuffer));
}

QCLuceneFileReader::~QCLuceneFileReader()
{
    // nothing todo
}

QT_END_NAMESPACE
