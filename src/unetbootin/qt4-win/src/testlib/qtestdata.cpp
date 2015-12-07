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

#include <QtCore/qmetaobject.h>

#include "QtTest/qtestassert.h"
#include "QtTest/qtestdata.h"
#include "QtTest/private/qtesttable_p.h"

#include <string.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

class QTestDataPrivate
{
public:
    QTestDataPrivate(): tag(0), parent(0), data(0), dataCount(0) {}

    char *tag;
    QTestTable *parent;
    void **data;
    int dataCount;
};

QTestData::QTestData(const char *tag, QTestTable *parent)
{
    QTEST_ASSERT(tag);
    QTEST_ASSERT(parent);
    d = new QTestDataPrivate;
    d->tag = qstrdup(tag);
    d->parent = parent;
    d->data = new void *[parent->elementCount()];
    memset(d->data, 0, parent->elementCount() * sizeof(void*));
}

QTestData::~QTestData()
{
    for (int i = 0; i < d->dataCount; ++i) {
        if (d->data[i])
            QMetaType::destroy(d->parent->elementTypeId(i), d->data[i]);
    }
    delete [] d->data;
    delete [] d->tag;
    delete d;
}

void QTestData::append(int type, const void *data)
{
    QTEST_ASSERT(d->dataCount < d->parent->elementCount());
    if (d->parent->elementTypeId(d->dataCount) != type) {
        qDebug("expected data of type '%s', got '%s' for element %d of data with tag '%s'",
                QMetaType::typeName(d->parent->elementTypeId(d->dataCount)),
                QMetaType::typeName(type),
                d->dataCount, d->tag);
        QTEST_ASSERT(false);
    }
    d->data[d->dataCount] = QMetaType::construct(type, data);
    ++d->dataCount;
}

void *QTestData::data(int index) const
{
    QTEST_ASSERT(index >= 0);
    QTEST_ASSERT(index < d->parent->elementCount());
    return d->data[index];
}

QTestTable *QTestData::parent() const
{
    return d->parent;
}

const char *QTestData::dataTag() const
{
    return d->tag;
}

int QTestData::dataCount() const
{
    return d->dataCount;
}

QT_END_NAMESPACE
