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

#include "qscriptbreakpointdata_p.h"

#include <QtCore/qdatastream.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptBreakpointData
  \internal

  \brief The QScriptBreakpointData class contains data associated with a breakpoint.
*/

class QScriptBreakpointDataPrivate
{
public:
    QScriptBreakpointDataPrivate();
    ~QScriptBreakpointDataPrivate();

    void init(int ln);

    qint64 scriptId;
    QString fileName;
    int lineNumber;
    bool enabled;
    bool singleShot;
    int ignoreCount;
    QString condition;
    QVariant data;
    int hitCount;
};

QScriptBreakpointDataPrivate::QScriptBreakpointDataPrivate()
{
}

QScriptBreakpointDataPrivate::~QScriptBreakpointDataPrivate()
{
}

void QScriptBreakpointDataPrivate::init(int ln)
{
    scriptId = -1;
    lineNumber = ln;
    enabled = true;
    singleShot = false;
    ignoreCount = 0;
    hitCount = 0;
}

/*!
  Constructs an empty QScriptBreakpointData.
*/
QScriptBreakpointData::QScriptBreakpointData()
    : d_ptr(new QScriptBreakpointDataPrivate)
{
    d_ptr->init(/*lineNumber=*/-1);
}

/*!
  Constructs a QScriptBreakpointData with the given \a lineNumber.
*/
QScriptBreakpointData::QScriptBreakpointData(qint64 scriptId, int lineNumber)
    : d_ptr(new QScriptBreakpointDataPrivate)
{
    d_ptr->init(lineNumber);
    d_ptr->scriptId = scriptId;
}

/*!
  Constructs a QScriptBreakpointData with the given \a lineNumber.
*/
QScriptBreakpointData::QScriptBreakpointData(const QString &fileName, int lineNumber)
    : d_ptr(new QScriptBreakpointDataPrivate)
{
    d_ptr->init(lineNumber);
    d_ptr->fileName = fileName;
}

/*!
  Constructs a QScriptBreakpointData that is a copy of \a other.
*/
QScriptBreakpointData::QScriptBreakpointData(const QScriptBreakpointData &other)
    : d_ptr(new QScriptBreakpointDataPrivate)
{
    Q_ASSERT(other.d_ptr != 0);
    *d_ptr = *other.d_ptr;
}

/*!
  Destroys this QScriptBreakpointData.
*/
QScriptBreakpointData::~QScriptBreakpointData()
{
    delete d_ptr;
}

/*!
  Assigns \a other to this QScriptBreakpointData.
*/
QScriptBreakpointData &QScriptBreakpointData::operator=(const QScriptBreakpointData &other)
{
    Q_ASSERT(d_ptr != 0);
    Q_ASSERT(other.d_ptr != 0);
    *d_ptr = *other.d_ptr;
    return *this;
}

qint64 QScriptBreakpointData::scriptId() const
{
    Q_D(const QScriptBreakpointData);
    return d->scriptId;
}

void QScriptBreakpointData::setScriptId(qint64 id)
{
    Q_D(QScriptBreakpointData);
    d->scriptId = id;
}

QString QScriptBreakpointData::fileName() const
{
    Q_D(const QScriptBreakpointData);
    return d->fileName;
}

void QScriptBreakpointData::setFileName(const QString &fileName)
{
    Q_D(QScriptBreakpointData);
    d->fileName = fileName;
}

/*!
  Returns the breakpoint line number.
*/
int QScriptBreakpointData::lineNumber() const
{
    Q_D(const QScriptBreakpointData);
    return d->lineNumber;
}

/*!
  Sets the breakpoint line number to \a lineNumber.
*/
void QScriptBreakpointData::setLineNumber(int lineNumber)
{
    Q_D(QScriptBreakpointData);
    d->lineNumber = lineNumber;
}

/*!
  Returns true if the breakpoint is enabled, false otherwise.
*/
bool QScriptBreakpointData::isEnabled() const
{
    Q_D(const QScriptBreakpointData);
    return d->enabled;
}

/*!
  Sets the \a enabled state of the breakpoint.
*/
void QScriptBreakpointData::setEnabled(bool enabled)
{
    Q_D(QScriptBreakpointData);
    d->enabled = enabled;
}

/*!
  Returns true if the breakpoint is single-shot, false otherwise.
*/
bool QScriptBreakpointData::isSingleShot() const
{
    Q_D(const QScriptBreakpointData);
    return d->singleShot;
}

