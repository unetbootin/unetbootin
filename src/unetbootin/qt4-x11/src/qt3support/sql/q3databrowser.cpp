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

#include "q3databrowser.h"

#ifndef QT_NO_SQL_VIEW_WIDGETS

#include "q3sqlform.h"
#include "private/q3sqlmanager_p.h"
#include "qsqlresult.h"

QT_BEGIN_NAMESPACE

class Q3DataBrowserPrivate
{
public:
    Q3DataBrowserPrivate() : boundaryCheck(true), readOnly(false) {}
    Q3SqlCursorManager cur;
    Q3SqlFormManager frm;
    Q3DataManager dat;
    bool boundaryCheck;
    bool readOnly;
};

/*!
    \class Q3DataBrowser qdatabrowser.h
    \brief The Q3DataBrowser class provides data manipulation and
    navigation for data entry forms.

    \compat

    A high-level API is provided for navigating through data records
    in a cursor, for inserting, updating and deleting records, and for
    refreshing data in the display.

    If you want a read-only form to present database data use
    Q3DataView; if you want a table-based presentation of your data use
    Q3DataTable.

    A Q3DataBrowser is used to associate a dataset with a form in much
    the same way as a Q3DataTable associates a dataset with a table.
    Once the data browser has been constructed it can be associated
    with a dataset with setSqlCursor(), and with a form with
    setForm(). Boundary checking, sorting and filtering can be set
    with setBoundaryChecking(), setSort() and setFilter(),
    respectively.

    The insertCurrent() function reads the fields from the default
    form into the default cursor and performs the insert. The
    updateCurrent() and deleteCurrent() functions perform similarly to
    update and delete the current record respectively.

    The user can be asked to confirm all edits with setConfirmEdits().
    For more precise control use setConfirmInsert(),
    setConfirmUpdate(), setConfirmDelete() and setConfirmCancels().
    Use setAutoEdit() to control the behavior of the form when the
    user edits a record and then navigates.

    The record set is navigated using first(), next(), prev(), last()
    and seek(). The form's display is updated with refresh(). When
    navigation takes place the firstRecordAvailable(),
    lastRecordAvailable(), nextRecordAvailable() and
    prevRecordAvailable() signals are emitted. When the cursor record
    is changed due to navigation the cursorChanged() signal is
    emitted.

    If you want finer control of the insert, update and delete
    processes then you can use the lower level functions to perform
    these operations as described below.

    The form is populated with data from the database with
    readFields(). If the user is allowed to edit, (see setReadOnly()),
    write the form's data back to the cursor's edit buffer with
    writeFields(). You can clear the values in the form with
    clearValues(). Editing is performed as follows:
    \list
    \i \e insert When the data browser enters insertion mode it emits the
    primeInsert() signal which you can connect to, for example to
    pre-populate fields. Call writeFields() to write the user's edits to
    the cursor's edit buffer then call insert() to insert the record
    into the database. The beforeInsert() signal is emitted just before
    the cursor's edit buffer is inserted into the database; connect to
    this for example, to populate fields such as an auto-generated
    primary key.
    \i \e update For updates the primeUpdate() signal is emitted when
    the data browser enters update mode. After calling writeFields()
    call update() to update the record and connect to the beforeUpdate()
    signal to manipulate the user's data before the update takes place.
    \i \e delete For deletion the primeDelete() signal is emitted when
    the data browser enters deletion mode. After calling writeFields()
    call del() to delete the record and connect to the beforeDelete()
    signal, for example to record an audit of the deleted record.
    \endlist

*/

/*!
    \enum Q3DataBrowser::Boundary

    This enum describes where the data browser is positioned.

    \value Unknown  the boundary cannot be determined (usually because
    there is no default cursor, or the default cursor is not active).

    \value None  the browser is not positioned on a boundary, but it is
    positioned on a record somewhere in the middle.

    \value BeforeBeginning  the browser is positioned before the
    first available record.

    \value Beginning  the browser is positioned at the first record.

    \value End  the browser is positioned at the last
    record.

    \value AfterEnd  the browser is positioned after the last
    available record.
*/

