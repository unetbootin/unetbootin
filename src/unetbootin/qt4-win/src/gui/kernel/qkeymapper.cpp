/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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


#include "qapplication.h"

#include <private/qobject_p.h>
#include "qkeymapper_p.h"
#include <qwidget.h>

QT_BEGIN_NAMESPACE

/*!
    \class QKeyMapper QKeyMapper.h
    \since 4.2
    \ingroup application
    \internal

    \sa QObject
*/

/*!
    Constructs a new key mapper.
*/
QKeyMapper::QKeyMapper()
    : QObject(*new QKeyMapperPrivate, 0)
{
}

/*!
    Destroys the key mapper.
*/
QKeyMapper::~QKeyMapper()
{
}

QList<int> QKeyMapper::possibleKeys(QKeyEvent *e)
{
    QList<int> result;

    if (!e->nativeScanCode()) {
        if (e->key() && (e->key() != Qt::Key_unknown))
            result << int(e->key() + e->modifiers());
        else if (!e->text().isEmpty())
            result << int(e->text().at(0).unicode() + e->modifiers());
        return result;
    }

    return instance()->d_func()->possibleKeys(e);
}

extern bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event); // in qapplication_*.cpp
void QKeyMapper::changeKeyboard()
{
    instance()->d_func()->clearMappings();

    // inform all toplevel widgets of the change
    QEvent e(QEvent::KeyboardLayoutChange);
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i = 0; i < list.size(); ++i) {
        QWidget *w = list.at(i);
        qt_sendSpontaneousEvent(w, &e);
    }
}

Q_GLOBAL_STATIC(QKeyMapper, keymapper)
/*!
    Returns the pointer to the single instance of QKeyMapper in the application.
    If none yet exists, the function ensures that one is created.
*/
QKeyMapper *QKeyMapper::instance()
{
    return keymapper();
}

QKeyMapperPrivate *qt_keymapper_private()
{
    return QKeyMapper::instance()->d_func();
}

Q_GUI_EXPORT QList<int> qt_keymapper_possibleKeys(QKeyEvent *e) { return QKeyMapper::instance()->possibleKeys(e); }

QT_END_NAMESPACE
