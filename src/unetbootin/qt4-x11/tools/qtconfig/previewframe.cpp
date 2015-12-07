/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "previewframe.h"

#include <QBoxLayout>
#include <QPainter>
#include <QMdiSubWindow>

QT_BEGIN_NAMESPACE

PreviewFrame::PreviewFrame( QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    setMinimumSize(200, 200);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(1);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    previewWidget = new PreviewWidget;
    workspace = new Workspace(this);
    vbox->addWidget(workspace);
    previewWidget->setAutoFillBackground(true);
}

void PreviewFrame::setPreviewPalette(QPalette pal)
{
    previewWidget->setPalette(pal);
}

QString PreviewFrame::previewText() const
{
    return m_previewWindowText;
}

void PreviewFrame::setPreviewVisible(bool visible)
{
    previewWidget->parentWidget()->setVisible(visible);
    if (visible)
        m_previewWindowText = QLatin1String("The moose in the noose\nate the goose who was loose.");
    else
        m_previewWindowText = tr("Desktop settings will only take effect after an application restart.");
    workspace->viewport()->update();
}

Workspace::Workspace(PreviewFrame* parent, const char* name)
    : QMdiArea(parent)
{
    previewFrame = parent;
    PreviewWidget *previewWidget = previewFrame->widget();
    setObjectName(QLatin1String(name));
    QMdiSubWindow *frame = addSubWindow(previewWidget, Qt::Window);
    frame->move(10,10);
    frame->show();
}

void Workspace::paintEvent( QPaintEvent* )
{
    QPainter p (viewport());
    p.fillRect(rect(), palette().color(backgroundRole()).dark());
    p.setPen( QPen( Qt::white ) );
    p.drawText ( 0, height() / 2,  width(), height(), Qt::AlignHCenter, previewFrame->previewText());
}

QT_END_NAMESPACE
