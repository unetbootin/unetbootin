/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "q3simplewidgets.h"

#include <q3groupbox.h>
#include <qlabel.h>

QT_BEGIN_NAMESPACE

QString Q_GUI_EXPORT qt_accStripAmp(const QString &text);

Q3AccessibleDisplay::Q3AccessibleDisplay(QWidget *w, Role role)
: QAccessibleWidget(w, role)
{
}

/*! \reimp */
QAccessible::Role Q3AccessibleDisplay::role(int child) const
{
    QLabel *l = qobject_cast<QLabel*>(object());
    if (l) {
        if (l->pixmap() || l->picture())
            return Graphic;
        if (l->picture())
            return Graphic;
        if (l->movie())
            return Animation;
    }
    return QAccessibleWidget::role(child);
}

/*! \reimp */
QString Q3AccessibleDisplay::text(Text t, int child) const
{
    QString str;
    switch (t) {
    case Name:
        if (qobject_cast<QLabel*>(object())) {
            str = qobject_cast<QLabel*>(object())->text();
        } else if (qobject_cast<Q3GroupBox*>(object())) {
            str = qobject_cast<Q3GroupBox*>(object())->title();
        }
        break;
    default:
        break;
    }
    if (str.isEmpty())
        str = QAccessibleWidget::text(t, child);;
    return qt_accStripAmp(str);
}

/*! \reimp */
QAccessible::Relation Q3AccessibleDisplay::relationTo(int child, const QAccessibleInterface *other,
                                                      int otherChild) const
{
    Relation relation = QAccessibleWidget::relationTo(child, other, otherChild);
    if (child || otherChild)
        return relation;

    QObject *o = other->object();
    QLabel *label = qobject_cast<QLabel*>(object());
    Q3GroupBox *groupbox = qobject_cast<Q3GroupBox*>(object());
    if (label) {
        if (o == label->buddy())
            relation |= Label;
    } else if (groupbox && !groupbox->title().isEmpty()) {
        if (groupbox->children().contains(o))
            relation |= Label;
    }
    return relation;
}

/*! \reimp */
int Q3AccessibleDisplay::navigate(RelationFlag rel, int entry, QAccessibleInterface **target) const
{
    *target = 0;
    if (rel == Labelled) {
        QObject *targetObject = 0;
        QLabel *label = qobject_cast<QLabel*>(object());
        Q3GroupBox *groupbox = qobject_cast<Q3GroupBox*>(object());
        if (label) {
            if (entry == 1)
                targetObject = label->buddy();
        } else if (groupbox && !groupbox->title().isEmpty()) {
            rel = Child;
        }
        *target = QAccessible::queryAccessibleInterface(targetObject);
        if (*target)
            return 0;
    }
    return QAccessibleWidget::navigate(rel, entry, target);
}

QT_END_NAMESPACE
