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

#include "window.h"

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QLabel *textLabel = new QLabel(tr("Data:"), this);
    textBrowser = new QTextBrowser(this);

    QLabel *mimeTypeLabel = new QLabel(tr("MIME types:"), this);
    mimeTypeCombo = new QComboBox(this);

    dropFrame = new QFrame(this);
    dropFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    QLabel *dropLabel = new QLabel(tr("Drop items here"), dropFrame);
    dropLabel->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *dropFrameLayout = new QVBoxLayout(dropFrame);
    dropFrameLayout->addWidget(dropLabel);

    QHBoxLayout *dropLayout = new QHBoxLayout;
    dropLayout->addStretch(0);
    dropLayout->addWidget(dropFrame);
    dropLayout->addStretch(0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(textLabel);
    mainLayout->addWidget(textBrowser);
    mainLayout->addWidget(mimeTypeLabel);
    mainLayout->addWidget(mimeTypeCombo);
    mainLayout->addSpacing(32);
    mainLayout->addLayout(dropLayout);

    setAcceptDrops(true);
    setWindowTitle(tr("Drop Rectangle"));
}

//! [0]
void Window::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain")
        && event->answerRect().intersects(dropFrame->geometry()))

        event->acceptProposedAction();
}
//! [0]

void Window::dropEvent(QDropEvent *event)
{
    textBrowser->setPlainText(event->mimeData()->text());
    mimeTypeCombo->clear();
    mimeTypeCombo->addItems(event->mimeData()->formats());

    event->acceptProposedAction();
}
