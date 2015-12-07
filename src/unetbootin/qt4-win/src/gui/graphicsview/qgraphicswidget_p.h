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

#ifndef QGRAPHICSWIDGET_P_H
#define QGRAPHICSWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include <private/qobject_p.h>
#include "qgraphicsitem_p.h"
#include "qgraphicswidget.h"
#include <QtGui/qfont.h>
#include <QtGui/qpalette.h>
#include <QtGui/qsizepolicy.h>
#include <QtGui/qstyle.h>

QT_BEGIN_NAMESPACE

class QGraphicsLayout;
class QStyleOptionTitleBar;

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class Q_AUTOTEST_EXPORT QGraphicsWidgetPrivate : public QGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsWidget)
public:
    QGraphicsWidgetPrivate()
        : leftMargin(0),
          topMargin(0),
          rightMargin(0),
          bottomMargin(0),
          leftLayoutItemMargin(0),
          topLayoutItemMargin(0),
          rightLayoutItemMargin(0),
          bottomLayoutItemMargin(0),
          layout(0),
          focusPolicy(Qt::NoFocus),
          focusNext(0),
          focusPrev(0),
          focusChild(0),
          windowFlags(0),
          hoveredSubControl(QStyle::SC_None),
          grabbedSection(Qt::NoSection),
          buttonMouseOver(false),
          buttonSunken(false),
          setWindowFrameMargins(false),
          leftWindowFrameMargin(0),
          topWindowFrameMargin(0),
          rightWindowFrameMargin(0),
          bottomWindowFrameMargin(0)
    { }

    void init(QGraphicsItem *parentItem, Qt::WindowFlags wFlags);
    qreal titleBarHeight(const QStyleOptionTitleBar &options) const;

    // Margins
    qreal leftMargin;
    qreal topMargin;
    qreal rightMargin;
    qreal bottomMargin;
    QRectF contentsRect;

    // Layout item margins
    void getLayoutItemMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const;
    void setLayoutItemMargins(qreal left, qreal top, qreal right, qreal bottom);
    void setLayoutItemMargins(QStyle::SubElement element, const QStyleOption *opt = 0);

    void fixFocusChainBeforeReparenting(QGraphicsWidget *newParent);
    void setLayout_helper(QGraphicsLayout *l);

    qreal leftLayoutItemMargin;
    qreal topLayoutItemMargin;
    qreal rightLayoutItemMargin;
    qreal bottomLayoutItemMargin;

    // Layouts
    QGraphicsLayout *layout;
    void setLayoutDirection_helper(Qt::LayoutDirection direction);
    void resolveLayoutDirection();

    // Style
    QPalette palette;
    void resolvePalette();
    QFont font;
    void resolveFont();

    // Window specific
    void initStyleOptionTitleBar(QStyleOptionTitleBar *option);
    void adjustWindowFlags(Qt::WindowFlags *wFlags);
    void windowFrameMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void windowFrameMousePressEvent(QGraphicsSceneMouseEvent *event);
    void windowFrameMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void windowFrameHoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void windowFrameHoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    bool hasDecoration() const;

    // State
    inline int attributeToBitIndex(Qt::WidgetAttribute att) const
    {
        int bit = -1;
        switch (att) {
        case Qt::WA_SetLayoutDirection: bit = 0; break;
        case Qt::WA_RightToLeft: bit = 1; break;
        case Qt::WA_SetStyle: bit = 2; break;
        case Qt::WA_Resized: bit = 3; break;
        case Qt::WA_DeleteOnClose: bit = 4; break;
        case Qt::WA_NoSystemBackground: bit = 5; break;
        case Qt::WA_OpaquePaintEvent: bit = 6; break;
        default: break;
        }
        return bit;
    }
    inline void setAttribute(Qt::WidgetAttribute att, bool value)
    {
        int bit = attributeToBitIndex(att);
        if (bit == -1) {
            qWarning("QGraphicsWidget::setAttribute: unsupported attribute %d", int(att));
            return;
        }
        if (value)
            attributes |= (1 << bit);
        else
            attributes &= ~(1 << bit);
    }
    inline bool testAttribute(Qt::WidgetAttribute att) const
    {
        int bit = attributeToBitIndex(att);
        if (bit == -1)
            return false;
        return (attributes & (1 << bit)) != 0;
    }
    quint32 attributes : 7;

    // Focus
    Qt::FocusPolicy focusPolicy;
    QGraphicsWidget *focusNext;
    QGraphicsWidget *focusPrev;
    QGraphicsWidget *focusChild;

    // Windows
    Qt::WindowFlags windowFlags;
    QString windowTitle;
    QStyle::SubControl hoveredSubControl;
    Qt::WindowFrameSection grabbedSection;
    uint buttonMouseOver : 1;
    uint buttonSunken : 1;
    QPointF mouseDelta; // to compensate for small error when interactively resizing
    QRectF startGeometry;

    bool setWindowFrameMargins;
    qreal leftWindowFrameMargin;
    qreal topWindowFrameMargin;
    qreal rightWindowFrameMargin;
    qreal bottomWindowFrameMargin;
};

#endif //!defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

QT_END_NAMESPACE

#endif //QGRAPHICSWIDGET_P_H