/*!
    Constructs a data browser which is a child of \a parent, with the
    name \a name and widget flags set to \a fl.
*/

Q3DataBrowser::Q3DataBrowser(QWidget *parent, const char *name, Qt::WindowFlags fl)
    : QWidget(parent, name, fl)
{
    d = new Q3DataBrowserPrivate();
    d->dat.setMode(QSql::Update);
}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3DataBrowser::~Q3DataBrowser()
{
    delete d;
}


/*!
    Returns an enum indicating the boundary status of the browser.

    This is achieved by moving the default cursor and checking the
    position, however the current default form values will not be
    altered. After checking for the boundary, the cursor is moved back
    to its former position. See \l Q3DataBrowser::Boundary.

    \sa Boundary
*/

Q3DataBrowser::Boundary Q3DataBrowser::boundary()
{
    Q3SqlCursor* cur = d->cur.cursor();
    if (!cur || !cur->isActive())
        return Unknown;
    if (!cur->isValid()) {
        if (cur->at() == QSql::BeforeFirst)
            return BeforeBeginning;
        if (cur->at() == QSql::AfterLast)
            return AfterEnd;
        return Unknown;
    }
    if (cur->at() == 0)
        return Beginning;
    int currentAt = cur->at();

    Boundary b = None;
    if (!cur->previous())
        b = Beginning;
    else
        cur->seek(currentAt);
    if (b == None && !cur->next())
        b = End;
    cur->seek(currentAt);
    return b;
}


/*!
    \property Q3DataBrowser::boundaryChecking
    \brief whether boundary checking is active

    When boundary checking is active (the default), signals are
    emitted indicating the current position of the default cursor.

    \sa boundary()
*/

void Q3DataBrowser::setBoundaryChecking(bool active)
{
    d->boundaryCheck = active;
}

bool Q3DataBrowser::boundaryChecking() const
{
    return d->boundaryCheck;
}

/*!
    \property Q3DataBrowser::sort
    \brief the data browser's sort

    The data browser's sort affects the order in which records are
    viewed in the browser. Call refresh() to apply the new sort.

    When retrieving the sort property, a string list is returned in
    the form 'fieldname order', e.g. 'id ASC', 'surname DESC'.

    There is no default sort.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_sql_q3databrowser.cpp 0
*/

void Q3DataBrowser::setSort(const QStringList& sort)
{
    d->cur.setSort(sort);
}

/*!
    \overload

    Sets the data browser's sort to the QSqlIndex \a sort. To apply
    the new sort, use refresh().

*/
void Q3DataBrowser::setSort(const QSqlIndex& sort)
{
    d->cur.setSort(sort);
}

QStringList Q3DataBrowser::sort() const
{
    return d->cur.sort();
}


/*!
    \property Q3DataBrowser::filter
    \brief the data browser's filter

    The filter applies to the data shown in the browser. Call
    refresh() to apply the new filter. A filter is a string containing
    a SQL WHERE clause without the WHERE keyword, e.g. "id>1000",
    "name LIKE 'A%'", etc.

    There is no default filter.

    \sa sort()
*/

void Q3DataBrowser::setFilter(const QString& filter)
{
    d->cur.setFilter(filter);
}


QString Q3DataBrowser::filter() const
{
    return d->cur.filter();
}


/*!
    Sets the default cursor used by the data browser to \a cursor. If
    \a autoDelete is true (the default is false), the data browser
    takes ownership of the \a cursor pointer, which will be deleted
    when the browser is destroyed, or when setSqlCursor() is called
    again. To activate the \a cursor use refresh(). The cursor's edit
    buffer is used in the default form to browse and edit records.

    \sa sqlCursor() form() setForm()
*/

void Q3DataBrowser::setSqlCursor(Q3SqlCursor* cursor, bool autoDelete)
{
    if (!cursor)
        return;
    d->cur.setCursor(cursor, autoDelete);
    d->frm.setRecord(cursor->editBuffer());
    if (cursor->isReadOnly())
        setReadOnly(true);
}


