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

#include "q3dataview.h"

#ifndef QT_NO_SQL_VIEW_WIDGETS

#include "private/q3sqlmanager_p.h"

QT_BEGIN_NAMESPACE

class Q3DataViewPrivate
{
public:
    Q3DataViewPrivate() {}
    Q3SqlFormManager frm;
};


/*!
    \class Q3DataView qdataview.h
    \brief The Q3DataView class provides read-only SQL forms.

    \compat

    This class provides a form which displays SQL field data from a
    record buffer. Because Q3DataView does not support editing it uses
    less resources than a Q3DataBrowser. This class is well suited for
    displaying read-only data from a SQL database.

    If you want a to present your data in an editable form use
    Q3DataBrowser; if you want a table-based presentation of your data
    use Q3DataTable.

    The form is associated with the data view with setForm() and the
    record is associated with setRecord(). You can also pass a
    QSqlRecord to the refresh() function which will set the record to
    the given record and read the record's fields into the form.
*/

/*!
    Constructs a data view which is a child of \a parent, called \a
    name, and with widget flags \a fl.
*/

Q3DataView::Q3DataView(QWidget *parent, const char *name, Qt::WindowFlags fl)
    : QWidget(parent, name, fl)
{
    d = new Q3DataViewPrivate();
}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3DataView::~Q3DataView()
{
    delete d;
}

/*!
    Clears the default form's values. If there is no default form,
    nothing happens. All the values are set to their 'zero state',
    e.g. 0 for numeric fields, "" for string fields.
*/

void Q3DataView::clearValues()
{
    d->frm.clearValues();
}

/*!
    Sets the form used by the data view to \a form. If a record has
    already been assigned to the data view, the form will display that
    record's data.

    \sa form()
*/

void Q3DataView::setForm(Q3SqlForm* form)
{
    d->frm.setForm(form);
}


/*!
    Returns the default form used by the data view, or 0 if there is
    none.

    \sa setForm()
*/

Q3SqlForm* Q3DataView::form()
{
    return d->frm.form();
}


/*!
    Sets the record used by the data view to \a record. If a form has
    already been assigned to the data view, the form will display the
    data from \a record in that form.

    \sa record()
*/

void Q3DataView::setRecord(QSqlRecord* record)
{
    d->frm.setRecord(record);
}


/*!
    Returns the default record used by the data view, or 0 if there is
    none.

    \sa setRecord()
*/

QSqlRecord* Q3DataView::record()
{
    return d->frm.record();
}


/*!
    Causes the default form to read its fields from the record buffer.
    If there is no default form, or no record, nothing happens.

    \sa setForm()
*/

void Q3DataView::readFields()
{
    d->frm.readFields();
}

/*!
    Causes the default form to write its fields to the record buffer.
    If there is no default form, or no record, nothing happens.

    \sa setForm()
*/

void Q3DataView::writeFields()
{
    d->frm.writeFields();
}

/*!
    Causes the default form to display the contents of \a buf. If
    there is no default form, nothing happens.The \a buf also becomes
    the default record for all subsequent calls to readFields() and
    writefields(). This slot is equivalant to calling:

    \snippet doc/src/snippets/code/src.qt3support.sql.q3dataview.cpp 0

    \sa setRecord() readFields()
*/

void Q3DataView::refresh(QSqlRecord* buf)
{
    if (buf && buf != record())
        setRecord(buf);
    readFields();
}

QT_END_NAMESPACE

#endif // QT_NO_SQL_VIEW_WIDGETS
