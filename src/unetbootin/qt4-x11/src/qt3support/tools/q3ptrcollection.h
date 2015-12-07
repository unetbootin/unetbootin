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

#ifndef Q3PTRCOLLECTION_H
#define Q3PTRCOLLECTION_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3GVector;
class Q3GList;
class Q3GDict;

class Q_COMPAT_EXPORT Q3PtrCollection			// inherited by all collections
{
public:
    bool autoDelete()	const	       { return del_item; }
    void setAutoDelete(bool enable)  { del_item = enable; }

    virtual uint  count() const = 0;
    virtual void  clear() = 0;			// delete all objects

    typedef void *Item;				// generic collection item

protected:
    Q3PtrCollection() { del_item = false; }		// no deletion of objects
    Q3PtrCollection(const Q3PtrCollection &) { del_item = false; }
    virtual ~Q3PtrCollection() {}

    bool del_item;				// default false

    virtual Item     newItem(Item);		// create object
    virtual void     deleteItem(Item) = 0;	// delete object
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PTRCOLLECTION_H
