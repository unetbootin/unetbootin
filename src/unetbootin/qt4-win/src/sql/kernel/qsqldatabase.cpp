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

#include "qsqldatabase.h"
#include "qsqlquery.h"

#ifdef Q_OS_WIN32
// Conflicting declarations of LPCBYTE in sqlfront.h and winscard.h
#define _WINSCARD_H_
#endif

#ifdef QT_SQL_PSQL
#include "../drivers/psql/qsql_psql.h"
#endif
#ifdef QT_SQL_MYSQL
#include "../drivers/mysql/qsql_mysql.h"
#endif
#ifdef QT_SQL_ODBC
#include "../drivers/odbc/qsql_odbc.h"
#endif
#ifdef QT_SQL_OCI
#include "../drivers/oci/qsql_oci.h"
#endif
#ifdef QT_SQL_TDS
#include "../drivers/tds/qsql_tds.h"
#endif
#ifdef QT_SQL_DB2
#include "../drivers/db2/qsql_db2.h"
#endif
#ifdef QT_SQL_SQLITE
#include "../drivers/sqlite/qsql_sqlite.h"
#endif
#ifdef QT_SQL_SQLITE2
#include "../drivers/sqlite2/qsql_sqlite2.h"
#endif
#ifdef QT_SQL_IBASE
#undef SQL_FLOAT  // avoid clash with ODBC
#undef SQL_DOUBLE
#undef SQL_TIMESTAMP
#undef SQL_TYPE_TIME
#undef SQL_TYPE_DATE
#undef SQL_DATE
#define SCHAR IBASE_SCHAR  // avoid clash with ODBC (older versions of ibase.h with Firebird)
#include "../drivers/ibase/qsql_ibase.h"
#undef SCHAR
#endif

#include "qdebug.h"
#include "qcoreapplication.h"
#include "qreadwritelock.h"
#include "qsqlresult.h"
#include "qsqldriver.h"
#include "qsqldriverplugin.h"
#include "qsqlindex.h"
#include "private/qfactoryloader_p.h"
#include "private/qsqlnulldriver_p.h"
#include "qmutex.h"
#include "qhash.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
                          (QSqlDriverFactoryInterface_iid,
                           QLatin1String("/sqldrivers")))
#endif

QT_STATIC_CONST_IMPL char *QSqlDatabase::defaultConnection = "qt_sql_default_connection";

typedef QHash<QString, QSqlDriverCreatorBase*> DriverDict;

class QConnectionDict: public QHash<QString, QSqlDatabase>
{
public:
    inline bool contains_ts(const QString &key)
    {
        QReadLocker locker(&lock);
        return contains(key);
    }
    inline QStringList keys_ts() const
    {
        QReadLocker locker(&lock);
        return keys();
    }

    mutable QReadWriteLock lock;
};
Q_GLOBAL_STATIC(QConnectionDict, dbDict)

class QSqlDatabasePrivate
{
public:
    QSqlDatabasePrivate(QSqlDriver *dr = 0):
        driver(dr),
        port(-1)
    {
        ref = 1;
    }
    QSqlDatabasePrivate(const QSqlDatabasePrivate &other);
    ~QSqlDatabasePrivate();
    void init(const QString& type);
    void copy(const QSqlDatabasePrivate *other);
    void disable();

    QAtomicInt ref;
    QSqlDriver* driver;
    QString dbname;
    QString uname;
    QString pword;
    QString hname;
    QString drvName;
    int port;
    QString connOptions;
    QString connName;

    static QSqlDatabasePrivate *shared_null();
    static QSqlDatabase database(const QString& name, bool open);
    static void addDatabase(const QSqlDatabase &db, const QString & name);
    static void removeDatabase(const QString& name);
    static void invalidateDb(const QSqlDatabase &db, const QString &name);
    static DriverDict &driverDict();
    static void cleanConnections();
};

QSqlDatabasePrivate::QSqlDatabasePrivate(const QSqlDatabasePrivate &other)
{
    ref = 1;
    dbname = other.dbname;
    uname = other.uname;
    pword = other.pword;
    hname = other.hname;
    drvName = other.drvName;
    port = other.port;
    connOptions = other.connOptions;
    driver = other.driver;
}

QSqlDatabasePrivate::~QSqlDatabasePrivate()
{
    if (driver != shared_null()->driver)
        delete driver;
}

void QSqlDatabasePrivate::cleanConnections()
{
    QConnectionDict *dict = dbDict();
    Q_ASSERT(dict);
    QWriteLocker locker(&dict->lock);

    QConnectionDict::iterator it = dict->begin();
    while (it != dict->end()) {
        invalidateDb(it.value(), it.key());
        ++it;
    }
    dict->clear();
}

