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

// for strtoll
#include <qplatformdefs.h>

#ifndef QT_NO_SCRIPT

#include "qscriptecmaglobal_p.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QVarLengthArray>
#include <QtCore/qnumeric.h>

QT_BEGIN_NAMESPACE

extern Q_CORE_EXPORT qlonglong qstrtoll(const char *nptr, const char **endptr, register int base, bool *ok);

static inline char toHex(char c)
{
    static const char hexnumbers[] = "0123456789ABCDEF";
    return hexnumbers[c & 0xf];
}

static int fromHex(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    return -1;
}

static QByteArray escape(const QString &input)
{
    QVarLengthArray<char> output;
    output.reserve(input.size() * 3);
    const int length = input.length();
    for (int i = 0; i < length; ++i) {
        ushort uc = input.at(i).unicode();
        if (uc < 0x100) {
            if (   (uc > 0x60 && uc < 0x7B)
                || (uc > 0x3F && uc < 0x5B)
                || (uc > 0x2C && uc < 0x3A)
                || (uc == 0x2A)
                || (uc == 0x2B)
                || (uc == 0x5F)) {
                output.append(char(uc));
            } else {
                output.append('%');
                output.append(toHex(uc >> 4));
                output.append(toHex(uc));
            }
        } else {
            output.append('%');
            output.append('u');
            output.append(toHex(uc >> 12));
            output.append(toHex(uc >> 8));
            output.append(toHex(uc >> 4));
            output.append(toHex(uc));
        }
    }
    return QByteArray(output.constData(), output.size());
}

static QString unescape(const QByteArray &input)
{
    QString result;
    int i = 0;
    const int length = input.length();
    while (i < length) {
        char c = input.at(i++);
        if ((c == '%') && (i + 1 < length)) {
            char a = input.at(i);
            if ((a == 'u') && (i + 4 < length)) {
                int d3 = fromHex(input.at(i+1));
                int d2 = fromHex(input.at(i+2));
                int d1 = fromHex(input.at(i+3));
                int d0 = fromHex(input.at(i+4));
                if ((d3 != -1) && (d2 != -1) && (d1 != -1) && (d0 != -1)) {
                    ushort uc = ushort((d3 << 12) | (d2 << 8) | (d1 << 4) | d0);
                    result.append(QChar(uc));
                    i += 5;
                } else {
                    result.append(QLatin1Char(c));
                }
            } else {
                int d1 = fromHex(a);
                int d0 = fromHex(input.at(i+1));
                if ((d1 != -1) && (d0 != -1)) {
                    c = (d1 << 4) | d0;
                    i += 2;
                }
                result.append(QLatin1Char(c));
            }
        } else {
            result.append(QLatin1Char(c));
        }
    }
    return result;
}

