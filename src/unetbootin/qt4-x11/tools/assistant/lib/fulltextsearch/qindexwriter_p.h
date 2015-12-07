/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QINDEXWRITER_P_H
#define QINDEXWRITER_P_H

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

#include "qanalyzer_p.h"
#include "qdocument_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(index)
    class IndexWriter;
CL_NS_END
CL_NS_USE(index)

QT_BEGIN_NAMESPACE

class QCLuceneIndexReader;

class QHELP_EXPORT QCLuceneIndexWriterPrivate : public QSharedData
{
public:
    QCLuceneIndexWriterPrivate();
    QCLuceneIndexWriterPrivate(const QCLuceneIndexWriterPrivate &other);

    ~QCLuceneIndexWriterPrivate();

    IndexWriter *writer;
    bool deleteCLuceneIndexWriter;

private:
    QCLuceneIndexWriterPrivate &operator=(const QCLuceneIndexWriterPrivate &other);
};

class QHELP_EXPORT QCLuceneIndexWriter
{
public:
    enum {
        DEFAULT_MERGE_FACTOR = 10,
        COMMIT_LOCK_TIMEOUT = 10000,
        DEFAULT_MAX_BUFFERED_DOCS = 10,
        DEFAULT_MAX_FIELD_LENGTH = 10000,
        DEFAULT_TERM_INDEX_INTERVAL = 128,
        DEFAULT_MAX_MERGE_DOCS = 0x7FFFFFFFL
    };

	QCLuceneIndexWriter(const QString &path, QCLuceneAnalyzer &analyzer, 
        bool create, bool closeDir = true);
    virtual ~QCLuceneIndexWriter();

	void close();
	void optimize();
    qint32 docCount();
    QCLuceneAnalyzer getAnalyzer();
	
    void addIndexes(const QList<QCLuceneIndexReader*> &readers);
    void addDocument(QCLuceneDocument &doc, QCLuceneAnalyzer &analyzer);

	qint32 getMaxFieldLength() const;
	void setMaxFieldLength(qint32 value);
	
    qint32 getMaxBufferedDocs() const;
    void setMaxBufferedDocs(qint32 value);
	
	qint64 getWriteLockTimeout() const;
    void setWriteLockTimeout(qint64 writeLockTimeout);
	
    qint64 getCommitLockTimeout() const;
	void setCommitLockTimeout(qint64 commitLockTimeout);

	qint32 getMergeFactor() const;
	void setMergeFactor(qint32 value);

    qint32 getTermIndexInterval() const;
	void setTermIndexInterval(qint32 interval);
	
	qint32 getMinMergeDocs() const;
	void setMinMergeDocs(qint32 value);

	qint32 getMaxMergeDocs() const;
	void setMaxMergeDocs(qint32 value);

    bool getUseCompoundFile() const;
	void setUseCompoundFile(bool value);

protected:
    QSharedDataPointer<QCLuceneIndexWriterPrivate> d;

private:
    QCLuceneAnalyzer analyzer;
};

QT_END_NAMESPACE

#endif  // QINDEXWRITER_P_H
