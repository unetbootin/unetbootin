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

#include "q3glist.h"
#include "q3gvector.h"
#include "qdatastream.h"
#include "q3valuelist.h"

QT_BEGIN_NAMESPACE

/*!
  \class Q3LNode qglist.h
  \reentrant
  \brief The Q3LNode class is an internal class for the Q3PtrList template collection.

  \internal

  Q3LNode is a doubly-linked list node. It has three pointers:
  \list 1
  \i Pointer to the previous node.
  \i Pointer to the next node.
  \i Pointer to the actual data.
  \endlist

  It might sometimes be practical to have direct access to the list nodes
  in a Q3PtrList, but it is seldom required.

  Be very careful if you want to access the list nodes. The heap can
  easily get corrupted if you make a mistake.

  \sa Q3PtrList::currentNode(), Q3PtrList::removeNode(), Q3PtrList::takeNode()
*/

/*!
  \fn Q3PtrCollection::Item Q3LNode::getData()
  Returns a pointer (\c void*) to the actual data in the list node.
*/


/*!
  \class Q3GList qglist.h
  \reentrant
  \brief The Q3GList class is an internal class for implementing Qt collection classes.

  \internal

  Q3GList is a strictly internal class that acts as a base class for
  several collection classes; Q3PtrList, Q3PtrQueue and Q3PtrStack.

  Q3GList has some virtual functions that can be reimplemented to
  customize the subclasses, namely compareItems(), read() and
  write. Normally, you do not have to reimplement any of these
  functions.  If you still want to reimplement them, see the QStrList
  class (qstrlist.h) for an example.
*/


/* Internal helper class for Q3GList. Contains some optimization for
   the typically case where only one iterators is activre on the list.
 */
class Q3GListIteratorList
{
public:
    Q3GListIteratorList()
	: list(0), iterator(0) {
    }
    ~Q3GListIteratorList() {
	notifyClear( true );
	delete list;
    }

    void add( Q3GListIterator* i ) {
	if ( !iterator ) {
	    iterator = i;
	} else if ( list ) {
	    list->push_front( i );
	} else {
	    list = new Q3ValueList<Q3GListIterator*>;
	    list->push_front( i );
	}
    }

    void remove( Q3GListIterator* i ) {
	if ( iterator == i ) {
	    iterator = 0;
	} else if ( list ) {
	    list->remove( i );
	    if ( list->isEmpty() ) {
		delete list;
		list = 0;
	    }
	}
    }

    void notifyClear( bool zeroList ) {
	if ( iterator ) {
	    if ( zeroList )
		iterator->list = 0;
	    iterator->curNode = 0;
	}
	if ( list ) {
	    for ( Q3ValueList<Q3GListIterator*>::Iterator i = list->begin(); i != list->end(); ++i ) {
		if ( zeroList )
		    (*i)->list = 0;
		(*i)->curNode = 0;
	    }
	}
    }

    void notifyRemove( Q3LNode* n, Q3LNode* curNode ) {
	if ( iterator ) {
	    if ( iterator->curNode == n )
		iterator->curNode = curNode;
	}
	if ( list ) {
	    for ( Q3ValueList<Q3GListIterator*>::Iterator i = list->begin(); i != list->end(); ++i ) {
		if ( (*i)->curNode == n )
		    (*i)->curNode = curNode;
	    }
	}
    }

private:
    Q3ValueList<Q3GListIterator*>* list;
    Q3GListIterator* iterator;
};



/*****************************************************************************
  Default implementation of virtual functions
 *****************************************************************************/

/*!
  Documented as Q3PtrList::compareItems().

  Compares \a item1 with \a item2.
*/
int Q3GList::compareItems( Q3PtrCollection::Item item1, Q3PtrCollection::Item item2 )
{
    return item1 != item2;			// compare pointers
}

#ifndef QT_NO_DATASTREAM
/*!
    \overload
  Reads a collection/list item from the stream \a s and returns a reference
  to the stream.

  The default implementation sets \a item to 0.

  \sa write()
*/

QDataStream &Q3GList::read( QDataStream &s, Q3PtrCollection::Item &item )
{
    item = 0;
    return s;
}

