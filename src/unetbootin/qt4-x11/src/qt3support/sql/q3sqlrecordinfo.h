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

#ifndef Q3SQLRECORDINFO_H
#define Q3SQLRECORDINFO_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_SQL
#   include <Qt3Support/q3valuelist.h>
#   include <QtSql/qsqlrecord.h>
#   include <Qt3Support/q3sqlfieldinfo.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_SQL

/* Q3SqlRecordInfo Class
   This class is obsolete, use QSqlRecord instead.
*/

typedef Q3ValueList<Q3SqlFieldInfo> Q3SqlFieldInfoList;

class Q_COMPAT_EXPORT Q3SqlRecordInfo: public Q3SqlFieldInfoList
{
public:
    Q3SqlRecordInfo(): Q3SqlFieldInfoList() {}
    Q3SqlRecordInfo(const Q3SqlFieldInfoList& other): Q3SqlFieldInfoList(other) {}
    Q3SqlRecordInfo(const QSqlRecord& other)
    {
        for (int i = 0; i < other.count(); ++i)
            push_back(Q3SqlFieldInfo(other.field(i)));
    }

    size_type contains(const QString& fieldName) const;
    Q3SqlFieldInfo find(const QString& fieldName) const;
    QSqlRecord toRecord() const;
};

inline Q3SqlRecordInfo::size_type Q3SqlRecordInfo::contains(const QString& fieldName) const
{
    size_type i = 0;
    QString fName = fieldName.toUpper();

    for(const_iterator it = begin(); it != end(); ++it) {
        if ((*it).name().toUpper() == fName) {
            ++i;
        }
    }
    return i;
}

inline Q3SqlFieldInfo Q3SqlRecordInfo::find(const QString& fieldName) const
{
    QString fName = fieldName.toUpper();
    for(const_iterator it = begin(); it != end(); ++it) {
        if ((*it).name().toUpper() == fName) {
            return *it;
        }
    }
    return Q3SqlFieldInfo();
}

inline QSqlRecord Q3SqlRecordInfo::toRecord() const
{
    QSqlRecord buf;
    for(const_iterator it = begin(); it != end(); ++it) {
        buf.append((*it).toField());
    }
    return buf;
}

#endif // QT_NO_SQL

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SQLRECORDINFO_H
