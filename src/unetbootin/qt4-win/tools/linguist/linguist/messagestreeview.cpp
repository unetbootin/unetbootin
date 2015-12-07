/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "messagestreeview.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QHeaderView>
#include <QtGui/QItemDelegate>

QT_BEGIN_NAMESPACE

class MessagesItemDelegate : public QItemDelegate
{
public:
    MessagesItemDelegate(QObject *parent) : QItemDelegate(parent) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        const QAbstractItemModel *model = index.model();
        Q_ASSERT(model);

        if (!model->parent(index).isValid()) {
            if (index.column() == 1) {
                QStyleOptionViewItem opt = option;
                opt.font.setBold(true);
                QItemDelegate::paint(painter, opt, index);
                return;
            }
        }
        QItemDelegate::paint(painter, option, index);
    }
};

MessagesTreeView::MessagesTreeView(QWidget *parent) : QTreeView(parent)
{
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setUniformRowHeights(true);
    setAlternatingRowColors(true);
    setAllColumnsShowFocus(true);

    setItemDelegate(new MessagesItemDelegate(this));
    header()->setMovable(false);
    setSortingEnabled(true);
}

void MessagesTreeView::setModel(QAbstractItemModel * model)
{
    QTreeView::setModel(model);
    QFontMetrics fm(font());
    header()->resizeSection(0, qMax(fm.width(tr("Done")), 64) );
    header()->setResizeMode(1, QHeaderView::Interactive);
    header()->setResizeMode(2, QHeaderView::Stretch);
    header()->setClickable(true);
    setColumnHidden(3, true);
}

QT_END_NAMESPACE