static const char uriReserved[] = ";/?:@&=+$,";
static const char uriUnescaped[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.!~*'()";

static QString encode(const QString &input, const QString &unescapedSet, bool *ok)
{
    QString output;
    const int length = input.length();
    int i = 0;
    while (i < length) {
        const QChar c = input.at(i);
        if (!unescapedSet.contains(c)) {
            ushort uc = c.unicode();
            if ((uc >= 0xDC00) && (uc <= 0xDFFF)) {
                // URIError
                break;
            }
            if (!((uc < 0xD800) || (uc > 0xDBFF))) {
                ++i;
                if (i == length) {
                    // URIError
                    break;
                }
                const ushort uc2 = input.at(i).unicode();
                if ((uc < 0xDC00) || (uc > 0xDFFF)) {
                    // URIError
                    break;
                }
                uc = ((uc - 0xD800) * 0x400) + (uc2 - 0xDC00) + 0x10000;
            }
            QString tmp(1, QChar(uc));
            QByteArray octets = tmp.toUtf8();
            for (int j = 0; j < octets.length(); ++j) {
                output.append(QLatin1Char('%'));
                output.append(QLatin1Char(toHex(octets.at(j) >> 4)));
                output.append(QLatin1Char(toHex(octets.at(j))));
            }
        } else {
            output.append(c);
        }
        ++i;
    }
    *ok = (i == length);
    return output;
}

static QString decode(const QString &input, const QString &reservedSet, bool *ok)
{
    QString output;
    const int length = input.length();
    int i = 0;
    const QChar percent = QLatin1Char('%');
    while (i < length) {
        const QChar c = input.at(i);
        if (c == percent) {
            int start = i;
            if (i + 2 >= length) {
                // URIError
                break;
            }
            int d1 = fromHex(input.at(i+1).toLatin1());
            int d0 = fromHex(input.at(i+2).toLatin1());
            if ((d1 == -1) || (d0 == -1)) {
                // URIError
                break;
            }
            int b = (d1 << 4) | d0;
            i += 2;
            if (b & 0x80) {
                int n = -1;
                while ((b << ++n) & 0x80) ;
                if ((n == 1) || (n > 4)) {
                    // URIError
                    break;
                }
                QByteArray octets;
                octets.append(b);
                if (i + (3 * (n - 1)) >= length) {
                    // URIError
                    break;
                }
                for (int j = 1; j < n; ++j) {
                    ++i;
                    if (input.at(i) != percent) {
                        // URIError
                        break;
                    }
                    d1 = fromHex(input.at(i+1).toLatin1());
                    d0 = fromHex(input.at(i+2).toLatin1());
                    if ((d1 == -1) || (d0 == -1)) {
                        // URIError
                        break;
                    }
                    b = (d1 << 4) | d0;
                    if ((b & 0xC0) != 0x80) {
                        // URIError
                        break;
                    }
                    i += 2;
                    octets.append(b);
                }
                QString tmp = QString::fromUtf8(octets);
                Q_ASSERT(tmp.length() == 1);
                uint v = tmp.at(0).unicode(); // ### need 32-bit value
                if (v < 0x10000) {
                    QChar z = QChar(ushort(v));
                    if (!reservedSet.contains(z)) {
                        output.append(z);
                    } else {
                        output.append(input.mid(start, i - start + 1));
                    }
                } else {
                    if (v > 0x10FFFF) {
                        // URIError
                        break;
                    }
                    ushort l = ushort(((v - 0x10000) & 0x3FF) + 0xDC00);
                    ushort h = ushort((((v - 0x10000) >> 10) & 0x3FF) + 0xD800);
                    output.append(QChar(l));
                    output.append(QChar(h));
                }
            } else {
                output.append(ushort(b));
            }
        } else {
            output.append(c);
        }
        ++i;
    }
    *ok = (i == length);
    return output;
}



namespace QScript {

class PrintFunction : public QScriptFunction
{
public:
    PrintFunction() {}

    virtual ~PrintFunction() {}

    virtual void execute(QScriptContextPrivate *context)
    {
        QScriptEnginePrivate *eng = context->enginePrivate();
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng->notifyFunctionEntry(context);
#endif
        QString result;
        for (int i = 0; i < context->argumentCount(); ++i) {
            if (i != 0)
                result.append(QLatin1String(" "));

            QString s = context->argument(i).toString();
            if (context->state() == QScriptContext::ExceptionState)
                break;
            result.append(s);
        }

        if (context->state() != QScriptContext::ExceptionState) {
            QTextStream qout(stdout, QIODevice::WriteOnly);
            qout << result << endl;
            context->setReturnValue(eng->undefinedValue());
        }
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng->notifyFunctionExit(context);
#endif
    }

    QString functionName() const
    {
        return QLatin1String("print");
    }
};

} // anonumous

namespace QScript { namespace Ecma {

Global::Global(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo)
    : m_engine(engine), m_classInfo(classInfo)
{
}

Global::~Global()
{
}

void Global::construct(QScriptValueImpl *object, QScriptEnginePrivate *eng)
{
    QScriptClassInfo *classInfo = eng->registerClass(QLatin1String("global"),
                                                     QScriptClassInfo::ActivationType);

    // create with prototype null, since Object.prototype doesn't exist at this point
    eng->newObject(object, eng->nullValue(), classInfo);

    Global *instance = new Global(eng, classInfo);
    object->setObjectData(instance);
}

void Global::initialize(QScriptValueImpl *object, QScriptEnginePrivate *eng)
{
    // set the real prototype
    object->setPrototype(eng->objectConstructor->publicPrototype);

    const QScriptValue::PropertyFlags flags = QScriptValue::Undeletable
                                              | QScriptValue::SkipInEnumeration;

    object->setProperty(QLatin1String("NaN"), QScriptValueImpl(eng, qSNaN()), flags);
    object->setProperty(QLatin1String("Infinity"), QScriptValueImpl(eng, qInf()), flags);
    object->setProperty(QLatin1String("undefined"), eng->undefinedValue(), flags);

    object->setProperty(QLatin1String("print"),
                        eng->createFunction(new PrintFunction()), flags);
    addFunction(*object, QLatin1String("parseInt"), method_parseInt, 2, flags);
    addFunction(*object, QLatin1String("parseFloat"), method_parseFloat, 1, flags);
    addFunction(*object, QLatin1String("isNaN"), method_isNaN, 1, flags);
    addFunction(*object, QLatin1String("isFinite"), method_isFinite, 1, flags);
    addFunction(*object, QLatin1String("decodeURI"), method_decodeURI, 1, flags);
    addFunction(*object, QLatin1String("decodeURIComponent"), method_decodeURIComponent, 1, flags);
    addFunction(*object, QLatin1String("encodeURI"), method_encodeURI, 1, flags);
    addFunction(*object, QLatin1String("encodeURIComponent"), method_encodeURIComponent, 1, flags);
    addFunction(*object, QLatin1String("escape"), method_escape, 1, flags);
    addFunction(*object, QLatin1String("unescape"), method_unescape, 1, flags);
    addFunction(*object, QLatin1String("version"), method_version, 0, flags);
    addFunction(*object, QLatin1String("gc"), method_gc, 0, flags);
}

QScriptValueImpl Global::method_parseInt(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *)
{
    if (context->argumentCount() == 0) {
        return QScriptValueImpl(eng, qSNaN());
    }
    qsreal radix = 0;
    if (context->argumentCount() > 1) {
        radix = context->argument(1).toInteger();
        if (qIsNaN(radix) || (radix && (radix < 2 || radix > 36))) {
            return QScriptValueImpl(eng, qSNaN());
        }
    }
    QString str = context->argument(0).toString().trimmed();
    if (radix == 0) {
        if ((str.length() >= 2) && (str.at(0) == QLatin1Char('0'))) {
            if ((str.at(1) == QLatin1Char('x'))
                || (str.at(1) == QLatin1Char('X'))) {
                str.remove(0, 2);
                radix = 16;
            } else {
                str.remove(0, 1);
                radix = 8;
            }
        }
    }

    const QByteArray ba = str.toUtf8();
    const char *startPtr = ba.constData();
    qsreal result;
//#if defined(Q_WS_WIN) && !defined(Q_CC_MINGW)
// always use the Qt version for now
#if 1
    const char *endPtr = 0;
    bool ok;
    result = qstrtoll(startPtr, &endPtr, int (radix), &ok);
#else
    char *endPtr = 0;
    result = strtoll(startPtr, &endPtr, int (radix));
#endif
    if (startPtr == endPtr) {
        if (str.isEmpty())
            result = qSNaN();
        else if (str == QLatin1String("Infinity"))
            result = +qInf();
        else if (str == QLatin1String("+Infinity"))
            result = +qInf();
        else if (str == QLatin1String("-Infinity"))
            result = -qInf();
        else
            result = qSNaN();
    }

    return QScriptValueImpl(eng, result);
}

QScriptValueImpl Global::method_parseFloat(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *)
{
    if (context->argumentCount() == 0)
        return QScriptValueImpl(eng, qSNaN());

    QString str = context->argument(0).toString().trimmed();
    bool ok = false;
    qsreal result = str.toDouble(&ok);
    if (!ok) {
        if (str == QLatin1String("Infinity"))
            result = +qInf();
        else if (str == QLatin1String("+Infinity"))
            result = +qInf();
        else if (str == QLatin1String("-Infinity"))
            result = -qInf();
        else if (str.isEmpty())
            result = qSNaN();
        else if (str.at(0).isNumber())
            result = 0;
        else
            result = qSNaN();
    }

    return QScriptValueImpl(eng, result);
}

QScriptValueImpl Global::method_isNaN(QScriptContextPrivate *context,
                                      QScriptEnginePrivate *eng,
                                      QScriptClassInfo *)
{
    qsreal v = qSNaN();
    if (context->argumentCount() > 0)
        v = context->argument(0).toNumber();
    return (QScriptValueImpl(eng, qIsNaN(v)));
}

QScriptValueImpl Global::method_isFinite(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *)
{
    qsreal v = qInf();
    if (context->argumentCount() > 0)
        v = context->argument(0).toNumber();
    return (QScriptValueImpl(eng, qIsFinite(v)));
}

QScriptValueImpl Global::method_decodeURI(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *)
{
    QScriptValueImpl result;

    if (context->argumentCount() > 0) {
        QString str = context->argument(0).toString();
        bool ok;
        QString out = decode(str, QString::fromUtf8(uriReserved) + QString::fromUtf8("#"), &ok);
        if (ok)
            return QScriptValueImpl(eng, out);
        else
            return context->throwError(QScriptContext::URIError,
                                         QLatin1String("malformed URI sequence"));
    }

    return eng->undefinedValue();
}

QScriptValueImpl Global::method_decodeURIComponent(QScriptContextPrivate *context,
                                                   QScriptEnginePrivate *eng,
                                                   QScriptClassInfo *)
{
    QScriptValueImpl result;
    if (context->argumentCount() > 0) {
        QString str = context->argument(0).toString();
        bool ok;
        QString out = decode(str, QString::fromUtf8(""), &ok);
        if (ok)
            result = QScriptValueImpl(eng, out);
        else
            result = context->throwError(QScriptContext::URIError,
                                         QLatin1String("malformed URI sequence"));
    } else {
        result = eng->undefinedValue();
    }
    return result;
}

QScriptValueImpl Global::method_encodeURI(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *)
{
    QScriptValueImpl result;
    if (context->argumentCount() > 0) {
        QString str = context->argument(0).toString();
        bool ok;
        QString out = encode(str,
                             QLatin1String(uriReserved)
                             + QLatin1String(uriUnescaped)
                             + QString::fromUtf8("#"),
                             &ok);
        if (ok)
            result = QScriptValueImpl(eng, out);
        else
            result = context->throwError(QScriptContext::URIError,
                                         QLatin1String("malformed URI sequence"));
    } else {
        result = eng->undefinedValue();
    }
    return result;
}

QScriptValueImpl Global::method_encodeURIComponent(QScriptContextPrivate *context,
                                                   QScriptEnginePrivate *eng,
                                                   QScriptClassInfo *)
{
    QScriptValueImpl result;
    if (context->argumentCount() > 0) {
        QString str = context->argument(0).toString();
        bool ok;
        QString out = encode(str, QLatin1String(uriUnescaped), &ok);
        if (ok)
            result = QScriptValueImpl(eng, out);
        else
            result = context->throwError(QScriptContext::URIError,
                                         QLatin1String("malformed URI sequence"));
    } else {
        result = eng->undefinedValue();
    }
    return result;
}

QScriptValueImpl Global::method_escape(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *)
{
    if (context->argumentCount() > 0) {
        QString str = context->argument(0).toString();
        return QScriptValueImpl(eng, QLatin1String(escape(str)));
    }
    return QScriptValueImpl(eng, QLatin1String("undefined"));
}

QScriptValueImpl Global::method_unescape(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *)
{
    if (context->argumentCount() > 0) {
        QByteArray data = context->argument(0).toString().toLatin1();
        return QScriptValueImpl(eng, unescape(data));
    }
    return QScriptValueImpl(eng, QLatin1String("undefined"));
}

QScriptValueImpl Global::method_version(QScriptContextPrivate *,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *)
{
    return (QScriptValueImpl(eng, 1));
}

QScriptValueImpl Global::method_gc(QScriptContextPrivate *,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *)
{
    eng->gc();
    return QScriptValueImpl(eng, eng->objectAllocator.freeBlocks());
}

void Global::addFunction(QScriptValueImpl &object, const QString &name,
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
