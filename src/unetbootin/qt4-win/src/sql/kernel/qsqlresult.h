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

#ifndef QSQLRESULT_H
#define QSQLRESULT_H

#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>
#include <QtSql/qsql.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sql)

class QString;
class QSqlRecord;
template <typename T> class QVector;
class QVariant;
class QSqlDriver;
class QSqlError;
class QSqlResultPrivate;

class Q_SQL_EXPORT QSqlResult
{
    friend class QSqlQuery;
    friend class QSqlTableModelPrivate;
    friend class QSqlResultPrivate;

public:
    virtual ~QSqlResult();
    virtual QVariant handle() const;

protected:
    enum BindingSyntax {
        PositionalBinding,
        NamedBinding
#ifdef QT3_SUPPORT
        , BindByPosition = PositionalBinding,
        BindByName = NamedBinding
#endif
    };

    explicit QSqlResult(const QSqlDriver * db);
    int at() const;
    QString lastQuery() const;
    QSqlError lastError() const;
    bool isValid() const;
    bool isActive() const;
    bool isSelect() const;
    bool isForwardOnly() const;
    const QSqlDriver* driver() const;
    virtual void setAt(int at);
    virtual void setActive(bool a);
    virtual void setLastError(const QSqlError& e);
    virtual void setQuery(const QString& query);
    virtual void setSelect(bool s);
    virtual void setForwardOnly(bool forward);

    // prepared query support
    virtual bool exec();
    virtual bool prepare(const QString& query);
    virtual bool savePrepare(const QString& sqlquery);
    virtual void bindValue(int pos, const QVariant& val, QSql::ParamType type);
    virtual void bindValue(const QString& placeholder, const QVariant& val,
                           QSql::ParamType type);
    void addBindValue(const QVariant& val, QSql::ParamType type);
    QVariant boundValue(const QString& placeholder) const;
    QVariant boundValue(int pos) const;
    QSql::ParamType bindValueType(const QString& placeholder) const;
    QSql::ParamType bindValueType(int pos) const;
    int boundValueCount() const;
    QVector<QVariant>& boundValues() const;
    QString executedQuery() const;
    QString boundValueName(int pos) const;
    void clear();
    bool hasOutValues() const;

    BindingSyntax bindingSyntax() const;

    virtual QVariant data(int i) = 0;
    virtual bool isNull(int i) = 0;
    virtual bool reset(const QString& sqlquery) = 0;
    virtual bool fetch(int i) = 0;
    virtual bool fetchNext();
    virtual bool fetchPrevious();
    virtual bool fetchFirst() = 0;
    virtual bool fetchLast() = 0;
    virtual int size() = 0;
    virtual int numRowsAffected() = 0;
    virtual QSqlRecord record() const;
    virtual QVariant lastInsertId() const;

    enum VirtualHookOperation { BatchOperation, DetachFromResultSet, SetNumericalPrecision, NextResult };
    virtual void virtual_hook(int id, void *data);
    bool execBatch(bool arrayBind = false);
    void detachFromResultSet();
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy policy);
    bool nextResult();

private:
    QSqlResultPrivate* d;
    void resetBindCount(); // HACK

private:
    Q_DISABLE_COPY(QSqlResult)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQLRESULT_H
