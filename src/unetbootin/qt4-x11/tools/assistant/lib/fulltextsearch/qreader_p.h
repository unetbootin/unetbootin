/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QREADER_P_H
#define QREADER_P_H

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

CL_NS_DEF(util)
    class Reader;
CL_NS_END
CL_NS_USE(util)

QT_BEGIN_NAMESPACE

class QCLuceneField;
class QCLuceneAnalyzer;
class QCLuceneDocument;
class QCLuceneQueryParser;
class QCLuceneStandardTokenizer;

class QHELP_EXPORT QCLuceneReaderPrivate : public QSharedData
{
public:
    QCLuceneReaderPrivate(); 
    QCLuceneReaderPrivate(const QCLuceneReaderPrivate &other);
    
    ~QCLuceneReaderPrivate();
    
    Reader* reader;
    bool deleteCLuceneReader;

private:
    QCLuceneReaderPrivate &operator=(const QCLuceneReaderPrivate &other);
};

class QHELP_EXPORT QCLuceneReader
{
public:
    QCLuceneReader();
    virtual ~QCLuceneReader();

protected:
    friend class QCLuceneField;
    friend class QCLuceneAnalyzer;
    friend class QCLuceneDocument;
    friend class QCLuceneQueryParser;
    friend class QCLuceneStandardTokenizer;
    QSharedDataPointer<QCLuceneReaderPrivate> d;
};

class QCLuceneStringReader : public QCLuceneReader
{
public:
    QCLuceneStringReader(const QString &value);
    QCLuceneStringReader(const QString &value, qint32 length);
    QCLuceneStringReader(const QString &value, qint32 length, bool copyData);

    ~QCLuceneStringReader();

private:
    TCHAR *string;
};

class QHELP_EXPORT QCLuceneFileReader : public QCLuceneReader
{
public:
    QCLuceneFileReader(const QString &path, const QString &encoding,
                       qint32 cacheLength = 13, qint32 cacheBuffer = 14);
    ~QCLuceneFileReader();
};

QT_END_NAMESPACE

#endif  // QREADER_P_H
