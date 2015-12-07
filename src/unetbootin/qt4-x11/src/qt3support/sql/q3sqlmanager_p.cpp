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

#include "q3sqlmanager_p.h"

#ifndef QT_NO_SQL

#include "qapplication.h"
#include "qcursor.h"
#include "qwidget.h"
#include "q3sqlcursor.h"
#include "qsqlfield.h"
#include "q3sqlform.h"
#include "qsqldriver.h"
#include "qstring.h"
#include "qmessagebox.h"
#include "qbitarray.h"

QT_BEGIN_NAMESPACE

//#define QT_DEBUG_DATAMANAGER

class Q3SqlCursorManagerPrivate
{
public:
    Q3SqlCursorManagerPrivate()
        : cur(0), autoDelete(false)
    {}

    QString ftr;
    QStringList srt;
    Q3SqlCursor* cur;
    bool autoDelete;
};

static QSqlIndex indexFromStringList(const QStringList& l, const Q3SqlCursor* cursor)
{
    QSqlIndex newSort;
    for (int i = 0; i < l.count(); ++i) {
        QString f = l[i];
        bool desc = false;
        if (f.mid(f.length()-3) == QLatin1String("ASC"))
            f = f.mid(0, f.length()-3);
        if (f.mid(f.length()-4) == QLatin1String("DESC")) {
            desc = true;
            f = f.mid(0, f.length()-4);
        }
        int dot = f.lastIndexOf(QLatin1Char('.'));
        if (dot != -1)
            f = f.mid(dot+1);
        const QSqlField field = cursor->field(f.trimmed());
        if (field.isValid())
            newSort.append(field, desc);
        else
            qWarning("QSqlIndex::indexFromStringList: unknown field: '%s'", f.latin1());
    }
    return newSort;
}


/*!
  \class Q3SqlCursorManager qsqlmanager_p.h
  \brief The Q3SqlCursorManager class manages a database cursor.

  \compat
  \internal

  This class provides common cursor management functionality.  This
  includes saving and applying sorts and filters, refreshing (i.e.,
  re-selecting) the cursor and searching for records within the
  cursor.

*/

/*!  \internal

  Constructs a cursor manager.

*/

Q3SqlCursorManager::Q3SqlCursorManager()
{
    d = new Q3SqlCursorManagerPrivate;
}


/*! \internal

  Destroys the object and frees any allocated resources.

*/

Q3SqlCursorManager::~Q3SqlCursorManager()
{
    if (d->autoDelete)
        delete d->cur;
    delete d;
}

/*! \internal

  Sets the manager's sort to the index \a sort.  To apply the new
  sort, use refresh().

 */

void Q3SqlCursorManager::setSort(const QSqlIndex& sort)
{
    setSort(sort.toStringList());
}

/*! \internal

  Sets the manager's sort to the stringlist \a sort.  To apply the
  new sort, use refresh().

 */

void Q3SqlCursorManager::setSort(const QStringList& sort)
{
    d->srt = sort;
}

/*! \internal

  Returns the current sort, or an empty stringlist if there is none.

*/

QStringList  Q3SqlCursorManager::sort() const
{
    return d->srt;
}

/*! \internal

  Sets the manager's filter to the string \a filter.  To apply the
  new filter, use refresh().

*/

void Q3SqlCursorManager::setFilter(const QString& filter)
{
    d->ftr = filter;
}

/*! \internal

  Returns the current filter, or an empty string if there is none.

*/

QString Q3SqlCursorManager::filter() const
{
    return d->ftr;
}

/*! \internal

  Sets auto-delete to \a enable.  If true, the default cursor will
  be deleted when necessary.

  \sa autoDelete()
*/

void Q3SqlCursorManager::setAutoDelete(bool enable)
{
    d->autoDelete = enable;
}


/*! \internal

  Returns true if auto-deletion is enabled, otherwise false.

  \sa setAutoDelete()

*/

bool Q3SqlCursorManager::autoDelete() const
{
    return d->autoDelete;
}

/*! \internal

  Sets the default cursor used by the manager to \a cursor.  If \a
  autoDelete is true (the default is false), the manager takes
  ownership of the \a cursor pointer, which will be deleted when the
  manager is destroyed, or when setCursor() is called again. To
  activate the \a cursor use refresh().

  \sa cursor()

*/