static bool qDriverDictInit = false;
static void cleanDriverDict()
{
    qDeleteAll(QSqlDatabasePrivate::driverDict());
    QSqlDatabasePrivate::driverDict().clear();
    QSqlDatabasePrivate::cleanConnections();
    qDriverDictInit = false;
}

DriverDict &QSqlDatabasePrivate::driverDict()
{
    static DriverDict dict;
    if (!qDriverDictInit) {
        qDriverDictInit = true;
        qAddPostRoutine(cleanDriverDict);
    }
    return dict;
}

QSqlDatabasePrivate *QSqlDatabasePrivate::shared_null()
{
    static QSqlNullDriver dr;
    static QSqlDatabasePrivate n(&dr);
    return &n;
}

void QSqlDatabasePrivate::invalidateDb(const QSqlDatabase &db, const QString &name)
{
    if (db.d->ref != 1) {
        qWarning("QSqlDatabasePrivate::removeDatabase: connection '%s' is still in use, "
                 "all queries will cease to work.", name.toLocal8Bit().constData());
        db.d->disable();
        db.d->connName = QString();
    }
}

void QSqlDatabasePrivate::removeDatabase(const QString &name)
{
    QConnectionDict *dict = dbDict();
    Q_ASSERT(dict);
    QWriteLocker locker(&dict->lock);

    if (!dict->contains(name))
        return;

    invalidateDb(dict->take(name), name);
}

void QSqlDatabasePrivate::addDatabase(const QSqlDatabase &db, const QString &name)
{
    QConnectionDict *dict = dbDict();
    Q_ASSERT(dict);
    QWriteLocker locker(&dict->lock);

    if (dict->contains(name)) {
        invalidateDb(dict->take(name), name);
        qWarning("QSqlDatabasePrivate::addDatabase: duplicate connection name '%s', old "
                 "connection removed.", name.toLocal8Bit().data());
    }
    dict->insert(name, db);
    db.d->connName = name;
}

/*! \internal
*/
QSqlDatabase QSqlDatabasePrivate::database(const QString& name, bool open)
{
    const QConnectionDict *dict = dbDict();
    Q_ASSERT(dict);

    dict->lock.lockForRead();
    QSqlDatabase db = dict->value(name);
    dict->lock.unlock();
    if (db.isValid() && !db.isOpen() && open) {
        db.open();
        if (!db.isOpen())
            qWarning("QSqlDatabasePrivate::database: unable to open database: %s",
                     db.lastError().text().toLocal8Bit().data());

    }
    return db;
}


/*! \internal
    Copies the connection data from \a other.
*/
void QSqlDatabasePrivate::copy(const QSqlDatabasePrivate *other)
{
    dbname = other->dbname;
    uname = other->uname;
    pword = other->pword;
    hname = other->hname;
    drvName = other->drvName;
    port = other->port;
    connOptions = other->connOptions;
}

void QSqlDatabasePrivate::disable()
{
    if (driver != shared_null()->driver) {
        delete driver;
        driver = shared_null()->driver;
    }
}

/*!
    \class QSqlDriverCreatorBase
    \brief The QSqlDriverCreatorBase class is the base class for
    SQL driver factories.

    \ingroup database
    \module sql

    Reimplement createObject() to return an instance of the specific
    QSqlDriver subclass that you want to provide.

    See QSqlDatabase::registerSqlDriver() for details.

    \sa QSqlDriverCreator
*/

/*!
    \fn QSqlDriverCreatorBase::~QSqlDriverCreatorBase()

    Destroys the SQL driver creator object.
*/

/*!
    \fn QSqlDriver *QSqlDriverCreatorBase::createObject() const

    Reimplement this function to returns a new instance of a
    QSqlDriver subclass.
*/

/*!
    \class QSqlDriverCreator
    \brief The QSqlDriverCreator class is a template class that
    provides a SQL driver factory for a specific driver type.

    \ingroup database
    \module sql

    QSqlDriverCreator<T> instantiates objects of type T, where T is a
    QSqlDriver subclass.

    See QSqlDatabase::registerSqlDriver() for details.
*/

/*!
    \fn QSqlDriver *QSqlDriverCreator::createObject() const
    \reimp
*/

