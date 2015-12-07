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

#ifndef QSQL_ODBC_H
#define QSQL_ODBC_H

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlresult.h>

#if defined (Q_OS_WIN32)
#include <QtCore/qt_windows.h>
#endif

#if defined (Q_OS_MAC) && (MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_3)
// assume we use iodbc on MACX
// comment next line out if you use a
// unicode compatible manager
# define Q_ODBC_VERSION_2
#endif

#ifdef QT_PLUGIN
#define Q_EXPORT_SQLDRIVER_ODBC
#else
#define Q_EXPORT_SQLDRIVER_ODBC Q_SQL_EXPORT
#endif

#ifdef Q_OS_UNIX
#define HAVE_LONG_LONG 1 // force UnixODBC NOT to fall back to a struct for BIGINTs
#endif

#if defined(Q_CC_BOR)
// workaround for Borland to make sure that SQLBIGINT is defined
#  define _MSC_VER 900
#endif
#include <sql.h>
#if defined(Q_CC_BOR)
#  undef _MSC_VER
#endif

#ifndef Q_ODBC_VERSION_2
#include <sqlucode.h>
#endif

#include <sqlext.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QODBCPrivate;
class QODBCDriverPrivate;
class QODBCDriver;
class QSqlRecordInfo;

class QODBCResult : public QSqlResult
{
public:
    QODBCResult(const QODBCDriver * db, QODBCDriverPrivate* p);
    virtual ~QODBCResult();

    bool prepare(const QString& query);
    bool exec();

    QVariant handle() const;

protected:
    bool fetchNext();
    bool fetchFirst();
    bool fetchLast();
    bool fetchPrevious();
    bool fetch(int i);
    bool reset (const QString& query);
    QVariant data(int field);
    bool isNull(int field);
    int size();
    int numRowsAffected();
    QSqlRecord record() const;
    void virtual_hook(int id, void *data);
    bool nextResult();

private:
    QODBCPrivate *d;
};

class Q_EXPORT_SQLDRIVER_ODBC QODBCDriver : public QSqlDriver
{
    Q_OBJECT
public:
    explicit QODBCDriver(QObject *parent=0);
    QODBCDriver(SQLHANDLE env, SQLHANDLE con, QObject * parent=0);
    virtual ~QODBCDriver();
    bool hasFeature(DriverFeature f) const;
    void close();
    QSqlResult *createResult() const;
    QStringList tables(QSql::TableType) const;
    QSqlRecord record(const QString& tablename) const;
    QSqlIndex primaryIndex(const QString& tablename) const;
    QVariant handle() const;
    QString formatValue(const QSqlField &field,
                        bool trimStrings) const;
    bool open(const QString& db,
              const QString& user,
              const QString& password,
              const QString& host,
              int port,
              const QString& connOpts);

    QString escapeIdentifier(const QString &identifier, IdentifierType type) const;

protected:
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
private:
    void init();
    bool endTrans();
    void cleanup();
    QODBCDriverPrivate* d;
    friend class QODBCPrivate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQL_ODBC_H
