/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include "qscriptcontextinfo.h"

#ifndef QT_NO_SCRIPT

#include "qscriptcontextinfo_p.h"
#include "qscriptengine_p.h"
#include "qscriptcontext_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include <QtCore/qdatastream.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.4
  \class QScriptContextInfo

  \brief The QScriptContextInfo class provides additional information about a QScriptContext.

  \ingroup script
  \mainclass

  QScriptContextInfo is typically used for debugging purposes. It can
  provide information about the code being executed, such as the type
  of the called function, and the original source code location of the
  current statement.

  If the called function is executing Qt Script code, you can obtain
  the script location with the functions fileName(), lineNumber() and
  columnNumber().

  You can obtain the starting line number and ending line number of a
  Qt Script function definition with functionStartLineNumber() and
  functionEndLineNumber(), respectively.

  For Qt Script functions and Qt methods (e.g. slots), you can call
  functionParameterNames() to get the names of the formal parameters of the
  function.

  For Qt methods and Qt property accessors, you can obtain the index
  of the underlying QMetaMethod or QMetaProperty by calling
  functionMetaIndex().

  \sa QScriptContext, QScriptEngineAgent
*/

/*!
    \enum QScriptContextInfo::FunctionType

    This enum specifies the type of function being called.

    \value ScriptFunction The function is a Qt Script function, i.e. it was defined through a call to QScriptEngine::evaluate().
    \value QtFunction The function is a Qt function (a signal, slot or method).
    \value QtPropertyFunction The function is a Qt property getter or setter.
    \value NativeFunction The function is a built-in Qt Script function, or it was defined through a call to QScriptEngine::newFunction().
*/

/*!
  \internal
*/
QScriptContextInfoPrivate::QScriptContextInfoPrivate()
{
    ref = 0;
    functionType = QScriptContextInfo::NativeFunction;
    functionMetaIndex = -1;
    functionStartLineNumber = -1;
    functionEndLineNumber = -1;
    scriptId = -1;
    lineNumber = -1;
    columnNumber = -1;
}

/*!
  \internal
*/
QScriptContextInfoPrivate::QScriptContextInfoPrivate(const QScriptContext *context)
{
    Q_ASSERT(context);
    ref = 0;
    functionType = QScriptContextInfo::NativeFunction;
    functionMetaIndex = -1;
    functionStartLineNumber = -1;
    functionEndLineNumber = -1;

    const QScriptContextPrivate *ctx_p = QScriptContextPrivate::get(context);
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    scriptId = ctx_p->scriptId();
#endif
    fileName = ctx_p->fileName();
    lineNumber = ctx_p->currentLine;
    columnNumber = ctx_p->currentColumn;

    QScriptValueImpl callee = ctx_p->engine()->toImpl(context->callee());
    QScriptFunction *fun = callee.toFunction();
    if (fun) {
        functionName = fun->functionName();
        functionStartLineNumber = fun->startLineNumber();
        functionEndLineNumber = fun->endLineNumber();

        switch (fun->type()) {
        case QScriptFunction::Unknown:
            functionType = QScriptContextInfo::NativeFunction;
            break;

        case QScriptFunction::Script:
            functionType = QScriptContextInfo::ScriptFunction;
            for (int i = 0; i < fun->formals.count(); ++i)
                parameterNames.append(fun->formals.at(i)->s);
            break;

        case QScriptFunction::C:
            functionType = QScriptContextInfo::NativeFunction;
            break;

        case QScriptFunction::C2:
            functionType = QScriptContextInfo::NativeFunction;
            break;

        case QScriptFunction::C3:
            functionType = QScriptContextInfo::NativeFunction;
            break;

        case QScriptFunction::Qt: {
            functionType = QScriptContextInfo::QtFunction;
            functionMetaIndex = ctx_p->calleeMetaIndex;

#ifndef QT_NO_QOBJECT
            const QMetaObject *meta;
            meta = static_cast<QScript::QtFunction*>(fun)->metaObject();
            if (meta) {
                QMetaMethod method = meta->method(functionMetaIndex);
                QList<QByteArray> formals = method.parameterNames();
                for (int i = 0; i < formals.count(); ++i)
                    parameterNames.append(QLatin1String(formals.at(i)));
            }
#endif
        }   break;

        case QScriptFunction::QtProperty:
            functionType = QScriptContextInfo::QtPropertyFunction;
            functionMetaIndex = ctx_p->calleeMetaIndex;
            break;
        }
    }
}

