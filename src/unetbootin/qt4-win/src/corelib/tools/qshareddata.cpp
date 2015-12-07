/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <qshareddata.h>

QT_BEGIN_NAMESPACE

/*! 
  \class QSharedData
  \brief The QSharedData class is a base class for shared data objects.
  \reentrant
  \ingroup misc

  QSharedData is designed to be used with QSharedDataPointer or
  QExplicitlySharedDataPointer to implement custom \l{implicitly
  shared} or \l {explicitly shared} classes. QSharedData provides
  \l{thread-safe} reference counting.

 See QSharedDataPointer and QExplicitlySharedDataPointer for details.
*/

/*! \fn QSharedData::QSharedData()
  Constructs a QSharedData object with a reference count of 0.
*/

/*! \fn QSharedData::QSharedData(const QSharedData& other)
  Constructs a QSharedData object with reference count 0.
  \a other is ignored.
*/

/*! 
  \class QSharedDataPointer
  \brief The QSharedDataPointer class represents a pointer to an implicitly shared object.
  \since 4.0
  \reentrant
  \ingroup misc
  \mainclass

  QSharedDataPointer\<T\> makes writing your own \l {implicitly
  shared} classes easy. QSharedDataPointer implements \l {thread-safe}
  reference counting, ensuring that adding QSharedDataPointers to your
  \l {reentrant} classes won't make them non-reentrant.

  \l {Implicit sharing} is used by many Qt classes to combine the
  speed and memory efficiency of pointers with the ease of use of
  classes. See the \l{Shared Classes} page for more information.

  \target Employee example
  Suppose you want to make an \c Employee class implicitly shared. The
  procedure is:

  \list

  \o Define the class \c Employee to have a single data member of
     type \c {QSharedDataPointer<EmployeeData>}.

  \o Define the \c EmployeeData class derived from \l QSharedData to
     contain all the data members you would normally have put in the
     \c Employee class.

  \endlist

  To show this in practice, we review the source code for the
  implicitly shared \c Employee class. In the header file we define the
  two classes \c Employee and \c EmployeeData.

  \snippet doc/src/snippets/sharedemployee/employee.h 0

  In class \c Employee, note the single data member, a \e {d pointer}
  of type \c {QSharedDataPointer<EmployeeData>}. All accesses of
  employee data must go through the \e {d pointer's} \c
  {operator->()}.  For write accesses, \c {operator->()} will
  automatically call detach(), which creates a copy of the shared data
  object if the shared data object's reference count is greater than
  1. This ensures that writes to one \c Employee object don't affect
  any other \c Employee objects that share the same \c EmployeeData
  object.

  Class \c EmployeeData inherits QSharedData, which provides the
  \e{behind the scenes} reference counter. \c EmployeeData has a default
  constructor, a copy constructor, and a destructor. Normally, trivial
  implementations of these are all that is needed in the \e {data}
  class for an implicitly shared class.

  Implementing the constructors for class \c Employee is also
  straightforward. Both create a new instance of \c EmployeeData
  and assign it to the \e{d pointer} .

  \snippet doc/src/snippets/sharedemployee/employee.h 1
  \codeline
  \snippet doc/src/snippets/sharedemployee/employee.h 2

  Behind the scenes, QSharedDataPointer automatically increments the
  reference count whenever an \c Employee object is copied, assigned,
  or passed as a parameter. It decrements the reference count whenever
  an \c Employee object is deleted or goes out of scope.  The shared
  \c EmployeeData object is deleted automatically if and when the
  reference count reaches 0.

  In a non-const member function of \c Employee, whenever the \e {d
  pointer} is dereferenced, QSharedDataPointer automatically calls
  detach() to ensure that the function operates on its own copy of the
  data.

  \snippet doc/src/snippets/sharedemployee/employee.h 3
  \codeline
  \snippet doc/src/snippets/sharedemployee/employee.h 4

  Note that if detach() is called more than once in a member function
  due to multiple dereferences of the \e {d pointer}, detach() will
  only create a copy of the shared data the first time it is called,
  if at all, because on the second and subsequent calls of detach(),
  the reference count will be 1 again.

  But note that in the second \c Employee constructor, which takes an
  employee ID and a name, both setId() and setName() are called, but
  they don't cause \e{copy on write}, because the reference count for
  the newly constructed \c EmployeeData object has just been set to 1.

  In \c Employee's \e const member functions, dereferencing the \e {d
  pointer} does \e not cause detach() to be called.

  \snippet doc/src/snippets/sharedemployee/employee.h 5
  \codeline
  \snippet doc/src/snippets/sharedemployee/employee.h 6

  Notice that there is no need to implement a copy constructor or an
  assignment operator for the \c Employee class, because the copy
  constructor and assignment operator provided by the C++ compiler
  will do the \e{member by member} shallow copy required. The only
  member to copy is the \e {d pointer}, which is a QSharedDataPointer,
  whose \c {operator=()} just increments the reference count of the
  shared \c EmployeeData object.

  \target Implicit vs Explicit Sharing
  \section1 Implicit vs Explicit Sharing

  Implicit sharing might not be right for the \c Employee class.
  Consider a simple example that creates two instances of the
  implicitly shared \c Employee class.

  \snippet doc/src/snippets/sharedemployee/main.cpp 0

  After the second employee e2 is created and e1 is assigned to it,
  both \c e1 and \c e2 refer to Albrecht Durer, employee 1001. Both \c
  Employee objects point to the same instance of \c EmployeeData,
  which has reference count 2. Then \c {e1.setName("Hans Holbein")} is
  called to change the employee name, but because the reference count
  is greater than 1, a \e{copy on write} is performed before the name
  is changed. Now \c e1 and \c e2 point to different \c EmployeeData
  objects. They have different names, but both have ID 1001, which is
  probably not what you want. You can, of course, just continue with
  \c {e1.setId(1002)}, if you really mean to create a second, unique
  employee, but if you only want to change the employee's name
  everywhere, consider using \l {QExplicitlySharedDataPointer}
  {explicit sharing} in the \c Employee class instead of implicit
  sharing.

  If you declare the \e {d pointer} in the \c Employee class to be
  \c {QExplicitlySharedDataPointer<EmployeeData>}, then explicit
  sharing is used and \e{copy on write} operations are not performed
  automatically (i.e. detach() is not called in non-const
  functions). In that case, after \c {e1.setName("Hans Holbein")}, the
  employee's name has been changed, but both e1 and e2 still refer to
  the same instance of \c EmployeeData, so there is only one employee
  with ID 1001.

  In the member function documentation, \e{d pointer} always refers
  to the internal pointer to the shared data object.

  \sa QSharedData, QExplicitlySharedDataPointer
*/

