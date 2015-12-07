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

#ifndef Q3SQLCURSOR_H
#define Q3SQLCURSOR_H

#include <QtCore/qvariant.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlrecord.h>
#include <QtCore/qstringlist.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlindex.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_SQL

class Q3SqlCursorPrivate;
class Q3SqlFieldInfo;

class Q_COMPAT_EXPORT Q3SqlCursor : public QSqlRecord, public QSqlQuery
{
public:
    Q3SqlCursor(const QString & name = QString(), bool autopopulate = true,
               QSqlDatabase db = QSqlDatabase());
    Q3SqlCursor(const Q3SqlCursor & other);
    Q3SqlCursor& operator=(const Q3SqlCursor& other);
    virtual ~Q3SqlCursor();

    enum Mode {
        ReadOnly = 0,
        Insert = 1,
        Update = 2,
        Delete = 4,
        Writable = 7
    };

    QVariant value(int i) const;
    inline QVariant value(const QString &name) const { return value(indexOf(name)); }
    virtual void setValue(int i, const QVariant &val);
    inline void setValue(const QString &name, const QVariant &val) { setValue(indexOf(name), val); }
    virtual QSqlIndex primaryIndex(bool prime = true) const;
    virtual QSqlIndex index(const QStringList& fieldNames) const;
    QSqlIndex index(const QString& fieldName) const;
    virtual void setPrimaryIndex(const QSqlIndex& idx);

    virtual void append(const Q3SqlFieldInfo& fieldInfo);
    virtual void insert(int pos, const Q3SqlFieldInfo &fieldInfo);
    virtual void remove(int pos);
    virtual void clear();
    virtual void setGenerated(const QString& name, bool generated);
    virtual void setGenerated(int i, bool generated);

    virtual QSqlRecord*        editBuffer(bool copy = false);
    virtual QSqlRecord*        primeInsert();
    virtual QSqlRecord*        primeUpdate();
    virtual QSqlRecord*        primeDelete();
    virtual int                insert(bool invalidate = true);
    virtual int                update(bool invalidate = true);
    virtual int                del(bool invalidate = true);

    virtual void        setMode(int flags);
    int                        mode() const;
    virtual void        setCalculated(const QString& name, bool calculated);
    bool                isCalculated(const QString& name) const;
    virtual void        setTrimmed(const QString& name, bool trim);
    bool                isTrimmed(const QString& name) const;

    bool                isReadOnly() const;
    bool                canInsert() const;
    bool                canUpdate() const;
    bool                canDelete() const;

    bool                select();
    bool                select(const QSqlIndex& sort);
    bool                select(const QSqlIndex & filter, const QSqlIndex & sort);
    virtual bool        select(const QString & filter, const QSqlIndex & sort = QSqlIndex());

    virtual void        setSort(const QSqlIndex& sort);
    QSqlIndex                sort() const;
    virtual void        setFilter(const QString& filter);
    QString                filter() const;
    virtual void        setName(const QString& name, bool autopopulate = true);
    QString                name() const;
    QString                toString(const QString& prefix = QString(),
                                const QString& sep = QLatin1String(",")) const;
    bool                 isNull(int i) const;
    bool                 isNull(const QString& name) const;
    virtual bool seek(int i, bool relative = false);
    virtual bool next();
    inline bool previous() { return prev(); }
    virtual bool prev();
    virtual bool first();
    virtual bool last();

protected:
    virtual bool        exec(const QString & sql);

    virtual QVariant calculateField(const QString& name);
    virtual int                update(const QString & filter, bool invalidate = true);
    virtual int                del(const QString & filter, bool invalidate = true);

    virtual QString        toString(const QString& prefix, QSqlField* field, const QString& fieldSep) const;
    virtual QString        toString(QSqlRecord* rec, const QString& prefix, const QString& fieldSep,
                                const QString& sep) const;
    virtual QString        toString(const QSqlIndex& i, QSqlRecord* rec, const QString& prefix,
                                const QString& fieldSep, const QString& sep) const;

private:
    void                sync();
    int                        apply(const QString& q, bool invalidate);
    int                        applyPrepared(const QString& q, bool invalidate);
    QSqlRecord&                operator=(const QSqlRecord & list);
    void                 append(const QSqlField& field);

    Q3SqlCursorPrivate*        d;
};

#endif // QT_NO_SQL

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SQLCURSOR_H