/*!
  \internal
*/
QScriptContextInfoPrivate::~QScriptContextInfoPrivate()
{
}

/*!
  Constructs a new QScriptContextInfo from the given \a context.

  The relevant information is extracted from the \a context at
  construction time; i.e. if you continue script execution in the \a
  context, the new state of the context will not be reflected in a
  previously created QScriptContextInfo.
*/
QScriptContextInfo::QScriptContextInfo(const QScriptContext *context)
{
    if (context) {
        d_ptr = new QScriptContextInfoPrivate(context);
        d_ptr->q_ptr = this;
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  Constructs a new QScriptContextInfo from the \a other info.
*/
QScriptContextInfo::QScriptContextInfo(const QScriptContextInfo &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  Constructs a null QScriptContextInfo.

  \sa isNull()
*/
QScriptContextInfo::QScriptContextInfo()
    : d_ptr(0)
{
}

/*!
  Destroys the QScriptContextInfo.
*/
QScriptContextInfo::~QScriptContextInfo()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
  Assigns the \a other info to this QScriptContextInfo,
  and returns a reference to this QScriptContextInfo.
*/
QScriptContextInfo &QScriptContextInfo::operator=(const QScriptContextInfo &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns the ID of the script where the code being executed was
  defined, or -1 if the ID is not available (i.e. a native function is
  being executed).

  \sa QScriptEngineAgent::scriptLoad()
*/
qint64 QScriptContextInfo::scriptId() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return -1;
    return d->scriptId;
}

/*!
  Returns the name of the file where the code being executed was
  defined, if available; otherwise returns an empty string.

  For Qt Script code, this function returns the fileName argument
  that was passed to QScriptEngine::evaluate().

  \sa lineNumber(), functionName()
*/
QString QScriptContextInfo::fileName() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return QString();
    return d->fileName;
}

/*!
  Returns the line number corresponding to the statement being
  executed, or -1 if the line number is not available.

  The line number is only available if Qt Script code is being
  executed.

  \sa columnNumber(), fileName()
*/
int QScriptContextInfo::lineNumber() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return -1;
    return d->lineNumber;
}

/*!
  Returns the column number corresponding to the statement being
  executed, or -1 if the column number is not available.

  The column number is only available if Qt Script code is being
  executed.

  \sa lineNumber(), fileName()
*/
int QScriptContextInfo::columnNumber() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return -1;
    return d->columnNumber;
}

/*!
  Returns the name of the called function, or an empty string if
  the name is not available.

  For script functions of type QtPropertyFunction, this function
  always returns the name of the property; you can use
  QScriptContext::argumentCount() to differentiate between reads and
  writes.

  \sa fileName(), functionType()
*/
QString QScriptContextInfo::functionName() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return QString();
    return d->functionName;
}

/*!
  Returns the type of the called function.

  \sa functionName(), QScriptContext::callee()
*/
QScriptContextInfo::FunctionType QScriptContextInfo::functionType() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return NativeFunction;
    return d->functionType;
}

/*!
  Returns the line number where the definition of the called function
  starts, or -1 if the line number is not available.

  The starting line number is only available if the functionType() is
  ScriptFunction.

  \sa functionEndLineNumber(), fileName()
*/
int QScriptContextInfo::functionStartLineNumber() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return -1;
    return d->functionStartLineNumber;
}

/*!
  Returns the line number where the definition of the called function
  ends, or -1 if the line number is not available.

  The ending line number is only available if the functionType() is
  ScriptFunction.

  \sa functionStartLineNumber()
*/
int QScriptContextInfo::functionEndLineNumber() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return -1;
    return d->functionEndLineNumber;
}

/*!
  Returns the names of the formal parameters of the called function,
  or an empty QStringList if the parameter names are not available.

  \sa QScriptContext::argument()
*/
QStringList QScriptContextInfo::functionParameterNames() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return QStringList();
    return d->parameterNames;
}

