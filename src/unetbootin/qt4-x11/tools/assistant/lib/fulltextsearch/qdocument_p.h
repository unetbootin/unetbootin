/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QDOCUMENT_P_H
#define QDOCUMENT_P_H

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

#include "qfield_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(document)
    class Document;
CL_NS_END
CL_NS_USE(document)

QT_BEGIN_NAMESPACE

class QCLuceneHits;
class QCLuceneIndexReader;
class QCLuceneIndexWriter;
class QCLuceneIndexSearcher;
class QCLuceneMultiSearcher;

class QHELP_EXPORT QCLuceneDocumentPrivate : public QSharedData
{
public:
    QCLuceneDocumentPrivate();
    QCLuceneDocumentPrivate(const QCLuceneDocumentPrivate &other);

    ~QCLuceneDocumentPrivate();

    Document *document;
    bool deleteCLuceneDocument;

private:
    QCLuceneDocumentPrivate &operator=(const QCLuceneDocumentPrivate &other);
};

class QHELP_EXPORT QCLuceneDocument
{
public:
    QCLuceneDocument();
    ~QCLuceneDocument();

    void add(QCLuceneField *field);
    QCLuceneField* getField(const QString &name) const;
    QString get(const QString &name) const;
    QString toString() const;
    void setBoost(qreal boost);
    qreal getBoost() const;
    void removeField(const QString &name);
    void removeFields(const QString &name);
    QStringList getValues(const QString &name) const;
    void clear();

protected:
    friend class QCLuceneHits;
    friend class QCLuceneIndexReader;
    friend class QCLuceneIndexWriter;
    friend class QCLuceneIndexSearcher;
    friend class QCLuceneMultiSearcher;
    QSharedDataPointer<QCLuceneDocumentPrivate> d;

private:
    mutable QList<QCLuceneField*> fieldList;
};

QT_END_NAMESPACE

#endif  // QDOCUMENT_P_H
