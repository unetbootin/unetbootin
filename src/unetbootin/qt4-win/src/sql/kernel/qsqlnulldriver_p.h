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

#ifndef QSQLNULLDRIVER_P_H
#define QSQLNULLDRIVER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//

#include "QtCore/qvariant.h"
#include "QtSql/qsqldriver.h"
#include "QtSql/qsqlerror.h"
#include "QtSql/qsqlresult.h"

QT_BEGIN_NAMESPACE

class QSqlNullResult : public QSqlResult
{
public:
    inline explicit QSqlNullResult(const QSqlDriver* d): QSqlResult(d)
    { QSqlResult::setLastError(
            QSqlError(QLatin1String("Driver not loaded"), QLatin1String("Driver not loaded"), QSqlError::ConnectionError)); }
protected:
    inline QVariant data(int) { return QVariant(); }
    inline bool reset (const QString&) { return false; }
    inline bool fetch(int) { return false; }
    inline bool fetchFirst() { return false; }
    inline bool fetchLast() { return false; }
    inline bool isNull(int) { return false; }
    inline int size()  { return -1; }
    inline int numRowsAffected() { return 0; }

    inline void setAt(int) {}
    inline void setActive(bool) {}
    inline void setLastError(const QSqlError&) {}
    inline void setQuery(const QString&) {}
    inline void setSelect(bool) {}
    inline void setForwardOnly(bool) {}

    inline bool exec() { return false; }
    inline bool prepare(const QString&) { return false; }
    inline bool savePrepare(const QString&) { return false; }
    inline void bindValue(int, const QVariant&, QSql::ParamType) {}
    inline void bindValue(const QString&, const QVariant&, QSql::ParamType) {}
};

class QSqlNullDriver : public QSqlDriver
{
public:
    inline QSqlNullDriver(): QSqlDriver()
    { QSqlDriver::setLastError(
            QSqlError(QLatin1String("Driver not loaded"), QLatin1String("Driver not loaded"), QSqlError::ConnectionError)); }
    inline bool hasFeature(DriverFeature) const { return false; }
    inline bool open(const QString &, const QString & , const QString & ,
              const QString &, int, const QString&)
    { return false; }
    inline void close() {}
    inline QSqlResult *createResult() const { return new QSqlNullResult(this); }

protected:
    inline void setOpen(bool) {}
    inline void setOpenError(bool) {}
    inline void setLastError(const QSqlError&) {}
};

QT_END_NAMESPACE

#endif // QSQLNULLDRIVER_P_H
