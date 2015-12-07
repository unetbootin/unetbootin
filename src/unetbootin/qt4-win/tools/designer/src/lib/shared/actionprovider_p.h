/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

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

#include <QtDesigner/extension.h>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtGui/QApplication>

QT_BEGIN_NAMESPACE

class QAction;

class QDesignerActionProviderExtension
{
public:
    virtual ~QDesignerActionProviderExtension() {}

    virtual QRect actionGeometry(QAction *action) const = 0;
    virtual QAction *actionAt(const QPoint &pos) const = 0;

    virtual void adjustIndicator(const QPoint &pos) = 0;
};

// Find action at the given position for a widget that has actionGeometry() (QToolBar,
// QMenuBar, QMenu). They usually have actionAt(), but that fails since Designer usually sets
// WA_TransparentForMouseEvents on the widgets.
template <class Widget>
    int actionIndexAt(const Widget *w, const QPoint &pos, Qt::Orientation orientation)
{
    const QList<QAction*> actions = w->actions();
    const int actionCount = actions.count();
    if (actionCount == 0)
        return -1;
    // actionGeometry() can be wrong sometimes; it returns a geometry that
    // stretches to the end of the toolbar/menu bar. So, check from the beginning
    // in the case of a horizontal right-to-left orientation.
    const bool checkTopRight = orientation == Qt::Horizontal && QApplication::layoutDirection() == Qt::RightToLeft;
    const QPoint topRight = QPoint(w->rect().width(), 0);
    for (int index = 0; index < actionCount; ++index) {
        QRect g = w->actionGeometry(actions.at(index));
        if (checkTopRight)
            g.setTopRight(topRight);
        else
            g.setTopLeft(QPoint(0, 0));

        if (g.contains(pos))
            return index;
    }
    return -1;
}

Q_DECLARE_EXTENSION_INTERFACE(QDesignerActionProviderExtension, "com.trolltech.Qt.Designer.ActionProvider")

QT_END_NAMESPACE

#endif // ACTIONPROVIDER_H
