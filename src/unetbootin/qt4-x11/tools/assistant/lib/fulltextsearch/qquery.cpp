/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qquery_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/search/PhraseQuery.h>
#include <CLucene/search/SearchHeader.h>

QT_BEGIN_NAMESPACE

QCLuceneQueryPrivate::QCLuceneQueryPrivate()
    : QSharedData()
{
    query = 0;
    deleteCLuceneQuery = true;
}

QCLuceneQueryPrivate::QCLuceneQueryPrivate(const QCLuceneQueryPrivate &other)
    : QSharedData()
{
    query = _CL_POINTER(other.query);
}

QCLuceneQueryPrivate::~QCLuceneQueryPrivate()
{
    if (deleteCLuceneQuery)
        _CLDECDELETE(query);
}


QCLuceneQuery::QCLuceneQuery()
    : d(new QCLuceneQueryPrivate())
{
    // nothing todo, private
}

QCLuceneQuery::~QCLuceneQuery()
{
    // nothing todo
}

void QCLuceneQuery::setBoost(qreal boost)
{
    d->query->setBoost(qreal(boost));
}

qreal QCLuceneQuery::getBoost() const
{
    return qreal(d->query->getBoost());
}

QString QCLuceneQuery::getQueryName() const
{
    return TCharToQString(d->query->getQueryName());
}

bool QCLuceneQuery::instanceOf(const QString &other) const
{
	if (other == getQueryName())
		return true;

	return false;
}

QString QCLuceneQuery::toString(const QString &field) const
{
    TCHAR *fieldName = QStringToTChar(field);
    QString retValue = TCharToQString(d->query->toString(fieldName));
    delete [] fieldName;

    return retValue;
}

quint32 QCLuceneQuery::hashCode() const
{
    return quint32(d->query->hashCode());
}

QString QCLuceneQuery::toString() const
{
    return TCharToQString(d->query->toString());
}

bool QCLuceneQuery::equals(const QCLuceneQuery &other) const
{
    return d->query->equals(other.d->query);
}


QCLucenePrefixQuery::QCLucenePrefixQuery(const QCLuceneTerm &prefix)
    : QCLuceneQuery()
    , prefix(prefix)
{
    d->query = new lucene::search::PrefixQuery(prefix.d->term);
}

QCLucenePrefixQuery::~QCLucenePrefixQuery()
{
    // nothing todo
}

QString QCLucenePrefixQuery::getClassName()
{
    return TCharToQString(lucene::search::PrefixQuery::getClassName());
}

QCLuceneTerm QCLucenePrefixQuery::getPrefix() const
{
    return prefix;
}


QCLuceneRangeQuery::QCLuceneRangeQuery(const QCLuceneTerm &lowerTerm,
                                       const QCLuceneTerm &upperTerm,
                                       bool inclusive)
    : QCLuceneQuery()
    , lowerTerm(lowerTerm)
    , upperTerm(upperTerm)
{
    d->query = new lucene::search::RangeQuery(lowerTerm.d->term,
        upperTerm.d->term, inclusive);
}

QCLuceneRangeQuery::~QCLuceneRangeQuery()
{
    // nothing todo
}

QString QCLuceneRangeQuery::getClassName()
{
    return TCharToQString(lucene::search::RangeQuery::getClassName());
}

QCLuceneTerm QCLuceneRangeQuery::getLowerTerm() const
{
    return lowerTerm;
}

QCLuceneTerm QCLuceneRangeQuery::getUpperTerm() const
{
    return upperTerm;
}

bool QCLuceneRangeQuery::isInclusive() const
{
    lucene::search::RangeQuery *query = 
        static_cast<lucene::search::RangeQuery*> (d->query);

    if (query == 0)
        return false;

    return query->isInclusive();
}

QString QCLuceneRangeQuery::getField() const
{
    lucene::search::RangeQuery *query = 
        static_cast<lucene::search::RangeQuery*> (d->query);

    if (query == 0)
        return QString();

    return TCharToQString(query->getField());
}


QCLuceneTermQuery::QCLuceneTermQuery(const QCLuceneTerm &term)
    : QCLuceneQuery()
    , term(term)
{
    d->query = new lucene::search::TermQuery(term.d->term);
}

QCLuceneTermQuery::~QCLuceneTermQuery()
{
    // nothing todo
}

