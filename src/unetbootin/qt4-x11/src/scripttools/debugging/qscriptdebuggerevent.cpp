/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptdebuggerevent_p.h"
#include "qscriptdebuggervalue_p.h"

#include <QtCore/qhash.h>
#include <QtCore/qdatastream.h>

Q_DECLARE_METATYPE(QScriptDebuggerValue)

QT_BEGIN_NAMESPACE

class QScriptDebuggerEventPrivate
{
public:
    QScriptDebuggerEventPrivate();
    ~QScriptDebuggerEventPrivate();

    QScriptDebuggerEvent::Type type;
    QHash<QScriptDebuggerEvent::Attribute, QVariant> attributes;
};

QScriptDebuggerEventPrivate::QScriptDebuggerEventPrivate()
{
}

QScriptDebuggerEventPrivate::~QScriptDebuggerEventPrivate()
{
}

QScriptDebuggerEvent::QScriptDebuggerEvent()
    : d_ptr(new QScriptDebuggerEventPrivate)
{
    d_ptr->type = None;
}

QScriptDebuggerEvent::QScriptDebuggerEvent(Type type)
    : d_ptr(new QScriptDebuggerEventPrivate)
{
    d_ptr->type = type;
}

QScriptDebuggerEvent::QScriptDebuggerEvent(Type type, qint64 scriptId,
                                           int lineNumber, int columnNumber)
    : d_ptr(new QScriptDebuggerEventPrivate)
{
    d_ptr->type = type;
    d_ptr->attributes[ScriptID] = scriptId;
    d_ptr->attributes[LineNumber] = lineNumber;
    d_ptr->attributes[ColumnNumber] = columnNumber;
}

QScriptDebuggerEvent::QScriptDebuggerEvent(const QScriptDebuggerEvent &other)
    : d_ptr(new QScriptDebuggerEventPrivate)
{
    *d_ptr = *other.d_ptr;
}

QScriptDebuggerEvent::~QScriptDebuggerEvent()
{
    delete d_ptr;
}

QScriptDebuggerEvent &QScriptDebuggerEvent::operator=(const QScriptDebuggerEvent &other)
{
    *d_ptr = *other.d_ptr;
    return *this;
}

QScriptDebuggerEvent::Type QScriptDebuggerEvent::type() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->type;
}

QVariant QScriptDebuggerEvent::attribute(Attribute attribute,
                                         const QVariant &defaultValue) const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(attribute, defaultValue);
}

void QScriptDebuggerEvent::setAttribute(Attribute attribute,
                                        const QVariant &value)
{
    Q_D(QScriptDebuggerEvent);
    if (!value.isValid())
        d->attributes.remove(attribute);
    else
        d->attributes[attribute] = value;
}

QHash<QScriptDebuggerEvent::Attribute, QVariant> QScriptDebuggerEvent::attributes() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes;
}

qint64 QScriptDebuggerEvent::scriptId() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(ScriptID, -1).toLongLong();
}

void QScriptDebuggerEvent::setScriptId(qint64 id)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[ScriptID] = id;
}

QString QScriptDebuggerEvent::fileName() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(FileName).toString();
}

void QScriptDebuggerEvent::setFileName(const QString &fileName)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[FileName] = fileName;
}

int QScriptDebuggerEvent::lineNumber() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(LineNumber, -1).toInt();
}

void QScriptDebuggerEvent::setLineNumber(int lineNumber)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[LineNumber] = lineNumber;
}

int QScriptDebuggerEvent::columnNumber() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(ColumnNumber, -1).toInt();
}

void QScriptDebuggerEvent::setColumnNumber(int columnNumber)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[ColumnNumber] = columnNumber;
}

int QScriptDebuggerEvent::breakpointId() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(BreakpointID, -1).toInt();
}

void QScriptDebuggerEvent::setBreakpointId(int id)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[BreakpointID] = id;
}

QString QScriptDebuggerEvent::message() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(Message).toString();
}

void QScriptDebuggerEvent::setMessage(const QString &message)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[Message] = message;
}

QScriptDebuggerValue QScriptDebuggerEvent::scriptValue() const
{
    Q_D(const QScriptDebuggerEvent);
    return qvariant_cast<QScriptDebuggerValue>(d->attributes[Value]);
}

void QScriptDebuggerEvent::setScriptValue(const QScriptDebuggerValue &value)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[Value] = qVariantFromValue(value);
}

void QScriptDebuggerEvent::setNestedEvaluate(bool nested)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[IsNestedEvaluate] = nested;
}

bool QScriptDebuggerEvent::isNestedEvaluate() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(IsNestedEvaluate).toBool();
}

void QScriptDebuggerEvent::setHasExceptionHandler(bool hasHandler)
{
    Q_D(QScriptDebuggerEvent);
    d->attributes[HasExceptionHandler] = hasHandler;
}

bool QScriptDebuggerEvent::hasExceptionHandler() const
{
    Q_D(const QScriptDebuggerEvent);
    return d->attributes.value(HasExceptionHandler).toBool();
}

/*!
  Returns true if this QScriptDebuggerEvent is equal to the \a other
  event, otherwise returns false.
*/
bool QScriptDebuggerEvent::operator==(const QScriptDebuggerEvent &other) const
{
    Q_D(const QScriptDebuggerEvent);
    const QScriptDebuggerEventPrivate *od = other.d_func();
    if (d == od)
        return true;
    if (!d || !od)
        return false;
    return ((d->type == od->type)
            && (d->attributes == od->attributes));
}

/*!
  Returns true if this QScriptDebuggerEvent is not equal to the \a
  other event, otherwise returns false.
*/
bool QScriptDebuggerEvent::operator!=(const QScriptDebuggerEvent &other) const
{
    return !(*this == other);
}

/*!
  \fn QDataStream &operator<<(QDataStream &stream, const QScriptDebuggerEvent &event)
  \relates QScriptDebuggerEvent

  Writes the given \a event to the specified \a stream.
*/
QDataStream &operator<<(QDataStream &out, const QScriptDebuggerEvent &event)
{
    const QScriptDebuggerEventPrivate *d = event.d_ptr;
    out << (quint32)d->type;
    out << (qint32)d->attributes.size();
    QHash<QScriptDebuggerEvent::Attribute, QVariant>::const_iterator it;
    for (it = d->attributes.constBegin(); it != d->attributes.constEnd(); ++it) {
        out << (quint32)it.key();
        out << it.value();
    }
    return out;
}

/*!
  \fn QDataStream &operator>>(QDataStream &stream, QScriptDebuggerEvent &event)
  \relates QScriptDebuggerEvent

  Reads a QScriptDebuggerEvent from the specified \a stream into the
  given \a event.
*/
QDataStream &operator>>(QDataStream &in, QScriptDebuggerEvent &event)
{
    QScriptDebuggerEventPrivate *d = event.d_ptr;

    quint32 type;
    in >> type;
    d->type = QScriptDebuggerEvent::Type(type);

    qint32 attribCount;
    in >> attribCount;
    QHash<QScriptDebuggerEvent::Attribute, QVariant> attribs;
    for (qint32 i = 0; i < attribCount; ++i) {
        quint32 key;
        in >> key;
        QVariant value;
        in >> value;
        attribs[QScriptDebuggerEvent::Attribute(key)] = value;
    }
    d->attributes = attribs;

    return in;
}

QT_END_NAMESPACE