/*!
    Returns the default cursor used for navigation, or 0 if there is
    no default cursor.

    \sa setSqlCursor()
*/

Q3SqlCursor* Q3DataBrowser::sqlCursor() const
{
    return d->cur.cursor();
}


/*!
    Sets the browser's default form to \a form. The cursor and all
    navigation and data manipulation functions that the browser
    provides become available to the \a form.
*/

void Q3DataBrowser::setForm(Q3SqlForm* form)
{
    d->frm.setForm(form);
}


/*!
    Returns the data browser's default form or 0 if no form has been
    set.
*/

Q3SqlForm* Q3DataBrowser::form()
{
    return d->frm.form();
}

/*!
    \property Q3DataBrowser::readOnly
    \brief whether the browser is read-only

    The default is false, i.e. data can be edited. If the data browser
    is read-only, no database edits will be allowed.
*/

void Q3DataBrowser::setReadOnly(bool active)
{
    d->readOnly = active;
}

bool Q3DataBrowser::isReadOnly() const
{
    return d->readOnly;
}

void Q3DataBrowser::setConfirmEdits(bool confirm)
{
    d->dat.setConfirmEdits(confirm);
}

/*!
    \property Q3DataBrowser::confirmInsert
    \brief whether the data browser confirms insertions

    If this property is true, the browser confirms insertions,
    otherwise insertions happen immediately.

    \sa confirmCancels() confirmEdits() confirmUpdate() confirmDelete() confirmEdit()
*/

void Q3DataBrowser::setConfirmInsert(bool confirm)
{
    d->dat.setConfirmInsert(confirm);
}

/*!
    \property Q3DataBrowser::confirmUpdate
    \brief whether the browser confirms updates

    If this property is true, the browser confirms updates, otherwise
    updates happen immediately.

    \sa confirmCancels() confirmEdits() confirmInsert() confirmDelete() confirmEdit()
*/

void Q3DataBrowser::setConfirmUpdate(bool confirm)
{
    d->dat.setConfirmUpdate(confirm);
}

/*!
    \property Q3DataBrowser::confirmDelete
    \brief whether the browser confirms deletions

    If this property is true, the browser confirms deletions,
    otherwise deletions happen immediately.

    \sa confirmCancels() confirmEdits() confirmUpdate() confirmInsert() confirmEdit()
*/

void Q3DataBrowser::setConfirmDelete(bool confirm)
{
    d->dat.setConfirmDelete(confirm);
}

/*!
    \property Q3DataBrowser::confirmEdits
    \brief whether the browser confirms edits

    If this property is true, the browser confirms all edit operations
    (insertions, updates and deletions), otherwise all edit operations
    happen immediately. Confirmation is achieved by presenting the
    user with a message box -- this behavior can be changed by
    reimplementing the confirmEdit() function,

    \sa confirmEdit() confirmCancels() confirmInsert() confirmUpdate() confirmDelete()
*/

bool Q3DataBrowser::confirmEdits() const
{
    return (d->dat.confirmEdits());
}

bool Q3DataBrowser::confirmInsert() const
{
    return (d->dat.confirmInsert());
}

bool Q3DataBrowser::confirmUpdate() const
{
    return (d->dat.confirmUpdate());
}

bool Q3DataBrowser::confirmDelete() const
{
    return (d->dat.confirmDelete());
}

/*!
    \property Q3DataBrowser::confirmCancels
    \brief whether the browser confirms cancel operations

    If this property is true, all cancels must be confirmed by the
    user through a message box (this behavior can be changed by
    overriding the confirmCancel() function), otherwise all cancels
    occur immediately. The default is false.

    \sa confirmEdits() confirmCancel()
*/

void Q3DataBrowser::setConfirmCancels(bool confirm)
{
    d->dat.setConfirmCancels(confirm);
}

bool Q3DataBrowser::confirmCancels() const
{
    return d->dat.confirmCancels();
}