/*!
    \class QSqlDatabase
    \brief The QSqlDatabase class represents a connection to
    a database.

    \ingroup database
    \mainclass
    \module sql

    The QSqlDatabase class provides an abstract interface for
    accessing database backends. It relies on database-specific
    \l{QSqlDriver}s to actually access and manipulate data.

    The following code shows how to initialize a connection:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 0

    Once a QSqlDatabase object has been created you can set the
    connection parameters with setDatabaseName(), setUserName(),
    setPassword(), setHostName(), setPort(), and setConnectOptions().
    Once the parameters have been set up you can call open() to open
    the connection.

    The connection defined above is a nameless connection. It is the
    default connection and can be accessed using database() later on:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 1

    To make programming more convenient, QSqlDatabase is a value
    class. Any changes done to a database connection through one
    QSqlDatabase object will affect other QSqlDatabase objects
    representing the same connection. Call cloneDatabase() if you want
    to create an independent database connection based on an existing
    one.

    If you need multiple database connections simultaneously, specify
    an arbitrary name to addDatabase() and database(). Call
    removeDatabase() to remove connections. QSqlDatabase will output
    a warning if you try to remove a connection referenced by other
    QSqlDatabase objects. Use contains() to see if a given connection
    name is in the list of connections.

    Once a connection is established you can see what tables the
    database offers with tables(), find the primary index for a table
    with primaryIndex(), get meta-information about a table's fields
    (e.g., their names) with record(), and execute a query with exec().

    If transactions are supported, you can use transaction() to start
    a transaction, and then commit() or rollback() to complete it.
    You can find out whether transactions are supported using
    QSqlDriver::hasFeature(). When using transactions you must start
    the transaction before you create your query.

    If an error occurred, it is given by lastError().

    The names of the underlying SQL drivers are available from
    drivers(); you can check for a particular driver with
    isDriverAvailable(). If you have created your own custom driver,
    you can register it with registerSqlDriver().

    \sa QSqlDriver, QSqlQuery, {QtSql Module}, {Threads and the SQL Module}
*/

/*!
    \threadsafe

    Adds a database to the list of database connections using the
    driver \a type and the connection name \a connectionName. If
    there already exists a database connection called \a
    connectionName, that connection is removed.

    The database connection is referred to by \a connectionName. The
    newly added database connection is returned.

    If \a connectionName is not specified, the newly added database
    connection becomes the default database connection for the
    application, and subsequent calls to database() without a
    database name parameter will return a reference to it. If \a
    connectionName is given, use database(\a connectionName) to
    retrieve a pointer to the database connection.

    \warning If you add a database with the same name as an
    existing database, the new database will replace the old one.
    This will happen automatically if you call this function more
    than once without specifying \a connectionName.

    To make use of the connection, you will need to set it up, for
    example by calling some or all of setDatabaseName(),
    setUserName(), setPassword(), setHostName(), setPort(), and
    setConnectOptions(), and then you'll need to open() the
    connection.

    \sa database() removeDatabase() {Threads and the SQL Module}
*/
QSqlDatabase QSqlDatabase::addDatabase(const QString &type, const QString &connectionName)
{
    QSqlDatabase db(type);
    QSqlDatabasePrivate::addDatabase(db, connectionName);
    return db;
}

/*!
    \threadsafe

    Returns the database connection called \a connectionName. The
    database connection must have been previously added with
    addDatabase(). If \a open is true (the default) and the database
    connection is not already open it is opened now. If no \a
    connectionName is specified the default connection is used. If \a
    connectionName does not exist in the list of databases, an invalid
    connection is returned.

    \sa isOpen() {Threads and the SQL Module}
*/

QSqlDatabase QSqlDatabase::database(const QString& connectionName, bool open)
{
    return QSqlDatabasePrivate::database(connectionName, open);
}

/*!
    \threadsafe

    Removes the database connection \a connectionName from the list of
    database connections.

    \warning There should be no open queries on the database
    connection when this function is called, otherwise a resource leak
    will occur.

    Example:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 0

    The correct way to do it:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 1

    To remove the default connection, which may have been created with a
    call to addDatabase() not specifying a connection name, you can
    retrieve the default connection name by calling connectionName() on
    the database returned by database(). Note that if a default database
    hasn't been created an invalid database will be returned.

    \sa database() connectionName() {Threads and the SQL Module}
*/

void QSqlDatabase::removeDatabase(const QString& connectionName)
{
    QSqlDatabasePrivate::removeDatabase(connectionName);
}

/*!
    Returns a list of all the available database drivers.

    \sa registerSqlDriver()
*/

