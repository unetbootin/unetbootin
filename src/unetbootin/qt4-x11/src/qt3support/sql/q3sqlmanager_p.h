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

#ifndef Q3SQLMANAGER_P_H
#define Q3SQLMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qvariant.h"
#include "QtCore/qglobal.h"
#include "QtCore/qstring.h"
#include "QtCore/qstringlist.h"
#include "QtSql/qsql.h"
#include "QtSql/qsqlerror.h"
#include "QtSql/qsqlindex.h"
#include "Qt3Support/q3sqlcursor.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SQL

class Q3SqlCursor;
class Q3SqlForm;
class Q3SqlCursorManagerPrivate;

class Q_COMPAT_EXPORT Q3SqlCursorManager
{
public:
    Q3SqlCursorManager();
    virtual ~Q3SqlCursorManager();

    virtual void setSort(const QSqlIndex& sort);
    virtual void setSort(const QStringList& sort);
    QStringList  sort() const;
    virtual void setFilter(const QString& filter);
    QString filter() const;
    virtual void setCursor(Q3SqlCursor* cursor, bool autoDelete = false);
    Q3SqlCursor* cursor() const;

    virtual void setAutoDelete(bool enable);
    bool autoDelete() const;

    virtual bool refresh();
    virtual bool findBuffer(const QSqlIndex& idx, int atHint = 0);

private:
    Q3SqlCursorManagerPrivate* d;
};

#ifndef QT_NO_SQL_FORM

class Q3SqlFormManagerPrivate;

class Q_COMPAT_EXPORT Q3SqlFormManager
{
public:
    Q3SqlFormManager();
    virtual ~Q3SqlFormManager();

    virtual void setForm(Q3SqlForm* form);
    Q3SqlForm* form();
    virtual void setRecord(QSqlRecord* record);
    QSqlRecord* record();

    virtual void clearValues();
    virtual void readFields();
    virtual void writeFields();

private:
    Q3SqlFormManagerPrivate* d;
};

#endif

class QWidget;
class Q3DataManagerPrivate;

class Q_COMPAT_EXPORT Q3DataManager
{
public:
    Q3DataManager();
    virtual ~Q3DataManager();

    virtual void setMode(QSql::Op m);
    QSql::Op mode() const;
    virtual void setAutoEdit(bool autoEdit);
    bool autoEdit() const;

    virtual void handleError(QWidget* parent, const QSqlError& error);
    virtual QSql::Confirm confirmEdit(QWidget* parent, QSql::Op m);
    virtual QSql::Confirm confirmCancel(QWidget* parent, QSql::Op m);

    virtual void setConfirmEdits(bool confirm);
    virtual void setConfirmInsert(bool confirm);
    virtual void setConfirmUpdate(bool confirm);
    virtual void setConfirmDelete(bool confirm);
    virtual void setConfirmCancels(bool confirm);

    bool confirmEdits() const;
    bool confirmInsert() const;
    bool confirmUpdate() const;
    bool confirmDelete() const;
    bool confirmCancels() const;

private:
    Q3DataManagerPrivate* d;
};

#endif // QT_NO_SQL

QT_END_NAMESPACE

#endif // Q3SQLMANAGER_P_H