/*!
    \overload
  Writes a collection/list item to the stream \a s and
  returns a reference to the stream.

  The default implementation does nothing.

  \sa read()
*/

QDataStream &Q3GList::write( QDataStream &s, Q3PtrCollection::Item ) const
{
    return s;
}
#endif // QT_NO_DATASTREAM

/*****************************************************************************
  Q3GList member functions
 *****************************************************************************/

/*!
  Constructs an empty list.
*/

Q3GList::Q3GList()
{
    firstNode = lastNode = curNode = 0;		// initialize list
    numNodes  = 0;
    curIndex  = -1;
    iterators = 0;				// initialize iterator list
}

/*!
  Constructs a copy of \a list.
*/

Q3GList::Q3GList( const Q3GList & list )
    : Q3PtrCollection( list )
{
    firstNode = lastNode = curNode = 0;		// initialize list
    numNodes  = 0;
    curIndex  = -1;
    iterators = 0;				// initialize iterator list
    Q3LNode *n = list.firstNode;
    while ( n ) {				// copy all items from list
	append( n->data );
	n = n->next;
    }
}

/*!
  Removes all items from the list and destroys the list.
*/

Q3GList::~Q3GList()
{
    clear();
    delete iterators;
    // Workaround for GCC 2.7.* bug. Compiler constructs 'static' Q3GList
    // instances twice on the same address and therefore tries to destruct
    // twice on the same address! This is insane but let's try not to crash
    // here.
    iterators = 0;
}


/*!
  Assigns \a list to this list.
*/

Q3GList& Q3GList::operator=( const Q3GList &list )
{
    if ( &list == this )
	return *this;

    clear();
    if ( list.count() > 0 ) {
	Q3LNode *n = list.firstNode;
	while ( n ) {				// copy all items from list
	    append( n->data );
	    n = n->next;
	}
	curNode	 = firstNode;
	curIndex = 0;
    }
    return *this;
}

/*!
  Compares this list with \a list. Returns true if the lists
  contain the same data, otherwise false.
*/

bool Q3GList::operator==( const Q3GList &list ) const
{
    if ( count() != list.count() )
	return false;

    if ( count() == 0 )
	return true;

    Q3LNode *n1 = firstNode;
    Q3LNode *n2 = list.firstNode;
    while ( n1 && n2 ) {
	// should be mutable
	if ( ( (Q3GList*)this )->compareItems( n1->data, n2->data ) != 0 )
	    return false;
	n1 = n1->next;
	n2 = n2->next;
    }

    return true;
}

/*!
  \fn uint Q3GList::count() const

  Returns the number of items in the list.
*/


/*!
  Returns the node at position \a index.  Sets this node to current.
*/

Q3LNode *Q3GList::locate( uint index )
{
    if ( index == (uint)curIndex )		// current node ?
	return curNode;
    if ( !curNode && firstNode ) {		// set current node
	curNode	 = firstNode;
	curIndex = 0;
    }
    register Q3LNode *node;
    int	 distance = index - curIndex;		// node distance to cur node
    bool forward;				// direction to traverse

    if ( index >= numNodes )
	return 0;

    if ( distance < 0 )
	distance = -distance;
    if ( (uint)distance < index && (uint)distance < numNodes - index ) {
	node =	curNode;			// start from current node
	forward = index > (uint)curIndex;
    } else if ( index < numNodes - index ) {	// start from first node
	node = firstNode;
	distance = index;
	forward = true;
    } else {					// start from last node
	node = lastNode;
	distance = numNodes - index - 1;
	if ( distance < 0 )
	    distance = 0;
	forward = false;
    }
    if ( forward ) {				// now run through nodes
	while ( distance-- )
	    node = node->next;
    } else {
	while ( distance-- )
	    node = node->prev;
    }
    curIndex = index;				// must update index
    return curNode = node;
}


/*!
  Inserts item \a d at its sorted position in the list.
*/

void Q3GList::inSort( Q3PtrCollection::Item d )
{
    int index = 0;
    register Q3LNode *n = firstNode;
    while ( n && compareItems(n->data,d) < 0 ){ // find position in list
	n = n->next;
	index++;
    }
    insertAt( index, d );
}


