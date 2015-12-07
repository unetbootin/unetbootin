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

#include <qplatformdefs.h>
#include "q3sqlcursor.h"

#ifndef QT_NO_SQL

#include "qsqldriver.h"
#include "qsqlresult.h"
#include "qdatetime.h"
#include "qsqldatabase.h"
#include "qsql.h"
#include "q3sqlrecordinfo.h"
#include "q3sqlfieldinfo.h"

QT_BEGIN_NAMESPACE

class Q3SqlCursorPrivate
{
public:

    Q3SqlCursorPrivate(const QString& name, QSqlDatabase sdb)
        : lastAt(QSql::BeforeFirst), nm(name), srt(name), md(0), db(sdb), q(0)
    {}
    ~Q3SqlCursorPrivate()
    {
        delete q;
    }

    QSqlQuery* query()
    {
        if (!q)
            q = new QSqlQuery(QString(), db);
        return q;
    }

    int lastAt;
    QString nm;         //name
    QSqlIndex srt;        //sort
    QString ftr;        //filter
    int md;         //mode
    QSqlIndex priIndx;    //primary index
    QSqlRecord editBuffer;
    // the primary index as it was before the user changed the values in editBuffer
    QString editIndex;
    Q3SqlRecordInfo infoBuffer;
    QSqlDatabase db;
    QSqlQuery *q;
};

QString qOrderByClause(const QSqlIndex & i, const QString& prefix = QString())
{
    QString str;
    int k = i.count();
    if(k == 0)
        return QString();
    str = QLatin1String(" order by ") + i.toString(prefix);
    return str;
}

QString qWhereClause(const QString& prefix, QSqlField* field, const QSqlDriver* driver)
{
    QString f;
    if (field && driver) {
        if (!prefix.isEmpty())
            f += prefix + QLatin1Char('.');
        f += field->name();
        if (field->isNull()) {
            f += QLatin1String(" IS NULL");
        } else {
            f += QLatin1String(" = ") + driver->formatValue(field);
        }
    }
    return f;
}

QString qWhereClause(QSqlRecord* rec, const QString& prefix, const QString& sep,
                      const QSqlDriver* driver)
{
    static QString blank(QLatin1Char(' '));
    QString filter;
    bool separator = false;
    for (int j = 0; j < rec->count(); ++j) {
        QSqlField f = rec->field(j);
        if (rec->isGenerated(j)) {
            if (separator)
                filter += sep + blank;
            filter += qWhereClause(prefix, &f, driver);
            filter += blank;
            separator = true;
        }
    }
    return filter;
}

/*!
    \class Q3SqlCursor q3sqlcursor.h
    \brief The Q3SqlCursor class provides browsing and editing of SQL
    tables and views.

    \compat

    A Q3SqlCursor is a database record (see \l QSqlRecord) that
    corresponds to a table or view within an SQL database (see \l
    QSqlDatabase). There are two buffers in a cursor, one used for
    browsing and one used for editing records. Each buffer contains a
    list of fields which correspond to the fields in the table or
    view.

    When positioned on a valid record, the browse buffer contains the
    values of the current record's fields from the database. The edit
    buffer is separate, and is used for editing existing records and
    inserting new records.

    For browsing data, a cursor must first select() data from the
    database. After a successful select() the cursor is active
    (isActive() returns true), but is initially not positioned on a
    valid record (isValid() returns false). To position the cursor on
    a valid record, use one of the navigation functions, next(),
    previous(), first(), last(), or seek(). Once positioned on a valid
    record, data can be retrieved from the browse buffer using
    value(). If a navigation function is not successful, it returns
    false, the cursor will no longer be positioned on a valid record
    and the values returned by value() are undefined.

    For example:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 0

    In the above example, a cursor is created specifying a table or
    view name in the database. Then, select() is called, which can be
    optionally parameterised to filter and order the records
    retrieved. Each record in the cursor is retrieved using next().
    When next() returns false, there are no more records to process,
    and the loop terminates.

    For editing records (rows of data), a cursor contains a separate
    edit buffer which is independent of the fields used when browsing.
    The functions insert(), update() and del() operate on the edit
    buffer. This allows the cursor to be repositioned to other
    records while simultaneously maintaining a separate buffer for
    edits. You can get a pointer to the edit buffer using
    editBuffer(). The primeInsert(), primeUpdate() and primeDelete()
    functions also return a pointer to the edit buffer and prepare it
    for insert, update and delete respectively. Edit operations only
    affect a single row at a time. Note that update() and del()
    require that the table or view contain a primaryIndex() to ensure
    that edit operations affect a unique record within the database.

    For example:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 1

    To edit an existing database record, first move to the record you
    wish to update. Call primeUpdate() to get the pointer to the
    cursor's edit buffer. Then use this pointer to modify the values
    in the edit buffer. Finally, call update() to save the changes to
    the database. The values in the edit buffer will be used to
    locate the appropriate record when updating the database (see
    primaryIndex()).

    Similarly, when deleting an existing database record, first move
    to the record you wish to delete. Then, call primeDelete() to get
    the pointer to the edit buffer. Finally, call del() to delete the
    record from the database. Again, the values in the edit buffer
    will be used to locate and delete the appropriate record.

    To insert a new record, call primeInsert() to get the pointer to
    the edit buffer. Use this pointer to populate the edit buffer
    with new values and then insert() the record into the database.

    After calling insert(), update() or del(), the cursor is no longer
    positioned on a valid record and can no longer be navigated
    (isValid() return false). The reason for this is that any changes
    made to the database will not be visible until select() is called
    to refresh the cursor. You can change this behavior by passing
    false to insert(), update() or del() which will prevent the cursor
    from becoming invalid. The edits will still not be visible when
    navigating the cursor until select() is called.

    Q3SqlCursor contains virtual methods which allow editing behavior
    to be customized by subclasses. This allows custom cursors to be
    created that encapsulate the editing behavior of a database table
    for an entire application. For example, a cursor can be customized
    to always auto-number primary index fields, or provide fields with
    suitable default values, when inserting new records. Q3SqlCursor
    generates SQL statements which are sent to the database engine;
    you can control which fields are included in these statements
    using setGenerated().

    Note that Q3SqlCursor does not inherit from QObject. This means
    that you are responsible for destroying instances of this class
    yourself. However if you create a Q3SqlCursor and use it in a
    \l Q3DataTable, \l Q3DataBrowser or a \l Q3DataView these classes will
    usually take ownership of the cursor and destroy it when they
    don't need it anymore. The documentation for Q3DataTable,
    Q3DataBrowser and Q3DataView explicitly states which calls take
    ownership of the cursor.
*/

