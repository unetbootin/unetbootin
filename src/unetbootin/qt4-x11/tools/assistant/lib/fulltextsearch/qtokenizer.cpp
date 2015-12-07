/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qtokenizer_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/analysis/AnalysisHeader.h>

QT_BEGIN_NAMESPACE

QCLuceneTokenizer::QCLuceneTokenizer()
    : QCLuceneTokenStream()
{
    // nothing todo
}

QCLuceneTokenizer::QCLuceneTokenizer(const QCLuceneReader &reader)
    : QCLuceneTokenStream()
    , reader(reader)
{
    // nothing todo
}

QCLuceneTokenizer::~QCLuceneTokenizer()
{
    close();
}

void QCLuceneTokenizer::close()
{
    d->tokenStream->close();
}

bool QCLuceneTokenizer::next(QCLuceneToken &token)
{
    return d->tokenStream->next(token.d->token);
}


QCLuceneStandardTokenizer::QCLuceneStandardTokenizer(const QCLuceneReader &reader)
    : QCLuceneTokenizer(reader)
{
    d->tokenStream = 
        new lucene::analysis::standard::StandardTokenizer(reader.d->reader);
}

QCLuceneStandardTokenizer::~QCLuceneStandardTokenizer()
{
    // nothing todo
}

bool QCLuceneStandardTokenizer::readApostrophe(const QString &string, 
                                               QCLuceneToken &token)
{
    lucene::analysis::standard::StandardTokenizer *stdTokenizer = 
        static_cast<lucene::analysis::standard::StandardTokenizer*> (d->tokenStream);

    if (stdTokenizer == 0)
        return false;

    TCHAR* value = QStringToTChar(string);
    lucene::util::StringBuffer buffer(value);
    bool retValue = stdTokenizer->ReadApostrophe(&buffer, token.d->token);
    delete [] value;
    
    return retValue;
}

bool QCLuceneStandardTokenizer::readAt(const QString &string, QCLuceneToken &token)
{
    lucene::analysis::standard::StandardTokenizer *stdTokenizer = 
        static_cast<lucene::analysis::standard::StandardTokenizer*> (d->tokenStream);

    if (stdTokenizer == 0)
        return false;

    TCHAR* value = QStringToTChar(string);
    lucene::util::StringBuffer buffer(value);
    bool retValue = stdTokenizer->ReadAt(&buffer, token.d->token);
    delete [] value;
    
    return retValue;
}

bool QCLuceneStandardTokenizer::readCompany(const QString &string, 
                                            QCLuceneToken &token)
{
    lucene::analysis::standard::StandardTokenizer *stdTokenizer = 
        static_cast<lucene::analysis::standard::StandardTokenizer*> (d->tokenStream);

    if (stdTokenizer == 0)
        return false;

    TCHAR* value = QStringToTChar(string);
    lucene::util::StringBuffer buffer(value);
    bool retValue = stdTokenizer->ReadCompany(&buffer, token.d->token);
    delete [] value;
    
    return retValue;
}

QT_END_NAMESPACE
