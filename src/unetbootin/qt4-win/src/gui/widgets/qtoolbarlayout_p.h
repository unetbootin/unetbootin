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

#ifndef QTOOLBARLAYOUT_P_H
#define QTOOLBARLAYOUT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/qlayout.h>
#include <private/qlayoutengine_p.h>
#include <QVector>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TOOLBAR

class QAction;
class QToolBarExtension;
class QMenu;

class Q_GUI_EXPORT QToolBarItem : public QWidgetItem
{
public:
    QToolBarItem(QWidget *widget);
    bool isEmpty() const;

    QAction *action;
    bool customWidget;
};

class Q_GUI_EXPORT QToolBarLayout : public QLayout
{
    Q_OBJECT

public:
    QToolBarLayout(QWidget *parent = 0);
    ~QToolBarLayout();

    void addItem(QLayoutItem *item);
    QLayoutItem *itemAt(int index) const;
    QLayoutItem *takeAt(int index);
    int count() const;

    bool isEmpty() const;
    void invalidate();
    Qt::Orientations expandingDirections() const;

    void setGeometry(const QRect &r);
    QSize minimumSize() const;
    QSize sizeHint() const;

    void insertAction(int index, QAction *action);
    int indexOf(QAction *action) const;
    int indexOf(QWidget *widget) const { return QLayout::indexOf(widget); }

    QRect handleRect() const;

    bool layoutActions(const QSize &size);
    QSize expandedSize(const QSize &size) const;
    bool expanded, animating;

    void setUsePopupMenu(bool set); // Yeah, there's no getter, but it's internal.
    void checkUsePopupMenu();

    bool movable() const;
    void updateMarginAndSpacing();
    bool hasExpandFlag() const;

public slots:
    void setExpanded(bool b);

private:
    QList<QToolBarItem*> items;
    QSize hint, minSize;
    bool dirty, expanding, empty, expandFlag;
    QVector<QLayoutStruct> geomArray;
    QRect handRect;
    QToolBarExtension *extension;

    void updateGeomArray() const;
    QToolBarItem *createItem(QAction *action);
    QMenu *popupMenu;
};

#endif // QT_NO_TOOLBAR

QT_END_NAMESPACE

#endif // QTOOLBARLAYOUT_P_H
