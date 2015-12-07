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

#ifndef WIDGETBOX_H
#define WIDGETBOX_H

#include "widgetbox_global.h"
#include <qdesigner_widgetbox_p.h>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class WidgetBoxTreeView;
class WidgetCollectionModel;
class Scratchpad;

class QT_WIDGETBOX_EXPORT WidgetBox : public QDesignerWidgetBox
{
    Q_OBJECT
public:
    explicit WidgetBox(QDesignerFormEditorInterface *core, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~WidgetBox();

    QDesignerFormEditorInterface *core() const;

    virtual int categoryCount() const;
    virtual Category category(int cat_idx) const;
    virtual void addCategory(const Category &cat);
    virtual void removeCategory(int cat_idx);

    virtual int widgetCount(int cat_idx) const;
    virtual Widget widget(int cat_idx, int wgt_idx) const;
    virtual void addWidget(int cat_idx, const Widget &wgt);
    virtual void removeWidget(int cat_idx, int wgt_idx);

    void dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint &global_mouse_pos);

    virtual void setFileName(const QString &file_name);
    virtual QString fileName() const;
    virtual bool load();
    virtual bool save();

    virtual bool loadContents(const QString &contents);

protected:
    virtual void dragEnterEvent (QDragEnterEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dropEvent (QDropEvent * event);

private slots:
    void handleMousePress(const QString &name, const QString &xml, bool custom, const QPoint &global_mouse_pos);

private:
    QDesignerFormEditorInterface *m_core;
    WidgetBoxTreeView *m_view;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // WIDGETBOX_H