/*!
  Inserts item \a d at the start of the list.
*/

void Q3GList::prepend( Q3PtrCollection::Item d )
{
    register Q3LNode *n = new Q3LNode( newItem(d) );
    Q_CHECK_PTR( n );
    n->prev = 0;
    if ( (n->next = firstNode) )		// list is not empty
	firstNode->prev = n;
    else					// initialize list
	lastNode = n;
    firstNode = curNode = n;			// curNode affected
    numNodes++;
    curIndex = 0;
}


/*!
  Inserts item \a d at the end of the list.
*/

void Q3GList::append( Q3PtrCollection::Item d )
{
    register Q3LNode *n = new Q3LNode( newItem(d) );
    Q_CHECK_PTR( n );
    n->next = 0;
    if ( (n->prev = lastNode) )			// list is not empty
	lastNode->next = n;
    else					// initialize list
	firstNode = n;
    lastNode = curNode = n;			// curNode affected
    curIndex = numNodes;
    numNodes++;
}


/*!
  Inserts item \a d at position \a index in the list.
*/

bool Q3GList::insertAt( uint index, Q3PtrCollection::Item d )
{
    if ( index == 0 ) {
	prepend( d );
	return true;
    } else if ( index == numNodes ) {
	append( d );
	return true;
    }
    Q3LNode *nextNode = locate( index );
    if ( !nextNode )
	return false;
    Q3LNode *prevNode = nextNode->prev;
    register Q3LNode *n = new Q3LNode( newItem(d) );
    Q_CHECK_PTR( n );
    nextNode->prev = n;
    prevNode->next = n;
    n->prev = prevNode;				// link new node into list
    n->next = nextNode;
    curNode = n;				// curIndex set by locate()
    numNodes++;
    return true;
}


/*!
  Relinks node \a n and makes it the first node in the list.
*/

void Q3GList::relinkNode( Q3LNode *n )
{
    if ( n == firstNode )			// already first
	return;
    curNode = n;
    unlink();
    n->prev = 0;
    if ( (n->next = firstNode) )		// list is not empty
	firstNode->prev = n;
    else					// initialize list
	lastNode = n;
    firstNode = curNode = n;			// curNode affected
    numNodes++;
    curIndex = 0;
}


/*!
  Unlinks the current list node and returns a pointer to this node.
*/

Q3LNode *Q3GList::unlink()
{
    if ( curNode == 0 )				// null current node
	return 0;
    register Q3LNode *n = curNode;		// unlink this node
    if ( n == firstNode ) {			// removing first node ?
	if ( (firstNode = n->next) ) {
	    firstNode->prev = 0;
	} else {
	    lastNode = curNode = 0;		// list becomes empty
	    curIndex = -1;
	}
    } else {
	if ( n == lastNode ) {			// removing last node ?
	    lastNode = n->prev;
	    lastNode->next = 0;
	} else {				// neither last nor first node
	    n->prev->next = n->next;
	    n->next->prev = n->prev;
	}
    }

    if ( n->next ) {                            // change current node
        curNode = n->next;
    } else if ( n->prev ) {
        curNode = n->prev;
        curIndex--;
    }

    if ( iterators )
	iterators->notifyRemove( n, curNode );
    numNodes--;
    return n;
}


/*!
  Removes the node \a n from the list.
*/

bool Q3GList::removeNode( Q3LNode *n )
{
#if defined(QT_CHECK_NULL)
    if ( n == 0 || (n->prev && n->prev->next != n) ||
	 (n->next && n->next->prev != n) ) {
	qWarning( "Q3GList::removeNode: Corrupted node" );
	return false;
    }
#endif
    curNode = n;
    unlink();					// unlink node
    deleteItem( n->data );			// deallocate this node
    delete n;
    curNode  = firstNode;
    curIndex = curNode ? 0 : -1;
    return true;
}

/*!
  Removes the item \a d from the list.	Uses compareItems() to find the item.

  If \a d is 0, removes the current item.
*/

