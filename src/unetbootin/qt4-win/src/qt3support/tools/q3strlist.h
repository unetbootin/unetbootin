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

#ifndef Q3STRLIST_H
#define Q3STRLIST_H

#include <QtCore/qstring.h>
#include <Qt3Support/q3ptrlist.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#if defined(qdoc)
class Q3StrListIterator : public Q3PtrListIterator<char>
{
};
#else
typedef Q3PtrListIterator<char> Q3StrListIterator;
#endif

class Q_COMPAT_EXPORT Q3StrList : public Q3PtrList<char>
{
public:
    Q3StrList( bool deepCopies=true ) { dc = deepCopies; del_item = deepCopies; }
    Q3StrList( const Q3StrList & );
    ~Q3StrList()			{ clear(); }
    Q3StrList& operator=( const Q3StrList & );
    Q3StrList(const QList<QByteArray> &list) {
        for (int i = 0; i < list.size(); ++i)
            append(list.at(i));
    }

    Q3StrList &operator =(const QList<QByteArray> &list) {
        clear();
        for (int i = 0; i < list.size(); ++i)
            append(list.at(i));
        return *this;
    }

    operator QList<QByteArray>() const {
        QList<QByteArray> list;
        for (Q3PtrListStdIterator<char> it = begin(); it != end(); ++it)
            list.append(QByteArray(*it));
        return list;
    }

private:
    Q3PtrCollection::Item newItem( Q3PtrCollection::Item d ) { return dc ? qstrdup( (const char*)d ) : d; }
    void deleteItem( Q3PtrCollection::Item d ) { if ( del_item ) delete[] (char*)d; }
    int compareItems( Q3PtrCollection::Item s1, Q3PtrCollection::Item s2 ) { return qstrcmp((const char*)s1,
							 (const char*)s2); }
#ifndef QT_NO_DATASTREAM
    QDataStream &read( QDataStream &s, Q3PtrCollection::Item &d )
				{ s >> (char *&)d; return s; }
    QDataStream &write( QDataStream &s, Q3PtrCollection::Item d ) const
				{ return s << (const char *)d; }
#endif
    bool  dc;
};


class Q_COMPAT_EXPORT Q3StrIList : public Q3StrList	// case insensitive string list
{
public:
    Q3StrIList( bool deepCopies=true ) : Q3StrList( deepCopies ) {}
    ~Q3StrIList()			{ clear(); }
private:
    int	  compareItems( Q3PtrCollection::Item s1, Q3PtrCollection::Item s2 )
				{ return qstricmp((const char*)s1,
						    (const char*)s2); }
};


inline Q3StrList & Q3StrList::operator=( const Q3StrList &strList )
{
    clear();
    dc = strList.dc;
    del_item = dc;
    Q3PtrList<char>::operator=( strList );
    return *this;
}

inline Q3StrList::Q3StrList( const Q3StrList &strList )
    : Q3PtrList<char>( strList )
{
    dc = false;
    operator=( strList );
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3STRLIST_H