/*!
    \enum Q3SqlCursor::Mode

    This enum type describes how Q3SqlCursor operates on records in the
    database.

    \value ReadOnly  the cursor can only SELECT records from the
    database.

    \value Insert  the cursor can INSERT records into the database.

    \value Update  the cursor can UPDATE records in the database.

    \value Delete  the cursor can DELETE records from the database.

    \value Writable  the cursor can INSERT, UPDATE and DELETE records
    in the database.
*/

/*!
    \fn QVariant Q3SqlCursor::value(const QString &name) const

    \overload

    Returns the value of the field named \a name.
*/

/*!
    \fn void Q3SqlCursor::setValue(const QString &name, const QVariant &val)

    \overload

    Sets the value for the field named \a name to \a val.
*/

/*!
    Constructs a cursor on database \a db using table or view \a name.

    If \a autopopulate is true (the default), the \a name of the
    cursor must correspond to an existing table or view name in the
    database so that field information can be automatically created.
    If the table or view does not exist, the cursor will not be
    functional.

    The cursor is created with an initial mode of Q3SqlCursor::Writable
    (meaning that records can be inserted, updated or deleted using
    the cursor). If the cursor does not have a unique primary index,
    update and deletes cannot be performed.

    Note that \a autopopulate refers to populating the cursor with
    meta-data, e.g. the names of the table's fields, not with
    retrieving data. The select() function is used to populate the
    cursor with data.

    \sa setName() setMode()
*/

Q3SqlCursor::Q3SqlCursor(const QString & name, bool autopopulate, QSqlDatabase db)
    : QSqlRecord(), QSqlQuery(QString(), db)
{
    d = new Q3SqlCursorPrivate(name, db);
    setMode(Writable);
    if (!d->nm.isEmpty())
        setName(d->nm, autopopulate);
}

/*!
    Constructs a copy of \a other.
*/

Q3SqlCursor::Q3SqlCursor(const Q3SqlCursor & other)
    : QSqlRecord(other), QSqlQuery(other)
{
    d = new Q3SqlCursorPrivate(other.d->nm, other.d->db);
    d->lastAt = other.d->lastAt;
    d->nm = other.d->nm;
    d->srt = other.d->srt;
    d->ftr = other.d->ftr;
    d->priIndx = other.d->priIndx;
    d->editBuffer = other.d->editBuffer;
    d->infoBuffer = other.d->infoBuffer;
    d->q = 0; // do not share queries
    setMode(other.mode());
}

/*!
    Destroys the object and frees any allocated resources.
*/

Q3SqlCursor::~Q3SqlCursor()
{
    delete d;
}

/*!
    Sets the cursor equal to \a other.
*/

Q3SqlCursor& Q3SqlCursor::operator=(const Q3SqlCursor& other)
{
    QSqlRecord::operator=(other);
    QSqlQuery::operator=(other);
    delete d;
    d = new Q3SqlCursorPrivate(other.d->nm, other.d->db);
    d->lastAt = other.d->lastAt;
    d->nm = other.d->nm;
    d->srt = other.d->srt;
    d->ftr = other.d->ftr;
    d->priIndx = other.d->priIndx;
    d->editBuffer = other.d->editBuffer;
    d->infoBuffer = other.d->infoBuffer;
    d->q = 0; // do not share queries
    setMode(other.mode());
    return *this;
}

/*!
    Sets the current sort to \a sort. Note that no new records are
    selected. To select new records, use select(). The \a sort will
    apply to any subsequent select() calls that do not explicitly
    specify a sort.
*/

void Q3SqlCursor::setSort(const QSqlIndex& sort)
{
    d->srt = sort;
}

/*!
    Returns the current sort, or an empty index if there is no current
    sort.
*/
QSqlIndex Q3SqlCursor::sort() const
{
    return d->srt;
}

