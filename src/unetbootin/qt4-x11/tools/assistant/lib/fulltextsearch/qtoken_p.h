/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QTOKEN_P_H
#define QTOKEN_P_H

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

CL_NS_DEF(analysis)
    class Token;
CL_NS_END
CL_NS_USE(analysis)

QT_BEGIN_NAMESPACE

class QCLuceneTokenizer;
class QCLuceneTokenStream;
class QCLuceneStandardTokenizer;

class QHELP_EXPORT QCLuceneTokenPrivate : public QSharedData
{
public:
    QCLuceneTokenPrivate();
    QCLuceneTokenPrivate(const QCLuceneTokenPrivate &other);

    ~QCLuceneTokenPrivate();

    Token *token;
    bool deleteCLuceneToken;

private:
    QCLuceneTokenPrivate &operator=(const QCLuceneTokenPrivate &other);
};

class QHELP_EXPORT QCLuceneToken
{
public:
    QCLuceneToken();
    QCLuceneToken(const QString &text, qint32 startOffset, 
                  qint32 endOffset, const QString &defaultTyp = QLatin1String("word"));
    
    virtual ~QCLuceneToken();

    void set(const QString &text, qint32 startOffset, 
             qint32 endOffset, const QString &defaultTyp = QLatin1String("word"));

    quint32 bufferLength() const;
    void growBuffer(quint32 size);

    qint32 positionIncrement() const;
    void setPositionIncrement(qint32 positionIncrement);

    QString termText() const;
    void setTermText(const QString &text);

    quint32 termTextLength() const;
    void resetTermTextLength() const;

    qint32 startOffset() const;
    void setStartOffset(qint32 value);

    qint32 endOffset() const;
    void setEndOffset(qint32 value);

    QString type() const;
    void setType(const QString &type);

    QString toString() const;

protected:
    friend class QCLuceneTokenizer;
    friend class QCLuceneTokenStream;
    friend class QCLuceneStandardTokenizer;
    QSharedDataPointer<QCLuceneTokenPrivate> d;

private:
    TCHAR *tokenText;
    TCHAR *tokenType;
};

QT_END_NAMESPACE

#endif  // QTOKEN_P_H