/*! \fn T& QSharedDataPointer::operator*()
  Provides access to the shared data object's members.
  This function calls detach().
*/

/*! \fn const T& QSharedDataPointer::operator*() const
  Provides const access to the shared data object's members.
  This function does \e not call detach().
*/

/*! \fn T* QSharedDataPointer::operator->()
  Provides access to the shared data object's members.
  This function calls detach().
*/

/*! \fn const T* QSharedDataPointer::operator->() const
  Provides const access to the shared data object's members.
  This function does \e not call detach().
*/

/*! \fn QSharedDataPointer::operator T*()
  Returns a pointer to the shared data object.
  This function calls detach().

  \sa data(), constData()
*/

/*! \fn QSharedDataPointer::operator const T*() const
  Returns a pointer to the shared data object.
  This function does \e not call detach().
*/

/*! \fn T* QSharedDataPointer::data()
  Returns a pointer to the shared data object.
  This function calls detach().

  \sa constData()
*/

/*! \fn const T* QSharedDataPointer::data() const
  Returns a pointer to the shared data object.
  This function does \e not call detach().
*/

/*! \fn const T* QSharedDataPointer::constData() const
  Returns a const pointer to the shared data object.
  This function does \e not call detach().

  \sa data()
*/

/*! \fn bool QSharedDataPointer::operator==(const QSharedDataPointer<T>& other) const
  Returns true if \a other and \e this have the same \e{d pointer}.
  This function does \e not call detach().
*/

/*! \fn bool QSharedDataPointer::operator!=(const QSharedDataPointer<T>& other) const
  Returns true if \a other and \e this do \e not have the same
  \e{d pointer}. This function does \e not call detach().
*/

/*! \fn QSharedDataPointer::QSharedDataPointer()
  Constructs a QSharedDataPointer initialized with a null \e{d pointer}.
*/

