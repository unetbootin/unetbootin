/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qqueryparser_p.h"
#include "qquery_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/queryParser/QueryParser.h>

QT_BEGIN_NAMESPACE

QCLuceneQueryParserPrivate::QCLuceneQueryParserPrivate()
    : QSharedData()
{
    queryParser = 0;
    deleteCLuceneQueryParser = true;
}

QCLuceneQueryParserPrivate::QCLuceneQueryParserPrivate(const QCLuceneQueryParserPrivate &other)
    : QSharedData()
{
    queryParser = _CL_POINTER(other.queryParser);
}

QCLuceneQueryParserPrivate::~QCLuceneQueryParserPrivate()
{
    if (deleteCLuceneQueryParser)
        _CLDECDELETE(queryParser);
}


QCLuceneQueryParser::QCLuceneQueryParser(const QString &field, 
                                         QCLuceneAnalyzer &analyzer)
    : d(new QCLuceneQueryParserPrivate())
    , field(field)
    , analyzer(analyzer)
{
    TCHAR *fieldName = QStringToTChar(field);
    
    d->queryParser = new lucene::queryParser::QueryParser(fieldName, 
        analyzer.d->analyzer);

    delete [] fieldName;
}

QCLuceneQueryParser::~QCLuceneQueryParser()
{
    // nothing todo
}

QCLuceneQuery* QCLuceneQueryParser::parse(const QString &query)
{
    TCHAR *string = QStringToTChar(query);

    QCLuceneQuery *retValue = 0;
    lucene::search::Query* q = d->queryParser->parse(string);
    if (q) {
        retValue = new QCLuceneQuery();
        retValue->d->query = q;
    }

    delete [] string;
    return retValue;
}

QCLuceneQuery* QCLuceneQueryParser::parse(QCLuceneReader &reader)
{
    QCLuceneQuery *retValue = 0;
    lucene::search::Query* q = d->queryParser->parse(reader.d->reader);
    if (q) {
        retValue = new QCLuceneQuery();
        retValue->d->query = q;
    }

    return retValue;
}

QCLuceneQuery* QCLuceneQueryParser::parse(const QString &query, const QString &field,
                                          QCLuceneAnalyzer &analyzer)
{
    QCLuceneQueryParser parser(field, analyzer);
    return parser.parse(query);
}

QCLuceneAnalyzer QCLuceneQueryParser::getAnalyzer()
{
    return analyzer;
}

QString QCLuceneQueryParser::getField()
{
    return field;
}


QCLuceneMultiFieldQueryParser::QCLuceneMultiFieldQueryParser(
    const QStringList &fieldList, QCLuceneAnalyzer &analyzer)
    : QCLuceneQueryParser(QLatin1String(""), analyzer)
{
    Q_UNUSED(fieldList)
}

QCLuceneMultiFieldQueryParser::~QCLuceneMultiFieldQueryParser()
{
    // nothing todo
}

QCLuceneQuery* QCLuceneMultiFieldQueryParser::parse(const QString &query, 
                                                    const QStringList &fieldList,
                                                    QCLuceneAnalyzer &analyzer)
{
    QCLuceneBooleanQuery *retValue = new QCLuceneBooleanQuery();
    foreach (const QString &field, fieldList) {
        QCLuceneQuery *q = QCLuceneQueryParser::parse(query, field, analyzer);
        if (!q) {
            delete retValue;
            retValue = 0; break;
        } else {
            retValue->add(q, true, false, false);
        }
    }

    return retValue;
}

QCLuceneQuery* QCLuceneMultiFieldQueryParser::parse(const QString &query,
                                                    const QStringList &fieldList,
                                                    QList<FieldFlags> flags, 
                                                    QCLuceneAnalyzer &analyzer)
{
    QCLuceneBooleanQuery *retValue = new QCLuceneBooleanQuery();
    qint32 i = 0;
    foreach (const QString &field, fieldList) {
		QCLuceneQuery *q = QCLuceneQueryParser::parse(query, field, analyzer);
        if (q) {
            qint32 flag = flags.at(i);
            switch (flag) {
                case QCLuceneMultiFieldQueryParser::REQUIRED_FIELD: {    
                    retValue->add(q, true, true, false);
                }   break;

                case QCLuceneMultiFieldQueryParser::PROHIBITED_FIELD: {
                    retValue->add(q, true, false, true);
                }   break;

                default: {
                    retValue->add(q, true, false, false);
                }   break;
            }

            ++i;
        } else {
            delete retValue;
            retValue = 0; break;
        }
    }
    return retValue;
}

QT_END_NAMESPACE
