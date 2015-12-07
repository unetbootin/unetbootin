/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QQUERY_P_H
#define QQUERY_P_H

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

#include "qterm_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(search)
    class Query;
CL_NS_END
CL_NS_USE(search)

QT_BEGIN_NAMESPACE

class QCLuceneHits;
class QCLuceneTermQuery;
class QCLuceneRangeQuery;
class QCLuceneQueryParser;
class QCLucenePrefixQuery;
class QCLuceneBooleanQuery;
class QCLucenePhraseQuery;

class QHELP_EXPORT QCLuceneQueryPrivate : public QSharedData
{
public:
    QCLuceneQueryPrivate();
    QCLuceneQueryPrivate(const QCLuceneQueryPrivate &other);

    ~QCLuceneQueryPrivate();

    Query *query;
    bool deleteCLuceneQuery;

private:
    QCLuceneQueryPrivate &operator=(const QCLuceneQueryPrivate &other);
};

class QHELP_EXPORT QCLuceneQuery
{
public:
    virtual ~QCLuceneQuery();

    void setBoost(qreal boost);
    qreal getBoost() const;
    QString getQueryName() const;
    bool instanceOf(const QString &other) const;
    QString toString(const QString &field) const;    
    quint32 hashCode() const;
    QString toString() const;
    bool equals(const QCLuceneQuery &other) const;

protected:
    friend class QCLuceneHits;
    friend class QCLuceneTermQuery;
    friend class QCLuceneRangeQuery;
    friend class QCLucenePrefixQuery;
    friend class QCLuceneQueryParser;
    friend class QCLuceneBooleanQuery;
    friend class QCLucenePhraseQuery;
    QSharedDataPointer<QCLuceneQueryPrivate> d;

private:
    QCLuceneQuery();
};

class QHELP_EXPORT QCLucenePrefixQuery : public QCLuceneQuery
{
public:
    QCLucenePrefixQuery(const QCLuceneTerm &prefix);
    ~QCLucenePrefixQuery();

    static QString getClassName();
	    
	QCLuceneTerm getPrefix() const;

private:
    QCLuceneTerm prefix;
};

class QHELP_EXPORT QCLuceneRangeQuery : public QCLuceneQuery
{
public:
    QCLuceneRangeQuery(const QCLuceneTerm &lowerTerm, 
        const QCLuceneTerm &upperTerm, bool inclusive);
    ~QCLuceneRangeQuery();

    static QString getClassName();
	    
	QCLuceneTerm getLowerTerm() const;
    QCLuceneTerm getUpperTerm() const;

    bool isInclusive() const;
    QString getField() const;

private:
    QCLuceneTerm lowerTerm;
    QCLuceneTerm upperTerm;
};

class QHELP_EXPORT QCLuceneTermQuery : public QCLuceneQuery
{
public:
    QCLuceneTermQuery(const QCLuceneTerm &term);
    ~QCLuceneTermQuery();

    static QString getClassName();
	    
	QCLuceneTerm getTerm() const;

private:
    QCLuceneTerm term;
};

class QHELP_EXPORT QCLuceneBooleanQuery : public QCLuceneQuery
{
public:
    QCLuceneBooleanQuery();
    ~QCLuceneBooleanQuery();

    static QString getClassName();

    quint32 getClauseCount() const;
    quint32 getMaxClauseCount() const;
    void setMaxClauseCount(quint32 maxClauseCount);

    void add(QCLuceneQuery *query, bool required, bool prohibited);
    void add(QCLuceneQuery *query, bool delQuery, bool required, bool prohibited);

private:
    QList<QCLuceneQuery*> queries;
};

class QHELP_EXPORT QCLucenePhraseQuery : public QCLuceneQuery
{
public:
    QCLucenePhraseQuery();
    ~QCLucenePhraseQuery();

    static QString getClassName();

    qint32 getSlop() const;
    void setSlop(const qint32 slop);
    
    void addTerm(const QCLuceneTerm &term);
    void addTerm(const QCLuceneTerm &term, qint32 position);

    QString getFieldName() const;
    QList<QCLuceneTerm> getTerms() const;

private:
    QList<QCLuceneTerm> termList;
};

QT_END_NAMESPACE

#endif  // QQUERY_P_H
