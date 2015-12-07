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

#ifndef QLAYOUTITEM_H
#define QLAYOUTITEM_H

#include <QtGui/qsizepolicy.h>
#include <QtCore/qrect.h>

#include <limits.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

static const int QLAYOUTSIZE_MAX = INT_MAX/256/16;

class QLayout;
class QLayoutItem;
class QSpacerItem;
class QWidget;
class QSize;

class Q_GUI_EXPORT QLayoutItem
{
public:
    inline explicit QLayoutItem(Qt::Alignment alignment = 0);
    virtual ~QLayoutItem();
    virtual QSize sizeHint() const = 0;
    virtual QSize minimumSize() const = 0;
    virtual QSize maximumSize() const = 0;
    virtual Qt::Orientations expandingDirections() const = 0;
    virtual void setGeometry(const QRect&) = 0;
    virtual QRect geometry() const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool hasHeightForWidth() const;
    virtual int heightForWidth(int) const;
    virtual int minimumHeightForWidth(int) const;
    virtual void invalidate();

    virtual QWidget *widget();
    virtual QLayout *layout();
    virtual QSpacerItem *spacerItem();

    Qt::Alignment alignment() const { return align; }
    void setAlignment(Qt::Alignment a);
    QSizePolicy::ControlTypes controlTypes() const;

protected:
    Qt::Alignment align;
};

inline QLayoutItem::QLayoutItem(Qt::Alignment aalignment)
    : align(aalignment) { }

class Q_GUI_EXPORT QSpacerItem : public QLayoutItem
{
public:
    QSpacerItem(int w, int h,
                 QSizePolicy::Policy hData = QSizePolicy::Minimum,
                 QSizePolicy::Policy vData = QSizePolicy::Minimum)
        : width(w), height(h), sizeP(hData, vData) { }
    void changeSize(int w, int h,
                     QSizePolicy::Policy hData = QSizePolicy::Minimum,
                     QSizePolicy::Policy vData = QSizePolicy::Minimum);
    QSize sizeHint() const;
    QSize minimumSize() const;
    QSize maximumSize() const;
    Qt::Orientations expandingDirections() const;
    bool isEmpty() const;
    void setGeometry(const QRect&);
    QRect geometry() const;
    QSpacerItem *spacerItem();

private:
    int width;
    int height;
    QSizePolicy sizeP;
    QRect rect;
};

class Q_GUI_EXPORT QWidgetItem : public QLayoutItem
{
    Q_DISABLE_COPY(QWidgetItem)

public:
    explicit QWidgetItem(QWidget *w) : wid(w) { }
    QSize sizeHint() const;
    QSize minimumSize() const;
    QSize maximumSize() const;
    Qt::Orientations expandingDirections() const;
    bool isEmpty() const;
    void setGeometry(const QRect&);
    QRect geometry() const;
    virtual QWidget *widget();

    bool hasHeightForWidth() const;
    int heightForWidth(int) const;

protected:
    QWidget *wid;
};

class Q_GUI_EXPORT QWidgetItemV2 : public QWidgetItem
{
public:
    explicit QWidgetItemV2(QWidget *widget);
    ~QWidgetItemV2();

    QSize sizeHint() const;
    QSize minimumSize() const;
    QSize maximumSize() const;
    int heightForWidth(int width) const;

private:
    enum { Dirty = -123, HfwCacheMaxSize = 3 };

    inline bool useSizeCache() const;
    void updateCacheIfNecessary() const;
    inline void invalidateSizeCache() {
        q_cachedMinimumSize.setWidth(Dirty);
        q_hfwCacheSize = 0;
    }

    mutable QSize q_cachedMinimumSize;
    mutable QSize q_cachedSizeHint;
    mutable QSize q_cachedMaximumSize;
    mutable QSize q_cachedHfws[HfwCacheMaxSize];
    mutable short q_firstCachedHfw;
    mutable short q_hfwCacheSize;
    void *d;

    friend class QWidgetPrivate;

    Q_DISABLE_COPY(QWidgetItemV2)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLAYOUTITEM_H
