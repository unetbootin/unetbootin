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

#ifndef Q3SQLFIELDINFO_H
#define Q3SQLFIELDINFO_H

#ifndef QT_NO_SQL

#include <QtCore/qglobal.h>
#include <QtSql/qsqlfield.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

/* Q3SqlFieldInfo Class
   obsoleted, use QSqlField instead
*/

class Q_COMPAT_EXPORT Q3SqlFieldInfo
{
    // class is obsoleted, won't change anyways,
    // so no d pointer
    int req, len, prec, tID;
    uint gen: 1;
    uint trim: 1;
    uint calc: 1;
    QString nm;
    QVariant::Type typ;
    QVariant defValue;

public:
    Q3SqlFieldInfo(const QString& name = QString(),
                   QVariant::Type typ = QVariant::Invalid,
                   int required = -1,
                   int len = -1,
                   int prec = -1,
                   const QVariant& defValue = QVariant(),
                   int sqlType = 0,
                   bool generated = true,
                   bool trim = false,
                   bool calculated = false) :
        req(required), len(len), prec(prec), tID(sqlType),
        gen(generated), trim(trim), calc(calculated),
        nm(name), typ(typ), defValue(defValue) {}

    virtual ~Q3SqlFieldInfo() {}

    Q3SqlFieldInfo(const QSqlField & other)
    {
        nm = other.name();
        typ = other.type();
        switch (other.requiredStatus()) {
        case QSqlField::Unknown: req = -1; break;
        case QSqlField::Required: req = 1; break;
        case QSqlField::Optional: req = 0; break;
        }
        len = other.length();
        prec = other.precision();
        defValue = other.defaultValue();
        tID = other.typeID();
        gen = other.isGenerated();
        calc = false;
        trim = false;
    }

    bool operator==(const Q3SqlFieldInfo& f) const
    {
        return (nm == f.nm &&
                typ == f.typ &&
                req == f.req &&
                len == f.len &&
                prec == f.prec &&
                defValue == f.defValue &&
                tID == f.tID &&
                gen == f.gen &&
                trim == f.trim &&
                calc == f.calc);
    }

    QSqlField toField() const
    { QSqlField f(nm, typ);
      f.setRequiredStatus(QSqlField::RequiredStatus(req));
      f.setLength(len);
      f.setPrecision(prec);
      f.setDefaultValue(defValue);
      f.setSqlType(tID);
      f.setGenerated(gen);
      return f;
    }
    int isRequired() const
    { return req; }
    QVariant::Type type() const
    { return typ; }
    int length() const
    { return len; }
    int precision() const
    { return prec; }
    QVariant defaultValue() const
    { return defValue; }
    QString name() const
    { return nm; }
    int typeID() const
    { return tID; }
    bool isGenerated() const
    { return gen; }
    bool isTrim() const
    { return trim; }
    bool isCalculated() const
    { return calc; }

    virtual void setTrim(bool trim)
    { this->trim = trim; }
    virtual void setGenerated(bool generated)
    { gen = generated; }
    virtual void setCalculated(bool calculated)
    { calc = calculated; }

};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SQL

#endif // Q3SQLFIELDINFO_H