/*!
    \property Q3DataBrowser::autoEdit
    \brief whether the browser automatically applies edits

    The default value for this property is true. When the user begins
    an insertion or an update on a form there are two possible
    outcomes when they navigate to another record:

    \list
    \i the insert or update is is performed -- this occurs if autoEdit is true
    \i the insert or update is discarded -- this occurs if autoEdit is false
    \endlist
*/

void Q3DataBrowser::setAutoEdit(bool autoEdit)
{
    d->dat.setAutoEdit(autoEdit);
}

bool Q3DataBrowser::autoEdit() const
{
    return d->dat.autoEdit();
}

/*!
    \fn void Q3DataBrowser::firstRecordAvailable(bool available)

    This signal is emitted whenever the position of the cursor
    changes. The \a available parameter indicates whether or not the
    first record in the default cursor is available.
*/

/*!
    \fn void Q3DataBrowser::lastRecordAvailable(bool available)

    This signal is emitted whenever the position of the cursor
    changes. The \a available parameter indicates whether or not the
    last record in the default cursor is available.
*/

/*!
    \fn void Q3DataBrowser::nextRecordAvailable(bool available)

    This signal is emitted whenever the position of the cursor
    changes. The \a available parameter indicates whether or not the
    next record in the default cursor is available.
*/


/*!
    \fn void Q3DataBrowser::prevRecordAvailable(bool available)

    This signal is emitted whenever the position of the cursor
    changes. The \a available parameter indicates whether or not the
    previous record in the default cursor is available.
*/


/*!
    \fn void Q3DataBrowser::currentChanged(const QSqlRecord* record)

    This signal is emitted whenever the current cursor position
    changes. The \a record parameter points to the contents of the
    current cursor's record.
*/


/*!
    \fn void Q3DataBrowser::primeInsert(QSqlRecord* buf)

    This signal is emitted when the data browser enters insertion
    mode. The \a buf parameter points to the record buffer that is to
    be inserted. Connect to this signal to, for example, prime the
    record buffer with default data values, auto-numbered fields etc.
    (Note that Q3SqlCursor::primeInsert() is \e not called on the
    default cursor, as this would corrupt values in the form.)

    \sa insert()
*/


/*!
    \fn void Q3DataBrowser::primeUpdate(QSqlRecord* buf)

    This signal is emitted when the data browser enters update mode.
    Note that during navigation (first(), last(), next(), prev()),
    each record that is shown in the default form is primed for
    update. The \a buf parameter points to the record buffer being
    updated. (Note that Q3SqlCursor::primeUpdate() is \e not called on
    the default cursor, as this would corrupt values in the form.)
    Connect to this signal in order to, for example, keep track of
    which records have been updated, perhaps for auditing purposes.

    \sa update()
*/

/*!
    \fn void Q3DataBrowser::primeDelete(QSqlRecord* buf)

    This signal is emitted when the data browser enters deletion mode.
    The \a buf parameter points to the record buffer being deleted.
    (Note that Q3SqlCursor::primeDelete() is \e not called on the
    default cursor, as this would corrupt values in the form.)
    Connect to this signal in order to, for example, save a copy of
    the deleted record for auditing purposes.

    \sa del()
*/


/*!
    \fn void Q3DataBrowser::cursorChanged(Q3SqlCursor::Mode mode)

    This signal is emitted whenever the cursor record was changed due
    to navigation. The \a mode parameter is the edit that just took
    place, e.g. Insert, Update or Delete. See \l Q3SqlCursor::Mode.
*/


/*!
    Refreshes the data browser's data using the default cursor. The
    browser's current filter and sort are applied if they have been
    set.

    \sa setFilter() setSort()
*/

void Q3DataBrowser::refresh()
{
    d->cur.refresh();
}


