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

#include "qscriptecmaregexp_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QtDebug>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

RegExp::RegExp(QScriptEnginePrivate *eng):
    Core(eng, QLatin1String("RegExp"), QScriptClassInfo::RegExpType)
{
    newRegExp(&publicPrototype, QString(), /*flags=*/0);

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("exec"), method_exec, 1);
    addPrototypeFunction(QLatin1String("test"), method_test, 1);
    addPrototypeFunction(QLatin1String("toString"), method_toString, 1);
}

RegExp::~RegExp()
{
}

RegExp::Instance *RegExp::Instance::get(const QScriptValueImpl &object, QScriptClassInfo *klass)
{
    if (! klass || klass == object.classInfo())
        return static_cast<Instance*> (object.objectData());

    return 0;
}

void RegExp::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    QString P;
    int F;
    QScriptValueImpl pattern = context->argument(0);
    QScriptValueImpl flags = context->argument(1);
    if (!context->isCalledAsConstructor()) {
        if ((pattern.classInfo() == classInfo()) && flags.isUndefined()) {
            context->m_result = pattern;
            goto Lout;
        }
    }
    if (pattern.classInfo() == classInfo()) {
        if (!flags.isUndefined()) {
            context->throwTypeError(QString::fromLatin1("cannot specify flags when creating a copy of a RegExp"));
            goto Lout;
        }
        Instance *data = Instance::get(pattern, classInfo());
#ifndef QT_NO_REGEXP
        P = data->value.pattern();
#else
        P = data->pattern;
#endif
        F = data->flags;
    } else {
        if (!pattern.isUndefined())
            P = pattern.toString();
        F = 0;
        if (!flags.isUndefined()) {
            QString flagsStr = flags.toString();
            for (int i = 0; i < flagsStr.length(); ++i) {
                int bitflag = flagFromChar(flagsStr.at(i));
                if (bitflag == 0) {
                    context->throwError(
                        QScriptContext::SyntaxError,
                        QString::fromUtf8("invalid regular expression flag '%0'")
                        .arg(flagsStr.at(i)));
                    goto Lout;
                }
                F |= bitflag;
            }
        }
    }
    if (context->isCalledAsConstructor()) {
        QScriptValueImpl &object = context->m_thisObject;
        object.setClassInfo(classInfo());
        object.setPrototype(publicPrototype);
#ifndef QT_NO_REGEXP
        initRegExp(&object, toRegExp(P, F), F);
#else
        initRegExp(&object, P, F);
#endif
    } else {
        newRegExp(&context->m_result, P, F);
    }
 Lout: ;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

void RegExp::newRegExp(QScriptValueImpl *result, const QString &pattern, int flags)
{
#ifndef QT_NO_REGEXP
    QRegExp rx = toRegExp(pattern, flags);
    newRegExp_helper(result, rx, flags);
#else
    engine()->newObject(result, publicPrototype, classInfo());
    initRegExp(result, pattern, flags);
#endif // QT_NO_REGEXP
}

#ifndef QT_NO_REGEXP
void RegExp::newRegExp(QScriptValueImpl *result, const QRegExp &rx, int flags)
{
    Q_ASSERT(!(flags & IgnoreCase) || (rx.caseSensitivity() == Qt::CaseInsensitive));
    newRegExp_helper(result, rx, flags);
}

void RegExp::newRegExp_helper(QScriptValueImpl *result, const QRegExp &rx,
                              int flags)
{
    engine()->newObject(result, publicPrototype, classInfo());
    initRegExp(result, rx, flags);
}

QRegExp RegExp::toRegExp(const QScriptValueImpl &value) const
{
    Instance *rx_data = Instance::get(value, classInfo());
    Q_ASSERT(rx_data != 0);
    return rx_data->value;
}

QRegExp RegExp::toRegExp(const QString &pattern, int flags)
{
    bool ignoreCase = (flags & IgnoreCase) != 0;
    return QRegExp(pattern,
                   (ignoreCase ? Qt::CaseInsensitive: Qt::CaseSensitive),
                   QRegExp::RegExp2);
}

#endif // QT_NO_REGEXP