/*!
    Sets the current filter to \a filter. Note that no new records are
    selected. To select new records, use select(). The \a filter will
    apply to any subsequent select() calls that do not explicitly
    specify a filter.

    The filter is a SQL \c WHERE clause without the keyword 'WHERE',
    e.g. \c{name='Dave'} which will be processed by the DBMS.
*/
void Q3SqlCursor::setFilter(const QString& filter)
{
    d->ftr = filter;
}

/*!
    Returns the current filter, or an empty string if there is no
    current filter.
*/
QString Q3SqlCursor::filter() const
{
    return d->ftr;
}

/*!
    Sets the name of the cursor to \a name. If \a autopopulate is true
    (the default), the \a name must correspond to a valid table or
    view name in the database. Also, note that all references to the
    cursor edit buffer become invalidated when fields are
    auto-populated. See the Q3SqlCursor constructor documentation for
    more information.
*/
void Q3SqlCursor::setName(const QString& name, bool autopopulate)
{
    d->nm = name;
    if (autopopulate) {
        if (driver()) {
            d->infoBuffer = driver()->record(name);
            *this = d->infoBuffer.toRecord();
            d->editBuffer = *this;
            d->priIndx = driver()->primaryIndex(name);
        }
        if (isEmpty())
            qWarning("Q3SqlCursor::setName: unable to build record, does '%s' exist?", name.latin1());
    }
}

/*!
    Returns the name of the cursor.
*/

QString Q3SqlCursor::name() const
{
    return d->nm;
}

/*! \internal
*/

QString Q3SqlCursor::toString(const QString& prefix, const QString& sep) const
{
    QString pflist;
    QString pfix =  prefix.isEmpty() ? prefix : prefix + QLatin1Char('.');
    bool comma = false;

    for (int i = 0; i < count(); ++i) {
        const QString fname = fieldName(i);
        if (isGenerated(i)) {
            if(comma)
                pflist += sep + QLatin1Char(' ');
            pflist += pfix + driver()->escapeIdentifier(fname, QSqlDriver::FieldName);
            comma = true;
        }
    }
    return pflist;
}

/*!
  \internal

  Assigns the record \a list.

*/
QSqlRecord & Q3SqlCursor::operator=(const QSqlRecord & list)
{
    return QSqlRecord::operator=(list);
}

/*!
    Append a copy of field \a fieldInfo to the end of the cursor. Note
    that all references to the cursor edit buffer become invalidated.
*/

void Q3SqlCursor::append(const Q3SqlFieldInfo& fieldInfo)
{
    d->editBuffer.append(fieldInfo.toField());
    d->infoBuffer.append(fieldInfo);
    QSqlRecord::append(fieldInfo.toField());
}

/*!
    Removes all fields from the cursor. Note that all references to
    the cursor edit buffer become invalidated.
*/
void Q3SqlCursor::clear()
{
    d->editBuffer.clear();
    d->infoBuffer.clear();
    QSqlRecord::clear();
}


/*!
    Insert a copy of \a fieldInfo at position \a pos. If a field
    already exists at \a pos, it is removed. Note that all references
    to the cursor edit buffer become invalidated.
*/

void  Q3SqlCursor::insert(int pos, const Q3SqlFieldInfo& fieldInfo)
{
    d->editBuffer.replace(pos, fieldInfo.toField());
    d->infoBuffer[pos] = fieldInfo;
    QSqlRecord::replace(pos, fieldInfo.toField());
}

/*!
    Removes the field at \a pos. If \a pos does not exist, nothing
    happens. Note that all references to the cursor edit buffer become
    invalidated.
*/

void Q3SqlCursor::remove(int pos)
{
    d->editBuffer.remove(pos);
    d->infoBuffer[pos] = Q3SqlFieldInfo();
    QSqlRecord::remove(pos);
}

/*!
    Sets the generated flag for the field \a name to \a generated. If
    the field does not exist, nothing happens. Only fields that have
    \a generated set to true are included in the SQL that is
    generated by insert(), update() or del().
*/

void Q3SqlCursor::setGenerated(const QString& name, bool generated)
{
    int pos = indexOf(name);
    if (pos == -1)
        return;
    QSqlRecord::setGenerated(name, generated);
    d->editBuffer.setGenerated(name, generated);
    d->infoBuffer[pos].setGenerated(generated);
}

/*!
    \overload

    Sets the generated flag for the field \a i to \a generated.
*/
void Q3SqlCursor::setGenerated(int i, bool generated)
{
    if (i < 0 || i >= (int)d->infoBuffer.count())
        return;
    QSqlRecord::setGenerated(i, generated);
    d->editBuffer.setGenerated(i, generated);
    d->infoBuffer[i].setGenerated(generated);
}

/*!
    Returns the primary index associated with the cursor as defined in
    the database, or an empty index if there is no primary index. If
    \a setFromCursor is true (the default), the index fields are
    populated with the corresponding values in the cursor's current
    record.
*/

QSqlIndex Q3SqlCursor::primaryIndex(bool setFromCursor) const
{
    if (setFromCursor) {
        for (int i = 0; i < d->priIndx.count(); ++i) {
            const QString fn = d->priIndx.fieldName(i);
            if (contains(fn))
                d->priIndx.setValue(i, QSqlRecord::value(fn));
        }
    }
    return d->priIndx;
}

/*!
    Sets the primary index associated with the cursor to the index \a
    idx. Note that this index must contain a field or set of fields
    which identify a unique record within the underlying database
    table or view so that update() and del() will execute as expected.

    \sa update() del()
*/

