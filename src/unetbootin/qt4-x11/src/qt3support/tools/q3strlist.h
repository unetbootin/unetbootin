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
            append(list.at(i).constData());
    }

    Q3StrList &operator =(const QList<QByteArray> &list) {
        clear();
        for (int i = 0; i < list.size(); ++i)
            append(list.at(i).constData());
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
