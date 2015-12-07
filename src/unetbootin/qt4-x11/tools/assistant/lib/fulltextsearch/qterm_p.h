/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QTERM_P_H
#define QTERM_P_H

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

#include "qclucene_global_p.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>

CL_NS_DEF(index)
    class Term;
CL_NS_END
CL_NS_USE(index)

QT_BEGIN_NAMESPACE

class QCLuceneTermQuery;
class QCLuceneRangeQuery;
class QCLucenePrefixQuery;
class QCLuceneIndexReader;
class QCLucenePhraseQuery;

class QHELP_EXPORT QCLuceneTermPrivate : public QSharedData
{
public:
    QCLuceneTermPrivate();
    QCLuceneTermPrivate(const QCLuceneTermPrivate &other);

    ~QCLuceneTermPrivate();

    Term *term;
    bool deleteCLuceneTerm;

private:
    QCLuceneTermPrivate &operator=(const QCLuceneTermPrivate &other);
};

class QHELP_EXPORT QCLuceneTerm
{
public:
    QCLuceneTerm();
    QCLuceneTerm(const QString &field, const QString &text);
    QCLuceneTerm(const QCLuceneTerm &fieldTerm, const QString &text);

    virtual ~QCLuceneTerm();

    QString field() const;
	QString text() const;
	
	void set(const QString &field, const QString &text);
	void set(const QCLuceneTerm &fieldTerm, const QString &text);
	void set(const QString &field, const QString &text, bool internField);

    bool equals(const QCLuceneTerm &other) const;
    qint32 compareTo(const QCLuceneTerm &other) const;
	
	QString toString() const;
	quint32 hashCode() const;
    quint32 textLength() const;

protected:
    friend class QCLuceneTermQuery;
    friend class QCLuceneRangeQuery;
    friend class QCLucenePrefixQuery;
    friend class QCLuceneIndexReader;
    friend class QCLucenePhraseQuery;
    QSharedDataPointer<QCLuceneTermPrivate> d;
};

QT_END_NAMESPACE

#endif  // QTERM_P_H
