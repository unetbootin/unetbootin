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

#ifndef QTOOLBOX_H
#define QTOOLBOX_H

#include <QtGui/qframe.h>
#include <QtGui/qicon.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_TOOLBOX

class QToolBoxPrivate;

class Q_GUI_EXPORT QToolBox : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)

public:
    explicit QToolBox(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QToolBox();

    int addItem(QWidget *widget, const QString &text);
    int addItem(QWidget *widget, const QIcon &icon, const QString &text);
    int insertItem(int index, QWidget *widget, const QString &text);
    int insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text);

    void removeItem(int index);

    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    void setItemText(int index, const QString &text);
    QString itemText(int index) const;

    void setItemIcon(int index, const QIcon &icon);
    QIcon itemIcon(int index) const;

#ifndef QT_NO_TOOLTIP
    void setItemToolTip(int index, const QString &toolTip);
    QString itemToolTip(int index) const;
#endif

    int currentIndex() const;
    QWidget *currentWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);

Q_SIGNALS:
    void currentChanged(int index);

protected:
    bool event(QEvent *e);
    virtual void itemInserted(int index);
    virtual void itemRemoved(int index);
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *);

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QToolBox(QWidget *parent, const char *name, Qt::WindowFlags f = 0);
    inline QT3_SUPPORT void setItemLabel(int index, const QString &text) { setItemText(index, text); }
    inline QT3_SUPPORT QString itemLabel(int index) const { return itemText(index); }
    inline QT3_SUPPORT QWidget *currentItem() const { return widget(currentIndex()); }
    inline QT3_SUPPORT void setCurrentItem(QWidget *item) { setCurrentIndex(indexOf(item)); }
    inline QT3_SUPPORT void setItemIconSet(int index, const QIcon &icon) { setItemIcon(index, icon); }
    inline QT3_SUPPORT QIcon itemIconSet(int index) const { return itemIcon(index); }
    inline QT3_SUPPORT int removeItem(QWidget *item)
    { int i = indexOf(item); removeItem(i); return i; }
    inline QT3_SUPPORT QWidget *item(int index) const { return widget(index); }
    QT3_SUPPORT void setMargin(int margin) { setContentsMargins(margin, margin, margin, margin); }
    QT3_SUPPORT int margin() const
    { int margin; int dummy; getContentsMargins(&margin, &dummy, &dummy, &dummy);  return margin; }
#endif

private:
    Q_DECLARE_PRIVATE(QToolBox)
    Q_DISABLE_COPY(QToolBox)
    Q_PRIVATE_SLOT(d_func(), void _q_buttonClicked())
    Q_PRIVATE_SLOT(d_func(), void _q_widgetDestroyed(QObject*))
};


inline int QToolBox::addItem(QWidget *item, const QString &text)
{ return insertItem(-1, item, QIcon(), text); }
inline int QToolBox::addItem(QWidget *item, const QIcon &iconSet,
                              const QString &text)
{ return insertItem(-1, item, iconSet, text); }
inline int QToolBox::insertItem(int index, QWidget *item, const QString &text)
{ return insertItem(index, item, QIcon(), text); }

#endif // QT_NO_TOOLBOX

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTOOLBOX_H