void Q3SqlCursor::setPrimaryIndex(const QSqlIndex& idx)
{
    d->priIndx = idx;
}


/*!
    Returns an index composed of \a fieldNames, all in ASCending
    order. Note that all field names must exist in the cursor,
    otherwise an empty index is returned.

    \sa QSqlIndex
*/

QSqlIndex Q3SqlCursor::index(const QStringList& fieldNames) const
{
    QSqlIndex idx;
    for (QStringList::ConstIterator it = fieldNames.begin(); it != fieldNames.end(); ++it) {
        QSqlField f = field((*it));
        if (!f.isValid()) { /* all fields must exist */
            idx.clear();
            break;
        }
        idx.append(f);
    }
    return idx;
}

/*!
    \overload

    Returns an index based on \a fieldName.
*/

QSqlIndex Q3SqlCursor::index(const QString& fieldName) const
{
    QStringList fl(fieldName);
    return index(fl);
}

/*!
    Selects all fields in the cursor from the database matching the
    filter criteria \a filter. The data is returned in the order
    specified by the index \a sort. Returns true if the data was
    successfully selected; otherwise returns false.

    The \a filter is a string containing a SQL \c WHERE clause but
    without the 'WHERE' keyword. The cursor is initially positioned at
    an invalid row after this function is called. To move to a valid
    row, use seek(), first(), last(), previous() or next().

    Example:
    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 2

    The filter will apply to any subsequent select() calls that do not
    explicitly specify another filter. Similarly the sort will apply
    to any subsequent select() calls that do not explicitly specify
    another sort.

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 3

*/

bool Q3SqlCursor::select(const QString & filter, const QSqlIndex & sort)
{
    QString fieldList(toString(d->nm));
    if (fieldList.isEmpty())
        return false;
    QString str(QLatin1String("select ") + fieldList);
    str += QLatin1String(" from ") + d->nm;
    if (!filter.isEmpty()) {
        d->ftr = filter;
        str += QLatin1String(" where ") + filter;
    } else
        d->ftr.clear();
    if (sort.count() > 0)
        str += QLatin1String(" order by ") + sort.toString(d->nm);
    d->srt = sort;
    return exec(str);
}

/*!
    \overload

    Selects all fields in the cursor from the database. The rows are
    returned in the order specified by the last call to setSort() or
    the last call to select() that specified a sort, whichever is the
    most recent. If there is no current sort, the order in which the
    rows are returned is undefined. The records are filtered according
    to the filter specified by the last call to setFilter() or the
    last call to select() that specified a filter, whichever is the
    most recent. If there is no current filter, all records are
    returned. The cursor is initially positioned at an invalid row. To
    move to a valid row, use seek(), first(), last(), previous() or
    next().

    \sa setSort() setFilter()
*/

bool Q3SqlCursor::select()
{
    return select(filter(), sort());
}

/*!
    \overload

    Selects all fields in the cursor from the database. The data is
    returned in the order specified by the index \a sort. The records
    are filtered according to the filter specified by the last call to
    setFilter() or the last call to select() that specified a filter,
    whichever is the most recent. The cursor is initially positioned
    at an invalid row. To move to a valid row, use seek(), first(),
    last(), previous() or next().
*/

bool Q3SqlCursor::select(const QSqlIndex& sort)
{
    return select(filter(), sort);
}

/*!
    \overload

    Selects all fields in the cursor matching the filter index \a
    filter. The data is returned in the order specified by the index
    \a sort. The \a filter index works by constructing a WHERE clause
    using the names of the fields from the \a filter and their values
    from the current cursor record. The cursor is initially positioned
    at an invalid row. To move to a valid row, use seek(), first(),
    last(), previous() or next(). This function is useful, for example,
    for retrieving data based upon a table's primary index:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 4

    In this example the QSqlIndex, pk, is used for two different
    purposes. When used as the filter (first) argument, the field
    names it contains are used to construct the WHERE clause, each set
    to the current cursor value, \c{WHERE id=10}, in this case. When
    used as the sort (second) argument the field names it contains are
    used for the ORDER BY clause, \c{ORDER BY id} in this example.
*/

bool Q3SqlCursor::select(const QSqlIndex & filter, const QSqlIndex & sort)
{
    return select(toString(filter, this, d->nm, QString(QLatin1Char('=')), QLatin1String("and")), sort);
}

/*!
    Sets the cursor mode to \a mode. This value can be an OR'ed
    combination of \l Q3SqlCursor::Mode values. The default mode for a
    cursor is Q3SqlCursor::Writable.

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 5
*/

void Q3SqlCursor::setMode(int mode)
{
    d->md = mode;
}

/*!
    Returns the current cursor mode.

    \sa setMode()
*/

int Q3SqlCursor::mode() const
{
    return d->md;
}

/*!
    Sets field \a name to \a calculated. If the field \a name does not
    exist, nothing happens. The value of a calculated field is set by
    the calculateField() virtual function which you must reimplement
    (or the field value will be an invalid QVariant). Calculated
    fields do not appear in generated SQL statements sent to the
    database.

    \sa calculateField()
*/

void Q3SqlCursor::setCalculated(const QString& name, bool calculated)
{
    int pos = indexOf(name);
    if (pos < 0)
        return;
    d->infoBuffer[pos].setCalculated(calculated);
    if (calculated)
        setGenerated(pos, false);
}

