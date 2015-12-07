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

#ifndef QCHECKBOX_H
#define QCHECKBOX_H

#include <QtGui/qabstractbutton.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QCheckBoxPrivate;
class QStyleOptionButton;

class Q_GUI_EXPORT QCheckBox : public QAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(bool tristate READ isTristate WRITE setTristate)

public:
    explicit QCheckBox(QWidget *parent=0);
    explicit QCheckBox(const QString &text, QWidget *parent=0);


    QSize sizeHint() const;

    void setTristate(bool y = true);
    bool isTristate() const;

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

Q_SIGNALS:
    void stateChanged(int);

protected:
    bool event(QEvent *e);
    bool hitButton(const QPoint &pos) const;
    void checkStateSet();
    void nextCheckState();
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void initStyleOption(QStyleOptionButton *option) const;

#ifdef QT3_SUPPORT
public:
    enum ToggleState {
        Off =      Qt::Unchecked,
        NoChange = Qt::PartiallyChecked,
        On =       Qt::Checked
    };
    inline QT3_SUPPORT ToggleState state() const
        { return static_cast<QCheckBox::ToggleState>(static_cast<int>(checkState())); }
    inline QT3_SUPPORT void setState(ToggleState state)
        { setCheckState(static_cast<Qt::CheckState>(static_cast<int>(state))); }
    inline QT3_SUPPORT void setNoChange()
        { setCheckState(Qt::PartiallyChecked); }
    QT3_SUPPORT_CONSTRUCTOR QCheckBox(QWidget *parent, const char* name);
    QT3_SUPPORT_CONSTRUCTOR QCheckBox(const QString &text, QWidget *parent, const char* name);
#endif

private:
    Q_DECLARE_PRIVATE(QCheckBox)
    Q_DISABLE_COPY(QCheckBox)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCHECKBOX_H
