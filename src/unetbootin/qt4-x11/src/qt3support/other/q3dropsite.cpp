/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3dropsite.h"

#ifndef QT_NO_DRAGANDDROP

#include "qwidget.h"

QT_BEGIN_NAMESPACE

/*!
  \class Q3DropSite qdropsite.h
  \brief The Q3DropSite class provides nothing and does nothing.

  \compat

  It was used in Qt 1.x to do some drag and drop; that has since been
  folded into QWidget.

  \sa Q3DragObject, Q3TextDrag, Q3ImageDrag
*/

/*!
  Constructs a Q3DropSite to handle events for the widget \a self.

  Pass \c this as the \a self parameter.
  This enables dropping by calling QWidget::setAcceptDrops(true).
*/
Q3DropSite::Q3DropSite(QWidget* self)
{
    self->setAcceptDrops(true);
}

/*!
  Destroys the drop site.
*/
Q3DropSite::~Q3DropSite()
{
}

QT_END_NAMESPACE

#endif // QT_NO_DRAGANDDROP