/*!
    Returns true if the field \a name exists and is calculated;
    otherwise returns false.

    \sa setCalculated()
*/

bool Q3SqlCursor::isCalculated(const QString& name) const
{
    int pos = indexOf(name);
    if (pos < 0)
        return false;
    return d->infoBuffer[pos].isCalculated();
}

/*!
    Sets field \a{name}'s trimmed status to \a trim. If the field \a
    name does not exist, nothing happens.

    When a trimmed field of type string is read from the
    database any trailing (right-most) spaces are removed.

    \sa isTrimmed() QVariant
*/

void Q3SqlCursor::setTrimmed(const QString& name, bool trim)
{
    int pos = indexOf(name);
    if (pos < 0)
        return;
    d->infoBuffer[pos].setTrim(trim);
}

/*!
    Returns true if the field \a name exists and is trimmed; otherwise
    returns false.

    When a trimmed field of type string or cstring is read from the
    database any trailing (right-most) spaces are removed.

    \sa setTrimmed()
*/

bool Q3SqlCursor::isTrimmed(const QString& name) const
{
    int pos = indexOf(name);
    if (pos < 0)
        return false;
    return d->infoBuffer[pos].isTrim();
}

/*!
    Returns true if the cursor is read-only; otherwise returns false.
    The default is false. Read-only cursors cannot be edited using
    insert(), update() or del().

    \sa setMode()
*/

bool Q3SqlCursor::isReadOnly() const
{
    return d->md == 0;
}

/*!
    Returns true if the cursor will perform inserts; otherwise returns
    false.

    \sa setMode()
*/

bool Q3SqlCursor::canInsert() const
{
    return ((d->md & Insert) == Insert) ;
}


/*!
    Returns true if the cursor will perform updates; otherwise returns
    false.

    \sa setMode()
*/

bool Q3SqlCursor::canUpdate() const
{
    return ((d->md & Update) == Update) ;
}

/*!
    Returns true if the cursor will perform deletes; otherwise returns
    false.

    \sa setMode()
*/

bool Q3SqlCursor::canDelete() const
{
    return ((d->md & Delete) == Delete) ;
}

/*!
    \overload

    Returns a formatted string composed of the \a prefix (e.g. table
    or view name), ".", the \a field name, the \a fieldSep and the
    field value. If the \a prefix is empty then the string will begin
    with the \a field name. This function is useful for generating SQL
    statements.
*/

QString Q3SqlCursor::toString(const QString& prefix, QSqlField* field, const QString& fieldSep) const
{
    QString f;
    if (field && driver()) {
        f = (prefix.length() > 0 ? prefix + QLatin1Char('.') : QString()) + field->name();
        f += QLatin1Char(' ') + fieldSep + QLatin1Char(' ');
        if (field->isNull()) {
            f += QLatin1String("NULL");
        } else {
            f += driver()->formatValue(field);
        }
    }
    return f;
}

/*!
    Returns a formatted string composed of all the fields in \a rec.
    Each field is composed of the \a prefix (e.g. table or view name),
    ".", the field name, the \a fieldSep and the field value. If the
    \a prefix is empty then each field will begin with the field name.
    The fields are then joined together separated by \a sep. Fields
    where isGenerated() returns false are not included. This function
    is useful for generating SQL statements.
*/

QString Q3SqlCursor::toString(QSqlRecord* rec, const QString& prefix, const QString& fieldSep,
                              const QString& sep) const
{
    static QString blank(QLatin1Char(' '));
    QString filter;
    bool separator = false;
    for (int j = 0; j < count(); ++j) {
        QSqlField f = rec->field(j);
        if (rec->isGenerated(j)) {
            if (separator)
                filter += sep + blank;
            filter += toString(prefix, &f, fieldSep);
            filter += blank;
            separator = true;
        }
    }
    return filter;
}

/*!
    \overload

    Returns a formatted string composed of all the fields in the index
    \a i. Each field is composed of the \a prefix (e.g. table or view
    name), ".", the field name, the \a fieldSep and the field value.
    If the \a prefix is empty then each field will begin with the field
    name. The field values are taken from \a rec. The fields are then
    joined together separated by \a sep. Fields where isGenerated()
    returns false are ignored. This function is useful for generating
    SQL statements.
*/

QString Q3SqlCursor::toString(const QSqlIndex& i, QSqlRecord* rec, const QString& prefix,
                                const QString& fieldSep, const QString& sep) const
{
    QString filter;
    bool separator = false;
    for(int j = 0; j < i.count(); ++j){
        if (rec->isGenerated(j)) {
            if(separator) {
                filter += QLatin1Char(' ') + sep + QLatin1Char(' ') ;
            }
            QString fn = i.fieldName(j);
            QSqlField f = rec->field(fn);
            filter += toString(prefix, &f, fieldSep);
            separator = true;
        }
    }
    return filter;
}

/*!
    Inserts the current contents of the cursor's edit record buffer
    into the database, if the cursor allows inserts. Returns the
    number of rows affected by the insert. For error information, use
    lastError().

    If \a invalidate is true (the default), the cursor will no longer
    be positioned on a valid record and can no longer be navigated. A
    new select() call must be made before navigating to a valid
    record.

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 6

    In the above example, a cursor is created on the 'prices' table
    and a pointer to the insert buffer is acquired using primeInsert().
    Each field's value is set to the desired value and then insert()
    is called to insert the data into the database. Remember: all edit
    operations (insert(), update() and delete()) operate on the
    contents of the cursor edit buffer and not on the contents of the
    cursor itself.

    \sa setMode() lastError()
*/

