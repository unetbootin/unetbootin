/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QTOKENSTREAM_P_H
#define QTOKENSTREAM_P_H

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

#include "qtoken_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(analysis)
    class TokenStream;
CL_NS_END
CL_NS_USE(analysis)

QT_BEGIN_NAMESPACE

class QCLuceneAnalyzer;
class QCLuceneTokenizer;
class QCLuceneStopAnalyzer;
class QCLuceneSimpleAnalyzer;
class QCLuceneKeywordAnalyzer;
class QCLuceneStandardAnalyzer;
class QCLuceneWhitespaceAnalyzer;
class QCLucenePerFieldAnalyzerWrapper;

class QHELP_EXPORT QCLuceneTokenStreamPrivate : public QSharedData
{
public:
    QCLuceneTokenStreamPrivate();
    QCLuceneTokenStreamPrivate(const QCLuceneTokenStreamPrivate &other);

    ~QCLuceneTokenStreamPrivate();

    TokenStream *tokenStream;
    bool deleteCLuceneTokenStream;

private:
    QCLuceneTokenStreamPrivate &operator=(const QCLuceneTokenStreamPrivate &other);
};

class QHELP_EXPORT QCLuceneTokenStream
{
public:
    virtual ~QCLuceneTokenStream();

    void close();
    bool next(QCLuceneToken &token);

protected:
    friend class QCLuceneAnalyzer;
    friend class QCLuceneTokenizer;
    friend class QCLuceneStopAnalyzer;
    friend class QCLuceneSimpleAnalyzer;
    friend class QCLuceneKeywordAnalyzer;
    friend class QCLuceneStandardAnalyzer;
    friend class QCLuceneWhitespaceAnalyzer;
    friend class QCLucenePerFieldAnalyzerWrapper;
    QSharedDataPointer<QCLuceneTokenStreamPrivate> d;

private:
    QCLuceneTokenStream();
};

QT_END_NAMESPACE

#endif  // QTOKENSTREAM_P_H