bool Q3GList::remove( Q3PtrCollection::Item d )
{
    if ( d && find(d) == -1 )
	return false;
    Q3LNode *n = unlink();
    if ( !n )
	return false;
    deleteItem( n->data );
    delete n;
    return true;
}

/*!
  Removes the item \a d from the list.
*/

bool Q3GList::removeRef( Q3PtrCollection::Item d )
{
    if ( findRef(d) == -1 )
	return false;
    Q3LNode *n = unlink();
    if ( !n )
	return false;
    deleteItem( n->data );
    delete n;
    return true;
}

/*!
  \fn bool Q3GList::removeFirst()

  Removes the first item in the list.
*/

/*!
  \fn bool Q3GList::removeLast()

  Removes the last item in the list.
*/

/*!
  Removes the item at position \a index from the list.
*/

bool Q3GList::removeAt( uint index )
{
    if ( !locate(index) )
	return false;
    Q3LNode *n = unlink();
    if ( !n )
	return false;
    deleteItem( n->data );
    delete n;
    return true;
}


/*!
  Replaces the item at index \a index with \a d.
*/
bool Q3GList::replaceAt( uint index, Q3PtrCollection::Item d )
{
    Q3LNode *n = locate( index );
    if ( !n )
	return false;
    if ( n->data != d ) {
	deleteItem( n->data );
	n->data = newItem( d );
    }
    return true;
}



/*!
  Takes the node \a n out of the list.
*/

Q3PtrCollection::Item Q3GList::takeNode( Q3LNode *n )
{
#if defined(QT_CHECK_NULL)
    if ( n == 0 || (n->prev && n->prev->next != n) ||
	 (n->next && n->next->prev != n) ) {
	qWarning( "Q3GList::takeNode: Corrupted node" );
	return 0;
    }
#endif
    curNode = n;
    unlink();					// unlink node
    Item d = n->data;
    delete n;					// delete the node, not data
    curNode  = firstNode;
    curIndex = curNode ? 0 : -1;
    return d;
}

/*!
  Takes the current item out of the list.
*/

Q3PtrCollection::Item Q3GList::take()
{
    Q3LNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;					// delete node, keep contents
    return d;
}

/*!
  Takes the item at position \a index out of the list.
*/

Q3PtrCollection::Item Q3GList::takeAt( uint index )
{
    if ( !locate(index) )
	return 0;
    Q3LNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;					// delete node, keep contents
    return d;
}

/*!
  Takes the first item out of the list.
*/

Q3PtrCollection::Item Q3GList::takeFirst()
{
    first();
    Q3LNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;
    return d;
}

/*!
  Takes the last item out of the list.
*/

Q3PtrCollection::Item Q3GList::takeLast()
{
    last();
    Q3LNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;
    return d;
}


/*!
  Removes all items from the list.
*/

void Q3GList::clear()
{
    register Q3LNode *n = firstNode;

    firstNode = lastNode = curNode = 0;		// initialize list
    numNodes = 0;
    curIndex = -1;

    if ( iterators )
	iterators->notifyClear( false );

    Q3LNode *prevNode;
    while ( n ) {				// for all nodes ...
	deleteItem( n->data );			// deallocate data
	prevNode = n;
	n = n->next;
	delete prevNode;			// deallocate node
    }
}


/*!
  Finds item \a d in the list. If \a fromStart is true the search
  begins at the first node; otherwise it begins at the current node.
*/

int Q3GList::findRef( Q3PtrCollection::Item d, bool fromStart )
{
    register Q3LNode *n;
    int	     index;
    if ( fromStart ) {				// start from first node
	n = firstNode;
	index = 0;
    } else {					// start from current node
	n = curNode;
	index = curIndex;
    }
    while ( n && n->data != d ) {		// find exact match
	n = n->next;
	index++;
    }
    curNode = n;
    curIndex = n ? index : -1;
    return curIndex;				// return position of item
}

/*!
  Finds item \a d in the list using compareItems(). If \a fromStart is
  true the search begins at the first node; otherwise it begins at the
  current node.
*/