int Q3SqlCursor::insert(bool invalidate)
{
    if ((d->md & Insert) != Insert || !driver())
        return false;
    int k = d->editBuffer.count();
    if (k == 0)
        return 0;

    QString fList;
    QString vList;
    bool comma = false;
    // use a prepared query if the driver supports it
    if (driver()->hasFeature(QSqlDriver::PreparedQueries)) {
        int cnt = 0;
        bool oraStyle = driver()->hasFeature(QSqlDriver::NamedPlaceholders);
        for(int j = 0; j < k; ++j) {
            QSqlField f = d->editBuffer.field(j);
            if (d->editBuffer.isGenerated(j)) {
                if (comma) {
                    fList += QLatin1Char(',');
                    vList += QLatin1Char(',');
                }
                fList += driver()->escapeIdentifier(f.name(), QSqlDriver::FieldName);
                vList += (oraStyle == true) ? QLatin1String(":f") + QString::number(cnt) : QString(QLatin1Char('?'));
                cnt++;
                comma = true;
            }
        }
        if (!comma) {
            return 0;
        }
        QString str;
        str.append(QLatin1String("insert into ")).append(name())
           .append(QLatin1String(" (")).append(fList)
           .append(QLatin1String(") values (")).append(vList). append(QLatin1Char(')'));

        return applyPrepared(str, invalidate);
    } else {
        for(int j = 0; j < k; ++j) {
            QSqlField f = d->editBuffer.field(j);
            if (d->editBuffer.isGenerated(j)) {
                if (comma) {
                    fList += QLatin1Char(',');
                    vList += QLatin1Char(',');
                }
                fList += driver()->escapeIdentifier(f.name(), QSqlDriver::FieldName);
                vList += driver()->formatValue(&f);
                comma = true;
            }
        }

        if (!comma) {
            // no valid fields found
            return 0;
        }
        QString str;
        str.append(QLatin1String("insert into ")).append(name()).append(QLatin1String(" ("))
           .append(fList).append(QLatin1String(") values (")).append(vList). append (QLatin1String(")"));
        return apply(str, invalidate);
    }
}

/*!
    Returns the current internal edit buffer. If \a copy is true (the
    default is false), the current cursor field values are first
    copied into the edit buffer. The edit buffer is valid as long as
    the cursor remains valid. The cursor retains ownership of the
    returned pointer, so it must not be deleted or modified.

    \sa primeInsert(), primeUpdate() primeDelete()
*/

QSqlRecord* Q3SqlCursor::editBuffer(bool copy)
{
    sync();
    if (copy) {
        for(int i = 0; i < d->editBuffer.count(); i++) {
            if (QSqlRecord::isNull(i)) {
                d->editBuffer.setNull(i);
            } else {
                d->editBuffer.setValue(i, value(i));
            }
        }
    }
    return &d->editBuffer;
}

/*!
    This function primes the edit buffer's field values for update and
    returns the edit buffer. The default implementation copies the
    field values from the current cursor record into the edit buffer
    (therefore, this function is equivalent to calling editBuffer(
    true)). The cursor retains ownership of the returned pointer, so
    it must not be deleted or modified.

    \sa editBuffer() update()
*/

QSqlRecord* Q3SqlCursor::primeUpdate()
{
    // memorize the primary keys as they were before the user changed the values in editBuffer
    QSqlRecord* buf = editBuffer(true);
    QSqlIndex idx = primaryIndex(false);
    if (!idx.isEmpty())
        d->editIndex = toString(idx, buf, d->nm, QString(QLatin1Char('=')), QLatin1String("and"));
    else
        d->editIndex = qWhereClause(buf, d->nm, QLatin1String("and"), driver());
    return buf;
}

/*!
    This function primes the edit buffer's field values for delete and
    returns the edit buffer. The default implementation copies the
    field values from the current cursor record into the edit buffer
    (therefore, this function is equivalent to calling editBuffer(
    true)). The cursor retains ownership of the returned pointer, so
    it must not be deleted or modified.

    \sa editBuffer() del()
*/

QSqlRecord* Q3SqlCursor::primeDelete()
{
    return editBuffer(true);
}

/*!
    This function primes the edit buffer's field values for insert and
    returns the edit buffer. The default implementation clears all
    field values in the edit buffer. The cursor retains ownership of
    the returned pointer, so it must not be deleted or modified.

    \sa editBuffer() insert()
*/

QSqlRecord* Q3SqlCursor::primeInsert()
{
    d->editBuffer.clearValues();
    return &d->editBuffer;
}


