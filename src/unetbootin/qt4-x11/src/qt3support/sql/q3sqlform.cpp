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

#include "q3sqlform.h"

#ifndef QT_NO_SQL_FORM

#include "qsqlfield.h"
#include "q3sqlpropertymap.h"
#include "qsqlrecord.h"
#include "qstringlist.h"
#include "qwidget.h"
#include "qhash.h"

QT_BEGIN_NAMESPACE

class Q3SqlFormPrivate
{
public:
    Q3SqlFormPrivate() : propertyMap(0), buf(0), dirty(false) {}
    ~Q3SqlFormPrivate() { if (propertyMap) delete propertyMap; }
    QStringList fld;
    QHash <QString, QWidget*> wgt;
    QMap <QWidget*, QSqlField *> map;
    Q3SqlPropertyMap * propertyMap;
    QSqlRecord* buf;
    bool dirty;
};

/*!
    \class Q3SqlForm
    \brief The Q3SqlForm class creates and manages data entry forms
    tied to SQL databases.

    \compat

    Typical use of a Q3SqlForm consists of the following steps:
    \list
    \i Create the widgets you want to appear in the form.
    \i Create a cursor and navigate to the record to be edited.
    \i Create the Q3SqlForm.
    \i Set the form's record buffer to the cursor's update buffer.
    \i Insert each widget and the field it is to edit into the form.
    \i Use readFields() to update the editor widgets with values from
    the database's fields.
    \i Display the form and let the user edit values etc.
    \i Use writeFields() to update the database's field values with
    the values in the editor widgets.
    \endlist

    Note that a Q3SqlForm does not access the database directly, but
    most often via QSqlFields which are part of a Q3SqlCursor. A
    Q3SqlCursor::insert(), Q3SqlCursor::update() or Q3SqlCursor::del()
    call is needed to actually write values to the database.

    Some sample code to initialize a form successfully:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlform.cpp 0

    If you want to use custom editors for displaying and editing data
    fields, you must install a custom Q3SqlPropertyMap. The form
    uses this object to get or set the value of a widget.

    \sa installPropertyMap(), Q3SqlPropertyMap
*/


/*!
    Constructs a Q3SqlForm with parent \a parent.
*/
Q3SqlForm::Q3SqlForm(QObject * parent)
    : QObject(parent)
{
    d = new Q3SqlFormPrivate();
}

/*!
    Destroys the object and frees any allocated resources.
*/
Q3SqlForm::~Q3SqlForm()
{
    delete d;
}

/*!
    Installs a custom Q3SqlPropertyMap. This is useful if you plan to
    create your own custom editor widgets.

    Q3SqlForm takes ownership of \a pmap, so \a pmap is deleted when
    Q3SqlForm goes out of scope.

    \sa Q3DataTable::installEditorFactory()
*/
void Q3SqlForm::installPropertyMap(Q3SqlPropertyMap * pmap)
{
    if(d->propertyMap)
        delete d->propertyMap;
    d->propertyMap = pmap;
}

/*!
    Sets \a buf as the record buffer for the form. To force the
    display of the data from \a buf, use readFields().

    \sa readFields() writeFields()
*/

void Q3SqlForm::setRecord(QSqlRecord* buf)
{
    d->dirty = true;
    d->buf = buf;
}

/*!
    Inserts a \a widget, and the name of the \a field it is to be
    mapped to, into the form. To actually associate inserted widgets
    with an edit buffer, use setRecord().

    \sa setRecord()
*/

void Q3SqlForm::insert(QWidget * widget, const QString& field)
{
    d->dirty = true;
    d->wgt.insert(field, widget);
    d->fld += field;
}

/*!
    \overload

    Removes \a field from the form.
*/

void Q3SqlForm::remove(const QString& field)
{
    d->dirty = true;
    int i = d->fld.indexOf(field);
    if (i >= 0)
        d->fld.removeAt(i);
    d->wgt.remove(field);
}

/*!
    \overload

    Inserts a \a widget, and the \a field it is to be mapped to, into
    the form.
*/

void Q3SqlForm::insert(QWidget * widget, QSqlField * field)
{
    d->map[widget] = field;
}

/*!
    Removes a \a widget, and hence the field it's mapped to, from the
    form.
*/

void Q3SqlForm::remove(QWidget * widget)
{
    d->map.remove(widget);
}

/*!
    Clears the values in all the widgets, and the fields they are
    mapped to, in the form, and sets them to NULL.
*/
void Q3SqlForm::clearValues()
{
    QMap< QWidget *, QSqlField * >::Iterator it;
    for(it = d->map.begin(); it != d->map.end(); ++it){
        QSqlField* f = (*it);
        if (f)
            f->clear();
    }
    readFields();
}

