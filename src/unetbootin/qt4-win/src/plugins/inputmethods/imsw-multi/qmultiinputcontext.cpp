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

/****************************************************************************
**
** Implementation of QMultiInputContext class
**
** Copyright (C) 2004 immodule for Qt Project.  All rights reserved.
**
** This file is written to contribute to Trolltech ASA under their own
** licence. You may use this file under your Qt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef QT_NO_IM
#include "qmultiinputcontext.h"
#include <qinputcontextfactory.h>
#include <qstringlist.h>
#include <qaction.h>
#include <qsettings.h>
#include <qmenu.h>

#include <stdlib.h>

QT_BEGIN_NAMESPACE

QMultiInputContext::QMultiInputContext()
    : QInputContext(), current(-1)
{
    QStringList keys = QInputContextFactory::keys();
    for (int i = keys.size()-1; i >= 0; --i)
        if (keys.at(i).contains(QLatin1String("imsw")))
            keys.removeAt(i);

    QString def = QLatin1String(getenv("QT_IM_MODULE"));
    if (def.isEmpty()) {
	QSettings settings;
        def = settings.value(QLatin1String("/qt/DefaultInputMethod"), QLatin1String("xim")).toString();
    }
    current = keys.indexOf(def);
    if (current < 0)
        current = 0;

    menu = new QMenu(tr("Select IM"));
    separator = new QAction(this);
    separator->setSeparator(true);

    QActionGroup *group = new QActionGroup(this);
    for (int i = 0; i < keys.size(); ++i) {
        slaves.append(QInputContextFactory::create(keys.at(i), this));
        QAction *a = menu->addAction(slaves.at(i)->identifierName());
        a->setCheckable(true);
        group->addAction(a);
        if (i == current)
            a->setChecked(true);
    }
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(changeSlave(QAction*)));


}

QMultiInputContext::~QMultiInputContext()
{
}


QString QMultiInputContext::identifierName()
{
    return (slave()) ? slave()->identifierName() : QLatin1String("");
}

QString QMultiInputContext::language()
{
    return (slave()) ? slave()->language() : QLatin1String("");
}


#if defined(Q_WS_X11)
bool QMultiInputContext::x11FilterEvent(QWidget *keywidget, XEvent *event)
{
    return (slave()) ? slave()->x11FilterEvent(keywidget, event) : false;
}
#endif // Q_WS_X11


bool QMultiInputContext::filterEvent(const QEvent *event)
{
    return (slave()) ? slave()->filterEvent(event) : false;
}

void QMultiInputContext::reset()
{
    if (slave())
	slave()->reset();
}

void QMultiInputContext::update()
{
    if (slave())
	slave()->update();
}

void QMultiInputContext::mouseHandler(int x, QMouseEvent *event)
{
    if (slave())
	slave()->mouseHandler(x, event);
}

QFont QMultiInputContext::font() const
{
    return (slave()) ? slave()->font() : QInputContext::font();
}

void QMultiInputContext::setFocusWidget(QWidget *w)
{
    QInputContext::setFocusWidget(w);
    if (slave())
	slave()->setFocusWidget(w);
}

QWidget *QMultiInputContext::focusWidget() const
{
    return QInputContext::focusWidget();
}

void QMultiInputContext::widgetDestroyed(QWidget *w)
{
    if (slave())
	slave()->widgetDestroyed(w);
}

bool QMultiInputContext::isComposing() const
{
    return (slave()) ? slave()->isComposing() : false;
}

QList<QAction *> QMultiInputContext::actions()
{
    QList<QAction *> a = slave()->actions();
    a.append(separator);
    a.append(menu->menuAction());
    return a;
}

void QMultiInputContext::changeSlave(QAction *a)
{
    for (int i = 0; i < slaves.size(); ++i) {
        if (slaves.at(i)->identifierName() == a->text()) {
            current = i;
            return;
        }
    }
}

QT_END_NAMESPACE

#endif // QT_NO_IM
