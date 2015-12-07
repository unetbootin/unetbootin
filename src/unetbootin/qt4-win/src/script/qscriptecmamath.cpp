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

#include "qscriptecmamath_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>
#include <QtCore/qnumeric.h>
#include <QtCore/QSysInfo>
#include <math.h>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

static const qsreal qt_PI = 2.0 * ::asin(1.0);

Math::Math(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo):
    m_engine(engine),
    m_classInfo(classInfo)
{
}

Math::~Math()
{
}

void Math::construct(QScriptValueImpl *object, QScriptEnginePrivate *eng)
{
    QScriptClassInfo *classInfo = eng->registerClass(QLatin1String("Math"));

    Math *instance = new Math(eng, classInfo);
    eng->newObject(object, classInfo);
    object->setObjectData(instance);

    QScriptValue::PropertyFlags flags = QScriptValue::Undeletable
                                        | QScriptValue::ReadOnly
                                        | QScriptValue::SkipInEnumeration;

    object->setProperty(QLatin1String("E"),
                        QScriptValueImpl(eng, ::exp(1.0)), flags);
    object->setProperty(QLatin1String("LN2"),
                        QScriptValueImpl(eng, ::log(2.0)), flags);
    object->setProperty(QLatin1String("LN10"),
                        QScriptValueImpl(eng, ::log(10.0)), flags);
    object->setProperty(QLatin1String("LOG2E"),
                        QScriptValueImpl(eng, 1.0/::log(2.0)), flags);
    object->setProperty(QLatin1String("LOG10E"),
                        QScriptValueImpl(eng, 1.0/::log(10.0)), flags);
    object->setProperty(QLatin1String("PI"),
                        QScriptValueImpl(eng, qt_PI), flags);
    object->setProperty(QLatin1String("SQRT1_2"),
                        QScriptValueImpl(eng, ::sqrt(0.5)), flags);
    object->setProperty(QLatin1String("SQRT2"),
                        QScriptValueImpl(eng, ::sqrt(2.0)), flags);

    flags = QScriptValue::SkipInEnumeration;
    addFunction(*object, QLatin1String("abs"), method_abs, 1, flags);
    addFunction(*object, QLatin1String("acos"), method_acos, 1, flags);
    addFunction(*object, QLatin1String("asin"), method_asin, 0, flags);
    addFunction(*object, QLatin1String("atan"), method_atan, 1, flags);
    addFunction(*object, QLatin1String("atan2"), method_atan2, 2, flags);
    addFunction(*object, QLatin1String("ceil"), method_ceil, 1, flags);
    addFunction(*object, QLatin1String("cos"), method_cos, 1, flags);
    addFunction(*object, QLatin1String("exp"), method_exp, 1, flags);
    addFunction(*object, QLatin1String("floor"), method_floor, 1, flags);
    addFunction(*object, QLatin1String("log"), method_log, 1, flags);
    addFunction(*object, QLatin1String("max"), method_max, 2, flags);
    addFunction(*object, QLatin1String("min"), method_min, 2, flags);
    addFunction(*object, QLatin1String("pow"), method_pow, 2, flags);
    addFunction(*object, QLatin1String("random"), method_random, 0, flags);
    addFunction(*object, QLatin1String("round"), method_round, 1, flags);
    addFunction(*object, QLatin1String("sin"), method_sin, 1, flags);
    addFunction(*object, QLatin1String("sqrt"), method_sqrt, 1, flags);
    addFunction(*object, QLatin1String("tan"), method_tan, 1, flags);
}

/* copies the sign from y to x and returns the result */
static qsreal copySign(qsreal x, qsreal y)
{
    uchar *xch = (uchar *)&x;
    uchar *ych = (uchar *)&y;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        xch[0] = (xch[0] & 0x7f) | (ych[0] & 0x80);
    else
        xch[7] = (xch[7] & 0x7f) | (ych[7] & 0x80);
    return x;
}

QScriptValueImpl Math::method_abs(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v == 0) // 0 | -0
        return (QScriptValueImpl(eng, 0));
    else
        return (QScriptValueImpl(eng, v < 0 ? -v : v));
}

QScriptValueImpl Math::method_acos(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v > 1)
        return QScriptValueImpl(eng, qSNaN());
    return (QScriptValueImpl(eng, ::acos(v)));
}

QScriptValueImpl Math::method_asin(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v > 1)
        return QScriptValueImpl(eng, qSNaN());
    return (QScriptValueImpl(eng, ::asin(v)));
}

QScriptValueImpl Math::method_atan(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v == 0.0)
         return QScriptValueImpl(eng, v); 
    return (QScriptValueImpl(eng, ::atan(v)));
}

