/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qanalyzer_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/analysis/AnalysisHeader.h>

QT_BEGIN_NAMESPACE

QCLuceneAnalyzerPrivate::QCLuceneAnalyzerPrivate()
    : QSharedData()
{
    analyzer = 0;
    deleteCLuceneAnalyzer = true;
}

QCLuceneAnalyzerPrivate::QCLuceneAnalyzerPrivate(const QCLuceneAnalyzerPrivate &other)
    : QSharedData()
{
    analyzer = _CL_POINTER(other.analyzer);
}

QCLuceneAnalyzerPrivate::~QCLuceneAnalyzerPrivate()
{
    if (deleteCLuceneAnalyzer)
        _CLDECDELETE(analyzer);
}


QCLuceneAnalyzer::QCLuceneAnalyzer()
    : d(new QCLuceneAnalyzerPrivate())
{
    //nothing todo, private
}

QCLuceneAnalyzer::~QCLuceneAnalyzer()
{
    // nothing todo
}

qint32 QCLuceneAnalyzer::positionIncrementGap(const QString &fieldName) const
{
    Q_UNUSED(fieldName);
    return 0;
}

QCLuceneTokenStream QCLuceneAnalyzer::tokenStream(const QString &fieldName,
                                                  const QCLuceneReader &reader) const
{
    TCHAR *fName = QStringToTChar(fieldName);
    QCLuceneTokenStream tokenStream;
    tokenStream.d->tokenStream = d->analyzer->tokenStream(fName, reader.d->reader);
    delete [] fName;

    return tokenStream;
}


QCLuceneStandardAnalyzer::QCLuceneStandardAnalyzer()
    : QCLuceneAnalyzer()
{
    d->analyzer = new lucene::analysis::standard::StandardAnalyzer();
}

QCLuceneStandardAnalyzer::~QCLuceneStandardAnalyzer()
{
    // nothing todo
}

QCLuceneStandardAnalyzer::QCLuceneStandardAnalyzer(const QStringList &stopWords)
{
    const TCHAR **tArray = new const TCHAR*[stopWords.count() +1];

    for(int i = 0; i < stopWords.count(); ++i) {
        TCHAR *stopWord = QStringToTChar(stopWords.at(i));
        tArray[i] = STRDUP_TtoT(stopWord);
        delete [] stopWord;
    }
    tArray[stopWords.count()] = 0;

    d->analyzer = new lucene::analysis::standard::StandardAnalyzer(tArray);
}


QCLuceneWhitespaceAnalyzer::QCLuceneWhitespaceAnalyzer()
    : QCLuceneAnalyzer()
{
    d->analyzer = new lucene::analysis::WhitespaceAnalyzer();
}

QCLuceneWhitespaceAnalyzer::~QCLuceneWhitespaceAnalyzer()
{
    // nothing todo
}


QCLuceneSimpleAnalyzer::QCLuceneSimpleAnalyzer()
    : QCLuceneAnalyzer()
{
    d->analyzer = new lucene::analysis::SimpleAnalyzer();
}

QCLuceneSimpleAnalyzer::~QCLuceneSimpleAnalyzer()
{
    // nothing todo
}


QCLuceneStopAnalyzer::QCLuceneStopAnalyzer()
    : QCLuceneAnalyzer()
{
    d->analyzer = new lucene::analysis::StopAnalyzer();
}

QCLuceneStopAnalyzer::~QCLuceneStopAnalyzer()
{
    // nothing todo
}

QCLuceneStopAnalyzer::QCLuceneStopAnalyzer(const QStringList &stopWords)
    : QCLuceneAnalyzer()
{
    const TCHAR **tArray = new const TCHAR*[stopWords.count() +1];

    for(int i = 0; i < stopWords.count(); ++i) {
        TCHAR *stopWord = QStringToTChar(stopWords.at(i));
        tArray[i] = STRDUP_TtoT(stopWord);
        delete [] stopWord;
    }
    tArray[stopWords.count()] = 0;

    d->analyzer = new lucene::analysis::StopAnalyzer(tArray);
}

QStringList QCLuceneStopAnalyzer::englishStopWords() const
{
    QStringList stopWordList;

    const TCHAR** stopWords = lucene::analysis::StopAnalyzer::ENGLISH_STOP_WORDS;
    for (qint32 i = 0; stopWords[i] != 0; ++i)
        stopWordList.append(TCharToQString(stopWords[i]));

    return stopWordList;
}


QCLuceneKeywordAnalyzer::QCLuceneKeywordAnalyzer()
    : QCLuceneAnalyzer()
{
    d->analyzer = new lucene::analysis::KeywordAnalyzer();
}

QCLuceneKeywordAnalyzer::~QCLuceneKeywordAnalyzer()
{
    // nothing todo
}


QCLucenePerFieldAnalyzerWrapper::QCLucenePerFieldAnalyzerWrapper(
    QCLuceneAnalyzer *defaultAnalyzer) 
    : QCLuceneAnalyzer()
{
    d->analyzer = new 
        lucene::analysis::PerFieldAnalyzerWrapper(defaultAnalyzer->d->analyzer);
    
    analyzers.append(defaultAnalyzer);
    defaultAnalyzer->d->deleteCLuceneAnalyzer = false;
}

QCLucenePerFieldAnalyzerWrapper::~QCLucenePerFieldAnalyzerWrapper()
{
    qDeleteAll(analyzers);
}

void QCLucenePerFieldAnalyzerWrapper::addAnalyzer(const QString &fieldName, 
                                                  QCLuceneAnalyzer *analyzer)
{
    lucene::analysis::PerFieldAnalyzerWrapper *analyzerWrapper = 
        static_cast<lucene::analysis::PerFieldAnalyzerWrapper*> (d->analyzer);

    if (analyzerWrapper == 0)
        return;

    analyzers.append(analyzer);
    analyzer->d->deleteCLuceneAnalyzer = false;

    TCHAR *fName = QStringToTChar(fieldName);
    analyzerWrapper->addAnalyzer(fName, analyzer->d->analyzer);
    delete [] fName;
}

QT_END_NAMESPACE
