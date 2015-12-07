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
                        QScriptValueImpl(::exp(1.0)), flags);
    object->setProperty(QLatin1String("LN2"),
                        QScriptValueImpl(::log(2.0)), flags);
    object->setProperty(QLatin1String("LN10"),
                        QScriptValueImpl(::log(10.0)), flags);
    object->setProperty(QLatin1String("LOG2E"),
                        QScriptValueImpl(1.0/::log(2.0)), flags);
    object->setProperty(QLatin1String("LOG10E"),
                        QScriptValueImpl(1.0/::log(10.0)), flags);
    object->setProperty(QLatin1String("PI"),
                        QScriptValueImpl(qt_PI), flags);
    object->setProperty(QLatin1String("SQRT1_2"),
                        QScriptValueImpl(::sqrt(0.5)), flags);
    object->setProperty(QLatin1String("SQRT2"),
                        QScriptValueImpl(::sqrt(2.0)), flags);

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
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v == 0) // 0 | -0
        return (QScriptValueImpl(0));
    else
        return (QScriptValueImpl(v < 0 ? -v : v));
}

QScriptValueImpl Math::method_acos(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v > 1)
        return QScriptValueImpl(qSNaN());
    return (QScriptValueImpl(::acos(v)));
}

QScriptValueImpl Math::method_asin(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v > 1)
        return QScriptValueImpl(qSNaN());
    return (QScriptValueImpl(::asin(v)));
}

QScriptValueImpl Math::method_atan(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v == 0.0)
         return QScriptValueImpl(v); 
    return (QScriptValueImpl(::atan(v)));
}

QScriptValueImpl Math::method_atan2(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *,
                                    QScriptClassInfo *)
{
    qsreal v1 = context->argument(0).toNumber();
    qsreal v2 = context->argument(1).toNumber();
#ifdef Q_OS_WINCE
    if (v1 == 0.0) {
        const bool v1MinusZero = _copysign(1.0, v1) < 0.0;
        const bool v2MinusZero = (v2 == 0 && _copysign(1.0, v2) < 0.0);
        if ((v1MinusZero && v2MinusZero) || (v1MinusZero && v2 == -1.0))
            return QScriptValueImpl(-qt_PI);
        if (v2MinusZero)
            return QScriptValueImpl(qt_PI);
        if (v1MinusZero && v2 == 1.0)
            return QScriptValueImpl(-0.0);
#if defined(_X86_)
        if (v2 == 0.0 && (v1MinusZero || (!v1MinusZero && !v2MinusZero)))
            return QScriptValueImpl(0.0);
#endif
    }
#endif
#if defined(Q_OS_WINCE) && defined(_X86_)
    if (v1 == -1.0 && !_finite(v2) && _copysign(1.0, v2) > 0.0)
        return QScriptValueImpl(-0.0);
#endif
    if ((v1 < 0) && qIsFinite(v1) && qIsInf(v2) && (copySign(1.0, v2) == 1.0))
        return QScriptValueImpl(copySign(0, -1.0));
    if ((v1 == 0.0) && (v2 == 0.0)) {
        if ((copySign(1.0, v1) == 1.0) && (copySign(1.0, v2) == -1.0))
            return QScriptValueImpl(qt_PI);
        else if ((copySign(1.0, v1) == -1.0) && (copySign(1.0, v2) == -1.0))
            return QScriptValueImpl(-qt_PI);
    }
    return (QScriptValueImpl(::atan2(v1, v2)));
}

QScriptValueImpl Math::method_ceil(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v < 0.0 && v > -1.0)
        return QScriptValueImpl(copySign(0, -1.0));
    return (QScriptValueImpl(::ceil(v)));
}

QScriptValueImpl Math::method_cos(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(::cos(v)));
}

QScriptValueImpl Math::method_exp(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (qIsInf(v)) {
        if (copySign(1.0, v) == -1.0)
            return QScriptValueImpl(0);
        else
            return QScriptValueImpl(qInf());
    }
    return (QScriptValueImpl(::exp(v)));
}

QScriptValueImpl Math::method_floor(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *,
                                    QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(::floor(v)));
}

QScriptValueImpl Math::method_log(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v < 0)
        return QScriptValueImpl(qSNaN());
    return (QScriptValueImpl(::log(v)));
}

QScriptValueImpl Math::method_max(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal mx = -qInf();
    for (int i = 0; i < context->argumentCount(); ++i) {
        qsreal x = context->argument(i).toNumber();
        if (x > mx || qIsNaN(x))
            mx = x;
    }
    return (QScriptValueImpl(mx));
}

QScriptValueImpl Math::method_min(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
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
    return (QScriptValueImpl(mx));
}

QScriptValueImpl Math::method_pow(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal x = context->argument(0).toNumber();
    qsreal y = context->argument(1).toNumber();
    if (qIsNaN(y))
        return QScriptValueImpl(qSNaN());
    if (y == 0)
        return QScriptValueImpl(1);
    if (((x == 1) || (x == -1)) && qIsInf(y))
        return QScriptValueImpl(qSNaN());
    if (((x == 0) && copySign(1.0, x) == 1.0) && (y < 0))
        return QScriptValueImpl(qInf());
    if ((x == 0) && copySign(1.0, x) == -1.0) {
        if (y < 0) {
            if (::fmod(-y, 2.0) == 1.0)
                return QScriptValueImpl(-qInf());
            else
                return QScriptValueImpl(qInf()); 
        } else if (y > 0) {
             if (::fmod(y, 2.0) == 1.0)
                return QScriptValueImpl(copySign(0, -1.0));
            else
                return QScriptValueImpl(0); 
        }
    } 
#ifdef Q_OS_AIX
    if (qIsInf(x) && copySign(1.0, x) == -1.0) {
        if (y > 0) {
            if (::fmod(y, 2.0) == 1.0)
                return QScriptValueImpl(-qInf());
            else
                return QScriptValueImpl(qInf());
        } else if (y < 0) { 
            if (::fmod(-y, 2.0) == 1.0)
                return QScriptValueImpl(copySign(0, -1.0));
            else
                return QScriptValueImpl(0);
        }
    } 
#endif
    return (QScriptValueImpl(::pow(x, y)));
}

QScriptValueImpl Math::method_random(QScriptContextPrivate *,
                                     QScriptEnginePrivate *,
                                     QScriptClassInfo *)
{
    return (QScriptValueImpl(qrand() / (qsreal) RAND_MAX));
}

QScriptValueImpl Math::method_round(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *,
                                    QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    v = copySign(::floor(v + 0.5), v);
    return (QScriptValueImpl(v));
}

QScriptValueImpl Math::method_sin(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(::sin(v)));
}

QScriptValueImpl Math::method_sqrt(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *,
                                   QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    return (QScriptValueImpl(::sqrt(v)));
}

QScriptValueImpl Math::method_tan(QScriptContextPrivate *context,
                                  QScriptEnginePrivate *,
                                  QScriptClassInfo *)
{
    qsreal v = context->argument(0).toNumber();
    if (v == 0.0)
         return QScriptValueImpl(v); 
    return (QScriptValueImpl(::tan(v)));
}

void Math::addFunction(QScriptValueImpl &object, const QString &name,
                       QScriptInternalFunctionSignature fun, int length,
                       const QScriptValue::PropertyFlags flags)
{
    QScriptEnginePrivate *eng_p = object.engine();
    QScriptValueImpl val = eng_p->createFunction(fun, length, object.classInfo(), name);
    object.setProperty(name, val, flags);
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