int Q3GList::find( Q3PtrCollection::Item d, bool fromStart )
{
    register Q3LNode *n;
    int	     index;
    if ( fromStart ) {				// start from first node
	n = firstNode;
	index = 0;
    } else {					// start from current node
	n = curNode;
	index = curIndex;
    }
    while ( n && compareItems(n->data,d) ){	// find equal match
	n = n->next;
	index++;
    }
    curNode = n;
    curIndex = n ? index : -1;
    return curIndex;				// return position of item
}


/*!
  Counts the number item \a d occurs in the list.
*/

uint Q3GList::containsRef( Q3PtrCollection::Item d ) const
{
    register Q3LNode *n = firstNode;
    uint     count = 0;
    while ( n ) {				// for all nodes...
	if ( n->data == d )			// count # exact matches
	    count++;
	n = n->next;
    }
    return count;
}

/*!
  Counts the number of times item \a d occurs in the list. Uses
  compareItems().
*/

uint Q3GList::contains( Q3PtrCollection::Item d ) const
{
    register Q3LNode *n = firstNode;
    uint     count = 0;
    Q3GList  *that = (Q3GList*)this;		// mutable for compareItems()
    while ( n ) {				// for all nodes...
	if ( !that->compareItems(n->data,d) )	// count # equal matches
	    count++;
	n = n->next;
    }
    return count;
}


/*!
  \fn Q3PtrCollection::Item Q3GList::at( uint index )
  \overload

  Sets the item at position \a index to the current item.
*/

/*!
  \fn int Q3GList::at() const

  Returns the current index.
*/

/*!
  \fn Q3LNode *Q3GList::currentNode() const

  Returns the current node.
*/

/*!
  \fn Q3PtrCollection::Item Q3GList::get() const

  Returns the current item.
*/

/*!
  \fn Q3PtrCollection::Item Q3GList::cfirst() const

  Returns the first item in the list.
*/

/*!
  \fn Q3PtrCollection::Item Q3GList::clast() const

  Returns the last item in the list.
*/


/*!
  Returns the first list item.	Sets this to current.
*/

Q3PtrCollection::Item Q3GList::first()
{
    if ( firstNode ) {
	curIndex = 0;
	return (curNode=firstNode)->data;
    }
    return 0;
}

/*!
  Returns the last list item.  Sets this to current.
*/

Q3PtrCollection::Item Q3GList::last()
{
    if ( lastNode ) {
	curIndex = numNodes-1;
	return (curNode=lastNode)->data;
    }
    return 0;
}

/*!
  Returns the next list item (after current).  Sets this to current.
*/

Q3PtrCollection::Item Q3GList::next()
{
    if ( curNode ) {
	if ( curNode->next ) {
	    curIndex++;
	    curNode = curNode->next;
	    return curNode->data;
	}
	curIndex = -1;
	curNode = 0;
    }
    return 0;
}

/*!
  Returns the previous list item (before current).  Sets this to current.
*/

Q3PtrCollection::Item Q3GList::prev()
{
    if ( curNode ) {
	if ( curNode->prev ) {
	    curIndex--;
	    curNode = curNode->prev;
	    return curNode->data;
	}
	curIndex = -1;
	curNode = 0;
    }
    return 0;
}


/*!
  Converts the list to a vector, \a vector.
*/

void Q3GList::toVector( Q3GVector *vector ) const
{
    vector->clear();
    if ( !vector->resize( count() ) )
	return;
    register Q3LNode *n = firstNode;
    uint i = 0;
    while ( n ) {
	vector->insert( i, n->data );
	n = n->next;
	i++;
    }
}

