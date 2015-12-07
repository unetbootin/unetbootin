/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#ifndef EMPLOYEE_H
#define EMPLOYEE_H

//! [0]
#include <QSharedData>
#include <QString>

class EmployeeData : public QSharedData
{
  public:
    EmployeeData() : id(-1) { name.clear(); }
    EmployeeData(const EmployeeData &other)
        : QSharedData(other), id(other.id), name(other.name) { }
    ~EmployeeData() { }

    int id;
    QString name;
};

class Employee
{
  public:
//! [1]
    Employee() { d = new EmployeeData; }
//! [1] //! [2]
    Employee(int id, QString name) {
        d = new EmployeeData;
        setId(id);
        setName(name);
    }
//! [2]

//! [3]
    void setId(int id) { d->id = id; }
//! [3] //! [4]
    void setName(QString name) { d->name = name; }
//! [4]

//! [5]
    int id() const { return d->id; }
//! [5] //! [6]
    QString name() const { return d->name; }
//! [6]

  private:
    QSharedDataPointer<EmployeeData> d;
};
//! [0]

#endif