/*!
    Performs an insert operation on the data browser's cursor. If
    there is no default cursor or no default form, nothing happens.

    If auto-editing is on (see setAutoEdit()), the following happens:

    \list
    \i If the browser is already actively inserting a record,
    the current form's data is inserted into the database.
    \i If the browser is not inserting a record, but the current record
    was changed by the user, the record is updated in the database with
    the current form's data (i.e. with the changes).
    \endlist

    If there is an error handling any of the above auto-edit actions,
    handleError() is called and no insert or update is performed.

    If no error occurred, or auto-editing is not enabled, the data browser
    begins actively inserting a record into the database by performing the
    following actions:

    \list
    \i The default cursor is primed for insert using Q3SqlCursor::primeInsert().
    \i The primeInsert() signal is emitted.
    \i The form is updated with the values in the default cursor's.
    edit buffer so that the user can fill in the values to be inserted.
    \endlist

*/

void Q3DataBrowser::insert()
{
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return;
    bool doIns = true;
    QSql::Confirm conf = QSql::Yes;
    switch (d->dat.mode()) {
    case QSql::Insert:
        if (autoEdit()) {
            if (confirmInsert())
                conf = confirmEdit(QSql::Insert);
            switch (conf) {
            case QSql::Yes:
                insertCurrent();
                break;
            case QSql::No:
                break;
            case QSql::Cancel:
                doIns = false;
                break;
            }
        }
        break;
    default:
        if (autoEdit() && currentEdited()) {
            if (confirmUpdate())
                conf = confirmEdit(QSql::Update);
            switch (conf) {
            case QSql::Yes:
                updateCurrent();
                break;
            case QSql::No:
                break;
            case QSql::Cancel:
                doIns = false;
                break;
            }
        }
        break;
    }
    if (doIns) {
        d->dat.setMode(QSql::Insert);
        sqlCursor()->primeInsert();
        emit primeInsert(d->frm.record());
        readFields();
    }
}


/*!
    Performs an update operation on the data browser's cursor.

    If there is no default cursor or no default form, nothing happens.
    Otherwise, the following happens:

    If the data browser is actively inserting a record (see insert()),
    that record is inserted into the database using insertCurrent().
    Otherwise, the database is updated with the current form's data
    using updateCurrent(). If there is an error handling either
    action, handleError() is called.
*/

void Q3DataBrowser::update()
{
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return;
    QSql::Confirm conf = QSql::Yes;
    switch (d->dat.mode()){
    case QSql::Insert:
        if (confirmInsert())
            conf = confirmEdit(QSql::Insert);
        switch (conf) {
            case QSql::Yes:
                if (insertCurrent())
                    d->dat.setMode(QSql::Update);
            break;
            case QSql::No:
                d->dat.setMode(QSql::Update);
            cur->editBuffer(true);
            readFields();
            break;
            case QSql::Cancel:
            break;
        }
    break;
    default:
        d->dat.setMode(QSql::Update);
        if (confirmUpdate())
            conf = confirmEdit(QSql::Update);
        switch (conf) {
        case QSql::Yes:
            updateCurrent();
            break;
        case QSql::No:
        case QSql::Cancel:
            break;
        }
        break;
    }
}


/*!
    Performs a delete operation on the data browser's cursor. If there
    is no default cursor or no default form, nothing happens.

    Otherwise, the following happens:

    The current form's record is deleted from the database, providing
    that the data browser is not in insert mode. If the data browser
    is actively inserting a record (see insert()), the insert action
    is canceled, and the browser navigates to the last valid record
    that was current. If there is an error, handleError() is called.
*/

void Q3DataBrowser::del()
{
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return;
    QSql::Confirm conf = QSql::Yes;
    switch (d->dat.mode()){
    case QSql::Insert:
        if (confirmCancels())
            conf = confirmCancel(QSql::Insert);
        if (conf == QSql::Yes) {
            cur->editBuffer(true); /* restore from cursor */
            readFields();
            d->dat.setMode(QSql::Update);
        } else
            d->dat.setMode(QSql::Insert);
        break;
    default:
        if (confirmDelete())
            conf = confirmEdit(QSql::Delete);
        switch (conf) {
        case QSql::Yes:
            emit primeDelete(buf);
            deleteCurrent();
            break;
        case QSql::No:
        case QSql::Cancel:
            break;
        }
        d->dat.setMode(QSql::Update);
        break;
    }
}