/*!
  Sets the \a singleShot state of the breakpoint.
*/
void QScriptBreakpointData::setSingleShot(bool singleShot)
{
    Q_D(QScriptBreakpointData);
    d->singleShot = singleShot;
}

/*!
  Returns the ignore count of the breakpoint.
*/
int QScriptBreakpointData::ignoreCount() const
{
    Q_D(const QScriptBreakpointData);
    return d->ignoreCount;
}

/*!
  Sets the ignore \a count of the breakpoint.
*/
void QScriptBreakpointData::setIgnoreCount(int count)
{
    Q_D(QScriptBreakpointData);
    d->ignoreCount = count;
}

/*!
  If the ignore count is 0, this function increments the hit count and
  returns true. Otherwise, it decrements the ignore count and returns
  false.
*/
bool QScriptBreakpointData::hit()
{
    Q_D(QScriptBreakpointData);
    if (d->ignoreCount == 0) {
        ++d->hitCount;
        return true;
    }
    --d->ignoreCount;
    return false;
}

/*!
  Returns the hit count of the breakpoint (the number of times the
  breakpoint has been triggered).
*/
int QScriptBreakpointData::hitCount() const
{
    Q_D(const QScriptBreakpointData);
    return d->hitCount;
}

/*!
  Returns the condition of the breakpoint.
*/
QString QScriptBreakpointData::condition() const
{
    Q_D(const QScriptBreakpointData);
    return d->condition;
}

/*!
  Sets the \a condition of the breakpoint.
*/
void QScriptBreakpointData::setCondition(const QString &condition)
{
    Q_D(QScriptBreakpointData);
    d->condition = condition;
}

/*!
  Returns custom data associated with the breakpoint.
*/
QVariant QScriptBreakpointData::data() const
{
    Q_D(const QScriptBreakpointData);
    return d->data;
}

/*!
  Sets custom \a data associated with the breakpoint.
*/
void QScriptBreakpointData::setData(const QVariant &data)
{
    Q_D(QScriptBreakpointData);
    d->data = data;
}

bool QScriptBreakpointData::isValid() const
{
    Q_D(const QScriptBreakpointData);
    return (((d->scriptId != -1) || !d->fileName.isEmpty())
            && (d->lineNumber != -1));
}

/*!
  Returns true if this QScriptBreakpointData is equal to the \a other
  data, otherwise returns false.
*/
bool QScriptBreakpointData::operator==(const QScriptBreakpointData &other) const
{
    Q_D(const QScriptBreakpointData);
    const QScriptBreakpointDataPrivate *od = other.d_func();
    if (d == od)
        return true;
    if (!d || !od)
        return false;
    return ((d->scriptId == od->scriptId)
            && (d->fileName == od->fileName)
            && (d->lineNumber == od->lineNumber)
            && (d->enabled == od->enabled)
            && (d->singleShot == od->singleShot)
            && (d->condition == od->condition)
            && (d->ignoreCount == od->ignoreCount)
            && (d->data == od->data)
            && (d->hitCount == od->hitCount));
}

/*!
  Returns true if this QScriptBreakpointData is not equal to the \a
  other data, otherwise returns false.
*/
bool QScriptBreakpointData::operator!=(const QScriptBreakpointData &other) const
{
    return !(*this == other);
}

/*!
  \fn QDataStream &operator<<(QDataStream &stream, const QScriptBreakpointData &data)
  \relates QScriptBreakpointData

  Writes the given \a data to the specified \a stream.
*/
QDataStream &operator<<(QDataStream &out, const QScriptBreakpointData &data)
{
    const QScriptBreakpointDataPrivate *d = data.d_ptr;
    out << d->scriptId;
    out << d->fileName;
    out << d->lineNumber;
    out << d->enabled;
    out << d->singleShot;
    out << d->ignoreCount;
    out << d->condition;
    out << d->data;
    out << d->hitCount;
    return out;
}

/*!
  \fn QDataStream &operator>>(QDataStream &stream, QScriptBreakpointData &data)
  \relates QScriptBreakpointData

  Reads a QScriptBreakpointData from the specified \a stream into the
  given \a data.
*/
QDataStream &operator>>(QDataStream &in, QScriptBreakpointData &data)
{
    QScriptBreakpointDataPrivate *d = data.d_ptr;
    in >> d->scriptId;
    in >> d->fileName;
    in >> d->lineNumber;
    in >> d->enabled;
    in >> d->singleShot;
    in >> d->ignoreCount;
    in >> d->condition;
    in >> d->data;
    in >> d->hitCount;
    return in;
}

QT_END_NAMESPACE
