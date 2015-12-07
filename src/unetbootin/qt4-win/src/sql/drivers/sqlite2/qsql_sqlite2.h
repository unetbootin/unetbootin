/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQL_SQLITE2_H
#define QSQL_SQLITE2_H

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlresult.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlindex.h>
#include <QtSql/private/qsqlcachedresult_p.h>

#if defined (Q_OS_WIN32)
# include <QtCore/qt_windows.h>
#endif

struct sqlite;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSQLite2DriverPrivate;
class QSQLite2ResultPrivate;
class QSQLite2Driver;

class QSQLite2Result : public QSqlCachedResult
{
    friend class QSQLite2Driver;
    friend class QSQLite2ResultPrivate;
public:
    explicit QSQLite2Result(const QSQLite2Driver* db);
    ~QSQLite2Result();
    QVariant handle() const;

protected:
    bool gotoNext(QSqlCachedResult::ValueCache& row, int idx);
    bool reset (const QString& query);
    int size();
    int numRowsAffected();
    QSqlRecord record() const;
    void virtual_hook(int id, void *data);

private:
    QSQLite2ResultPrivate* d;
};

class QSQLite2Driver : public QSqlDriver
{
    Q_OBJECT
    friend class QSQLite2Result;
public:
    explicit QSQLite2Driver(QObject *parent = 0);
    explicit QSQLite2Driver(sqlite *connection, QObject *parent = 0);
    ~QSQLite2Driver();
    bool hasFeature(DriverFeature f) const;
    bool open(const QString & db,
                   const QString & user,
                   const QString & password,
                   const QString & host,
                   int port,
                   const QString & connOpts);
    bool open(const QString & db,
            const QString & user,
            const QString & password,
            const QString & host,
            int port) { return open (db, user, password, host, port, QString()); }
    void close();
    QSqlResult *createResult() const;
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    QStringList tables(QSql::TableType) const;

    QSqlRecord record(const QString& tablename) const;
    QSqlIndex primaryIndex(const QString &table) const;
    QVariant handle() const;

private:
    QSQLite2DriverPrivate* d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQL_SQLITE2_H
