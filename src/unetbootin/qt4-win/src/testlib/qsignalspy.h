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

#ifndef QSIGNALSPY_H
#define QSIGNALSPY_H

#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

class QVariant;

/* ### Qt5: change the class to use regular BC mechanisms, such that we can
 * implement things like suggested in task 160192. */
class QSignalSpy: public QObject, public QList<QList<QVariant> >
{
public:
    QSignalSpy(QObject *obj, const char *aSignal)
    {
#ifdef Q_CC_BOR
        const int memberOffset = QObject::staticMetaObject.methodCount();
#else
        static const int memberOffset = QObject::staticMetaObject.methodCount();
#endif
        Q_ASSERT(obj);
        Q_ASSERT(aSignal);

        if (aSignal[0] - '0' != QSIGNAL_CODE) {
            qWarning("QSignalSpy: Not a valid signal, use the SIGNAL macro");
            return;
        }

        QByteArray ba = QMetaObject::normalizedSignature(aSignal + 1);
        const QMetaObject *mo = obj->metaObject();
        int sigIndex = mo->indexOfMethod(ba.constData());
        if (sigIndex < 0) {
            qWarning("QSignalSpy: No such signal: '%s'", ba.constData());
            return;
        }

        if (!QMetaObject::connect(obj, sigIndex, this, memberOffset,
                    Qt::DirectConnection, 0)) {
            qWarning("QSignalSpy: QMetaObject::connect returned false. Unable to connect.");
            return;
        }
        sig = ba;
        initArgs(mo->method(sigIndex));
    }

    inline bool isValid() const { return !sig.isEmpty(); }
    inline QByteArray signal() const { return sig; }


    int qt_metacall(QMetaObject::Call call, int id, void **a)
    {
        id = QObject::qt_metacall(call, id, a);
        if (id < 0)
            return id;

        if (call == QMetaObject::InvokeMetaMethod) {
            if (id == 0) {
                appendArgs(a);
            }
            --id;
        }
        return id;
    }

private:
    void initArgs(const QMetaMethod &member)
    {
        QList<QByteArray> params = member.parameterTypes();
        for (int i = 0; i < params.count(); ++i) {
            int tp = QMetaType::type(params.at(i).constData());
            if (tp == QMetaType::Void)
                qWarning("Don't know how to handle '%s', use qRegisterMetaType to register it.",
                         params.at(i).constData());
            args << tp;
        }
    }

    void appendArgs(void **a)
    {
        QList<QVariant> list;
        for (int i = 0; i < args.count(); ++i) {
            QMetaType::Type type = static_cast<QMetaType::Type>(args.at(i));
            list << QVariant(type, a[i + 1]);
        }
        append(list);
    }

    // the full, normalized signal name
    QByteArray sig;
    // holds the QMetaType types for the argument list of the signal
    QList<int> args;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
