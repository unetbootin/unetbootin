/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "QtTest/private/qtesttable_p.h"
#include "QtTest/qtestdata.h"

#include <QtCore/qmetaobject.h>

#include <string.h>

#include "QtTest/qtestassert.h"

QT_BEGIN_NAMESPACE

class QTestTablePrivate
{
public:
    struct ElementList
    {
        ElementList(): elementName(0), elementType(0), next(0) {}
        const char *elementName;
        int elementType;
        ElementList *next;
    };

    struct DataList
    {
        DataList(): data(0), next(0) {}
        QTestData *data;
        DataList *next;
    };

    QTestTablePrivate(): list(0), dataList(0) {}
    ~QTestTablePrivate();

    ElementList *list;
    DataList *dataList;

    void append(int elemType, const char *elemName);
    void append(QTestData *data);
    ElementList *elementAt(int index);
    QTestData *dataAt(int index);

    static QTestTable *currentTestTable;
    static QTestTable *gTable;
};

QTestTable *QTestTablePrivate::currentTestTable = 0;
QTestTable *QTestTablePrivate::gTable = 0;

QTestTablePrivate::ElementList *QTestTablePrivate::elementAt(int index)
{
    ElementList *iter = list;
    for (int i = 0; i < index; ++i) {
        if (!iter)
            return 0;
        iter = iter->next;
    }
    return iter;
}

QTestData *QTestTablePrivate::dataAt(int index)
{
    DataList *iter = dataList;
    for (int i = 0; i < index; ++i) {
        if (!iter)
            return 0;
        iter = iter->next;
    }
    return iter ? iter->data : 0;
}

QTestTablePrivate::~QTestTablePrivate()
{
    DataList *dit = dataList;
    while (dit) {
        DataList *next = dit->next;
        delete dit->data;
        delete dit;
        dit = next;
    }
    ElementList *iter = list;
    while (iter) {
        ElementList *next = iter->next;
        delete iter;
        iter = next;
    }
}

void QTestTablePrivate::append(int elemType, const char *elemName)
{
    ElementList *item = new ElementList;
    item->elementName = elemName;
    item->elementType = elemType;
    if (!list) {
        list = item;
        return;
    }
    ElementList *last = list;
    while (last->next != 0)
        last = last->next;
    last->next = item;
}

void QTestTablePrivate::append(QTestData *data)
{
    DataList *item = new DataList;
    item->data = data;
    if (!dataList) {
        dataList = item;
        return;
    }
    DataList *last = dataList;
    while (last->next != 0)
        last = last->next;
    last->next = item;
}

void QTestTable::addColumn(int type, const char *name)
{
    QTEST_ASSERT(type);
    QTEST_ASSERT(name);

    d->append(type, name);
}

int QTestTable::elementCount() const
{
    QTestTablePrivate::ElementList *item = d->list;
    int count = 0;
    while (item) {
        ++count;
        item = item->next;
    }
    return count;
}


int QTestTable::dataCount() const
{
    QTestTablePrivate::DataList *item = d->dataList;
    int count = 0;
    while (item) {
        ++count;
        item = item->next;
    }
    return count;
}

bool QTestTable::isEmpty() const
{
    return !d->list;
}

QTestData *QTestTable::newData(const char *tag)
{
    QTestData *dt = new QTestData(tag, this);
    d->append(dt);
    return dt;
}

QTestTable::QTestTable()
{
    d = new QTestTablePrivate;
    QTestTablePrivate::currentTestTable = this;
}

QTestTable::~QTestTable()
{
    QTestTablePrivate::currentTestTable = 0;
    delete d;
}

int QTestTable::elementTypeId(int index) const
{
    QTestTablePrivate::ElementList *item = d->elementAt(index);
    if (!item)
        return -1;
    return item->elementType;
}

const char *QTestTable::dataTag(int index) const
{
    QTestTablePrivate::ElementList *item = d->elementAt(index);
    if (!item)
        return 0;
    return item->elementName;
}

QTestData *QTestTable::testData(int index) const
{
    return d->dataAt(index);
}

int QTestTable::indexOf(const char *elementName) const
{
    QTEST_ASSERT(elementName);

    QTestTablePrivate::ElementList *item = d->list;
    int i = 0;
    while (item) {
        if (strcmp(elementName, item->elementName) == 0)
            return i;
        item = item->next;
        ++i;
    }
    return -1;
}

QTestTable *QTestTable::globalTestTable()
{
    if (!QTestTablePrivate::gTable)
        QTestTablePrivate::gTable = new QTestTable();
    return QTestTablePrivate::gTable;
}

void QTestTable::clearGlobalTestTable()
{
    delete QTestTablePrivate::gTable;
    QTestTablePrivate::gTable = 0;
}

QTestTable *QTestTable::currentTestTable()
{
    return QTestTablePrivate::currentTestTable;
}

QT_END_NAMESPACE
