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

#ifndef QBOXLAYOUT_H
#define QBOXLAYOUT_H

#include <QtGui/qlayout.h>
#ifdef QT_INCLUDE_COMPAT
#include <QtGui/qwidget.h>
#endif

#include <limits.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QBoxLayoutPrivate;

class Q_GUI_EXPORT QBoxLayout : public QLayout
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QBoxLayout)
public:
    enum Direction { LeftToRight, RightToLeft, TopToBottom, BottomToTop,
                     Down = TopToBottom, Up = BottomToTop };

    explicit QBoxLayout(Direction, QWidget *parent = 0);

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QBoxLayout(QWidget *parent, Direction, int border = 0, int spacing = -1,
                const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR  QBoxLayout(QLayout *parentLayout, Direction, int spacing = -1,
                const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR  QBoxLayout(Direction, int spacing, const char *name = 0);
#endif
    ~QBoxLayout();

    Direction direction() const;
    void setDirection(Direction);

    void addSpacing(int size);
    void addStretch(int stretch = 0);
    void addSpacerItem(QSpacerItem *spacerItem);
    void addWidget(QWidget *, int stretch = 0, Qt::Alignment alignment = 0);
    void addLayout(QLayout *layout, int stretch = 0);
    void addStrut(int);
    void addItem(QLayoutItem *);

    void insertSpacing(int index, int size);
    void insertStretch(int index, int stretch = 0);
    void insertSpacerItem(int index, QSpacerItem *spacerItem);
    void insertWidget(int index, QWidget *widget, int stretch = 0, Qt::Alignment alignment = 0);
    void insertLayout(int index, QLayout *layout, int stretch = 0);

    int spacing() const;
    void setSpacing(int spacing);

    bool setStretchFactor(QWidget *w, int stretch);
    bool setStretchFactor(QLayout *l, int stretch);

    QSize sizeHint() const;
    QSize minimumSize() const;
    QSize maximumSize() const;

    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int minimumHeightForWidth(int) const;

    Qt::Orientations expandingDirections() const;
    void invalidate();
    QLayoutItem *itemAt(int) const;
    QLayoutItem *takeAt(int);
    int count() const;
    void setGeometry(const QRect&);
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT int findWidget(QWidget* w) {return indexOf(w);}
#endif
protected:
    // ### Qt 5: make public
    void insertItem(int index, QLayoutItem *);

private:
    Q_DISABLE_COPY(QBoxLayout)
};

class Q_GUI_EXPORT QHBoxLayout : public QBoxLayout
{
    Q_OBJECT
public:
    QHBoxLayout();
    explicit QHBoxLayout(QWidget *parent);
    ~QHBoxLayout();

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QHBoxLayout(QWidget *parent, int border,
                 int spacing = -1, const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR QHBoxLayout(QLayout *parentLayout,
                 int spacing = -1, const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR QHBoxLayout(int spacing, const char *name = 0);
#endif

private:
    Q_DISABLE_COPY(QHBoxLayout)
};

class Q_GUI_EXPORT QVBoxLayout : public QBoxLayout
{
    Q_OBJECT
public:
    QVBoxLayout();
    explicit QVBoxLayout(QWidget *parent);
    ~QVBoxLayout();

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QVBoxLayout(QWidget *parent, int border,
                 int spacing = -1, const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR QVBoxLayout(QLayout *parentLayout,
                 int spacing = -1, const char *name = 0);
    QT3_SUPPORT_CONSTRUCTOR QVBoxLayout(int spacing, const char *name = 0);
#endif

private:
    Q_DISABLE_COPY(QVBoxLayout)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QBOXLAYOUT_H