/*! \fn QSharedDataPointer::~QSharedDataPointer()
  Decrements the reference count of the shared data object.
  If the reference count becomes 0, the shared data object
  is deleted. \e This is then destroyed.
*/

/*! \fn QSharedDataPointer::QSharedDataPointer(T* sharedData)
  Constructs a QSharedDataPointer with \e{d pointer} set to
  \a sharedData and increments \a{sharedData}'s reference count.
*/

/*! \fn QSharedDataPointer::QSharedDataPointer(const QSharedDataPointer<T>& other)
  Sets the \e{d pointer} of \e this to the \e{d pointer} in
  \a other and increments the reference count of the shared
  data object.
*/

/*! \fn QSharedDataPointer<T>& QSharedDataPointer::operator=(const QSharedDataPointer<T>& other)
  Sets the \e{d pointer} of \e this to the \e{d pointer} of
  \a other and increments the reference count of the shared
  data object. The reference count of the old shared data
  object of \e this is decremented.  If the reference count
  of the old shared data object becomes 0, the old shared
  data object is deleted.
*/

/*! \fn QSharedDataPointer& QSharedDataPointer::operator=(T* sharedData)
  Sets the \e{d pointer} og \e this to \a sharedData and increments
  \a{sharedData}'s reference count. The reference count of the old
  shared data object of \e this is decremented.  If the reference
  count of the old shared data object becomes 0, the old shared data
  object is deleted.
*/

/*! \fn bool QSharedDataPointer::operator!() const
  Returns true if the \e{d pointer} of \e this is null.
*/

/*! \fn void QSharedDataPointer::detach()
  If the shared data object's reference count is greater than 1, this
  function creates a deep copy of the shared data object and sets the
  \e{d pointer} of \e this to the copy.
  
  This function is called automatically by non-const member
  functions of QSharedDataPointer if \e{copy on write} is
  required. You don't need to call it yourself.
*/

/*! 
  \class QExplicitlySharedDataPointer
  \brief The QExplicitlySharedDataPointer class represents a pointer to an explicitly shared object.
  \since 4.4
  \reentrant
  \ingroup misc
  \mainclass

  QExplicitlySharedDataPointer\<T\> makes writing your own explicitly
  shared classes easy. QExplicitlySharedDataPointre implements
  \l {thread-safe} reference counting, ensuring that adding
  QExplicitlySharedDataPointers to your \l {reentrant} classes won't
  make them non-reentrant.

  Except for one big difference, QExplicitlySharedDataPointer is just
  like QSharedDataPointer. The big difference is that member functions
  of QExplicitlySharedDataPointer \e{do not} do the automatic
  \e{copy on write} operation (detach()) that non-const members of
  QSharedDataPointer do before allowing the shared data object to be
  modified. There is a detach() function available, but if you really
  want to detach(), you have to call it yourself. This means that
  QExplicitlySharedDataPointers behave like regular C++ pointers,
  except that by doing reference counting and not deleting the shared
  data object until the reference count is 0, they avoid the dangling
  pointer problem.

  It is instructive to compare QExplicitlySharedDataPointer with
  QSharedDataPointer by way of an example. Consider the \l {Employee
  example} in QSharedDataPointer, modified to use explicit sharing as
  explained in the discussion \l {Implicit vs Explicit Sharing}.

  Note that if you use this class but find you are calling detach() a
  lot, you probably should be using QSharedDataPointer instead.

  In the member function documentation, \e{d pointer} always refers
  to the internal pointer to the shared data object.

  \sa QSharedData, QSharedDataPointer
*/

/*! \fn T& QExplicitlySharedDataPointer::operator*()
  Provides access to the shared data object's members.
*/

/*! \fn const T& QExplicitlySharedDataPointer::operator*() const
  Provides const access to the shared data object's members.
*/

/*! \fn T* QExplicitlySharedDataPointer::operator->()
  Provides access to the shared data object's members.
*/

/*! \fn const T* QExplicitlySharedDataPointer::operator->() const
  Provides const access to the shared data object's members.
*/

/*! \fn T* QExplicitlySharedDataPointer::data() const
  Returns a pointer to the shared data object.
*/

/*! \fn const T* QExplicitlySharedDataPointer::constData() const
  Returns a const pointer to the shared data object.

  \sa data()
*/