QScriptValueImpl Math::method_atan2(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *eng,
                                    QScriptClassInfo *)
{
    qsreal v1 = context->argument(0).toNumber();
    qsreal v2 = context->argument(1).toNumber();
#ifdef Q_OS_WINCE
    if (v1 == 0.0) {
        const bool v1MinusZero = _copysign(1.0, v1) < 0.0;
        const bool v2MinusZero = (v2 == 0 && _copysign(1.0, v2) < 0.0);
        if ((v1MinusZero && v2MinusZero) || (v1MinusZero && v2 == -1.0))
            return QScriptValueImpl(eng, -qt_PI);
        if (v2MinusZero)
            return QScriptValueImpl(eng, qt_PI);
        if (v1MinusZero && v2 == 1.0)
            return QScriptValueImpl(eng, -0.0);
#if defined(_X86_)
        if (v2 == 0.0 && (v1MinusZero || (!v1MinusZero && !v2MinusZero)))
            return QScriptValueImpl(eng, 0.0);
#endif
    }
#endif
#if defined(Q_OS_WINCE) && defined(_X86_)
    if (v1 == -1.0 && !_finite(v2) && _copysign(1.0, v2) > 0.0)
        return QScriptValueImpl(eng, -0.0);
#endif
    if ((v1 < 0) && qIsFinite(v1) && qIsInf(v2) && (copySign(1.0, v2) == 1.0))
        return QScriptValueImpl(eng, copySign(0, -1.0));
    if ((v1 == 0.0) && (v2 == 0.0)) {
        if ((copySign(1.0, v1) == 1.0) && (copySign(1.0, v2) == -1.0))
            return QScriptValueImpl(eng, qt_PI);
        else if ((copySign(1.0, v1) == -1.0) && (copySign(1.0, v2) == -1.0))
            return QScriptValueImpl(eng, -qt_PI);
    }
    return (QScriptValueImpl(eng, ::atan2(v1, v2)));
}

QScriptValueImpl Math::method_ceil(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v < 0.0 && v > -1.0)
        return QScriptValueImpl(eng, copySign(0, -1.0));
    return (QScriptValueImpl(eng, ::ceil(v)));
}

QScriptValueImpl Math::method_cos(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(eng, ::cos(v)));
}

QScriptValueImpl Math::method_exp(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (qIsInf(v)) {
        if (copySign(1.0, v) == -1.0)
            return QScriptValueImpl(eng, 0);
        else
            return QScriptValueImpl(eng, qInf());
    }
    return (QScriptValueImpl(eng, ::exp(v)));
}

QScriptValueImpl Math::method_floor(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *eng,
                                    QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(eng, ::floor(v)));
}

QScriptValueImpl Math::method_log(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v < 0)
        return QScriptValueImpl(eng, qSNaN());
    return (QScriptValueImpl(eng, ::log(v)));
}

QScriptValueImpl Math::method_max(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal mx = -qInf();
    for (int i = 0; i < context->argumentCount(); ++i) {
        qsreal x = context->argument(i).toNumber();
        if (x > mx || qIsNaN(x))
            mx = x;
    }
    return (QScriptValueImpl(eng, mx));
}

QScriptValueImpl Math::method_min(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal mx = qInf();
    for (int i = 0; i < context->argumentCount(); ++i) {
        qsreal x = context->argument(i).toNumber();
        if ((x == 0 && mx == x && copySign(1.0, x) == -1.0)
            || (x < mx) || qIsNaN(x)) {
            mx = x;
        }
    }
    return (QScriptValueImpl(eng, mx));
}

QScriptValueImpl Math::method_pow(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal x = context->argument(0).toNumber();
    qsreal y = context->argument(1).toNumber();
    if (qIsNaN(y))
        return QScriptValueImpl(eng, qSNaN());
    if (y == 0)
        return QScriptValueImpl(eng, 1);
    if (((x == 1) || (x == -1)) && qIsInf(y))
        return QScriptValueImpl(eng, qSNaN());
    if (((x == 0) && copySign(1.0, x) == 1.0) && (y < 0))
        return QScriptValueImpl(eng, qInf());
    if ((x == 0) && copySign(1.0, x) == -1.0) {
        if (y < 0) {
            if (::fmod(-y, 2.0) == 1.0)
                return QScriptValueImpl(eng, -qInf());
            else
                return QScriptValueImpl(eng, qInf()); 
        } else if (y > 0) {
             if (::fmod(y, 2.0) == 1.0)
                return QScriptValueImpl(eng, copySign(0, -1.0));
            else
                return QScriptValueImpl(eng, 0); 
        }
    } 
#ifdef Q_OS_AIX
    if (qIsInf(x) && copySign(1.0, x) == -1.0) {
        if (y > 0) {
            if (::fmod(y, 2.0) == 1.0)
                return QScriptValueImpl(eng, -qInf());
            else
                return QScriptValueImpl(eng, qInf());
        } else if (y < 0) { 
            if (::fmod(-y, 2.0) == 1.0)
                return QScriptValueImpl(eng, copySign(0, -1.0));
            else
                return QScriptValueImpl(eng, 0);
        }
    } 
#endif
    return (QScriptValueImpl(eng, ::pow(x, y)));
}

QScriptValueImpl Math::method_random(QScriptContextPrivate *,
                                     QScriptEnginePrivate *eng,
                                     QScriptClassInfo *)
{
    return (QScriptValueImpl(eng, qrand() / (qsreal) RAND_MAX));
}

QScriptValueImpl Math::method_round(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *eng,
                                    QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    v = copySign(::floor(v + 0.5), v);
    return (QScriptValueImpl(eng, v));
}

QScriptValueImpl Math::method_sin(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(eng, ::sin(v)));
}

QScriptValueImpl Math::method_sqrt(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(eng, ::sqrt(v)));
}

QScriptValueImpl Math::method_tan(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *eng,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v == 0.0)
         return QScriptValueImpl(eng, v); 
    return (QScriptValueImpl(eng, ::tan(v)));
}

void Math::addFunction(QScriptValueImpl &object, const QString &name,
                       QScriptInternalFunctionSignature fun, int length,
                       const QScriptValue::PropertyFlags flags)
{
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(object.engine());
    QScriptValueImpl val = eng_p->createFunction(fun, length, object.classInfo(), name);
    object.setProperty(name, val, flags);
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