void Q3GList::heapSortPushDown( Q3PtrCollection::Item* heap, int first, int last )
{
    int r = first;
    while( r <= last/2 ) {
	// Node r has only one child ?
	if ( last == 2*r ) {
	    // Need for swapping ?
	    if ( compareItems( heap[r], heap[ 2*r ] ) > 0 ) {
		Q3PtrCollection::Item tmp = heap[r];
		heap[ r ] = heap[ 2*r ];
		heap[ 2*r ] = tmp;
	    }
	    // That's it ...
	    r = last;
	} else {
	    // Node has two children
	    if ( compareItems( heap[r], heap[ 2*r ] ) > 0 &&
		 compareItems( heap[ 2*r ], heap[ 2*r+1 ] ) <= 0 ) {
		// Swap with left child
		Q3PtrCollection::Item tmp = heap[r];
		heap[ r ] = heap[ 2*r ];
		heap[ 2*r ] = tmp;
		r *= 2;
	    } else if ( compareItems( heap[r], heap[ 2*r+1 ] ) > 0 &&
			compareItems( heap[ 2*r+1 ], heap[ 2*r ] ) < 0 ) {
		// Swap with right child
		Q3PtrCollection::Item tmp = heap[r];
		heap[ r ] = heap[ 2*r+1 ];
		heap[ 2*r+1 ] = tmp;
		r = 2*r+1;
	    } else {
		// We are done
		r = last;
	    }
	}
    }
}


/*! Sorts the list by the result of the virtual compareItems() function.

  The Heap-Sort algorithm is used for sorting.  It sorts n items with
  O(n*log n) compares.  This is the asymptotic optimal solution of the
  sorting problem.
*/

void Q3GList::sort()
{
    uint n = count();
    if ( n < 2 )
	return;

    // Create the heap
    Q3PtrCollection::Item* realheap = new Q3PtrCollection::Item[ n ];
    // Wow, what a fake. But I want the heap to be indexed as 1...n
    Q3PtrCollection::Item* heap = realheap - 1;
    int size = 0;
    Q3LNode* insert = firstNode;
    for( ; insert != 0; insert = insert->next ) {
	heap[++size] = insert->data;
	int i = size;
	while( i > 1 && compareItems( heap[i], heap[ i / 2 ] ) < 0 ) {
	    Q3PtrCollection::Item tmp = heap[ i ];
	    heap[ i ] = heap[ i/2 ];
	    heap[ i/2 ] = tmp;
	    i /= 2;
	}
    }

    insert = firstNode;
    // Now do the sorting
    for ( int i = n; i > 0; i-- ) {
	insert->data = heap[1];
	insert = insert->next;
	if ( i > 1 ) {
	    heap[1] = heap[i];
	    heapSortPushDown( heap, 1, i - 1 );
	}
    }

    delete [] realheap;
}


/*****************************************************************************
  Q3GList stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
QDataStream &operator>>( QDataStream &s, Q3GList &list )
{						// read list
    return list.read( s );
}

QDataStream &operator<<( QDataStream &s, const Q3GList &list )
{						// write list
    return list.write( s );
}

/*!
  Reads a list from the stream \a s.
*/

QDataStream &Q3GList::read( QDataStream &s )
{
    uint num;
    s >> num;					// read number of items
    clear();					// clear list
    while ( num-- ) {				// read all items
	Item d;
	read( s, d );
	Q_CHECK_PTR( d );
	if ( !d )				// no memory
	    break;
	Q3LNode *n = new Q3LNode( d );
	Q_CHECK_PTR( n );
	if ( !n )				// no memory
	    break;
	n->next = 0;
	if ( (n->prev = lastNode) )		// list is not empty
	    lastNode->next = n;
	else					// initialize list
	    firstNode = n;
	lastNode = n;
	numNodes++;
    }
    curNode  = firstNode;
    curIndex = curNode ? 0 : -1;
    return s;
}

/*!
  Writes the list to the stream \a s.
*/

QDataStream &Q3GList::write( QDataStream &s ) const
{
    s << count();				// write number of items
    Q3LNode *n = firstNode;
    while ( n ) {				// write all items
	write( s, n->data );
	n = n->next;
    }
    return s;
}

#endif // QT_NO_DATASTREAM



/*! \internal
 */
Q3LNode* Q3GList::erase( Q3LNode* it )
{
    Q3LNode* n = it;
    it = it->next;
    removeNode( n );
    return it;
}


/*****************************************************************************
  Q3GListIterator member functions
 *****************************************************************************/

/*!
  \class Q3GListIterator qglist.h
  \reentrant
  \brief The Q3GListIterator class is an internal class for implementing Q3PtrListIterator.

  \internal

  Q3GListIterator is a strictly internal class that does the heavy work for
  Q3PtrListIterator.
*/