/*!
  Moves the default cursor to the record specified by index \a i
  and refreshes the default form to display that record. If there is
  no default form or no default cursor, nothing happens. If
  \a relative is true (the default is false), the cursor is moved
  relative to its current position. If the data browser successfully
  navigated to the desired record, the default cursor is primed for
  update and the primeUpdate() signal is emitted.
  
  If the browser is already positioned on the desired record nothing
  happens. Returns false if there is no cursor. Otherwise returns
  true.
*/

bool Q3DataBrowser::seek(int i, bool relative)
{
    int b = 0;
    Q3SqlCursor* cur = d->cur.cursor();
    if (!cur)
        return false;
    if (preNav())
        b = cur->seek(i, relative);
    postNav(b);
    return b;
}

/*!
    Moves the default cursor to the first record and refreshes the
    default form to display this record. If there is no default form
    or no default cursor, nothing happens. If the data browser
    successfully navigated to the first record, the default cursor is
    primed for update and the primeUpdate() signal is emitted.

    If the browser is already positioned on the first record nothing
    happens.

*/

void Q3DataBrowser::first()
{
    nav(&Q3SqlCursor::first);
}


/*!
    Moves the default cursor to the last record and refreshes the
    default form to display this record. If there is no default form
    or no default cursor, nothing happens. If the data browser
    successfully navigated to the last record, the default cursor is
    primed for update and the primeUpdate() signal is emitted.

    If the browser is already positioned on the last record nothing
    happens.
*/

void Q3DataBrowser::last()
{
    nav(&Q3SqlCursor::last);
}


/*!
    Moves the default cursor to the next record and refreshes the
    default form to display this record. If there is no default form
    or no default cursor, nothing happens. If the data browser
    successfully navigated to the next record, the default cursor is
    primed for update and the primeUpdate() signal is emitted.

    If the browser is positioned on the last record nothing happens.
*/

void Q3DataBrowser::next()
{
    nav(&Q3SqlCursor::next);
}


/*!
    Moves the default cursor to the previous record and refreshes the
    default form to display this record. If there is no default form
    or no default cursor, nothing happens. If the data browser
    successfully navigated to the previous record, the default cursor
    is primed for update and the primeUpdate() signal is emitted.

    If the browser is positioned on the first record nothing happens.
*/

void Q3DataBrowser::prev()
{
    nav(&Q3SqlCursor::previous);
}

/*!
    Reads the fields from the default cursor's edit buffer and
    displays them in the form. If there is no default cursor or no
    default form, nothing happens.
*/

void Q3DataBrowser::readFields()
{
    d->frm.readFields();
}


/*!
    Writes the form's data to the default cursor's edit buffer. If
    there is no default cursor or no default form, nothing happens.
*/

void Q3DataBrowser::writeFields()
{
    d->frm.writeFields();
}


/*!
    Clears all the values in the form.

    All the edit buffer field values are set to their 'zero state',
    e.g. 0 for numeric fields and "" for string fields. Then the
    widgets are updated using the property map. For example, a
    combobox that is property-mapped to integers would scroll to the
    first item. See the \l Q3SqlPropertyMap constructor for the default
    mappings of widgets to properties.
*/

void Q3DataBrowser::clearValues()
{
    d->frm.clearValues();
}

/*!
    Reads the fields from the default form into the default cursor and
    performs an insert on the default cursor. If there is no default
    form or no default cursor, nothing happens. If an error occurred
    during the insert into the database, handleError() is called and
    false is returned. If the insert was successful, the cursor is
    refreshed and relocated to the newly inserted record, the
    cursorChanged() signal is emitted, and true is returned.

    \sa cursorChanged() sqlCursor() form() handleError()
*/