QStringList QSqlDatabase::drivers()
{
    QStringList list;

#ifdef QT_SQL_PSQL
    list << QLatin1String("QPSQL7");
    list << QLatin1String("QPSQL");
#endif
#ifdef QT_SQL_MYSQL
    list << QLatin1String("QMYSQL3");
    list << QLatin1String("QMYSQL");
#endif
#ifdef QT_SQL_ODBC
    list << QLatin1String("QODBC3");
    list << QLatin1String("QODBC");
#endif
#ifdef QT_SQL_OCI
    list << QLatin1String("QOCI8");
    list << QLatin1String("QOCI");
#endif
#ifdef QT_SQL_TDS
    list << QLatin1String("QTDS7");
    list << QLatin1String("QTDS");
#endif
#ifdef QT_SQL_DB2
    list << QLatin1String("QDB2");
#endif
#ifdef QT_SQL_SQLITE
    list << QLatin1String("QSQLITE");
#endif
#ifdef QT_SQL_SQLITE2
    list << QLatin1String("QSQLITE2");
#endif
#ifdef QT_SQL_IBASE
    list << QLatin1String("QIBASE");
#endif

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if (QFactoryLoader *fl = loader()) {
        QStringList keys = fl->keys();
        for (QStringList::const_iterator i = keys.constBegin(); i != keys.constEnd(); ++i) {
            if (!list.contains(*i))
                list << *i;
        }
    }
#endif

    DriverDict dict = QSqlDatabasePrivate::driverDict();
    for (DriverDict::const_iterator i = dict.constBegin(); i != dict.constEnd(); ++i) {
        if (!list.contains(i.key()))
            list << i.key();
    }

    return list;
}

/*!
    This function registers a new SQL driver called \a name, within
    the SQL framework. This is useful if you have a custom SQL driver
    and don't want to compile it as a plugin.

    Example:
    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 2

    QSqlDatabase takes ownership of the \a creator pointer, so you
    mustn't delete it yourself.

    \sa drivers()
*/
void QSqlDatabase::registerSqlDriver(const QString& name, QSqlDriverCreatorBase *creator)
{
    delete QSqlDatabasePrivate::driverDict().take(name);
    if (creator)
        QSqlDatabasePrivate::driverDict().insert(name, creator);
}

/*!
    \threadsafe

    Returns true if the list of database connections contains \a
    connectionName; otherwise returns false.

    \sa connectionNames(), database(), {Threads and the SQL Module}
*/

bool QSqlDatabase::contains(const QString& connectionName)
{
    return dbDict()->contains_ts(connectionName);
}

/*!
    \threadsafe

    Returns a list containing the names of all connections.

    \sa contains(), database(), {Threads and the SQL Module}
*/
QStringList QSqlDatabase::connectionNames()
{
    return dbDict()->keys_ts();
}

/*!
    \overload

    Creates a QSqlDatabase connection that uses the driver referred
    to by \a type. If the \a type is not recognized, the database
    connection will have no functionality.

    The currently available driver types are:

    \table
    \header \i Driver Type \i Description
    \row \i QDB2     \i IBM DB2
    \row \i QIBASE   \i Borland InterBase Driver
    \row \i QMYSQL   \i MySQL Driver
    \row \i QOCI     \i Oracle Call Interface Driver
    \row \i QODBC    \i ODBC Driver (includes Microsoft SQL Server)
    \row \i QPSQL    \i PostgreSQL Driver
    \row \i QSQLITE  \i SQLite version 3 or above
    \row \i QSQLITE2 \i SQLite version 2
    \row \i QTDS     \i Sybase Adaptive Server
    \endtable

    Additional third party drivers, including your own custom
    drivers, can be loaded dynamically.

    \sa {SQL Database Drivers}, registerSqlDriver(), drivers()
*/

QSqlDatabase::QSqlDatabase(const QString &type)
{
    d = new QSqlDatabasePrivate();
    d->init(type);
}

/*!
    \overload

    Creates a database connection using the given \a driver.
*/

QSqlDatabase::QSqlDatabase(QSqlDriver *driver)
{
    d = new QSqlDatabasePrivate(driver);
}

/*!
    Creates an empty, invalid QSqlDatabase object. Use addDatabase(),
    removeDatabase(), and database() to get valid QSqlDatabase
    objects.
*/
QSqlDatabase::QSqlDatabase()
{
    d = QSqlDatabasePrivate::shared_null();
    d->ref.ref();
}

/*!
    Creates a copy of \a other.
*/
QSqlDatabase::QSqlDatabase(const QSqlDatabase &other)
{
    d = other.d;
    d->ref.ref();
}

