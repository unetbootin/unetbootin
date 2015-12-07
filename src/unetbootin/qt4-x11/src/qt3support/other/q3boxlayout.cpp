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

/*!
    \class Q3HBoxLayout q3boxlayout.h
    \compat

    \brief The Q3HBoxLayout class lines up widgets horizontally.
    \sa Q3VBoxLayout
*/

/*!
    \fn Q3HBoxLayout::Q3HBoxLayout(QWidget *parent, int margin = 0, int spacing = -1, const char *name = 0)

    Constructs a new top-level horizontal box called \a name, with parent
    \a parent. The \a margin is the number of pixels between the edge of the
    widget and its managed children. The \a spacing is the default number of
    pixels between neighboring children. If \a spacing is -1 the value of margin
    is used for spacing.
*/

/*!
    \fn Q3HBoxLayout::Q3HBoxLayout(QLayout *parentLayout, int spacing = -1, const char *name = 0)

    Constructs a new horizontal box called \a name and adds it to
    \a parentLayout. The \a spacing is the default number of pixels between
    neighboring children. If \a spacing is -1, this Q3HBoxLayout will inherit
    its parent's spacing.
*/

/*!
    \fn Q3HBoxLayout::Q3HBoxLayout(int spacing = -1, const char *name = 0)

    Constructs a new horizontal box called \a name. You must add it to another
    layout. The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, this QHBoxLayout will inherit its parent's
    spacing(). 
*/

/*!
    \fn Q3HBoxLayout::Q3HBoxLayout()
    \internal
*/

/*!
    \fn Q3HBoxLayout::Q3HBoxLayout(QWidget *parent)
    \internal
*/

/*!
    \class Q3VBoxLayout q3boxlayout.h
    \compat

    \brief The Q3VBoxLayout class lines up widgets vertically.
    \sa Q3HBoxLayout
*/

/*!
    \fn Q3VBoxLayout::Q3VBoxLayout(QWidget *parent, int margin = 0, int spacing = -1, const char *name = 0)

    Constructs a new top-level vertical box called \a name, with parent
    \a parent. The \a margin is the number of pixels between the edge of the
    widget and its managed children. The \a spacing is the default number of
    pixels between neighboring children. If \a spacing is -1 the value of
    margin is used for spacing.
*/

/*!
    \fn Q3VBoxLayout::Q3VBoxLayout(QLayout *parentLayout, int spacing = -1, const char *name = 0)

    Constructs a new vertical box called \a name and adds it to
    \a parentLayout. The \a spacing is the default number of pixels between
    neighboring children. If \a spacing is -1, this Q3VBoxLayout will inherit
    its parent's spacing().
*/

/*!
    \fn Q3VBoxLayout::Q3VBoxLayout(int spacing = -1, const char *name = 0)

    Constructs a new vertical box called \a  name. You must add it to another
    layout. The \a spacing is the default number of pixels between neighboring
    children. If \a spacing is -1, this Q3VBoxLayout will inherit its parent's
    spacing().
*/

/*!
    \fn Q3VBoxLayout::Q3VBoxLayout()
    \internal
*/

/*!
    \fn Q3VBoxLayout::Q3VBoxLayout(QWidget *parent)
    \internal
*/