QString QCLuceneTermQuery::getClassName()
{
    return TCharToQString(lucene::search::TermQuery::getClassName());
}

QCLuceneTerm QCLuceneTermQuery::getTerm() const
{
    return term;
}


QCLuceneBooleanQuery::QCLuceneBooleanQuery()
    : QCLuceneQuery()
{
    d->query = new lucene::search::BooleanQuery();
}

QCLuceneBooleanQuery::~QCLuceneBooleanQuery()
{
    qDeleteAll(queries);
}

QString QCLuceneBooleanQuery::getClassName()
{
    return TCharToQString(lucene::search::BooleanQuery::getClassName());
}

quint32 QCLuceneBooleanQuery::getClauseCount() const
{
    lucene::search::BooleanQuery *query = 
        static_cast<lucene::search::BooleanQuery*> (d->query);

    if (query == 0)
        return 1024;

    return quint32(query->getClauseCount());
}

quint32 QCLuceneBooleanQuery::getMaxClauseCount() const
{
    lucene::search::BooleanQuery *query = 
        static_cast<lucene::search::BooleanQuery*> (d->query);

    if (query == 0)
        return 1024;

    return quint32(query->getMaxClauseCount());
}

void QCLuceneBooleanQuery::setMaxClauseCount(quint32 maxClauseCount)
{
    lucene::search::BooleanQuery *query = 
        static_cast<lucene::search::BooleanQuery*> (d->query);

    if (query == 0)
        return;

    query->setMaxClauseCount(size_t(maxClauseCount));
}

void QCLuceneBooleanQuery::add(QCLuceneQuery *query, bool required, bool prohibited)
{ 
    add(query, false, required, prohibited); 
}

void QCLuceneBooleanQuery::add(QCLuceneQuery *query, bool delQuery, 
                               bool required, bool prohibited)
{
    lucene::search::BooleanQuery *booleanQuery = 
        static_cast<lucene::search::BooleanQuery*> (d->query);

    if (booleanQuery == 0)
        return;

    booleanQuery->add(query->d->query, delQuery, required, prohibited);

    if (delQuery) {
        queries.append(query);
        query->d->deleteCLuceneQuery = false;
    }
}


QCLucenePhraseQuery::QCLucenePhraseQuery()
    : QCLuceneQuery()
{
    d->query = new lucene::search::PhraseQuery();
}

QCLucenePhraseQuery::~QCLucenePhraseQuery()
{
    termList.clear();
}

QString QCLucenePhraseQuery::getClassName()
{
    return TCharToQString(lucene::search::RangeQuery::getClassName());
}

qint32 QCLucenePhraseQuery::getSlop() const
{
    lucene::search::PhraseQuery *phraseQuery = 
        static_cast<lucene::search::PhraseQuery*> (d->query);

    if (phraseQuery == 0)
        return 0;

    return qint32(phraseQuery->getSlop());
}

void QCLucenePhraseQuery::setSlop(const qint32 slop)
{
    lucene::search::PhraseQuery *phraseQuery = 
        static_cast<lucene::search::PhraseQuery*> (d->query);

    if (phraseQuery == 0)
        return;

    phraseQuery->setSlop(int32_t(slop));
}

void QCLucenePhraseQuery::addTerm(const QCLuceneTerm &term)
{
    lucene::search::PhraseQuery *phraseQuery = 
        static_cast<lucene::search::PhraseQuery*> (d->query);

    if (phraseQuery == 0)
        return;

    termList.append(term);
    phraseQuery->add(term.d->term);
}

void QCLucenePhraseQuery::addTerm(const QCLuceneTerm &term, qint32 position)
{
    lucene::search::PhraseQuery *phraseQuery = 
        static_cast<lucene::search::PhraseQuery*> (d->query);

    if (phraseQuery == 0)
        return;

    termList.insert(position, term);
    phraseQuery->add(term.d->term, int32_t(position));

}

QString QCLucenePhraseQuery::getFieldName() const
{
    lucene::search::PhraseQuery *phraseQuery = 
        static_cast<lucene::search::PhraseQuery*> (d->query);

    if (phraseQuery == 0)
        return QString();

    return TCharToQString(phraseQuery->getFieldName());
}

QList<QCLuceneTerm> QCLucenePhraseQuery::getTerms() const
{
    return termList;
}

QT_END_NAMESPACE
