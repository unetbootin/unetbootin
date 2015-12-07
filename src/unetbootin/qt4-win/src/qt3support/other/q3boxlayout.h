/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
