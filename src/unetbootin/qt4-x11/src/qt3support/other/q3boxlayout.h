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

#ifndef Q3BOXLAYOUT_H
#define Q3BOXLAYOUT_H

#include <QtGui/qboxlayout.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3BoxLayout : public QBoxLayout
{
public:
    inline explicit Q3BoxLayout(Direction dir, QWidget *parent = 0)
        : QBoxLayout(dir, parent) { setMargin(0); setSpacing(0); }

    inline Q3BoxLayout(QWidget *parent, Direction dir, int margin = 0, int spacing = -1,
                       const char *name = 0)
        : QBoxLayout(parent, dir, margin, spacing, name) {}

    inline Q3BoxLayout(QLayout *parentLayout, Direction dir, int spacing = -1,
                       const char *name = 0)
        : QBoxLayout(parentLayout, dir, spacing, name) { setMargin(0); }

    inline Q3BoxLayout(Direction dir, int spacing, const char *name = 0)
        : QBoxLayout(dir, spacing, name) { setMargin(0); }

private:
    Q_DISABLE_COPY(Q3BoxLayout)
};

class Q3HBoxLayout : public Q3BoxLayout
{
public:
    inline Q3HBoxLayout() : Q3BoxLayout(LeftToRight) {}

    inline explicit Q3HBoxLayout(QWidget *parent) : Q3BoxLayout(LeftToRight, parent) {}

    inline Q3HBoxLayout(QWidget *parent, int margin,
                 int spacing = -1, const char *name = 0)
        : Q3BoxLayout(parent, LeftToRight, margin, spacing, name) {}

    inline Q3HBoxLayout(QLayout *parentLayout,
                 int spacing = -1, const char *name = 0)
        : Q3BoxLayout(parentLayout, LeftToRight, spacing, name) {}

    inline Q3HBoxLayout(int spacing, const char *name = 0)
        : Q3BoxLayout(LeftToRight, spacing, name) {}

private:
    Q_DISABLE_COPY(Q3HBoxLayout)
};

class Q3VBoxLayout : public Q3BoxLayout
{
public:
    inline Q3VBoxLayout() : Q3BoxLayout(TopToBottom) {}

    inline explicit Q3VBoxLayout(QWidget *parent) : Q3BoxLayout(TopToBottom, parent) {}

    inline Q3VBoxLayout(QWidget *parent, int margin,
                 int spacing = -1, const char *name = 0)
        : Q3BoxLayout(parent, TopToBottom, margin, spacing, name) {}

    inline Q3VBoxLayout(QLayout *parentLayout,
                 int spacing = -1, const char *name = 0)
        : Q3BoxLayout(parentLayout, TopToBottom, spacing, name) {}

    inline Q3VBoxLayout(int spacing, const char *name = 0)
        : Q3BoxLayout(TopToBottom, spacing, name) {}

private:
    Q_DISABLE_COPY(Q3VBoxLayout)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3BOXLAYOUT_H