/*!
    Updates the database with the current contents of the edit buffer.
    Returns the number of records which were updated.
    For error information, use lastError().

    Only records which meet the filter criteria specified by the
    cursor's primary index are updated. If the cursor does not contain
    a primary index, no update is performed and 0 is returned.

    If \a invalidate is true (the default), the current cursor can no
    longer be navigated. A new select() call must be made before you
    can move to a valid record. For example:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 7

    In the above example, a cursor is created on the 'prices' table
    and is positioned on the record to be updated. Then a pointer to
    the cursor's edit buffer is acquired using primeUpdate(). A new
    value is calculated and placed into the edit buffer with the
    setValue() call. Finally, an update() call is made on the cursor
    which uses the tables's primary index to update the record in the
    database with the contents of the cursor's edit buffer. Remember:
    all edit operations (insert(), update() and delete()) operate on
    the contents of the cursor edit buffer and not on the contents of
    the cursor itself.

    Note that if the primary index does not uniquely distinguish
    records the database may be changed into an inconsistent state.

    \sa setMode() lastError()
*/

int Q3SqlCursor::update(bool invalidate)
{
    if (d->editIndex.isEmpty())
        return 0;
    return update(d->editIndex, invalidate);
}

/*!
    \overload

    Updates the database with the current contents of the cursor edit
    buffer using the specified \a filter. Returns the number of
    records which were updated.
    For error information, use lastError().

    Only records which meet the filter criteria are updated, otherwise
    all records in the table are updated.

    If \a invalidate is true (the default), the cursor can no longer
    be navigated. A new select() call must be made before you can move
    to a valid record.

    \sa primeUpdate() setMode() lastError()
*/

int Q3SqlCursor::update(const QString & filter, bool invalidate)
{
    if ((d->md & Update) != Update) {
        return false;
    }
    int k = count();
    if (k == 0) {
        return 0;
    }
    
    // use a prepared query if the driver supports it
    if (driver()->hasFeature(QSqlDriver::PreparedQueries)) {
        QString fList;
        bool comma = false;
        int cnt = 0;
        bool oraStyle = driver()->hasFeature(QSqlDriver::NamedPlaceholders);
        for(int j = 0; j < k; ++j) {
            QSqlField f = d->editBuffer.field(j);
            if (d->editBuffer.isGenerated(j)) {
                if (comma) {
                    fList += QLatin1Char(',');
                }
                fList += f.name() + QLatin1String(" = ") + (oraStyle == true ? QLatin1String(":f") + QString::number(cnt) : QString(QLatin1Char('?')));
                cnt++;
                comma = true;
            }
        }
        if (!comma) {
            return 0;
        }
        QString str(QLatin1String("update ") + name() + QLatin1String(" set ") + fList);
        if (filter.length()) {
            str+= QLatin1String(" where ") + filter;
        }
        return applyPrepared(str, invalidate);
    } else {
        QString str = QLatin1String("update ") + name();
        str += QLatin1String(" set ") + toString(&d->editBuffer, QString(), QString(QLatin1Char('=')), QString(QLatin1Char(',')));
        if (filter.length()) {
            str+= QLatin1String(" where ") + filter;
        }
        return apply(str, invalidate);
    }
}

/*!
    Deletes a record from the database using the cursor's primary
    index and the contents of the cursor edit buffer. Returns the
    number of records which were deleted.
    For error information, use lastError().

    Only records which meet the filter criteria specified by the
    cursor's primary index are deleted. If the cursor does not contain
    a primary index, no delete is performed and 0 is returned. If \a
    invalidate is true (the default), the current cursor can no longer
    be navigated. A new select() call must be made before you can move
    to a valid record. For example:

    \snippet doc/src/snippets/code/src_qt3support_sql_q3sqlcursor.cpp 8

    In the above example, a cursor is created on the 'prices' table
    and positioned to the record to be deleted. First primeDelete() is
    called to populate the edit buffer with the current cursor values,
    e.g. with an id of 999, and then del() is called to actually
    delete the record from the database. Remember: all edit operations
    (insert(), update() and delete()) operate on the contents of the
    cursor edit buffer and not on the contents of the cursor itself.

    \sa primeDelete() setMode() lastError()
*/

int Q3SqlCursor::del(bool invalidate)
{
    QSqlIndex idx = primaryIndex(false);
    if (idx.isEmpty())
        return del(qWhereClause(&d->editBuffer, d->nm, QLatin1String("and"), driver()), invalidate);
    return del(toString(primaryIndex(), &d->editBuffer, d->nm, QString(QLatin1Char('=')), QLatin1String("and")), invalidate);
}

/*!
    \overload

    Deletes the current cursor record from the database using the
    filter \a filter. Only records which meet the filter criteria are
    deleted. Returns the number of records which were deleted. If \a
    invalidate is true (the default), the current cursor can no longer
    be navigated. A new select() call must be made before you can move
    to a valid record. For error information, use lastError().

    The \a filter is an SQL \c WHERE clause, e.g. \c{id=500}.

    \sa setMode() lastError()
*/

int Q3SqlCursor::del(const QString & filter, bool invalidate)
{
    if ((d->md & Delete) != Delete)
        return 0;
    int k = count();
    if(k == 0) return 0;
    QString str = QLatin1String("delete from ") + name();
    if (filter.length())
        str+= QLatin1String(" where ") + filter;
    return apply(str, invalidate);
}

/*
  \internal
*/

int Q3SqlCursor::apply(const QString& q, bool invalidate)
{
    int ar = 0;
    if (invalidate) {
        if (exec(q))
            ar = numRowsAffected();
    } else if (driver()) {
        QSqlQuery* sql = d->query();
        if (sql && sql->exec(q))
            ar = sql->numRowsAffected();
    }
    return ar;
}

