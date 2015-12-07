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

#ifndef QSQLQUERY_H
#define QSQLQUERY_H

#include <QtSql/qsql.h>
#include <QtSql/qsqldatabase.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sql)

class QVariant;
class QSqlDriver;
class QSqlError;
class QSqlResult;
class QSqlRecord;
template <class Key, class T> class QMap;
class QSqlQueryPrivate;

class Q_SQL_EXPORT QSqlQuery
{
public:
    QSqlQuery(QSqlResult *r);
    QSqlQuery(const QString& query = QString(), QSqlDatabase db = QSqlDatabase());
    explicit QSqlQuery(QSqlDatabase db);
    QSqlQuery(const QSqlQuery& other);
    QSqlQuery& operator=(const QSqlQuery& other);
    ~QSqlQuery();

    bool isValid() const;
    bool isActive() const;
    bool isNull(int field) const;
    int at() const;
    QString lastQuery() const;
    int numRowsAffected() const;
    QSqlError lastError() const;
    bool isSelect() const;
    int size() const;
    const QSqlDriver* driver() const;
    const QSqlResult* result() const;
    bool isForwardOnly() const;
    QSqlRecord record() const;

    void setForwardOnly(bool forward);
    bool exec(const QString& query);
    QVariant value(int i) const;

    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;

    bool seek(int i, bool relative = false);
    bool next();
    bool previous();
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT bool prev() { return previous(); }
#endif
    bool first();
    bool last();

    void clear();

    // prepared query support
    bool exec();
    enum BatchExecutionMode { ValuesAsRows, ValuesAsColumns };
    bool execBatch(BatchExecutionMode mode = ValuesAsRows);
    bool prepare(const QString& query);
    void bindValue(const QString& placeholder, const QVariant& val,
                   QSql::ParamType type = QSql::In);
    void bindValue(int pos, const QVariant& val, QSql::ParamType type = QSql::In);
    void addBindValue(const QVariant& val, QSql::ParamType type = QSql::In);
    QVariant boundValue(const QString& placeholder) const;
    QVariant boundValue(int pos) const;
    QMap<QString, QVariant> boundValues() const;
    QString executedQuery() const;
    QVariant lastInsertId() const;
    void finish();
    bool nextResult();

private:
    QSqlQueryPrivate* d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQLQUERY_H
