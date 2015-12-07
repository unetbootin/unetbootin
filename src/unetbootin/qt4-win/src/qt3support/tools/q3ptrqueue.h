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

#ifndef Q3PTRQUEUE_H
#define Q3PTRQUEUE_H

#include <Qt3Support/q3glist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3PtrQueue : protected Q3GList
{
public:
    Q3PtrQueue()				{}
    Q3PtrQueue( const Q3PtrQueue<type> &q ) : Q3GList(q) {}
    ~Q3PtrQueue()			{ clear(); }
    Q3PtrQueue<type>& operator=(const Q3PtrQueue<type> &q)
			{ return (Q3PtrQueue<type>&)Q3GList::operator=(q); }
    bool  autoDelete() const		{ return Q3PtrCollection::autoDelete(); }
    void  setAutoDelete( bool del )	{ Q3PtrCollection::setAutoDelete(del); }
    uint  count()   const		{ return Q3GList::count(); }
    bool  isEmpty() const		{ return Q3GList::count() == 0; }
    void  enqueue( const type *d )	{ Q3GList::append(Item(d)); }
    type *dequeue()			{ return (type *)Q3GList::takeFirst();}
    bool  remove()			{ return Q3GList::removeFirst(); }
    void  clear()			{ Q3GList::clear(); }
    type *head()    const		{ return (type *)Q3GList::cfirst(); }
	  operator type *() const	{ return (type *)Q3GList::cfirst(); }
    type *current() const		{ return (type *)Q3GList::cfirst(); }

#ifdef qdoc
protected:
    virtual QDataStream& read( QDataStream&, Q3PtrCollection::Item& );
    virtual QDataStream& write( QDataStream&, Q3PtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3PtrQueue<void>::deleteItem( Q3PtrCollection::Item )
{
}
#endif

template<class type> inline void Q3PtrQueue<type>::deleteItem( Q3PtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PTRQUEUE_H