/*
  \internal
*/

int Q3SqlCursor::applyPrepared(const QString& q, bool invalidate)
{
    int ar = 0;
    QSqlQuery* sql = 0;

    if (invalidate) {
        sql = (QSqlQuery*)this;
        d->lastAt = QSql::BeforeFirst;
    } else {
        sql = d->query();
    }
    if (!sql)
        return 0;

    if (invalidate || sql->lastQuery() != q) {
        if (!sql->prepare(q))
            return 0;
    }

    int cnt = 0;
    int fieldCount = (int)count();
    for (int j = 0; j < fieldCount; ++j) {
        const QSqlField f = d->editBuffer.field(j);
        if (d->editBuffer.isGenerated(j)) {
            if (f.type() == QVariant::ByteArray)
                sql->bindValue(cnt, f.value(), QSql::In | QSql::Binary);
            else
                sql->bindValue(cnt, f.value());
            cnt++;
        }
    }
    if (sql->exec()) {
        ar = sql->numRowsAffected();
    }
    return ar;
}

/*!
  Executes the SQL query \a sql. Returns true of the cursor is
  active, otherwise returns false.
*/
bool Q3SqlCursor::exec(const QString & sql)
{
    d->lastAt = QSql::BeforeFirst;
    QSqlQuery::exec(sql);
    return isActive();
}

/*!
    Protected virtual function which is called whenever a field needs
    to be calculated. If calculated fields are being used, derived
    classes must reimplement this function and return the appropriate
    value for field \a name. The default implementation returns an
    invalid QVariant.

    \sa setCalculated()
*/

QVariant Q3SqlCursor::calculateField(const QString&)
{
    return QVariant();
}

/*! \internal
   Ensure fieldlist is synced with query.

*/

static QString qTrim(const QString& s)
{
    QString result = s;
    int end = result.length() - 1;
    while (end >= 0 && result[end].isSpace()) // skip white space from end
        end--;
    result.truncate(end + 1);
    return result;
}

/*! \internal
 */

void Q3SqlCursor::sync()
{
    if (isActive() && isValid() && d->lastAt != at()) {
        d->lastAt = at();
        int i = 0;
        int j = 0;
        bool haveCalculatedFields = false;
        for (; i < count(); ++i) {
            if (!haveCalculatedFields && d->infoBuffer[i].isCalculated()) {
                haveCalculatedFields = true;
            }
            if (QSqlRecord::isGenerated(i)) {
                QVariant v = QSqlQuery::value(j);
                if ((v.type() == QVariant::String) &&
                        d->infoBuffer[i].isTrim()) {
                    v = qTrim(v.toString());
                }
                QSqlRecord::setValue(i, v);
                if (QSqlQuery::isNull(j))
                    QSqlRecord::field(i).clear();
                j++;
            }
        }
        if (haveCalculatedFields) {
            for (i = 0; i < count(); ++i) {
                if (d->infoBuffer[i].isCalculated())
                    QSqlRecord::setValue(i, calculateField(fieldName(i)));
            }
        }
    }
}

/*!
    Returns the value of field number \a i.
*/

QVariant Q3SqlCursor::value(int i) const
{
    const_cast<Q3SqlCursor *>(this)->sync();
    return QSqlRecord::value(i);
}

/*! \internal
  cursors should be filled with Q3SqlFieldInfos...
*/
void Q3SqlCursor::append(const QSqlField& field)
{
    append(Q3SqlFieldInfo(field));
}

/*!
    Returns true if the field \a i is NULL or if there is no field at
    position \a i; otherwise returns false.

    This is the same as calling QSqlRecord::isNull(\a i)
*/
bool Q3SqlCursor::isNull(int i) const
{
    const_cast<Q3SqlCursor *>(this)->sync();
    return QSqlRecord::isNull(i);
}
/*!
    \overload

    Returns true if the field called \a name is NULL or if there is no
    field called \a name; otherwise returns false.

    This is the same as calling QSqlRecord::isNull(\a name)
*/
bool Q3SqlCursor::isNull(const QString& name) const
{
    const_cast<Q3SqlCursor *>(this)->sync();
    return QSqlRecord::isNull(name);
}

/*! \internal */
void Q3SqlCursor::setValue(int i, const QVariant& val)
{
    sync();
#ifdef QT_DEBUG
    qDebug("Q3SqlCursor::setValue(): This will not affect actual database values. Use primeInsert(), primeUpdate() or primeDelete().");
#endif
    QSqlRecord::setValue(i, val);
}

/*! \internal */
bool Q3SqlCursor::seek(int i, bool relative)
{
    bool res = QSqlQuery::seek(i, relative);
    sync();
    return res;
}

/*! \internal */
bool Q3SqlCursor::next()
{
    bool res = QSqlQuery::next();
    sync();
    return res;
}

/*!
    \fn Q3SqlCursor::previous()

    \internal
*/

/*! \internal */
bool Q3SqlCursor::prev()
{
    bool res = QSqlQuery::previous();
    sync();
    return res;
}

/*! \internal */
bool Q3SqlCursor::first()
{
    bool res = QSqlQuery::first();
    sync();
    return res;
}

/*! \internal */
bool Q3SqlCursor::last()
{
    bool res = QSqlQuery::last();
    sync();
    return res;
}

QT_END_NAMESPACE

#endif
