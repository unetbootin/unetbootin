/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QINPUTDIALOG_H
#define QINPUTDIALOG_H

#include <QtGui/qdialog.h>
#include <QtCore/qstring.h>
#include <QtGui/qlineedit.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_INPUTDIALOG

class QInputDialogPrivate;

class Q_GUI_EXPORT QInputDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QInputDialog)

private:
    // ### Qt 5: remove
    enum Type { LineEdit, SpinBox, DoubleSpinBox, ComboBox, EditableComboBox };

    // ### Qt 5: remove
    QInputDialog(const QString &label, QWidget* parent, Type type, Qt::WindowFlags f);

    QInputDialog(
        const QString &title, const QString &label, QWidget *parent, QWidget *input,
        Qt::WindowFlags f);
    ~QInputDialog();

public:
    static QString getText(QWidget *parent, const QString &title, const QString &label,
                           QLineEdit::EchoMode echo = QLineEdit::Normal,
                           const QString &text = QString(), bool *ok = 0, Qt::WindowFlags f = 0);
    static int getInteger(QWidget *parent, const QString &title, const QString &label, int value = 0,
                          int minValue = -2147483647, int maxValue = 2147483647,
                          int step = 1, bool *ok = 0, Qt::WindowFlags f = 0);
    static double getDouble(QWidget *parent, const QString &title, const QString &label, double value = 0,
                            double minValue = -2147483647, double maxValue = 2147483647,
                            int decimals = 1, bool *ok = 0, Qt::WindowFlags f = 0);
    static QString getItem(QWidget *parent, const QString &title, const QString &label, const QStringList &list,
                           int current = 0, bool editable = true, bool *ok = 0,Qt::WindowFlags f = 0);

#ifdef QT3_SUPPORT
    inline static QT3_SUPPORT QString getText(const QString &title, const QString &label,
                                              QLineEdit::EchoMode echo = QLineEdit::Normal,
                                              const QString &text = QString(), bool *ok = 0,
                                              QWidget *parent = 0, const char * = 0, Qt::WindowFlags f = 0)
        { return getText(parent, title, label, echo, text, ok, f); }
    inline static QT3_SUPPORT int getInteger(const QString &title, const QString &label, int value = 0,
                                             int minValue = -2147483647, int maxValue = 2147483647,
                                             int step = 1, bool *ok = 0,
                                             QWidget *parent = 0, const char * = 0, Qt::WindowFlags f = 0)
        { return getInteger(parent, title, label, value, minValue, maxValue, step, ok, f); }
    inline static QT3_SUPPORT double getDouble(const QString &title, const QString &label, double value = 0,
                                               double minValue = -2147483647, double maxValue = 2147483647,
                                               int decimals = 1, bool *ok = 0,
                                               QWidget *parent = 0, const char * = 0, Qt::WindowFlags f = 0)
        { return getDouble(parent, title, label, value, minValue, maxValue, decimals, ok, f); }
    inline static QT3_SUPPORT QString getItem(const QString &title, const QString &label, const QStringList &list,
                                              int current = 0, bool editable = true, bool *ok = 0,
                                              QWidget *parent = 0, const char * = 0, Qt::WindowFlags f = 0)
        { return getItem(parent, title, label, list, current, editable, ok, f); }
#endif

private:
    Q_DISABLE_COPY(QInputDialog)
};

#endif // QT_NO_INPUTDIALOG

QT_END_NAMESPACE

QT_END_HEADER

#endif // QINPUTDIALOG_H
