/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3POPUPMENU_H
#define Q3POPUPMENU_H

#include <QtGui/qmenu.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3PopupMenu : public QMenu
{
    Q_OBJECT
public:
    inline Q3PopupMenu(QWidget *parent = 0, const char * =0) : QMenu(parent)
    { }

    inline int exec() { return findIdForAction(QMenu::exec()); }
    inline int exec(const QPoint & pos, int indexAtPoint = 0) {
        return findIdForAction(QMenu::exec(pos, actions().value(indexAtPoint)));
    }

    void setFrameRect(QRect) {}
    QRect frameRect() const { return QRect(); }
    enum DummyFrame { Box, Sunken, Plain, Raised, MShadow, NoFrame, Panel, StyledPanel, 
                      HLine, VLine, GroupBoxPanel, WinPanel, ToolBarPanel, MenuBarPanel, 
                      PopupPanel, LineEditPanel, TabWidgetPanel, MShape };
    void setFrameShadow(DummyFrame) {}
    DummyFrame frameShadow() const { return Plain; }
    void setFrameShape(DummyFrame) {}
    DummyFrame frameShape() const { return NoFrame; }
    void setFrameStyle(int) {}
    int frameStyle() const  { return 0; }
    int frameWidth() const { return 0; }
    void setLineWidth(int) {}
    int lineWidth() const { return 0; }    
    void setMargin(int margin) { setContentsMargins(margin, margin, margin, margin); }
    int margin() const 
    { int margin; int dummy; getContentsMargins(&margin, &dummy, &dummy, &dummy);  return margin; }    
    void setMidLineWidth(int) {}
    int midLineWidth() const { return 0; }

private:
    Q_DISABLE_COPY(Q3PopupMenu)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPOPUPMENU_H
