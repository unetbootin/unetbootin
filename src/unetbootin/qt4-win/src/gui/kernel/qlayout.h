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

#ifndef QLAYOUT_H
#define QLAYOUT_H

#include <QtCore/qobject.h>
#include <QtGui/qlayoutitem.h>
#include <QtGui/qsizepolicy.h>
#include <QtCore/qrect.h>

#include <limits.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QLayout;
class QSize;

#ifdef QT3_SUPPORT
class Q_GUI_EXPORT QLayoutIterator
{
public:
    inline QT3_SUPPORT_CONSTRUCTOR QLayoutIterator(QLayout *i) : layout(i), index(0) {}
    inline QLayoutIterator(const QLayoutIterator &i)
        : layout(i.layout), index(i.index) {}
    inline QLayoutIterator &operator=(const QLayoutIterator &i) {
        layout = i.layout;
        index = i.index;
        return *this;
    }
    inline QT3_SUPPORT QLayoutItem *operator++();
    inline QT3_SUPPORT QLayoutItem *current();
    inline QT3_SUPPORT QLayoutItem *takeCurrent();
    inline QT3_SUPPORT void deleteCurrent();

private:
    // hack to avoid deprecated warning
    friend class QLayout;
    inline QLayoutIterator(QLayout *i, bool) : layout(i), index(0) {}
    QLayout *layout;
    int index;
};
#endif

class QLayoutPrivate;

class Q_GUI_EXPORT QLayout : public QObject, public QLayoutItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QLayout)

    Q_ENUMS(SizeConstraint)
    Q_PROPERTY(int margin READ margin WRITE setMargin)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(SizeConstraint sizeConstraint READ sizeConstraint WRITE setSizeConstraint)
public:
    enum SizeConstraint {
        SetDefaultConstraint,
        SetNoConstraint,
        SetMinimumSize,
        SetFixedSize,
        SetMaximumSize,
        SetMinAndMaxSize
#if defined(QT3_SUPPORT) && !defined(Q_MOC_RUN)
        , Auto = SetDefaultConstraint,
        FreeResize = SetNoConstraint,
        Minimum = SetMinimumSize,
        Fixed = SetFixedSize
#endif
    };

    QLayout(QWidget *parent);
    QLayout();
    ~QLayout();

    int margin() const;
    int spacing() const;

    void setMargin(int);
    void setSpacing(int);

    void setContentsMargins(int left, int top, int right, int bottom);
    void getContentsMargins(int *left, int *top, int *right, int *bottom) const;
    QRect contentsRect() const;

    bool setAlignment(QWidget *w, Qt::Alignment alignment);
    bool setAlignment(QLayout *l, Qt::Alignment alignment);
#ifdef Q_NO_USING_KEYWORD
    inline void setAlignment(Qt::Alignment alignment) { QLayoutItem::setAlignment(alignment); }
#else
    using QLayoutItem::setAlignment;
#endif

    void setSizeConstraint(SizeConstraint);
    SizeConstraint sizeConstraint() const;
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT void setResizeMode(SizeConstraint s) {setSizeConstraint(s);}
    inline QT3_SUPPORT SizeConstraint resizeMode() const {return sizeConstraint();}
#endif
    void setMenuBar(QWidget *w);
    QWidget *menuBar() const;

    QWidget *parentWidget() const;

    void invalidate();
    QRect geometry() const;
    bool activate();
    void update();

    void addWidget(QWidget *w);
    virtual void addItem(QLayoutItem *) = 0;

    void removeWidget(QWidget *w);
    void removeItem(QLayoutItem *);

    Qt::Orientations expandingDirections() const;
    QSize minimumSize() const;
    QSize maximumSize() const;
    virtual void setGeometry(const QRect&);
    virtual QLayoutItem *itemAt(int index) const = 0;
    virtual QLayoutItem *takeAt(int index) = 0;
    virtual int indexOf(QWidget *) const;
    virtual int count() const = 0;
    bool isEmpty() const;

    int totalHeightForWidth(int w) const;
    QSize totalMinimumSize() const;
    QSize totalMaximumSize() const;
    QSize totalSizeHint() const;
    QLayout *layout();

    void setEnabled(bool);
    bool isEnabled() const;

#ifdef QT3_SUPPORT
    QT3_SUPPORT void freeze(int w=0, int h=0);
    QT3_SUPPORT bool isTopLevel() const;
#endif

    static QSize closestAcceptableSize(const QWidget *w, const QSize &s);

protected:
    void widgetEvent(QEvent *);
    void childEvent(QChildEvent *e);
    void addChildLayout(QLayout *l);
    void addChildWidget(QWidget *w);
#ifdef QT3_SUPPORT
    QT3_SUPPORT void deleteAllItems();
#endif

    QRect alignmentRect(const QRect&) const;
protected:
    QLayout(QLayoutPrivate &d, QLayout*, QWidget*);

private:
    Q_DISABLE_COPY(QLayout)

    static void activateRecursiveHelper(QLayoutItem *item);

    friend class QApplicationPrivate;
    friend class QWidget;

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QLayout(QWidget *parent, int margin, int spacing = -1,
                             const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR QLayout(QLayout *parentLayout, int spacing = -1, const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR QLayout(int spacing, const char *name = 0);
    inline QT3_SUPPORT QWidget *mainWidget() const { return parentWidget(); }
    inline QT3_SUPPORT void remove(QWidget *w) { removeWidget(w); }
    inline QT3_SUPPORT void add(QWidget *w) { addWidget(w); }

    QT3_SUPPORT void setAutoAdd(bool a);
    QT3_SUPPORT bool autoAdd() const;
    inline QT3_SUPPORT QLayoutIterator iterator() { return QLayoutIterator(this,true); }

    inline QT3_SUPPORT int defaultBorder() const { return spacing(); }
#endif
};

#ifdef QT3_SUPPORT
inline QLayoutItem *QLayoutIterator::operator++() { return layout->itemAt(++index); }
inline QLayoutItem *QLayoutIterator::current() { return layout->itemAt(index); }
inline QLayoutItem *QLayoutIterator::takeCurrent() { return layout->takeAt(index); }
inline void QLayoutIterator::deleteCurrent() { delete  layout->takeAt(index); }
#endif

//### support old includes
#if 1 //def QT3_SUPPORT
QT_BEGIN_INCLUDE_NAMESPACE
#include <QtGui/qboxlayout.h>
#include <QtGui/qgridlayout.h>
QT_END_INCLUDE_NAMESPACE
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLAYOUT_H
