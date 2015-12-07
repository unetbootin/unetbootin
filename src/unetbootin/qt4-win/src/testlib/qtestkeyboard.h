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

#ifndef QTESTKEYBOARD_H
#define QTESTKEYBOARD_H

#if 0
// inform syncqt
#pragma qt_no_master_include
#endif

#include <QtTest/qtestassert.h>
#include <QtTest/qtest_global.h>
#include <QtTest/qtestsystem.h>
#include <QtTest/qtestspontaneevent.h>

#include <QtCore/qpointer.h>
#include <QtGui/qapplication.h>
#include <QtGui/qevent.h>
#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

namespace QTest
{
    enum KeyAction { Press, Release, Click };

    static void simulateEvent(QWidget *widget, bool press, int code,
                              Qt::KeyboardModifiers modifier, QString text, bool repeat, int delay=-1)
    {
        QTEST_ASSERT(widget);
        extern int Q_TESTLIB_EXPORT defaultKeyDelay();

        if (delay == -1 || delay < defaultKeyDelay())
            delay = defaultKeyDelay();
        if(delay > 0)
            QTest::qWait(delay);

        QKeyEvent a(press ? QEvent::KeyPress : QEvent::KeyRelease, code, modifier, text, repeat);
        QSpontaneKeyEvent::setSpontaneous(&a);
        if (!qApp->notify(widget, &a))
            QTest::qWarn("Keyboard event not accepted by receiving widget");
    }

    static void sendKeyEvent(KeyAction action, QWidget *widget, Qt::Key code,
                             QString text, Qt::KeyboardModifiers modifier, int delay=-1)
    {
        QTEST_ASSERT(qApp);

        if (!widget)
            widget = QWidget::keyboardGrabber();
        if (!widget) {
            if (QWidget *apw = QApplication::activePopupWidget())
                widget = apw->focusWidget() ? apw->focusWidget() : apw;
            else
                widget = QApplication::focusWidget();
        }
        if (!widget)
            widget = QApplication::activeWindow();

        QTEST_ASSERT(widget);

        if (action == Click) {
            QPointer<QWidget> ptr(widget);
            sendKeyEvent(Press, widget, code, text, modifier, delay);
            if (!ptr) {
                // if we send key-events to embedded widgets, they might be destroyed
                // when the user presses Return
                return;
            }
            sendKeyEvent(Release, widget, code, text, modifier, delay);
            return;
        }

        bool repeat = false;

        if (action == Press) {
            if (modifier & Qt::ShiftModifier)
                simulateEvent(widget, true, Qt::Key_Shift, 0, QString(), false, delay);

            if (modifier & Qt::ControlModifier)
                simulateEvent(widget, true, Qt::Key_Control, modifier & Qt::ShiftModifier, QString(), false, delay);

            if (modifier & Qt::AltModifier)
                simulateEvent(widget, true, Qt::Key_Alt,
                              modifier & (Qt::ShiftModifier | Qt::ControlModifier), QString(), false, delay);
            if (modifier & Qt::MetaModifier)
                simulateEvent(widget, true, Qt::Key_Meta, modifier & (Qt::ShiftModifier
                                                                      | Qt::ControlModifier | Qt::AltModifier), QString(), false, delay);
            simulateEvent(widget, true, code, modifier, text, repeat, delay);
        } else if (action == Release) {
            simulateEvent(widget, false, code, modifier, text, repeat, delay);

            if (modifier & Qt::MetaModifier)
                simulateEvent(widget, false, Qt::Key_Meta, modifier, QString(), false, delay);
            if (modifier & Qt::AltModifier)
                simulateEvent(widget, false, Qt::Key_Alt, modifier &
                              (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier), QString(), false, delay);

            if (modifier & Qt::ControlModifier)
                simulateEvent(widget, false, Qt::Key_Control,
                              modifier & (Qt::ShiftModifier | Qt::ControlModifier), QString(), false, delay);

            if (modifier & Qt::ShiftModifier)
                simulateEvent(widget, false, Qt::Key_Shift, modifier & Qt::ShiftModifier, QString(), false, delay);
        }
    }

    // Convenience function
    static void sendKeyEvent(KeyAction action, QWidget *widget, Qt::Key code,
                             char ascii, Qt::KeyboardModifiers modifier, int delay=-1)
    {
        QString text;
        if (ascii)
            text = QString(QChar::fromLatin1(ascii));
        sendKeyEvent(action, widget, code, text, modifier, delay);
    }

    inline static void keyEvent(KeyAction action, QWidget *widget, char ascii,
                                Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { sendKeyEvent(action, widget, asciiToKey(ascii), ascii, modifier, delay); }
    inline static void keyEvent(KeyAction action, QWidget *widget, Qt::Key key,
                                Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { sendKeyEvent(action, widget, key, keyToAscii(key), modifier, delay); }

    inline static void keyClicks(QWidget *widget, const QString &sequence,
                                 Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    {
        for (int i=0; i < sequence.length(); i++)
            keyEvent(Click, widget, sequence.at(i).toLatin1(), modifier, delay);
    }

    inline static void keyPress(QWidget *widget, char key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { keyEvent(Press, widget, key, modifier, delay); }
    inline static void keyRelease(QWidget *widget, char key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { keyEvent(Release, widget, key, modifier, delay); }
    inline static void keyClick(QWidget *widget, char key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { keyEvent(Click, widget, key, modifier, delay); }
    inline static void keyPress(QWidget *widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { keyEvent(Press, widget, key, modifier, delay); }
    inline static void keyRelease(QWidget *widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { keyEvent(Release, widget, key, modifier, delay); }
    inline static void keyClick(QWidget *widget, Qt::Key key, Qt::KeyboardModifiers modifier = Qt::NoModifier, int delay=-1)
    { keyEvent(Click, widget, key, modifier, delay); }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTESTKEYBOARD_H