/*!
    Assigns \a other to this object.
*/
QSqlDatabase &QSqlDatabase::operator=(const QSqlDatabase &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

/*!
    \internal

    Create the actual driver instance \a type.
*/

void QSqlDatabasePrivate::init(const QString &type)
{
    drvName = type;

    if (!driver) {
#ifdef QT_SQL_PSQL
        if (type == QLatin1String("QPSQL") || type == QLatin1String("QPSQL7"))
            driver = new QPSQLDriver();
#endif
#ifdef QT_SQL_MYSQL
        if (type == QLatin1String("QMYSQL") || type == QLatin1String("QMYSQL3"))
            driver = new QMYSQLDriver();
#endif
#ifdef QT_SQL_ODBC
        if (type == QLatin1String("QODBC") || type == QLatin1String("QODBC3"))
            driver = new QODBCDriver();
#endif
#ifdef QT_SQL_OCI
        if (type == QLatin1String("QOCI") || type == QLatin1String("QOCI8"))
            driver = new QOCIDriver();
#endif
#ifdef QT_SQL_TDS
        if (type == QLatin1String("QTDS") || type == QLatin1String("QTDS7"))
            driver = new QTDSDriver();
#endif
#ifdef QT_SQL_DB2
        if (type == QLatin1String("QDB2"))
            driver = new QDB2Driver();
#endif
#ifdef QT_SQL_SQLITE
        if (type == QLatin1String("QSQLITE"))
            driver = new QSQLiteDriver();
#endif
#ifdef QT_SQL_SQLITE2
        if (type == QLatin1String("QSQLITE2"))
            driver = new QSQLite2Driver();
#endif
#ifdef QT_SQL_IBASE
        if (type == QLatin1String("QIBASE"))
            driver = new QIBaseDriver();
#endif
    }

    if (!driver) {
        DriverDict dict = QSqlDatabasePrivate::driverDict();
        for (DriverDict::const_iterator it = dict.constBegin();
             it != dict.constEnd() && !driver; ++it) {
            if (type == it.key()) {
                driver = ((QSqlDriverCreatorBase*)(*it))->createObject();
            }
        }
    }

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if (!driver && loader()) {
        if (QSqlDriverFactoryInterface *factory = qobject_cast<QSqlDriverFactoryInterface*>(loader()->instance(type)))
            driver = factory->create(type);
    }
#endif // QT_NO_LIBRARY

    if (!driver) {
        qWarning("QSqlDatabase: %s driver not loaded", type.toLatin1().data());
        qWarning("QSqlDatabase: available drivers: %s",
                        QSqlDatabase::drivers().join(QLatin1String(" ")).toLatin1().data());
        if (QCoreApplication::instance() == 0)
            qWarning("QSqlDatabase: an instance of QCoreApplication is required for loading driver plugins");
        driver = shared_null()->driver;
    }
}

/*!
    Destroys the object and frees any allocated resources.

    If this is the last QSqlDatabase object that uses a certain
    database connection, the is automatically closed.

    \sa close()
*/

QSqlDatabase::~QSqlDatabase()
{
    if (!d->ref.deref()) {
        close();
        delete d;
    }
}

/*!
    Executes a SQL statement on the database and returns a QSqlQuery
    object. Use lastError() to retrieve error information. If \a
    query is empty, an empty, invalid query is returned and
    lastError() is not affected.

    \sa QSqlQuery, lastError()
*/

QSqlQuery QSqlDatabase::exec(const QString & query) const
{
    QSqlQuery r(d->driver->createResult());
    if (!query.isEmpty()) {
        r.exec(query);
        d->driver->setLastError(r.lastError());
    }
    return r;
}

/*!
    Opens the database connection using the current connection
    values. Returns true on success; otherwise returns false. Error
    information can be retrieved using lastError().

    \sa lastError() setDatabaseName() setUserName() setPassword() setHostName() setPort() setConnectOptions()
*/

bool QSqlDatabase::open()
{
    return d->driver->open(d->dbname, d->uname, d->pword, d->hname,
                            d->port, d->connOptions);
}

/*!
    \overload

    Opens the database connection using the given \a user name and \a
    password. Returns true on success; otherwise returns false. Error
    information can be retrieved using the lastError() function.

    This function does not store the password it is given. Instead,
    the password is passed directly to the driver for opening the
    connection and it is then discarded.

    \sa lastError()
*/

bool QSqlDatabase::open(const QString& user, const QString& password)
{
    setUserName(user);
    return d->driver->open(d->dbname, user, password, d->hname,
                            d->port, d->connOptions);
}

/*!
    Closes the database connection, freeing any resources acquired, and
    invalidating any existing QSqlQuery objects that are used with the
    database.

    This will also affect copies of this QSqlDatabase object.

    \sa removeDatabase()
*/

void QSqlDatabase::close()
{
    d->driver->close();
}

/*!
    Returns true if the database connection is currently open;
    otherwise returns false.
*/

bool QSqlDatabase::isOpen() const
{
    return d->driver->isOpen();
}

/*!
    Returns true if there was an error opening the database
    connection; otherwise returns false. Error information can be
    retrieved using the lastError() function.
*/

bool QSqlDatabase::isOpenError() const
{
    return d->driver->isOpenError();
}

/*!
    Begins a transaction on the database if the driver supports
    transactions. Returns true if the operation succeeded; otherwise
    returns false.

    \sa QSqlDriver::hasFeature(), commit(), rollback()
*/

bool QSqlDatabase::transaction()
{
    if (!d->driver->hasFeature(QSqlDriver::Transactions))
        return false;
    return d->driver->beginTransaction();
}

/*!
    Commits a transaction to the database if the driver supports
    transactions and a transaction() has been started. Returns true if
    the operation succeeded; otherwise returns false.

    Note that on some databases, this function will not work if there
    is an active QSqlQuery on the database. Use the lastError()
    function to retrieve database-specific error data about the error
    that occurred.

    \sa QSqlDriver::hasFeature() rollback()
*/

bool QSqlDatabase::commit()
{
    if (!d->driver->hasFeature(QSqlDriver::Transactions))
        return false;
    return d->driver->commitTransaction();
}

/*!
    Rolls a transaction back on the database if the driver supports
    transactions and a transaction() has been started. Returns true
    if the operation succeeded; otherwise returns false.

    \sa QSqlDriver::hasFeature() commit()
*/

bool QSqlDatabase::rollback()
{
    if (!d->driver->hasFeature(QSqlDriver::Transactions))
        return false;
    return d->driver->rollbackTransaction();
}

/*!
    Sets the connection's name to \a name. This must be done before
    the connection is opened or it has no effect; (or you can close()
    the connection, call this function and open() the connection
    again). The name is database-specific.

    For the QOCI (Oracle) driver, the database name is the TNS
    Service Name.

    For the QODBC driver, the \a name can either be a DSN, a DSN
    filename (in which case the file must have a \c .dsn extension),
    or a connection string.

    For example, Microsoft Access users can use the following
    connection string to open an \c .mdb file directly, instead of
    having to create a DSN entry in the ODBC manager:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 3

    There is no default value.

    \sa databaseName() setUserName() setPassword() setHostName() setPort() setConnectOptions() open()
*/

void QSqlDatabase::setDatabaseName(const QString& name)
{
    if (isValid())
        d->dbname = name;
}

/*!
    Sets the connection's user name to \a name. This must be done
    before the connection is opened or it has no effect (or you can
    close() the connection, call this function and open() the
    connection again).

    There is no default value.

    \sa userName() setDatabaseName() setPassword() setHostName()
    setPort() setConnectOptions() open()
*/

void QSqlDatabase::setUserName(const QString& name)
{
    if (isValid())
        d->uname = name;
}

/*!
    Sets the connection's password to \a password. This must be done
    before the connection is opened or it has no effect (or you can
    close() the connection, call this function and open() the
    connection again).

    There is no default value.

    \warning This function stores the password in plain text within
    Qt. Use the open() call that takes a password as parameter to
    avoid this behavior.

    \sa password() setUserName() setDatabaseName() setHostName() setPort() setConnectOptions() open()
*/

void QSqlDatabase::setPassword(const QString& password)
{
    if (isValid())
        d->pword = password;
}

/*!
    Sets the connection's host name to \a host. This must be done
    before the connection is opened or it has no effect (or you can
    close() the connection, call this function and open() the
    connection again).

    There is no default value.

    \sa hostName() setUserName() setPassword() setDatabaseName() setPort() setConnectOptions() open()
*/

void QSqlDatabase::setHostName(const QString& host)
{
    if (isValid())
        d->hname = host;
}

/*!
    Sets the connection's port number to \a port. This must be done
    before the connection is opened or it has no effect (or you can
    close() the connection, call this function and open() the
    connection again).

    There is no default value.

    \sa port() setUserName() setPassword() setHostName()
    setDatabaseName() setConnectOptions() open()
*/

void QSqlDatabase::setPort(int port)
{
    if (isValid())
        d->port = port;
}

/*!
    Returns the connection's database name; it may be empty.

    \sa setDatabaseName()
*/
QString QSqlDatabase::databaseName() const
{
    return d->dbname;
}

/*!
    Returns the connection's user name; it may be empty.

    \sa setUserName()
*/
QString QSqlDatabase::userName() const
{
    return d->uname;
}

/*!
    Returns the connection's password. If the password was not set
    with setPassword(), and if the password was given in the open()
    call, or if no password was used, an empty string is returned.
*/
QString QSqlDatabase::password() const
{
    return d->pword;
}

/*!
    Returns the connection's host name; it may be empty.

    \sa setHostName()
*/
QString QSqlDatabase::hostName() const
{
    return d->hname;
}

/*!
    Returns the connection's driver name.

    \sa addDatabase(), driver()
*/
QString QSqlDatabase::driverName() const
{
    return d->drvName;
}

/*!
    Returns the connection's port number. The value is undefined if
    the port number has not been set.

    \sa setPort()
*/
int QSqlDatabase::port() const
{
    return d->port;
}

/*!
    Returns the database driver used to access the database
    connection.

    \sa addDatabase() drivers()
*/

QSqlDriver* QSqlDatabase::driver() const
{
    return d->driver;
}

/*!
    Returns information about the last error that occurred on the
    database.

    Failures that occur in conjunction with an individual query are
    reported by QSqlQuery::lastError().

    \sa QSqlError, QSqlQuery::lastError()
*/

QSqlError QSqlDatabase::lastError() const
{
    return d->driver->lastError();
}


/*!
    Returns a list of the database's tables, system tables and views,
    as specified by the parameter \a type.

    \sa primaryIndex(), record()
*/

QStringList QSqlDatabase::tables(QSql::TableType type) const
{
    return d->driver->tables(type);
}

/*!
    Returns the primary index for table \a tablename. If no primary
    index exists an empty QSqlIndex is returned.

    \sa tables(), record()
*/

QSqlIndex QSqlDatabase::primaryIndex(const QString& tablename) const
{
    return d->driver->primaryIndex(tablename);
}


/*!
    Returns a QSqlRecord populated with the names of all the fields in
    the table (or view) called \a tablename. The order in which the
    fields appear in the record is undefined. If no such table (or
    view) exists, an empty record is returned.
*/

QSqlRecord QSqlDatabase::record(const QString& tablename) const
{
    return d->driver->record(tablename);
}


/*!
    Sets database-specific \a options. This must be done before the
    connection is opened or it has no effect (or you can close() the
    connection, call this function and open() the connection again).

    The format of the \a options string is a semicolon separated list
    of option names or option=value pairs. The options depend on the
    database client used:

    \table
    \header \i ODBC \i MySQL \i PostgreSQL
    \row

    \i
    \list
    \i SQL_ATTR_ACCESS_MODE
    \i SQL_ATTR_LOGIN_TIMEOUT
    \i SQL_ATTR_CONNECTION_TIMEOUT
    \i SQL_ATTR_CURRENT_CATALOG
    \i SQL_ATTR_METADATA_ID
    \i SQL_ATTR_PACKET_SIZE
    \i SQL_ATTR_TRACEFILE
    \i SQL_ATTR_TRACE
    \i SQL_ATTR_CONNECTION_POOLING
    \i SQL_ATTR_ODBC_VERSION
    \endlist

    \i
    \list
    \i CLIENT_COMPRESS
    \i CLIENT_FOUND_ROWS
    \i CLIENT_IGNORE_SPACE
    \i CLIENT_SSL
    \i CLIENT_ODBC
    \i CLIENT_NO_SCHEMA
    \i CLIENT_INTERACTIVE
    \i UNIX_SOCKET
    \endlist

    \i
    \list
    \i connect_timeout
    \i options
    \i tty
    \i requiressl
    \i service
    \endlist

    \header \i DB2 \i OCI \i TDS
    \row

    \i
    \list
    \i SQL_ATTR_ACCESS_MODE
    \i SQL_ATTR_LOGIN_TIMEOUT
    \endlist

    \i
    \list
    \i OCI_ATTR_PREFETCH_ROWS
    \i OCI_ATTR_PREFETCH_MEMORY
    \endlist

    \i
    \e none

    \header \i SQLite \i Interbase
    \row

    \i
    \list
    \i QSQLITE_BUSY_TIMEOUT
    \endlist

    \i
    \list
    \i ISC_DPB_LC_CTYPE
    \i ISC_DPB_SQL_ROLE_NAME
    \endlist

    \endtable

    Examples:
    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 4

    Refer to the client library documentation for more information
    about the different options.

    \sa connectOptions()
*/

void QSqlDatabase::setConnectOptions(const QString &options)
{
    if (isValid())
        d->connOptions = options;
}

/*!
    Returns the connection options string used for this connection.
    The string may be empty.

    \sa setConnectOptions()
 */
QString QSqlDatabase::connectOptions() const
{
    return d->connOptions;
}

/*!
    Returns true if a driver called \a name is available; otherwise
    returns false.

    \sa drivers()
*/

bool QSqlDatabase::isDriverAvailable(const QString& name)
{
    return drivers().contains(name);
}

/*! \overload

    This function is useful if you need to set up the database
    connection and instantiate the driver yourself. If you do this,
    it is recommended that you include the driver code in your own
    application. For example, setting up a custom PostgreSQL
    connection and instantiating the QPSQL driver can be done like
    this:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 5

    (We assume that \c qtdir is the directory where Qt is installed.)
    This will pull in the code that is needed to use the PostgreSQL
    client library and to instantiate a QPSQLDriver object, assuming
    that you have the PostgreSQL headers somewhere in your include
    search path.

    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 6

    The above code sets up a PostgreSQL connection and instantiates a
    QPSQLDriver object. Next, addDatabase() is called to add the
    connection to the known connections so that it can be used by the
    Qt SQL classes. When a driver is instantiated with a connection
    handle (or set of handles), Qt assumes that you have already
    opened the database connection.

    Remember that you must link your application against the database
    client library as well. The simplest way to do this is to add
    lines like the ones below to your \c .pro file:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 7

    You will need to have the client library in your linker's search
    path.

    The method described above will work for all the drivers, the only
    difference is the arguments the driver constructors take. Below is
    an overview of the drivers and their constructor arguments.

    \table
    \header \i Driver \i Class name \i Constructor arguments \i File to include
    \row
    \i QPSQL
    \i QPSQLDriver
    \i PGconn *connection
    \i \c qsql_psql.cpp
    \row
    \i QMYSQL
    \i QMYSQLDriver
    \i MYSQL *connection
    \i \c qsql_mysql.cpp
    \row
    \i QOCI
    \i QOCIDriver
    \i OCIEnv *environment, OCISvcCtx *serviceContext
    \i \c qsql_oci.cpp
    \row
    \i QODBC
    \i QODBCDriver
    \i SQLHANDLE environment, SQLHANDLE connection
    \i \c qsql_odbc.cpp
    \row
    \i QDB2
    \i QDB2
    \i SQLHANDLE environment, SQLHANDLE connection
    \i \c qsql_db2.cpp
    \row
    \i QTDS
    \i QTDSDriver
    \i LOGINREC *loginRecord, DBPROCESS *dbProcess, const QString &hostName
    \i \c qsql_tds.cpp
    \row
    \i QSQLITE
    \i QSQLiteDriver
    \i sqlite *connection
    \i \c qsql_sqlite.cpp
    \row
    \i QIBASE
    \i QIBaseDriver
    \i isc_db_handle connection
    \i \c qsql_ibase.cpp
    \endtable

    The host name (or service name) is needed when constructing
    the QTDSDriver for creating new connections for internal
    queries. This is to prevent the simultaneous usage of several
    QSqlQuery objects from blocking each other.

    \warning If you add a database with the same name as an
    existing database, the new database will replace the old one.

    \warning The SQL framework takes ownership of the \a driver pointer,
    and it should not be deleted. If you want to
    explicitly remove the connection, use removeDatabase().

    \sa drivers()
*/

QSqlDatabase QSqlDatabase::addDatabase(QSqlDriver* driver, const QString& connectionName)
{
    QSqlDatabase db(driver);
    QSqlDatabasePrivate::addDatabase(db, connectionName);
    return db;
}

/*!
    Returns true if the QSqlDatabase has a valid driver.

    Example:
    \snippet doc/src/snippets/code/src.sql.kernel.qsqldatabase.cpp 8
*/
bool QSqlDatabase::isValid() const
{
    return d->driver && d->driver != d->shared_null()->driver;
}

#ifdef QT3_SUPPORT
/*!
    Use query.record() instead.
*/
QSqlRecord QSqlDatabase::record(const QSqlQuery& query) const
{ return query.record(); }

/*!
    Use query.record() instead.
*/
QSqlRecord QSqlDatabase::recordInfo(const QSqlQuery& query) const
{ return query.record(); }

/*!
    \fn QSqlRecord QSqlDatabase::recordInfo(const QString& tablename) const

    Use record() instead.
*/
#endif

/*!
    Clones the database connection \a other and and stores it as \a
    connectionName. All the settings from the original database, e.g.
    databaseName(), hostName(), etc., are copied across. Does nothing
    if \a other is an invalid database. Returns the newly created
    database connection.

    Note that the connection is not opened, to use it, it is
    necessary to call open() first.
*/
QSqlDatabase QSqlDatabase::cloneDatabase(const QSqlDatabase &other, const QString &connectionName)
{
    if (!other.isValid())
        return QSqlDatabase();

    QSqlDatabase db(other.driverName());
    db.d->copy(other.d);
    QSqlDatabasePrivate::addDatabase(db, connectionName);
    return db;
}

/*!
    Returns the connection's name; it may be empty.

    \sa addDatabase()
*/
QString QSqlDatabase::connectionName() const
{
    return d->connName;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QSqlDatabase &d)
{
    if (!d.isValid()) {
        dbg.nospace() << "QSqlDatabase(invalid)";
        return dbg.space();
    }

    dbg.nospace() << "QSqlDatabase(driver=\"" << d.driverName() << "\", database=\""
                  << d.databaseName() << "\", host=\"" << d.hostName() << "\", port=" << d.port()
                  << ", user=\"" << d.userName() << "\", open=" << d.isOpen() << ")";
    return dbg.space();
}
#endif

QT_END_NAMESPACE
