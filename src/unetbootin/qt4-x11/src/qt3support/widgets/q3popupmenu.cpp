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
#include "q3popupmenu.h"

QT_BEGIN_NAMESPACE

/*!
    \fn void Q3PopupMenu::setFrameRect(QRect)
    \internal
*/

/*!
    \fn QRect Q3PopupMenu::frameRect() const
    \internal
*/
/*!
    \enum Q3PopupMenu::DummyFrame
    \internal

    \value Box
    \value Sunken
    \value Plain
    \value Raised
    \value MShadow
    \value NoFrame
    \value Panel 
    \value StyledPanel
    \value HLine 
    \value VLine 
    \value GroupBoxPanel
    \value WinPanel 
    \value ToolBarPanel 
    \value MenuBarPanel 
    \value PopupPanel 
    \value LineEditPanel 
    \value TabWidgetPanel 
    \value MShape
*/

/*!
    \fn void Q3PopupMenu::setFrameShadow(DummyFrame)
    \internal
*/

/*!
    \fn DummyFrame Q3PopupMenu::frameShadow() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setFrameShape(DummyFrame)
    \internal
*/

/*!
    \fn DummyFrame Q3PopupMenu::frameShape() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setFrameStyle(int)
    \internal
*/

/*!
    \fn int Q3PopupMenu::frameStyle() const
    \internal
*/

/*!
    \fn int Q3PopupMenu::frameWidth() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setLineWidth(int)
    \internal
*/

/*!
    \fn int Q3PopupMenu::lineWidth() const
    \internal
*/

/*!
    \fn void Q3PopupMenu::setMargin(int margin)
    \since 4.2

    Sets the width of the margin around the contents of the widget to \a margin.
    
    This function uses QWidget::setContentsMargins() to set the margin.
    \sa margin(), QWidget::setContentsMargins()
*/

/*!
    \fn int Q3PopupMenu::margin() const 
    \since 4.2

    Returns the with of the the margin around the contents of the widget.
    
    This function uses QWidget::getContentsMargins() to get the margin.
    \sa setMargin(), QWidget::getContentsMargins()
*/

/*!
    \fn void Q3PopupMenu::setMidLineWidth(int)
    \internal
*/

/*!
    \fn int Q3PopupMenu::midLineWidth() const
    \internal
*/

QT_END_NAMESPACE
