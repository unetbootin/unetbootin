/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

#include "dragwidget.h"

DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    dragDropLabel = new QLabel("", this);
    dragDropLabel->setAlignment(Qt::AlignHCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addStretch(0);
    layout->addWidget(dragDropLabel);
    layout->addStretch(0);

    setAcceptDrops(true);
}

// Accept all actions, but deal with them separately later.
//! [0]
void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
//! [0]

//! [1]
void DragWidget::dropEvent(QDropEvent *event)
{
    if (event->source() == this && event->possibleActions() & Qt::MoveAction)
        return;
//! [1]

//! [2]
    if (event->proposedAction() == Qt::MoveAction) {
        event->acceptProposedAction();
        // Process the data from the event.
//! [2]
        emit dragResult(tr("The data was moved here."));
//! [3]
    } else if (event->proposedAction() == Qt::CopyAction) {
        event->acceptProposedAction();
        // Process the data from the event.
//! [3]
        emit dragResult(tr("The data was copied here."));
//! [4]
    } else {
        // Ignore the drop.
        return;
    }
//! [4]
    // End of quote

    emit mimeTypes(event->mimeData()->formats());
    setData(event->mimeData()->formats()[0],
            event->mimeData()->data(event->mimeData()->formats()[0]));
//! [5]
}
//! [5]

//! [6]
void DragWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();
}
//! [6]

//! [7]
void DragWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setData(mimeType, data);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
//! [7]

    switch (dropAction) {
        case Qt::CopyAction:
            emit dragResult(tr("The text was copied."));
            break;
        case Qt::MoveAction:
            emit dragResult(tr("The text was moved."));
            break;
        default:
            emit dragResult(tr("Unknown action."));
            break;
    }
//! [8]
}
//! [8]

void DragWidget::setData(const QString &mimetype, const QByteArray &newData)
{
    mimeType = mimetype;
    data = QByteArray(newData);

    dragDropLabel->setText(tr("%1 bytes").arg(data.size()));

    QStringList formats;
    formats << mimetype;
    emit mimeTypes(formats);
}
