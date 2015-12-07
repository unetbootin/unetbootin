/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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
    addPrototypeFunction(QLatin1String("toFixed"), method_toFixed, 0);
    addPrototypeFunction(QLatin1String("toExponential"), method_toExponential, 0);
    addPrototypeFunction(QLatin1String("toPrecision"), method_toPrecision, 0);

    QScriptValue::PropertyFlags flags = QScriptValue::Undeletable
                                        | QScriptValue::ReadOnly
                                        | QScriptValue::SkipInEnumeration;
    ctor.setProperty(QLatin1String("NaN"),
                     QScriptValueImpl(eng, qSNaN()), flags);
    ctor.setProperty(QLatin1String("NEGATIVE_INFINITY"),
                     QScriptValueImpl(eng, -qInf()), flags);
    ctor.setProperty(QLatin1String("POSITIVE_INFINITY"),
                     QScriptValueImpl(eng, qInf()), flags);
    ctor.setProperty(QLatin1String("MAX_VALUE"),
                     QScriptValueImpl(eng, 1.7976931348623158e+308), flags);
#ifdef __INTEL_COMPILER
# pragma warning( push )
# pragma warning(disable: 239)
#endif
    ctor.setProperty(QLatin1String("MIN_VALUE"),
                     QScriptValueImpl(eng, 5e-324), flags);
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

    QScriptValueImpl num(engine(), value);
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
    result->setInternalValue(QScriptValueImpl(engine(), value));
}

QScriptValueImpl Number::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo)
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Number.prototype.toString"));

    QScriptValueImpl arg = context->argument(0);
    if (!arg.isUndefined()) {
        int radix = arg.toInt32();
        if (radix < 2 || radix > 36)
            return context->throwError(QString::fromLatin1("Number.prototype.toString: %0 is not a valid radix")
                                       .arg(radix));
        if (radix != 10) {
            QString str;
            qsreal num = self.internalValue().toInteger();
            do {
                char c = (char)::fmod(num, radix);
                if (c < 10)
                    c += '0';
                else
                    c = c - 10 + 'a';
                str.prepend(QLatin1Char(c));
                num = ::floor(num / radix);
            } while (num != 0);
            return QScriptValueImpl(eng, str);
        }
    }
    QString str = self.internalValue().toString();
    return (QScriptValueImpl(eng, str));
}

QScriptValueImpl Number::method_toLocaleString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo)
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Number.prototype.toLocaleString"));

    QString str = self.internalValue().toString();
    return (QScriptValueImpl(eng, str));
}

QScriptValueImpl Number::method_valueOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo)
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Number.prototype.valueOf"));

    return (self.internalValue());
}

QScriptValueImpl Number::method_toFixed(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo)
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Number.prototype.toFixed"));

    qsreal fdigits = 0;

    if (context->argumentCount() > 0)
        fdigits = context->argument(0).toInteger();

    if (qIsNaN(fdigits))
        fdigits = 0;

    qsreal v = self.internalValue().toNumber();
    return (QScriptValueImpl(eng, QString::number(v, 'f', int (fdigits))));
}

QScriptValueImpl Number::method_toExponential(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo)
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Number.prototype.toExponential"));

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
    if (self.classInfo() != classInfo)
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Number.prototype.toPrecision"));

    qsreal fdigits = 0;

    if (context->argumentCount() > 0)
        fdigits = context->argument(0).toInteger();

    qsreal v = self.internalValue().toNumber();
    return (QScriptValueImpl(eng, QString::number(v, 'g', int (fdigits))));
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
