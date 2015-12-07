/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QANALYZER_P_H
#define QANALYZER_P_H

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

#include "qreader_p.h"
#include "qtokenstream_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(analysis)
    class Analyzer;
CL_NS_END
CL_NS_USE(analysis)

QT_BEGIN_NAMESPACE

class QCLuceneIndexWriter;
class QCLuceneQueryParser;
class QCLuceneStopAnalyzer;
class QCLuceneSimpleAnalyzer;
class QCLuceneKeywordAnalyzer;
class QCLuceneStandardAnalyzer;
class QCLuceneWhitespaceAnalyzer;
class QCLucenePerFieldAnalyzerWrapper;

class QHELP_EXPORT QCLuceneAnalyzerPrivate : public QSharedData
{
public:
    QCLuceneAnalyzerPrivate();
    QCLuceneAnalyzerPrivate(const QCLuceneAnalyzerPrivate &other);

    ~QCLuceneAnalyzerPrivate();

    Analyzer *analyzer;
    bool deleteCLuceneAnalyzer;

private:
    QCLuceneAnalyzerPrivate &operator=(const QCLuceneAnalyzerPrivate &other);
};

class QHELP_EXPORT QCLuceneAnalyzer
{
public:
    virtual ~QCLuceneAnalyzer();

    qint32 positionIncrementGap(const QString &fieldName) const;
    QCLuceneTokenStream tokenStream(const QString &fieldName, 
                                    const QCLuceneReader &reader) const;

protected:
    friend class QCLuceneIndexWriter;
    friend class QCLuceneQueryParser;
    friend class QCLuceneStopAnalyzer;
    friend class QCLuceneSimpleAnalyzer;
    friend class QCLuceneKeywordAnalyzer;
    friend class QCLuceneStandardAnalyzer;
    friend class QCLuceneWhitespaceAnalyzer;
    friend class QCLucenePerFieldAnalyzerWrapper;
    QSharedDataPointer<QCLuceneAnalyzerPrivate> d;

private:
    QCLuceneAnalyzer();
};

class QHELP_EXPORT QCLuceneStandardAnalyzer : public QCLuceneAnalyzer
{
public:
    QCLuceneStandardAnalyzer();
    QCLuceneStandardAnalyzer(const QStringList &stopWords);

    ~QCLuceneStandardAnalyzer();
};

class QHELP_EXPORT QCLuceneWhitespaceAnalyzer : public QCLuceneAnalyzer
{
public:
    QCLuceneWhitespaceAnalyzer();
    ~QCLuceneWhitespaceAnalyzer();
};

class QHELP_EXPORT QCLuceneSimpleAnalyzer : public QCLuceneAnalyzer
{
public:
    QCLuceneSimpleAnalyzer();
    ~QCLuceneSimpleAnalyzer();
};

class QHELP_EXPORT QCLuceneStopAnalyzer : public QCLuceneAnalyzer
{
public:
    QCLuceneStopAnalyzer();
    QCLuceneStopAnalyzer(const QStringList &stopWords);

    ~QCLuceneStopAnalyzer();

    QStringList englishStopWords() const;
};

class QHELP_EXPORT QCLuceneKeywordAnalyzer : public QCLuceneAnalyzer
{
public:
    QCLuceneKeywordAnalyzer();
    ~QCLuceneKeywordAnalyzer();
};

class QHELP_EXPORT QCLucenePerFieldAnalyzerWrapper : public QCLuceneAnalyzer
{
public:
    QCLucenePerFieldAnalyzerWrapper(QCLuceneAnalyzer *defaultAnalyzer);
    ~QCLucenePerFieldAnalyzerWrapper();

    void addAnalyzer(const QString &fieldName, QCLuceneAnalyzer *analyzer);

private:
    QList<QCLuceneAnalyzer*> analyzers;
};

QT_END_NAMESPACE

#endif  // QANALYZER_P_H