void Q3SqlCursorManager::setCursor(Q3SqlCursor* cursor, bool autoDelete)
{
    if (d->autoDelete)
        delete d->cur;
    d->cur = cursor;
    d->autoDelete = autoDelete;
}

/*! \internal

  Returns a pointer to the default cursor used for navigation, or 0
  if there is no default cursor.

  \sa setCursor()

*/

Q3SqlCursor* Q3SqlCursorManager::cursor() const
{
    return d->cur;
}


/*! \internal

  Refreshes the manager using the default cursor.  The manager's
  filter and sort are applied.  Returns true on success, false if an
  error occurred or there is no current cursor.

  \sa setFilter() setSort()

*/

bool Q3SqlCursorManager::refresh()
{
    Q3SqlCursor* cur = cursor();
    if (!cur)
        return false;
    QString currentFilter = d->ftr;
    QStringList currentSort = d->srt;
    QSqlIndex newSort = indexFromStringList(currentSort, cur);
    return cur->select(currentFilter, newSort);
}

/* \internal

   Returns true if the \a buf field values that correspond to \a idx
   match the field values in \a cur that correspond to \a idx.
*/

static bool index_matches(const Q3SqlCursor* cur, const QSqlRecord* buf,
                           const QSqlIndex& idx)
{
    bool indexEquals = false;
    for (int i = 0; i < idx.count(); ++i) {
        const QString fn(idx.field(i).name());
        if (cur->value(fn) == buf->value(fn))
            indexEquals = true;
        else {
            indexEquals = false;
            break;
        }
    }
    return indexEquals;
}

/*
  Return less than, equal to or greater than 0 if buf1 is less than,
  equal to or greater than buf2 according to fields described in idx.
  (### Currently only uses first field.)
*/

static int compare_recs(const QSqlRecord* buf1, const QSqlRecord* buf2,
                         const QSqlIndex& idx)
{
    int cmp = 0;

    int i = 0;
    const QString fn(idx.field(i).name());
    const QSqlField f1 = buf1->field(fn);

    if (f1.isValid()) {
        switch (f1.type()) { // ### more types?
        case QVariant::String:
            cmp = f1.value().toString().trimmed().compare(
                          buf2->value(fn).toString().trimmed());
            break;
        default:
            if (f1.value().toDouble() < buf2->value(fn).toDouble())
                cmp = -1;
            else if (f1.value().toDouble() > buf2->value(fn).toDouble())
                cmp = 1;
        }
    }

    if (idx.isDescending(i))
        cmp = -cmp;
    return cmp;
}

