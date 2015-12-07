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