bool Q3DataBrowser::insertCurrent()
{
    if (isReadOnly())
        return false;
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return false;
    writeFields();
    emit beforeInsert(buf);
    int ar = cur->insert();
    if (!ar || !cur->isActive()) {
        handleError(cur->lastError());
        refresh();
        updateBoundary();
    } else {
        refresh();
        d->cur.findBuffer(cur->primaryIndex());
        updateBoundary();
        cursorChanged(Q3SqlCursor::Insert);
        return true;
    }
    return false;
}


/*!
    Reads the fields from the default form into the default cursor and
    performs an update on the default cursor. If there is no default
    form or no default cursor, nothing happens. If an error occurred
    during the update on the database, handleError() is called and
    false is returned. If the update was successful, the cursor is
    refreshed and relocated to the updated record, the cursorChanged()
    signal is emitted, and true is returned.

    \sa cursor() form() handleError()
*/

bool Q3DataBrowser::updateCurrent()
{
    if (isReadOnly())
        return false;
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return false;
    writeFields();
    emit beforeUpdate(buf);
    int ar = cur->update();
    if (!ar || !cur->isActive()) {
        handleError(cur->lastError());
        refresh();
        updateBoundary();
    } else {
        refresh();
        d->cur.findBuffer(cur->primaryIndex());
        updateBoundary();
        cur->editBuffer(true);
        cursorChanged(Q3SqlCursor::Update);
        readFields();
        return true;
    }
    return false;
}


/*!
    Performs a delete on the default cursor using the values from the
    default form and updates the default form. If there is no default
    form or no default cursor, nothing happens. If the deletion was
    successful, the cursor is repositioned to the nearest record and
    true is returned. The nearest record is the next record if there
    is one otherwise the previous record if there is one. If an error
    occurred during the deletion from the database, handleError() is
    called and false is returned.

    \sa cursor() form() handleError()
*/

bool Q3DataBrowser::deleteCurrent()
{
    if (isReadOnly())
        return false;
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return false;
    writeFields();
    int n = cur->at();
    emit beforeDelete(buf);
    int ar = cur->del();
    if (ar) {
        refresh();
        updateBoundary();
        cursorChanged(Q3SqlCursor::Delete);
        if (!cur->seek(n))
            last();
        if (cur->isValid()) {
            cur->editBuffer(true);
            readFields();
        } else {
            clearValues();
        }
        return true;
    } else {
        if (!cur->isActive()) {
            handleError(cur->lastError());
            refresh();
            updateBoundary();
        }
    }
    return false;
}


/*!
    Returns true if the form's edit buffer differs from the current
    cursor buffer; otherwise returns false.
*/

bool Q3DataBrowser::currentEdited()
{
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return false;
    if (!cur->isActive() || !cur->isValid())
        return false;
    writeFields();
    for (int i = 0; i < cur->count(); ++i) {
        if (cur->value(i) != buf->value(i))
            return true;
    }
    return false;
}

/*! \internal

  Pre-navigation checking.
*/

bool Q3DataBrowser::preNav()
{
    QSqlRecord* buf = d->frm.record();
    Q3SqlCursor* cur = d->cur.cursor();
    if (!buf || !cur)
        return false;

    if (!isReadOnly() && autoEdit() && currentEdited()) {
        bool ok = true;
        QSql::Confirm conf = QSql::Yes;
        switch (d->dat.mode()){
        case QSql::Insert:
            if (confirmInsert())
                conf = confirmEdit(QSql::Insert);
            switch (conf) {
            case QSql::Yes:
                ok = insertCurrent();
                d->dat.setMode(QSql::Update);
                break;
            case QSql::No:
                d->dat.setMode(QSql::Update);
                break;
            case QSql::Cancel:
                return false;
            }
            break;
        default:
            if (confirmUpdate())
                conf = confirmEdit(QSql::Update);
            switch (conf) {
            case QSql::Yes:
                ok = updateCurrent();
                break;
            case QSql::No:
                break;
            case QSql::Cancel:
                return false;
            }
        }
        return ok;
    }
    return true;
}

