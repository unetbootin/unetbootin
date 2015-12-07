/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QFIELD_P_H
#define QFIELD_P_H

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

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(document)
    class Field;
CL_NS_END
CL_NS_USE(document)

QT_BEGIN_NAMESPACE

class QCLuceneReader;
class QCLuceneDocument;

class QHELP_EXPORT QCLuceneFieldPrivate : public QSharedData
{
public:
    QCLuceneFieldPrivate();
    QCLuceneFieldPrivate(const QCLuceneFieldPrivate &other);

    ~QCLuceneFieldPrivate();

    Field *field;
    bool deleteCLuceneField;

private:
    QCLuceneFieldPrivate &operator=(const QCLuceneFieldPrivate &other);
};

class QHELP_EXPORT QCLuceneField
{
public:
	enum Store { 
		STORE_YES = 1, 
		STORE_NO = 2,
	    STORE_COMPRESS = 4
	};

	enum Index { 
		INDEX_NO = 16, 
		INDEX_TOKENIZED = 32, 
		INDEX_UNTOKENIZED = 64, 
		INDEX_NONORMS = 128
	};

	enum TermVector {
		TERMVECTOR_NO = 256,
		TERMVECTOR_YES = 512,    
		TERMVECTOR_WITH_POSITIONS = 1024,
		TERMVECTOR_WITH_OFFSETS = 2048
	};

	QCLuceneField(const QString &name, const QString &value, int configs);
	QCLuceneField(const QString &name, QCLuceneReader *reader, int configs);
    ~QCLuceneField();

    QString name() const;
    QString stringValue() const;
    QCLuceneReader* readerValue() const;
	bool isStored() const;
	bool isIndexed() const;
	bool isTokenized() const;
	bool isCompressed() const;
	void setConfig(int termVector);
	bool isTermVectorStored() const;
	bool isStoreOffsetWithTermVector() const;
	bool isStorePositionWithTermVector() const;
	qreal getBoost() const;
	void setBoost(qreal value);
	bool isBinary() const;
	bool getOmitNorms() const;
	void setOmitNorms(bool omitNorms);
	QString toString() const;

protected:
    QCLuceneField();
    friend class QCLuceneDocument;
    QSharedDataPointer<QCLuceneFieldPrivate> d;

private:
    QCLuceneReader* reader;
};

QT_END_NAMESPACE

#endif  // QFIELD_P_H
