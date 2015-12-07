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

#include "q3cleanuphandler.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qspinbox.h"
#include "qcombobox.h"

#include "q3editorfactory.h"
#include "qdatetimeedit.h"

#ifndef QT_NO_SQL_EDIT_WIDGETS

QT_BEGIN_NAMESPACE

/*!
    \class Q3EditorFactory qeditorfactory.h
    \brief The Q3EditorFactory class is used to create editor widgets
    for QVariant data types.

    \compat

    Each editor factory provides the createEditor() function which
    given a QVariant will create and return a QWidget that can edit
    that QVariant. For example if you have a QVariant::String type, a
    QLineEdit would be the default editor returned, whereas a
    QVariant::Int's default editor would be a QSpinBox.

    If you want to create different editors for fields with the same
    data type, subclass Q3EditorFactory and reimplement the
    createEditor() function.
*/

/*!
    Constructs an editor factory with parent \a parent.
*/

Q3EditorFactory::Q3EditorFactory (QObject * parent)
    : QObject(parent)
{

}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3EditorFactory::~Q3EditorFactory()
{

}

static Q3EditorFactory * defaultfactory = 0;
static Q3CleanupHandler< Q3EditorFactory > q_cleanup_editor_factory;

/*!
    Returns an instance of a default editor factory.
*/

Q3EditorFactory * Q3EditorFactory::defaultFactory()
{
    if(defaultfactory == 0){
        defaultfactory = new Q3EditorFactory();
        q_cleanup_editor_factory.add(&defaultfactory);
    }

    return defaultfactory;
}

/*!
    Replaces the default editor factory with \a factory.
    \e{Q3EditorFactory takes ownership of factory, and destroys it
    when it is no longer needed.}
*/

void Q3EditorFactory::installDefaultFactory(Q3EditorFactory * factory)
{
    if(factory == 0 || factory == defaultfactory) return;

    if(defaultfactory != 0){
        q_cleanup_editor_factory.remove(&defaultfactory);
        delete defaultfactory;
    }
    defaultfactory = factory;
    q_cleanup_editor_factory.add(&defaultfactory);
}

/*!
    Creates and returns the appropriate editor for the QVariant \a v.
    If the QVariant is invalid, 0 is returned. The \a parent is passed
    to the appropriate editor's constructor.
*/

QWidget * Q3EditorFactory::createEditor(QWidget * parent, const QVariant & v)
{
    QWidget * w = 0;
    switch(v.type()){
        case QVariant::Invalid:
            w = 0;
            break;
        case QVariant::Bool:
            w = new QComboBox(parent, "qt_editor_bool");
            ((QComboBox *) w)->insertItem(QLatin1String("False"));
            ((QComboBox *) w)->insertItem(QLatin1String("True"));
            break;
        case QVariant::UInt:
            w = new QSpinBox(0, 999999, 1, parent, "qt_editor_spinbox");
            break;
        case QVariant::Int:
            w = new QSpinBox(-999999, 999999, 1, parent, "qt_editor_int");
            break;
        case QVariant::String:
        case QVariant::Double:
            w = new QLineEdit(parent, "qt_editor_double");
            ((QLineEdit*)w)->setFrame(false);
            break;
        case QVariant::Date: {
            QDateTimeEdit *edit = new QDateTimeEdit(parent);
            edit->setDisplayFormat(QLatin1String("yyyy/MM/dd"));
            edit->setObjectName(QLatin1String("qt_editor_date"));
            w = edit; }
            break;
        case QVariant::Time: {
            QDateTimeEdit *edit = new QDateTimeEdit(parent);
            edit->setDisplayFormat(QLatin1String("hh:mm"));
            edit->setObjectName(QLatin1String("qt_editor_time"));
            w = edit; }
            break;
        case QVariant::DateTime:
            w = new QDateTimeEdit(parent);
            w->setObjectName(QLatin1String("qt_editor_datetime"));
            break;
#ifndef QT_NO_LABEL
        case QVariant::Pixmap:
            w = new QLabel(parent, QLatin1String("qt_editor_pixmap"));
            break;
#endif
        case QVariant::Palette:
        case QVariant::Color:
        case QVariant::Font:
        case QVariant::Brush:
        case QVariant::Bitmap:
        case QVariant::Cursor:
        case QVariant::Map:
        case QVariant::StringList:
        case QVariant::Rect:
        case QVariant::Size:
        case QVariant::IconSet:
        case QVariant::Point:
        case QVariant::PointArray:
        case QVariant::Region:
        case QVariant::SizePolicy:
        case QVariant::ByteArray:
        default:
            w = new QWidget(parent, "qt_editor_default");
            break;
    }
    return w;
}

QT_END_NAMESPACE

#endif // QT_NO_SQL