/*! \internal

  Handles post-navigation according to \a primeUpd.
*/

void Q3DataBrowser::postNav(bool primeUpd)
{
    if (primeUpd) {
        QSqlRecord* buf = d->frm.record();
        Q3SqlCursor* cur = d->cur.cursor();
        if (!buf || !cur)
            return;
        currentChanged(cur);
        cur->primeUpdate();
        emit primeUpdate(buf);
        readFields();
    }
    updateBoundary();
}

/*! \internal

  Navigate default cursor according to \a nav. Handles autoEdit.

*/
void Q3DataBrowser::nav(Nav nav)
{
    int b = 0;
    Q3SqlCursor* cur = d->cur.cursor();
    if (!cur)
        return;
    if (preNav())
        b = (cur->*nav)();
    postNav(b);
}

/*!
    If boundaryChecking() is true, checks the boundary of the current
    default cursor and emits signals which indicate the position of
    the cursor.
*/

void Q3DataBrowser::updateBoundary()
{
    if (d->boundaryCheck) {
        Boundary bound = boundary();
        switch (bound) {
        case Unknown:
        case None:
            emit firstRecordAvailable(true);
            emit prevRecordAvailable(true);
            emit nextRecordAvailable(true);
            emit lastRecordAvailable(true);
            break;

        case BeforeBeginning:
            emit firstRecordAvailable(false);
            emit prevRecordAvailable(false);
            emit nextRecordAvailable(true);
            emit lastRecordAvailable(true);
            break;

        case Beginning:
            emit firstRecordAvailable(false);
            emit prevRecordAvailable(false);
            emit nextRecordAvailable(true);
            emit lastRecordAvailable(true);
            break;

        case End:
            emit firstRecordAvailable(true);
            emit prevRecordAvailable(true);
            emit nextRecordAvailable(false);
            emit lastRecordAvailable(false);
            break;

        case AfterEnd:
            emit firstRecordAvailable(true);
            emit prevRecordAvailable(true);
            emit nextRecordAvailable(false);
            emit lastRecordAvailable(false);
            break;
        }
    }
}

/*!
    Virtual function which handles the error \a error. The default
    implementation warns the user with a message box.
*/

void Q3DataBrowser::handleError(const QSqlError& error)
{
    d->dat.handleError(this, error);
}

/*!
    Protected virtual function which returns a confirmation for an
    edit of mode \a m. Derived classes can reimplement this function
    and provide their own confirmation dialog. The default
    implementation uses a message box which prompts the user to
    confirm the edit action.
*/

QSql::Confirm Q3DataBrowser::confirmEdit(QSql::Op m)
{
    return d->dat.confirmEdit(this, m);
}

/*!
    Protected virtual function which returns a confirmation for
    canceling an edit mode \a m. Derived classes can reimplement this
    function and provide their own confirmation dialog. The default
    implementation uses a message box which prompts the user to
    confirm the edit action.
*/

QSql::Confirm  Q3DataBrowser::confirmCancel(QSql::Op m)
{
    return d->dat.confirmCancel(this, m);
}

/*!
    \fn void Q3DataBrowser::beforeInsert(QSqlRecord* buf)

    This signal is emitted just before the cursor's edit buffer is
    inserted into the database. The \a buf parameter points to the
    edit buffer being inserted. You might connect to this signal to
    populate a generated primary key for example.
*/

/*!
    \fn void Q3DataBrowser::beforeUpdate(QSqlRecord* buf)

    This signal is emitted just before the cursor's edit buffer is
    updated in the database. The \a buf parameter points to the edit
    buffer being updated. You might connect to this signal to capture
    some auditing information about the update.
*/

/*!
    \fn void Q3DataBrowser::beforeDelete(QSqlRecord* buf)

    This signal is emitted just before the cursor's edit buffer  is
    deleted from the database. The \a buf parameter points to the edit
    buffer being deleted. You might connect to this signal to capture
    some auditing information about the deletion.
*/

QT_END_NAMESPACE

#endif
