/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qdocument_p.h"
#include "qreader_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/util/Reader.h>
#include <CLucene/document/Document.h>

QT_BEGIN_NAMESPACE

QCLuceneDocumentPrivate::QCLuceneDocumentPrivate()
    : QSharedData()
{
    document = 0;
    deleteCLuceneDocument = true;
}

QCLuceneDocumentPrivate::QCLuceneDocumentPrivate(const QCLuceneDocumentPrivate &other)
    : QSharedData()
{
    document = _CL_POINTER(other.document);
}

QCLuceneDocumentPrivate::~QCLuceneDocumentPrivate()
{
    if (deleteCLuceneDocument)
        _CLDECDELETE(document);
}


QCLuceneDocument::QCLuceneDocument()
    : d(new QCLuceneDocumentPrivate())
{
    // nothing todo
    d->document = new lucene::document::Document();
}

QCLuceneDocument::~QCLuceneDocument()
{
    qDeleteAll(fieldList);
    fieldList.clear();
}

void QCLuceneDocument::add(QCLuceneField *field)
{
    field->d->deleteCLuceneField = false;
    d->document->add(*field->d->field);
    fieldList.append(field);
}

QCLuceneField* QCLuceneDocument::getField(const QString &name) const
{
    QCLuceneField* field = 0;
    foreach (field, fieldList) {
        if (field->name() == name && field->d->field != 0)
            return field;
    }

    field = 0;
    TCHAR *fieldName = QStringToTChar(name);
    lucene::document::Field *f = d->document->getField(fieldName);
    if (f) {
        field = new QCLuceneField();
        field->d->field = f;
        fieldList.append(field);
        field->d->deleteCLuceneField = false;

        lucene::util::Reader *r = f->readerValue();
        if (r) {
            field->reader->d->reader = r;
            field->reader->d->deleteCLuceneReader = false;
        }
    }
    delete [] fieldName;
    
    return field;
}

QString QCLuceneDocument::get(const QString &name) const
{
    QCLuceneField* field = getField(name);
    if (field)
        return field->stringValue();
    
    return QString();
}

QString QCLuceneDocument::toString() const
{
    return TCharToQString(d->document->toString());
}

void QCLuceneDocument::setBoost(qreal boost)
{
    d->document->setBoost(qreal(boost));
}

qreal QCLuceneDocument::getBoost() const
{
    return qreal(d->document->getBoost());
}

void QCLuceneDocument::removeField(const QString &name)
{
    TCHAR *fieldName = QStringToTChar(name);
    d->document->removeField(fieldName);
    delete [] fieldName;

    QList<QCLuceneField*> tmp;
    lucene::document::DocumentFieldEnumeration *dfe = d->document->fields();
    while (dfe->hasMoreElements()) {
        const lucene::document::Field* f = dfe->nextElement();
        foreach (QCLuceneField* field, fieldList) {
            if (f == field->d->field) {
                tmp.append(field);
                break;
            }
        }
    }
    _CLDELETE(dfe);
    fieldList = tmp;
}

void QCLuceneDocument::removeFields(const QString &name)
{
    for (qint32 i = fieldList.count() -1; i >= 0; --i) {
        QCLuceneField* field = fieldList.at(i);
        if (field->name() == name)
            delete fieldList.takeAt(i);
    }

    TCHAR *fieldName = QStringToTChar(name);
    d->document->removeFields(fieldName);
    delete [] fieldName;
}

QStringList QCLuceneDocument::getValues(const QString &name) const
{
    TCHAR *fieldName = QStringToTChar(name);
    TCHAR **values = d->document->getValues(fieldName);

    QStringList retValue;
    if (values) {
        for (qint32 i = 0; 0 != values[i]; ++i) {
            retValue.append(TCharToQString((const TCHAR*)values[i]));
            delete [] values[i]; values[i] = 0;
        }
        delete values;
    }

    delete [] fieldName;
    return retValue;
}

void QCLuceneDocument::clear()
{
    d->document->clear();
    qDeleteAll(fieldList);
    fieldList.clear();
}

QT_END_NAMESPACE