void RegExp::initRegExp(QScriptValueImpl *result,
#ifndef QT_NO_REGEXP
                        const QRegExp &rx,
#else
                        const QString &pattern,
#endif
                        int flags)
{
    Instance *instance = new Instance();
#ifndef QT_NO_REGEXP
    instance->value = rx;
#else
    instance->pattern = pattern;
#endif
    instance->flags = flags;
    result->setObjectData(instance);

    bool global = (flags & Global) != 0;
    bool ignoreCase = (flags & IgnoreCase) != 0;
    bool multiline = (flags & Multiline) != 0;

    QScriptValue::PropertyFlags propertyFlags = QScriptValue::SkipInEnumeration
                                                | QScriptValue::Undeletable
                                                | QScriptValue::ReadOnly;

    result->setProperty(QLatin1String("global"), QScriptValueImpl(global),
                        propertyFlags);
    result->setProperty(QLatin1String("ignoreCase"), QScriptValueImpl(ignoreCase),
                        propertyFlags);
    result->setProperty(QLatin1String("multiline"), QScriptValueImpl(multiline),
                        propertyFlags);
#ifndef QT_NO_REGEXP
    const QString &pattern = rx.pattern();
#endif
    result->setProperty(QLatin1String("source"), QScriptValueImpl(engine(), pattern),
                        propertyFlags);
    result->setProperty(QLatin1String("lastIndex"), QScriptValueImpl(0),
                        propertyFlags & ~QScriptValue::ReadOnly);
}

int RegExp::flagFromChar(const QChar &ch)
{
    static QHash<QChar, int> flagsHash;
    if (flagsHash.isEmpty()) {
        flagsHash[QLatin1Char('g')] = Global;
        flagsHash[QLatin1Char('i')] = IgnoreCase;
        flagsHash[QLatin1Char('m')] = Multiline;
    }
    QHash<QChar, int>::const_iterator it;
    it = flagsHash.constFind(ch);
    if (it == flagsHash.constEnd())
        return 0;
    return it.value();
}

QString RegExp::flagsToString(int flags)
{
    QString result;
    if (flags & Global)
        result += QLatin1Char('g');
    if (flags & IgnoreCase)
        result += QLatin1Char('i');
    if (flags & Multiline)
        result += QLatin1Char('m');
    return result;
}

QScriptValueImpl RegExp::method_exec(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("RegExp.prototype.exec"));
    }
    Instance *rx_data = Instance::get(self, classInfo);
    Q_ASSERT(rx_data != 0);

    QString S = context->argument(0).toString();
    int length = S.length();
    QScriptValueImpl lastIndex = self.property(QLatin1String("lastIndex"));

    int i = lastIndex.isValid() ? int (lastIndex.toInteger()) : 0;
    bool global = self.property(QLatin1String("global")).toBoolean();

    if (! global)
        i = 0;

    if (i < 0 || i >= length)
        return (eng->nullValue());

#ifndef QT_NO_REGEXP
    int index = rx_data->value.indexIn(S, i);
    if (index == -1)
#endif // QT_NO_REGEXP
        return eng->nullValue();

#ifndef QT_NO_REGEXP
    int e = index + rx_data->value.matchedLength();

    if (global)
        self.setProperty(QLatin1String("lastIndex"), QScriptValueImpl(e));

    QScript::Array elts(eng);
    QStringList capturedTexts = rx_data->value.capturedTexts();
    for (int i = 0; i < capturedTexts.count(); ++i)
        elts.assign(i, QScriptValueImpl(eng, capturedTexts.at(i)));

    QScriptValueImpl r = eng->newArray(elts);

    r.setProperty(QLatin1String("index"), QScriptValueImpl(index));
    r.setProperty(QLatin1String("input"), QScriptValueImpl(eng, S));

    return r;
#endif // QT_NO_REGEXP
}

QScriptValueImpl RegExp::method_test(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    QScriptValueImpl r = method_exec(context, eng, classInfo);
    return QScriptValueImpl(!r.isNull());
}

QScriptValueImpl RegExp::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        QString result;
        result += QLatin1Char('/');
#ifndef QT_NO_REGEXP
        const QString &pattern = instance->value.pattern();
#else
        const QString &pattern = instance->pattern;
#endif
        if (pattern.isEmpty())
            result += QLatin1String("(?:)");
        else
            result += pattern; // ### quote
        result += QLatin1Char('/');
        result += flagsToString(instance->flags);
        return (QScriptValueImpl(eng, result));
    }

    return throwThisObjectTypeError(
        context, QLatin1String("RegExp.prototype.toString"));
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