/*!
  \internal
  Constructs an iterator that operates on the list \a l.
*/

Q3GListIterator::Q3GListIterator( const Q3GList &l )
{
    list = (Q3GList *)&l;			// get reference to list
    curNode = list->firstNode;			// set to first node
    if ( !list->iterators ) {
	list->iterators = new Q3GListIteratorList;		// create iterator list
	Q_CHECK_PTR( list->iterators );
    }
    list->iterators->add( this );		// attach iterator to list
}

/*!
  \internal
  Constructs a copy of the iterator \a it.
*/

Q3GListIterator::Q3GListIterator( const Q3GListIterator &it )
{
    list = it.list;
    curNode = it.curNode;
    if ( list )
	list->iterators->add( this );	// attach iterator to list
}

/*!
  \internal
  Assigns a copy of the iterator \a it and returns a reference to this
  iterator.
*/

Q3GListIterator &Q3GListIterator::operator=( const Q3GListIterator &it )
{
    if ( list )					// detach from old list
	list->iterators->remove( this );
    list = it.list;
    curNode = it.curNode;
    if ( list )
	list->iterators->add( this );	// attach to new list
    return *this;
}

/*!
  \internal
  Destroys the iterator.
*/

Q3GListIterator::~Q3GListIterator()
{
    if ( list )					// detach iterator from list
	list->iterators->remove(this);
}


/*!
  \fn bool Q3GListIterator::atFirst() const
  \internal
  Returns true if the iterator points to the first item, otherwise false.
*/

/*!
  \fn bool Q3GListIterator::atLast() const
  \internal
  Returns true if the iterator points to the last item, otherwise false.
*/


/*!
  \internal
  Sets the list iterator to point to the first item in the list.
*/

Q3PtrCollection::Item Q3GListIterator::toFirst()
{
    if ( !list ) {
#if defined(QT_CHECK_NULL)
	qWarning( "Q3GListIterator::toFirst: List has been deleted" );
#endif
	return 0;
    }
    return list->firstNode ? (curNode = list->firstNode)->getData() : 0;
}

/*!
  \internal
  Sets the list iterator to point to the last item in the list.
*/

Q3PtrCollection::Item Q3GListIterator::toLast()
{
    if ( !list ) {
#if defined(QT_CHECK_NULL)
	qWarning( "Q3GListIterator::toLast: List has been deleted" );
#endif
	return 0;
    }
    return list->lastNode ? (curNode = list->lastNode)->getData() : 0;
}


/*!
  \fn Q3PtrCollection::Item Q3GListIterator::get() const
  \internal
  Returns the iterator item.
*/


/*!
  \internal
  Moves to the next item (postfix).
*/

Q3PtrCollection::Item Q3GListIterator::operator()()
{
    if ( !curNode )
	return 0;
    Q3PtrCollection::Item d = curNode->getData();
    curNode = curNode->next;
    return  d;
}

/*!
  \internal
  Moves to the next item (prefix).
*/

Q3PtrCollection::Item Q3GListIterator::operator++()
{
    if ( !curNode )
	return 0;
    curNode = curNode->next;
    return curNode ? curNode->getData() : 0;
}

/*!
  \internal
  Moves \a jumps positions forward.
*/

Q3PtrCollection::Item Q3GListIterator::operator+=( uint jumps )
{
    while ( curNode && jumps-- )
	curNode = curNode->next;
    return curNode ? curNode->getData() : 0;
}

/*!
  \internal
  Moves to the previous item (prefix).
*/

Q3PtrCollection::Item Q3GListIterator::operator--()
{
    if ( !curNode )
	return 0;
    curNode = curNode->prev;
    return curNode ? curNode->getData() : 0;
}

/*!
  \internal
  Moves \a jumps positions backward.
*/

Q3PtrCollection::Item Q3GListIterator::operator-=( uint jumps )
{
    while ( curNode && jumps-- )
	curNode = curNode->prev;
    return curNode ? curNode->getData() : 0;
}

QT_END_NAMESPACE