/*!
  Returns the meta index of the called function, or -1 if the meta
  index is not available.

  The meta index is only available if the functionType() is QtFunction
  or QtPropertyFunction. For QtFunction, the meta index can be passed
  to QMetaObject::method() to obtain the corresponding method
  definition; for QtPropertyFunction, the meta index can be passed to
  QMetaObject::property() to obtain the corresponding property
  definition.

  \sa QScriptContext::thisObject()
*/
int QScriptContextInfo::functionMetaIndex() const
{
    Q_D(const QScriptContextInfo);
    if (!d)
        return -1;
    return d->functionMetaIndex;
}

/*!
  Returns true if this QScriptContextInfo is null, i.e. does not
  contain any information.
*/
bool QScriptContextInfo::isNull() const
{
    Q_D(const QScriptContextInfo);
    return (d == 0);
}

/*!
  Returns true if this QScriptContextInfo is equal to the \a other
  info, otherwise returns false.
*/
bool QScriptContextInfo::operator==(const QScriptContextInfo &other) const
{
    Q_D(const QScriptContextInfo);
    const QScriptContextInfoPrivate *od = other.d_func();
    if (d == od)
        return true;
    if (!d || !od)
        return false;
    return ((d->scriptId == od->scriptId)
            && (d->lineNumber == od->lineNumber)
            && (d->columnNumber == od->columnNumber)
            && (d->fileName == od->fileName)
            && (d->functionName == od->functionName)
            && (d->functionType == od->functionType)
            && (d->functionStartLineNumber == od->functionStartLineNumber)
            && (d->functionEndLineNumber == od->functionEndLineNumber)
            && (d->functionMetaIndex == od->functionMetaIndex)
            && (d->parameterNames == od->parameterNames));
}

/*!
  Returns true if this QScriptContextInfo is not equal to the \a other
  info, otherwise returns false.
*/
bool QScriptContextInfo::operator!=(const QScriptContextInfo &other) const
{
    return !(*this == other);
}

#ifndef QT_NO_DATASTREAM
/*!
  \fn QDataStream &operator<<(QDataStream &stream, const QScriptContextInfo &info)
  \since 4.4
  \relates QScriptContextInfo

  Writes the given \a info to the specified \a stream.
*/
QDataStream &operator<<(QDataStream &out, const QScriptContextInfo &info)
{
    out << info.scriptId();
    out << (qint32)info.lineNumber();
    out << (qint32)info.columnNumber();

    out << (quint32)info.functionType();
    out << (qint32)info.functionStartLineNumber();
    out << (qint32)info.functionEndLineNumber();
    out << (qint32)info.functionMetaIndex();

    out << info.fileName();
    out << info.functionName();
    out << info.functionParameterNames();

    return out;
}

/*!
  \fn QDataStream &operator>>(QDataStream &stream, QScriptContextInfo &info)
  \since 4.4
  \relates QScriptContextInfo

  Reads a QScriptContextInfo from the specified \a stream into the
  given \a info.
*/
Q_SCRIPT_EXPORT QDataStream &operator>>(QDataStream &in, QScriptContextInfo &info)
{
    if (!info.d_ptr) {
        info.d_ptr = new QScriptContextInfoPrivate();
        info.d_ptr->ref.ref();
    }

    in >> info.d_ptr->scriptId;

    qint32 line;
    in >> line;
    info.d_ptr->lineNumber = line;

    qint32 column;
    in >> column;
    info.d_ptr->columnNumber = column;

    quint32 ftype;
    in >> ftype;
    info.d_ptr->functionType = QScriptContextInfo::FunctionType(ftype);

    qint32 startLine;
    in >> startLine;
    info.d_ptr->functionStartLineNumber = startLine;

    qint32 endLine;
    in >> endLine;
    info.d_ptr->functionEndLineNumber = endLine;

    qint32 metaIndex;
    in >> metaIndex;
    info.d_ptr->functionMetaIndex = metaIndex;

    in >> info.d_ptr->fileName;
    in >> info.d_ptr->functionName;
    in >> info.d_ptr->parameterNames;

    return in;
}
#endif

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
