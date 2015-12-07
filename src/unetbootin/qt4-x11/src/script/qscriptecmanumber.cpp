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

#include "qscriptecmanumber_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>
#include <QtCore/qnumeric.h>
#include <math.h>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

Number::Number(QScriptEnginePrivate *eng):
    Core(eng, QLatin1String("Number"), QScriptClassInfo::NumberType)
{
    newNumber(&publicPrototype, 0);

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 0);
    addPrototypeFunction(QLatin1String("toLocaleString"), method_toLocaleString, 0);
    addPrototypeFunction(QLatin1String("valueOf"), method_valueOf, 0);
    addPrototypeFunction(QLatin1String("toFixed"), method_toFixed, 1);
    addPrototypeFunction(QLatin1String("toExponential"), method_toExponential, 1);
    addPrototypeFunction(QLatin1String("toPrecision"), method_toPrecision, 1);

    QScriptValue::PropertyFlags flags = QScriptValue::Undeletable
                                        | QScriptValue::ReadOnly
                                        | QScriptValue::SkipInEnumeration;
    ctor.setProperty(QLatin1String("NaN"),
                     QScriptValueImpl(qSNaN()), flags);
    ctor.setProperty(QLatin1String("NEGATIVE_INFINITY"),
                     QScriptValueImpl(-qInf()), flags);
    ctor.setProperty(QLatin1String("POSITIVE_INFINITY"),
                     QScriptValueImpl(qInf()), flags);
    ctor.setProperty(QLatin1String("MAX_VALUE"),
                     QScriptValueImpl(1.7976931348623158e+308), flags);
#ifdef __INTEL_COMPILER
# pragma warning( push )
# pragma warning(disable: 239)
#endif
    ctor.setProperty(QLatin1String("MIN_VALUE"),
                     QScriptValueImpl(5e-324), flags);
#ifdef __INTEL_COMPILER
# pragma warning( pop )
#endif
}

Number::~Number()
{
}

void Number::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    qsreal value;
    if (context->argumentCount() > 0)
        value = context->argument(0).toNumber();
    else
        value = 0;

    QScriptValueImpl num(value);
    if (!context->isCalledAsConstructor()) {
        context->setReturnValue(num);
    } else {
        QScriptValueImpl &obj = context->m_thisObject;
        obj.setClassInfo(classInfo());
        obj.setInternalValue(num);
        obj.setPrototype(publicPrototype);
        context->setReturnValue(obj);
    }
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

void Number::newNumber(QScriptValueImpl *result, qsreal value)
{
    engine()->newObject(result, publicPrototype, classInfo());
    result->setInternalValue(QScriptValueImpl(value));
}

QScriptValueImpl Number::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Number.prototype.toString"));
    }
    QScriptValueImpl arg = context->argument(0);
    if (!arg.isUndefined()) {
        int radix = arg.toInt32();
        if (radix < 2 || radix > 36)
            return context->throwError(QString::fromLatin1("Number.prototype.toString: %0 is not a valid radix")
                                       .arg(radix));
        if (radix != 10) {
            QString str;
            qsreal num = self.internalValue().toNumber();
            if (qIsNaN(num))
                return QScriptValueImpl(eng, QLatin1String("NaN"));
            else if (qIsInf(num))
                return QScriptValueImpl(eng, QLatin1String(num < 0 ? "-Infinity" : "Infinity"));
            bool negative = false;
            if (num < 0) {
                negative = true;
                num = -num;
            }
            qsreal frac = num - ::floor(num);
            num = QScriptEnginePrivate::toInteger(num);
            do {
                char c = (char)::fmod(num, radix);
                c = (c < 10) ? (c + '0') : (c - 10 + 'a');
                str.prepend(QLatin1Char(c));
                num = ::floor(num / radix);
            } while (num != 0);
            if (frac != 0) {
                str.append(QLatin1Char('.'));
                do {
                    frac = frac * radix;
                    char c = (char)::floor(frac);
                    c = (c < 10) ? (c + '0') : (c - 10 + 'a');
                    str.append(QLatin1Char(c));
                    frac = frac - ::floor(frac);
                } while (frac != 0);
            }
            if (negative)
                str.prepend(QLatin1Char('-'));
            return QScriptValueImpl(eng, str);
        }
    }
    QString str = self.internalValue().toString();
    return (QScriptValueImpl(eng, str));
}

QScriptValueImpl Number::method_toLocaleString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Number.prototype.toLocaleString"));
    }
    QString str = self.internalValue().toString();
    return (QScriptValueImpl(eng, str));
}

QScriptValueImpl Number::method_valueOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Number.prototype.valueOf"));
    }
    return (self.internalValue());
}

QScriptValueImpl Number::method_toFixed(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Number.prototype.toFixed"));
    }
    qsreal fdigits = 0;

    if (context->argumentCount() > 0)
        fdigits = context->argument(0).toInteger();

    if (qIsNaN(fdigits))
        fdigits = 0;

    qsreal v = self.internalValue().toNumber();
    QString str;
    if (qIsNaN(v))
        str = QString::fromLatin1("NaN");
    else if (qIsInf(v))
        str = QString::fromLatin1(v < 0 ? "-Infinity" : "Infinity");
    else
        str = QString::number(v, 'f', int (fdigits));
    return (QScriptValueImpl(eng, str));
}

QScriptValueImpl Number::method_toExponential(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Number.prototype.toExponential"));
    }
    qsreal fdigits = 0;

    if (context->argumentCount() > 0)
        fdigits = context->argument(0).toInteger();

    qsreal v = self.internalValue().toNumber();
    QString z = QString::number(v, 'e', int (fdigits));
    return (QScriptValueImpl(eng, z));
}

QScriptValueImpl Number::method_toPrecision(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Number.prototype.toPrecision"));
    }
    qsreal fdigits = 0;

    if (context->argumentCount() > 0)
        fdigits = context->argument(0).toInteger();

    qsreal v = self.internalValue().toNumber();
    return (QScriptValueImpl(eng, QString::number(v, 'g', int (fdigits))));
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