#ifdef QT_DEBUG_DATAMANAGER
static void debug_datamanager_buffer(const QString& msg, QSqlRecord* cursor)
{
    qDebug("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    qDebug("%s", msg.latin1());
    for (int j = 0; j < cursor->count(); ++j) {
        qDebug("%s", (cursor->field(j)->name() + " type:"
                       + QString(cursor->field(j)->value().typeName())
                       + " value:" + cursor->field(j)->value().toString())
                       .latin1());
    }
}
#endif


/*! \internal

  Relocates the default cursor to the record matching the cursor's
edit buffer.  Only the field names specified by \a idx are used to
determine an exact match of the cursor to the edit buffer. However,
other fields in the edit buffer are also used during the search,
therefore all fields in the edit buffer should be primed with desired
values for the record being sought.  This function is typically used
to relocate a cursor to the correct position after an insert or
update.  For example:

\snippet doc/src/snippets/code/src_qt3support_sql_q3sqlmanager_p.cpp 0

*/

//## possibly add sizeHint parameter
bool Q3SqlCursorManager::findBuffer(const QSqlIndex& idx, int atHint)
{
#ifdef QT_DEBUG_DATAMANAGER
    qDebug("Q3SqlCursorManager::findBuffer:");
#endif
    Q3SqlCursor* cur = cursor();
    if (!cur)
        return false;
    if (!cur->isActive())
        return false;
    if (!idx.count()) {
        if (cur->at() == QSql::BeforeFirst)
            cur->next();
        return false;
    }
    QSqlRecord* buf = cur->editBuffer();
    bool indexEquals = false;
#ifdef QT_DEBUG_DATAMANAGER
    qDebug(" Checking hint...");
#endif
    /* check the hint */
    if (cur->seek(atHint))
        indexEquals = index_matches(cur, buf, idx);

    if (!indexEquals) {
#ifdef QT_DEBUG_DATAMANAGER
        qDebug(" Checking current page...");
#endif
        /* check current page */
        int pageSize = 20;
        int startIdx = qMax(atHint - pageSize, 0);
        int endIdx = atHint + pageSize;
        for (int j = startIdx; j <= endIdx; ++j) {
            if (cur->seek(j)) {
                indexEquals = index_matches(cur, buf, idx);
                if (indexEquals)
                    break;
            }
        }
    }

    if (!indexEquals && cur->driver()->hasFeature(QSqlDriver::QuerySize)
         && cur->sort().count()) {
#ifdef QT_DEBUG_DATAMANAGER
        qDebug(" Using binary search...");
#endif
        // binary search based on record buffer and current sort fields
        int lo = 0;
        int hi = cur->size();
        int mid;
        if (compare_recs(buf, cur, cur->sort()) >= 0)
            lo = cur->at();
        while (lo != hi) {
            mid = lo + (hi - lo) / 2;
            if (!cur->seek(mid))
                break;
            if (index_matches(cur, buf, idx)) {
                indexEquals = true;
                break;
            }
            int c = compare_recs(buf, cur, cur->sort());
            if (c < 0) {
                hi = mid;
            } else if (c == 0) {
                // found it, but there may be duplicates
                int at = mid;
                do {
                    mid--;
                    if (!cur->seek(mid))
                        break;
                    if (index_matches(cur, buf, idx)) {
                        indexEquals = true;
                        break;
                    }
                } while (compare_recs(buf, cur, cur->sort()) == 0);

                if (!indexEquals) {
                    mid = at;
                    do {
                        mid++;
                        if (!cur->seek(mid))
                            break;
                        if (index_matches(cur, buf, idx)) {
                            indexEquals = true;
                            break;
                        }
                    } while (compare_recs(buf, cur, cur->sort()) == 0);
                }
                break;
            } else if (c > 0) {
                lo = mid + 1;
            }
        }
    }

    if (!indexEquals) {
#ifdef QT_DEBUG_DATAMANAGER
        qDebug(" Using brute search...");
#endif
#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        /* give up, use brute force */
        int startIdx = 0;
        if (cur->at() != startIdx) {
            cur->seek(startIdx);
        }
        for (;;) {
            indexEquals = false;
            indexEquals = index_matches(cur, buf, idx);
            if (indexEquals)
                break;
            if (!cur->next())
                break;
        }
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
    }
#ifdef QT_DEBUG_DATAMANAGER
        qDebug(" Done, result:" + QString::number(indexEquals));
#endif
    return indexEquals;
}

#ifndef QT_NO_SQL_FORM

class Q3SqlFormManagerPrivate
{
public:
    Q3SqlFormManagerPrivate() : frm(0), rcd(0) {}
    Q3SqlForm* frm;
    QSqlRecord* rcd;
};


/*! \internal

  Creates a form manager.

*/

Q3SqlFormManager::Q3SqlFormManager()
{
    d = new Q3SqlFormManagerPrivate();
}

/*! \internal

  Destroys the object and frees any allocated resources.

*/

Q3SqlFormManager::~Q3SqlFormManager()
{
    delete d;
}

/*!  \internal

  Clears the default form values.  If there is no default form,
  nothing happens,

*/

void Q3SqlFormManager::clearValues()
{
    if (form())
        form()->clearValues();
}

/*! \internal

  Sets the form used by the form manager to \a form.  If a record has
  already been assigned to the form manager, that record is also used by
  the \a form to display data.

  \sa form()

*/

void Q3SqlFormManager::setForm(Q3SqlForm* form)
{
    d->frm = form;
    if (d->rcd && d->frm)
        d->frm->setRecord(d->rcd);
}


/*! \internal

  Returns the default form used by the form manager, or 0 if there is
  none.

  \sa setForm()

*/

Q3SqlForm* Q3SqlFormManager::form()
{
    return d->frm;
}


/*! \internal

  Sets the record used by the form manager to \a record.  If a form has
  already been assigned to the form manager, \a record is also used by
  the default form to display data.

  \sa record()

*/

void Q3SqlFormManager::setRecord(QSqlRecord* record)
{
    d->rcd = record;
    if (d->frm) {
        d->frm->setRecord(d->rcd);
    }
}


/*! \internal

  Returns the default record used by the form manager, or 0 if there is
  none.

  \sa setRecord()
*/

QSqlRecord* Q3SqlFormManager::record()
{
    return d->rcd;
}


/*! \internal

  Causes the default form to read its fields .  If there is no
  default form, nothing happens.

  \sa setForm()

*/

void Q3SqlFormManager::readFields()
{
    if (d->frm) {
        d->frm->readFields();
    }
}

/*! \internal

  Causes the default form to write its fields .  If there is no
  default form, nothing happens.

  \sa setForm()

*/

void Q3SqlFormManager::writeFields()
{
    if (d->frm) {
        d->frm->writeFields();
    }
}

#endif // QT_NO_SQL_FORM

class Q3DataManagerPrivate
{
public:
    Q3DataManagerPrivate()
        : mode(QSql::None), autoEd(true), confEdits(3),
          confCancs(false) {}
    QSql::Op mode;
    bool autoEd;
    QBitArray confEdits;
    bool confCancs;

};

/*!
  \class Q3DataManager qsqlmanager_p.h

  \brief The Q3DataManager class is an internal class for implementing
  the data-aware widgets.

  \internal
  \compat

  Q3DataManager is a strictly internal class that acts as a base class
  for other data-aware widgets.

*/


/*!  \internal

  Constructs an empty data handler.

*/

Q3DataManager::Q3DataManager()
{
    d = new Q3DataManagerPrivate();
}


/*! \internal

  Destroys the object and frees any allocated resources.

*/

Q3DataManager::~Q3DataManager()
{
    delete d;
}


/*!  \internal

  Virtual function which is called when an error has occurred The
  default implementation displays a warning message to the user with
  information about the error.

*/
void Q3DataManager::handleError(QWidget* parent, const QSqlError& e)
{
#ifndef QT_NO_MESSAGEBOX
    if (e.driverText().isEmpty() && e.databaseText().isEmpty()) {
        QMessageBox::warning (parent, QLatin1String("Warning"), QLatin1String("An error occurred while accessing the database"));
    } else {
        QMessageBox::warning (parent, QLatin1String("Warning"), e.driverText() + QLatin1Char('\n') + e.databaseText(),
                           0, 0);
    }
#endif // QT_NO_MESSAGEBOX
}


/*! \internal

  Sets the internal mode to \a m.

*/

void Q3DataManager::setMode(QSql::Op m)
{
    d->mode = m;
}


/*! \internal

  Returns the current mode.

*/

QSql::Op Q3DataManager::mode() const
{
    return d->mode;
}


/*! \internal

  Sets the auto-edit mode to \a auto.

*/

void Q3DataManager::setAutoEdit(bool autoEdit)
{
    d->autoEd = autoEdit;
}



/*! \internal

  Returns true if auto-edit mode is enabled; otherwise returns false.

*/

bool Q3DataManager::autoEdit() const
{
    return d->autoEd;
}

/*! \internal

  If \a confirm is true, all edit operations (inserts, updates and
  deletes) will be confirmed by the user.  If \a confirm is false (the
  default), all edits are posted to the database immediately.

*/
void Q3DataManager::setConfirmEdits(bool confirm)
{
    d->confEdits = QBitArray(d->confEdits.size(), confirm);
}

/*! \internal

  If \a confirm is true, all inserts will be confirmed by the user.
  If \a confirm is false (the default), all edits are posted to the
  database immediately.

*/

void Q3DataManager::setConfirmInsert(bool confirm)
{
    d->confEdits[QSql::Insert] = confirm;
}

/*! \internal

  If \a confirm is true, all updates will be confirmed by the user.
  If \a confirm is false (the default), all edits are posted to the
  database immediately.

*/

void Q3DataManager::setConfirmUpdate(bool confirm)
{
    d->confEdits[QSql::Update] = confirm;
}

/*! \internal

  If \a confirm is true, all deletes will be confirmed by the user.
  If \a confirm is false (the default), all edits are posted to the
  database immediately.

*/

void Q3DataManager::setConfirmDelete(bool confirm)
{
    d->confEdits[QSql::Delete] = confirm;
}

/*! \internal

  Returns true if the table confirms all edit operations (inserts,
  updates and deletes), otherwise returns false.
*/

bool Q3DataManager::confirmEdits() const
{
    return (confirmInsert() && confirmUpdate() && confirmDelete());
}

/*! \internal

  Returns true if the table confirms inserts, otherwise returns
  false.
*/

bool Q3DataManager::confirmInsert() const
{
    return d->confEdits[QSql::Insert];
}

/*! \internal

  Returns true if the table confirms updates, otherwise returns
  false.
*/

bool Q3DataManager::confirmUpdate() const
{
    return d->confEdits[QSql::Update];
}

/*! \internal

  Returns true if the table confirms deletes, otherwise returns
  false.
*/

bool Q3DataManager::confirmDelete() const
{
    return d->confEdits[QSql::Delete];
}

/*! \internal

  If \a confirm is true, all cancels will be confirmed by the user
  through a message box.  If \a confirm is false (the default), all
  cancels occur immediately.
*/

void Q3DataManager::setConfirmCancels(bool confirm)
{
    d->confCancs = confirm;
}

/*! \internal

  Returns true if the table confirms cancels, otherwise returns false.
*/

bool Q3DataManager::confirmCancels() const
{
    return d->confCancs;
}

/*!  \internal

  Virtual function which returns a confirmation for an edit of mode \a
  m.  Derived classes can reimplement this function and provide their
  own confirmation dialog.  The default implementation uses a message
  box which prompts the user to confirm the edit action.  The dialog
  is centered over \a parent.

*/

QSql::Confirm Q3DataManager::confirmEdit(QWidget* parent, QSql::Op m)
{
    int ans = 2;
    if (m == QSql::Delete) {
#ifndef QT_NO_MESSAGEBOX
        ans = QMessageBox::information(parent,
                                        qApp->translate("QSql", "Delete"),
                                        qApp->translate("QSql", "Delete this record?"),
                                        qApp->translate("QSql", "Yes"),
                                        qApp->translate("QSql", "No"),
                                        QString(), 0, 1);
#else
        ans = QSql::No;
#endif // QT_NO_MESSAGEBOX
    } else if (m != QSql::None) {
        QString caption;
        if (m == QSql::Insert) {
            caption = qApp->translate("QSql", "Insert");
        } else { // QSql::Update
            caption = qApp->translate("QSql", "Update");
        }
#ifndef QT_NO_MESSAGEBOX
        ans = QMessageBox::information(parent, caption,
                                        qApp->translate("QSql", "Save edits?"),
                                        qApp->translate("QSql", "Yes"),
                                        qApp->translate("QSql", "No"),
                                        qApp->translate("QSql", "Cancel"),
                                        0, 2);
#else
        ans = QSql::No;
#endif // QT_NO_MESSAGEBOX
    }

    switch (ans) {
    case 0:
        return QSql::Yes;
    case 1:
        return QSql::No;
    default:
        return QSql::Cancel;
    }
}

/*!  \internal

  Virtual function which returns a confirmation for canceling an edit
  mode \a m.  Derived classes can reimplement this function and
  provide their own confirmation dialog.  The default implementation
  uses a message box which prompts the user to confirm the edit
  action. The dialog is centered over \a parent.


*/

QSql::Confirm Q3DataManager::confirmCancel(QWidget* parent, QSql::Op)
{
#ifndef QT_NO_MESSAGEBOX
    switch (QMessageBox::information(parent,
                                       qApp->translate("QSql", "Confirm"),
                                       qApp->translate("QSql", "Cancel your edits?"),
                                       qApp->translate("QSql", "Yes"),
                                       qApp->translate("QSql", "No"),
                                       QString(), 0, 1)) {
    case 0:
        return QSql::Yes;
    case 1:
        return QSql::No;
    default:
        return QSql::Cancel;
    }
#else
    return QSql::Yes;
#endif // QT_NO_MESSAGEBOX
}

QT_END_NAMESPACE

#endif