/*! \fn bool QExplicitlySharedDataPointer::operator==(const QExplicitlySharedDataPointer<T>& other) const
  Returns true if \a other and \e this have the same \e{d pointer}.
*/

/*! \fn bool QExplicitlySharedDataPointer::operator==(const T* ptr) const
  Returns true if the \e{d pointer} of \e this is \a ptr.
 */

/*! \fn bool QExplicitlySharedDataPointer::operator!=(const QExplicitlySharedDataPointer<T>& other) const
  Returns true if \a other and \e this do \e not have the same
  \e{d pointer}.
*/

/*! \fn bool QExplicitlySharedDataPointer::operator!=(const T* ptr) const
  Returns true if the \e{d pointer} of \e this is \e not \a ptr.
 */

/*! \fn QExplicitlySharedDataPointer::QExplicitlySharedDataPointer()
  Constructs a QExplicitlySharedDataPointer initialized with a null
  \e{d pointer}.
*/

/*! \fn QExplicitlySharedDataPointer::~QExplicitlySharedDataPointer()
  Decrements the reference count of the shared data object.
  If the reference count becomes 0, the shared data object
  is deleted. \e This is then destroyed.
*/

/*! \fn QExplicitlySharedDataPointer::QExplicitlySharedDataPointer(T* sharedData)
  Constructs a QExplicitlySharedDataPointer with \e{d pointer}
  set to \a sharedData and increments \a{sharedData}'s reference
  count.
*/

/*! \fn QExplicitlySharedDataPointer::QExplicitlySharedDataPointer(const QExplicitlySharedDataPointer<T>& other)
  This standard copy constructor sets the \e {d pointer} of \e this to
  the \e {d pointer} in \a other and increments the reference count of
  the shared data object.
*/

/*! \fn QExplicitlySharedDataPointer::QExplicitlySharedDataPointer(const QExplicitlySharedDataPointer<X>& other)
  This copy constructor is different in that it allows \a other to be
  a different type of explicitly shared data pointer but one that has
  a compatible shared data object. It performs a static cast of the
  \e{d pointer} in \a other and sets the \e {d pointer} of \e this to
  the converted \e{d pointer}. It increments the reference count of
  the shared data object.
 */

/*! \fn QExplicitlySharedDataPointer<T>& QExplicitlySharedDataPointer::operator=(const QExplicitlySharedDataPointer<T>& other)
  Sets the \e{d pointer} of \e this to the \e{d pointer} of
  \a other and increments the reference count of the shared
  data object. The reference count of the old shared data
  object of \e this is decremented.  If the reference count
  of the old shared data object becomes 0, the old shared
  data object is deleted.
*/

/*! \fn QExplicitlySharedDataPointer& QExplicitlySharedDataPointer::operator=(T* sharedData)
  Sets the \e{d pointer} of \e this to \a sharedData and
  increments \a{sharedData}'s reference count. The reference
  count of the old shared data object of \e this is decremented.
  If the reference count of the old shared data object becomes
  0, the old shared data object is deleted.
*/

/*! \fn void QExplicitlySharedDataPointer::reset()
  Resets \e this to be null. i.e., this function sets the
  \e{d pointer} of \e this to 0, but first it decrements
  the reference count of the shared data object and deletes
  the shared data object if the reference count became 0.
 */

/*! \fn QExplicitlySharedDataPointer::operator bool () const
  Returns true if the \e{d pointer} of \e this is \e not null.
 */

/*! \fn bool QExplicitlySharedDataPointer::operator!() const
  Returns true if the \e{d pointer} of \e this is null.
*/

/*! \fn void QExplicitlySharedDataPointer::detach()
  If the shared data object's reference count is greater than 1, this
  function creates a deep copy of the shared data object and sets the
  \e{d pointer} of \e this to the copy.

  Because QExplicitlySharedDataPointer does not do the automatic
  \e{copy on write} operations that members of QSharedDataPointer do,
  detach() is \e not called automatically anywhere in the member
  functions of this class. If you find that you are calling detach()
  everywhere in your code, consider using QSharedDataPointer instead.
*/

/*!
 \typedef QExplicitlySharedDataPointer::Type

 This is the type of the shared data object. The \e{d pointer}
 points to an object of this type.
 */

QT_END_NAMESPACE
