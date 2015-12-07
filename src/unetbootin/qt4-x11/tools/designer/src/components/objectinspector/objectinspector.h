/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef OBJECTINSPECTOR_H
#define OBJECTINSPECTOR_H

#include "objectinspector_global.h"
#include "qdesigner_objectinspector_p.h"

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

class QItemSelection;

namespace qdesigner_internal {

class QT_OBJECTINSPECTOR_EXPORT ObjectInspector: public QDesignerObjectInspector
{
    Q_OBJECT
public:
    explicit ObjectInspector(QDesignerFormEditorInterface *core, QWidget *parent = 0);
    virtual ~ObjectInspector();

    virtual QDesignerFormEditorInterface *core() const;

    virtual void getSelection(Selection &s) const;
    virtual bool selectObject(QObject *o);
    virtual void clearSelection();

    void setFormWindow(QDesignerFormWindowInterface *formWindow);

public slots:
    virtual void mainContainerChanged();

private slots:
    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void slotPopupContextMenu(const QPoint &pos);
    void slotHeaderDoubleClicked(int column);

protected:
    virtual void dragEnterEvent (QDragEnterEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dragLeaveEvent(QDragLeaveEvent * event);
    virtual void dropEvent (QDropEvent * event);

private:
    class ObjectInspectorPrivate;
    ObjectInspectorPrivate *m_impl;
};

}  // namespace qdesigner_internal

#endif // OBJECTINSPECTOR_H

QT_END_NAMESPACE
