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

#ifndef QPUSHBUTTON_H
#define QPUSHBUTTON_H

#include <QtGui/qabstractbutton.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPushButtonPrivate;
class QMenu;
class QStyleOptionButton;

class Q_GUI_EXPORT QPushButton : public QAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(bool autoDefault READ autoDefault WRITE setAutoDefault)
    Q_PROPERTY(bool default READ isDefault WRITE setDefault)
    Q_PROPERTY(bool flat READ isFlat WRITE setFlat)

public:
    explicit QPushButton(QWidget *parent=0);
    explicit QPushButton(const QString &text, QWidget *parent=0);
    QPushButton(const QIcon& icon, const QString &text, QWidget *parent=0);
    ~QPushButton();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    bool autoDefault() const;
    void setAutoDefault(bool);
    bool isDefault() const;
    void setDefault(bool);

#ifndef QT_NO_MENU
    void setMenu(QMenu* menu);
    QMenu* menu() const;
#endif

    void setFlat(bool);
    bool isFlat() const;

public Q_SLOTS:
#ifndef QT_NO_MENU
    void showMenu();
#endif

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void initStyleOption(QStyleOptionButton *option) const;
    QPushButton(QPushButtonPrivate &dd, QWidget* parent = 0);

public:
#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QPushButton(QWidget *parent, const char* name);
    QT3_SUPPORT_CONSTRUCTOR QPushButton(const QString &text, QWidget *parent, const char* name);
    QT3_SUPPORT_CONSTRUCTOR QPushButton(const QIcon& icon, const QString &text, QWidget *parent, const char* name);
    inline QT3_SUPPORT void openPopup()  { showMenu(); }
    inline QT3_SUPPORT bool isMenuButton() const { return menu() !=  0; }
    inline QT3_SUPPORT void setPopup(QMenu* popup) {setMenu(popup); }
    inline QT3_SUPPORT QMenu* popup() const { return menu(); }
#endif

private:
    Q_DISABLE_COPY(QPushButton)
    Q_DECLARE_PRIVATE(QPushButton)
#ifndef QT_NO_MENU        
    Q_PRIVATE_SLOT(d_func(), void _q_popupPressed())
#endif
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPUSHBUTTON_H
