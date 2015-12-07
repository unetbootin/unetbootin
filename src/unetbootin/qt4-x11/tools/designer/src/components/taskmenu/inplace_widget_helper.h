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

#ifndef INPLACE_WIDGETHELPER_H
#define INPLACE_WIDGETHELPER_H


#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <qglobal.h>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {

    // A helper class to make an editor widget suitable for form inline
    // editing. Derive from the editor widget class and  make InPlaceWidgetHelper  a member.
    //
    // Sets "destructive close" on the editor widget and
    // wires "ESC" to it.
    // Installs an event filter on the parent to listen for
    // resize events and passes them on to the child.
    // You might want to connect editingFinished() to close() of the editor widget.
    class InPlaceWidgetHelper: public QObject
    {
        Q_OBJECT
    public:
        InPlaceWidgetHelper(QWidget *editorWidget, QWidget *parentWidget, QDesignerFormWindowInterface *fw);
        virtual ~InPlaceWidgetHelper();

        virtual bool eventFilter(QObject *object, QEvent *event);

        // returns a recommended alignment for the editor widget determined from the parent.
        Qt::Alignment alignment() const;
    private:
        QWidget *m_editorWidget;
        QWidget *m_parentWidget;
        const bool m_noChildEvent;
        QPoint m_posOffset;
        QSize m_sizeOffset;
    };

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // INPLACE_WIDGETHELPER_H
