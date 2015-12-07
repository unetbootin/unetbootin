/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "QtTest/private/qsignaldumper_p.h"

#include <QtCore/qlist.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

#include "QtTest/private/qtestlog_p.h"

QT_BEGIN_NAMESPACE

namespace QTest
{

inline static void qPrintMessage(const QByteArray &ba)
{
    QTestLog::info(ba.constData(), 0, 0);
}

Q_GLOBAL_STATIC(QList<QByteArray>, ignoreClasses)
static int iLevel = 0;
static int ignoreLevel = 0;
enum { IndentSpacesCount = 4 };

static QByteArray memberName(const QMetaMethod &member)
{
    QByteArray ba = member.signature();
    return ba.left(ba.indexOf('('));
}

static void qSignalDumperCallback(QObject *caller, int method_index, void **argv)
{
    Q_ASSERT(caller); Q_ASSERT(argv); Q_UNUSED(argv);
    const QMetaObject *mo = caller->metaObject();
    Q_ASSERT(mo);
    QMetaMethod member = mo->method(method_index);
    Q_ASSERT(member.signature());

    if (QTest::ignoreClasses() && QTest::ignoreClasses()->contains(mo->className())) {
        ++QTest::ignoreLevel;
        return;
    }

    QByteArray str;
    str.fill(' ', QTest::iLevel++ * QTest::IndentSpacesCount);
    str += "Signal: ";
    str += mo->className();
    str += "(";
    str += caller->objectName().toLocal8Bit();
    str += ") ";
    str += QTest::memberName(member);
    str += " (";

    QList<QByteArray> args = member.parameterTypes();
    for (int i = 0; i < args.count(); ++i) {
        const QByteArray &arg = args.at(i);
        int typeId = QMetaType::type(args.at(i).constData());
        if (arg.endsWith('*') || arg.endsWith('&')) {
            str += '(';
            str += arg;
            str += ')';
            if (arg.endsWith('&'))
                str += '@';

            quintptr addr = quintptr(*reinterpret_cast<void **>(argv[i + 1]));
            str.append(QByteArray::number(addr, 16));
        } else if (typeId != QMetaType::Void) {
            str.append(arg)
                .append("(")
                .append(QVariant(typeId, argv[i + 1]).toString().toLocal8Bit())
                .append(")");
        }
        str.append(", ");
    }
    if (str.endsWith(", "))
        str.chop(2);
    str.append(")");
    qPrintMessage(str);
}

static void qSignalDumperCallbackSlot(QObject *caller, int method_index, void **argv)
{
    Q_ASSERT(caller); Q_ASSERT(argv); Q_UNUSED(argv);
    const QMetaObject *mo = caller->metaObject();
    Q_ASSERT(mo);
    QMetaMethod member = mo->method(method_index);
    if (!member.signature())
        return;

    if (QTest::ignoreLevel ||
            (QTest::ignoreClasses() && QTest::ignoreClasses()->contains(mo->className())))
        return;

    QByteArray str;
    str.fill(' ', QTest::iLevel * QTest::IndentSpacesCount);
    str += "Slot: ";
    str += mo->className();
    str += "(";
    str += caller->objectName().toLocal8Bit();
    str += ") ";
    str += member.signature();
    qPrintMessage(str);
}

static void qSignalDumperCallbackEndSignal(QObject *caller, int /*method_index*/)
{
    Q_ASSERT(caller); Q_ASSERT(caller->metaObject());
    if (QTest::ignoreClasses()
            && QTest::ignoreClasses()->contains(caller->metaObject()->className())) {
        --QTest::ignoreLevel;
        Q_ASSERT(QTest::ignoreLevel >= 0);
        return;
    }
    --QTest::iLevel;
    Q_ASSERT(QTest::iLevel >= 0);
}

}

// this struct is copied from qobject_p.h to prevent us
// from including private Qt headers.
struct QSignalSpyCallbackSet
{
    typedef void (*BeginCallback)(QObject *caller, int method_index, void **argv);
    typedef void (*EndCallback)(QObject *caller, int method_index);
    BeginCallback signal_begin_callback,
                  slot_begin_callback;
    EndCallback signal_end_callback,
                slot_end_callback;
};
extern void Q_CORE_EXPORT qt_register_signal_spy_callbacks(const QSignalSpyCallbackSet &);

void QSignalDumper::startDump()
{
    static QSignalSpyCallbackSet set = { QTest::qSignalDumperCallback,
        QTest::qSignalDumperCallbackSlot, QTest::qSignalDumperCallbackEndSignal, 0 };
    qt_register_signal_spy_callbacks(set);
}

void QSignalDumper::endDump()
{
    static QSignalSpyCallbackSet nset = { 0, 0, 0 ,0 };
    qt_register_signal_spy_callbacks(nset);
}

void QSignalDumper::ignoreClass(const QByteArray &klass)
{
    if (QTest::ignoreClasses())
        QTest::ignoreClasses()->append(klass);
}

void QSignalDumper::clearIgnoredClasses()
{
    if (QTest::ignoreClasses())
        QTest::ignoreClasses()->clear();
}

QT_END_NAMESPACE
