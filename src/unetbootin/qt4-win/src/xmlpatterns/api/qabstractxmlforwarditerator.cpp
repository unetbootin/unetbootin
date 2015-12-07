/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

/*!
  \class QAbstractXmlForwardIterator
  \brief The QAbstractXmlForwardIterator class is an abstract forward iterator.
  \reentrant
  \since 4.4
  \internal
  \ingroup xml-tools

   In order for an item to be able to be instantiated in QAbstractXmlForwardIterator, it needs to:
   - have a copy constructor, default constructor, and an assignment operator
   - An appropriate qIsForwardIteratorEnd() function

   @ingroup Patternist_iterators
   @author Frans Englich <fenglich@trolltech.com>
 */

/*!
 \typedef QAbstractXmlForwardIterator::Ptr

 A smart pointer wrapping an instance of a  QAbstractXmlForwardIterator subclass.
 */

/*!
 \typedef QAbstractXmlForwardIterator::List
 A QList instance, containing QAbstractXmlForwardIterator::Ptr instances.
 */

/*!
 \typedef QAbstractXmlForwardIterator::Vector
 A QVector instance, containing QAbstractXmlForwardIterator::Ptr instances.
 */

/*!
  \fn QAbstractXmlForwardIterator::QAbstractXmlForwardIterator()

  Default constructor.
 */

/*!
  \fn QAbstractXmlForwardIterator::~QAbstractXmlForwardIterator()

  Destructs this instance.
 */

/*!
  \fn T QAbstractXmlForwardIterator::next() = 0;

 Returns the next item in the sequence, or
 a null object if the end has been reached.
 */

/*!
  \fn T QAbstractXmlForwardIterator::current() const = 0;

  Returns the current item in the sequence. If it is called before any call
  to next(), a null object is returned. If the end of the sequence have been reached a null object
  is returned.
 */

/*!
  \fn qint64 QAbstractXmlForwardIterator::position() const = 0;

   Returns the current position in the sequence that this
   QAbstractXmlForwardIterator represents.

   The first position is 1, not 0. If next() hasn't been called, 0 is returned. If
   the end of this QAbstractXmlForwardIterator has been reached, -1 is returned.
 */

/*!
  \fn bool qIsForwardIteratorEnd(const T &unit)
  \relates QAbstractXmlForwardIterator

  Callback QAbstractXmlForwardIterator uses for determining whether \a unit is the end
  of a sequence.

  If \a unit is a value that would signal the end of a sequence(typically a default constructed value), this
  function returns \c true, otherwise \c false.

  This implementation works for all types that has a boolean operator. In other words,
  this function should work satisfactory for pointers, for example.
 */

/*!
  \fn qint64 QAbstractXmlForwardIterator::count()
  \internal

   Determines the amount of items this QAbstractXmlForwardIterator represents.

   Note that this function is not \c const, it modifies the QAbstractXmlForwardIterator
   as opposed to for example Qt's container classes functions by the same name. The
   reason for this is efficiency. If this QAbstractXmlForwardIterator should stay intact, one should
   use a copy().

   The default implementation simply calls next() until the end is reached. Hence, it may
   be of interest to override this function if the sub-class knows a better way of computing
   its count.

   Returns the size of the sequence, the number of items in the sequence.
 */

/*!
  \fn QAbstractXmlForwardIterator<T>::Ptr QAbstractXmlForwardIterator::toReversed();
  \internal

  Retrieves an iterator working in a reverse direction over the sequence.

  \note This function may modify the iterator, it can be considered a function
  that evaluates this QAbstractXmlForwardIterator. It is not a getter, but potentially alters
  the iterator in the same way the next() function does. If this QAbstractXmlForwardIterator should
  stay intact, such that it can be used for evaluation with next(), one should
  first copy this QAbstractXmlForwardIterator with the copy() function.

  Returns an iterator that iterates the sequence this iterator
  represents, in reverse order.
 */

/*!
  \fn QList<T> QAbstractXmlForwardIterator<T>::toList();
  \internal

   Performs a copy of this QAbstractXmlForwardIterator(with copy()), and returns its items
   in a QList. Thus, this function acts as a conversion function, by allowing
   a sequence of items to be converted into a QList, instead of being represented
   by an QAbstractXmlForwardIterator.

   @note This function may modify the iterator, it can be considered a function
   that evaluates this QAbstractXmlForwardIterator. It is not a getter, but potentially alters
   the iterator in the same way the next() function does. If this QAbstractXmlForwardIterator should
   stay intact, such that it can be used for evaluation with next(), one should
   first copy this QAbstractXmlForwardIterator with the copy() function.
 */

/*!
  \fn T QAbstractXmlForwardIterator::last();
  \internal

   Returns the item at the end of this QAbstractXmlForwardIterator.
   The default implementation calls next() until the end is reached.
 */

/*!
  \fn T QAbstractXmlForwardIterator::isEmpty();
  \internal


 */

/*!
  \fn qint64 QAbstractXmlForwardIterator::sizeHint() const;
  \internal

  Gives a hint to the size of the contained sequence. The hint is assumed
  to be as close as possible to the actual size.

  If no sensible estimation can be done at all about the
  size, -1 should be returned.
 */

/*!
  \fn typename QAbstractXmlForwardIterator<T>::Ptr QAbstractXmlForwardIterator::copy() const;
  \internal

   Copies this QAbstractXmlForwardIterator and returns the copy.

   A copy and the original instance are completely independent of each other, and
   doesn't affect each other in anyway. Since evaluating an QAbstractXmlForwardIterator modifies it,
   one should always use a copy when an QAbstractXmlForwardIterator needs to be used several times.

   Returns a copy of this QAbstractXmlForwardIterator.
 */

/*!
  \class QPatternist::ListIteratorPlatform
  \brief Helper class for ListIterator, and should only be instantiated through sub-classing.
  \reentrant
  \since 4.4
  \internal
  \ingroup xml-tools

   ListIteratorPlatform iterates an InputList with instances
   of InputType. For every item in it, it returns an item from it,
   that is converted to OutputType by calling a function on Derived
   that has the following signature:

   \snippet doc/src/snippets/code/src.xmlpatterns.api.qabstractxmlforwarditerator.cpp 0

   TODO Document why this class doesn't duplicate ItemMappingIterator.
 */

/*!
  \fn QPatternist::ListIteratorPlatform::ListIteratorPlatform(const InputList &list);

  Constructs a ListIteratorPlatform that walks the given \a list.
 */

/*!
  \class QPatternist::ListIterator
  \brief Bridges values in Qt's QList container class into an QAbstractXmlForwardIterator.
  \reentrant
  \since 4.4
  \internal
  \ingroup xml-tools

   ListIterator takes a reference to a QList<T> instance, and allows access
   to that list via its QAbstractXmlForwardIterator interface. ListIterator is parameterized on
   one argument, the type to iterate upon, such as Item or Expression::Ptr.

   ListIterator is for example used by the ExpressionSequence, to create an iterator
   over its operands that is subsequently passed to
   an MappingIterator.
 */

/*!
   \fn QPatternist::makeListIterator(const QList<T> &qList)
   \relates QPatternist::ListIterator

   An object generator for ListIterator.

   makeListIterator() is a convenience function for avoiding specifying
   the full template instantiation for ListIterator. Conceptually, it
   is identical to Qt's qMakePair().

 */