/*!
    Removes every widget, and the fields they're mapped to, from the form.
*/
void Q3SqlForm::clear()
{
    d->dirty = true;
    d->fld.clear();
    clearMap();
}

/*!
    Returns the number of widgets in the form.
*/
int Q3SqlForm::count() const
{
    return d->map.size();
}

/*!
    Returns the \a{i}-th widget in the form. Useful for traversing
    the widgets in the form.
*/
QWidget * Q3SqlForm::widget(int i) const
{
    QMap< QWidget *, QSqlField * >::ConstIterator it;
    int cnt = 0;

    if(i > d->map.size())
        return 0;
    for(it = d->map.constBegin(); it != d->map.constEnd(); ++it){
        if(cnt++ == i)
            return it.key();
    }
    return 0;
}

/*!
    Returns the widget that field \a field is mapped to.
*/
QWidget * Q3SqlForm::fieldToWidget(QSqlField * field) const
{
    QMap< QWidget *, QSqlField * >::ConstIterator it;
    for(it = d->map.constBegin(); it != d->map.constEnd(); ++it){
        if(*it == field)
            return it.key();
    }
    return 0;
}

/*!
    Returns the SQL field that widget \a widget is mapped to.
*/
QSqlField * Q3SqlForm::widgetToField(QWidget * widget) const
{
    return d->map.value(widget, 0);
}

/*!
    Updates the widgets in the form with current values from the SQL
    fields they are mapped to.
*/
void Q3SqlForm::readFields()
{
    sync();
    QSqlField * f;
    QMap< QWidget *, QSqlField * >::Iterator it;
    Q3SqlPropertyMap * pmap = (d->propertyMap == 0) ?
                             Q3SqlPropertyMap::defaultMap() : d->propertyMap;
    for(it = d->map.begin() ; it != d->map.end(); ++it){
        f = widgetToField(it.key());
        if(!f)
            continue;
        pmap->setProperty(it.key(), f->value());
    }
}

/*!
    Updates the SQL fields with values from the widgets they are
    mapped to. To actually update the database with the contents of
    the record buffer, use Q3SqlCursor::insert(), Q3SqlCursor::update()
    or Q3SqlCursor::del() as appropriate.
*/
void Q3SqlForm::writeFields()
{
    sync();
    QSqlField * f;
    QMap< QWidget *, QSqlField * >::Iterator it;
    Q3SqlPropertyMap * pmap = (d->propertyMap == 0) ?
                             Q3SqlPropertyMap::defaultMap() : d->propertyMap;

    for(it = d->map.begin() ; it != d->map.end(); ++it){
        f = widgetToField(it.key());
        if(!f)
            continue;
        f->setValue(pmap->property(it.key()));
    }
}

/*!
    Updates the widget \a widget with the value from the SQL field it
    is mapped to. Nothing happens if no SQL field is mapped to the \a
    widget.
*/
void Q3SqlForm::readField(QWidget * widget)
{
    sync();
    QSqlField * field = 0;
    Q3SqlPropertyMap * pmap = (d->propertyMap == 0) ?
                             Q3SqlPropertyMap::defaultMap() : d->propertyMap;
    field = widgetToField(widget);
    if(field)
        pmap->setProperty(widget, field->value());
}

/*!
    Updates the SQL field with the value from the \a widget it is
    mapped to. Nothing happens if no SQL field is mapped to the \a
    widget.
*/
void Q3SqlForm::writeField(QWidget * widget)
{
    sync();
    QSqlField * field = 0;
    Q3SqlPropertyMap * pmap = (d->propertyMap == 0) ?
                             Q3SqlPropertyMap::defaultMap() : d->propertyMap;
    field = widgetToField(widget);
    if(field)
        field->setValue(pmap->property(widget));
}

/*! \internal
*/

void Q3SqlForm::sync()
{
    if (d->dirty) {
        clearMap();
        if (d->buf) {
            for (int i = 0; i < d->fld.count(); ++i) {
                const QSqlField *field = d->buf->fieldPtr(d->fld.at(i));
                insert(d->wgt.value(d->fld.at(i)), const_cast<QSqlField *>(field));
            }
        }
    }
    d->dirty = false;
}

/*! \internal

  Clears the internal map of widget/field associations
*/

void Q3SqlForm::clearMap()
{
    d->map.clear();
}

QT_END_NAMESPACE

#endif // QT_NO_SQL
