/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

/*
 * QXmlName is conceptually identical to QPatternist::QName. The
 * difference is that the latter is elegant, powerful and fast.
 *
 * However, it is too powerful and too open and not at all designed
 * for being public. QXmlName, in contrast, is only a public marker,
 * that for instance uses a qint64 instead of qint32, such that we in
 * the future can use that, if needed.
 */

#include "qnamepool_p.h"
#include "qxmlname.h"
#include "qxmlnamepool.h"
#include "qxpathhelper_p.h"
#include "private/qxmlutils_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QXmlName
  \brief The QXmlName class represents the name of an XML node, in an efficient, namespace-aware way.
  \reentrant
  \since 4.4
  \ingroup xml-tools

  QXmlName represents the name of an XML node in a way that
  is both efficient and safe for comparing names. Normally,
  an XML node represents an XML element or attribute, but
  QXmlName can also represent the names of other kinds of
  nodes, e.g., QAbstractXmlReceiver::processingInstruction()
  and QAbstractXmlReceiver::namespaceBinding().

  The name of an XML node has three components: The \e {namespace
  URI}, the \e {local name}, and the \e {prefix}. To see what these
  refer to in XML, consider the following snippet.

  \quotefile doc/src/snippets/patternist/mobeyDick.xml

  For the element named \e book, localName() returns \e book,
  namespaceUri() returns \e http://example.com/MyDefault,
  and prefix() returns an empty string. For the element named
  \e title, localName() returns \e title, namespaceUri() returns
  \e http://purl.org/dc/elements/1.1, and prefix() returns \e dc.

  To ensure that operations with QXmlName are efficient, e.g.,
  copying names and comparing them, each instance of QXmlName is
  associated with a \l {QXmlNamePool} {name pool}, which must be
  specified at QXmlName construction time. The three components
  of the QXmlName, i.e., the namespace URI, the local name, and
  the prefix, are stored in the name pool mapped to identifiers
  so they can be shared. For this reason, the only way to create
  a valid instance of QXmlName is to use the class constructor,
  where the \l {QXmlNamePool} {name pool}, local name, namespace
  URI, and prefix must all be specified.

  Note that QXmlName's default constructor constructs a null
  instance. It is typically used for allocating unused entries
  in collections of QXmlName.

  A side effect of associating each instance of QXmlName with
  a \l {QXmlNamePool} {name pool} is that each instance of
  QXmlName is tied to the QXmlNamePool with which it was created.
  However, the QXmlName class does not keep track of the name pool,
  so all the accessor functions, e.g., namespaceUri(), prefix(),
  localName(), and toClarkName() require that the correct name
  pool be passed to them. Failure to provide the correct name
  pool to these accessor functions results in undefined behavior.

  Note that a \l {QXmlNamePool} {name pool} is \e not an XML
  namespace. One \l {QXmlNamePool} {name pool} can represent
  instances of QXmlName from different XML namespaces, and the
  instances of QXmlName from one XML namespace can be distributed
  over multiple  \l {QXmlNamePool} {name pools}.

  \target Comparing QXmlNames
  \section1 Comparing QXmlNames

  To determine what a QXmlName refers to, the \e {namespace URI}
  and the \e {local name} are used. The \e prefix is not used
  because the prefix is simply a shorthand name for use in place
  of the normally much longer namespace URI. Nor is the prefix
  used in name comparisons. For example, the following two element
  nodes represent the same element and compare equal.

  \quotefile doc/src/snippets/patternist/svgDocumentElement.xml

  \quotefile doc/src/snippets/patternist/xsvgDocumentElement.xml

  Although the second name has the prefix \e x, the two names compare
  equal as instances of QXmlName, because the prefix is not used in
  the comparison.

  A local name can never be an empty string, although the prefix and
  namespace URI can. If the prefix is not empty, the namespace URI
  cannot be empty. Local names and prefixes must be valid
  \l {http://www.w3.org/TR/REC-xml-names/#NT-NCName} {NCNames},
  e.g., \e abc.def or \e abc123.

  QXmlName represents what is sometimes called an \e {expanded QName},
  or simply a QName.

  \sa {http://www.w3.org/TR/REC-xml-names/#NT-NCName} {Namespaces in XML 1.0 (Second Edition), [4] NCName}
 */

/*!
   \enum QXmlName::Constant
   \internal
   Various constants used in the QPatternist::NamePool and QXmlName.

   Setting of the mask enums use essentially this:

   \quotefile doc/src/snippets/code/src_xmlpatterns_api_qxmlname.cpp

   The masks, such as LocalNameMask, are positive. That is, for the
   area which the name resides, the bits are set.
 */

/*!
 Constructs a QXmlName instance that inserts \a localName,
 \a namespaceURI and \a prefix into \a namePool if they aren't
 already there. The accessor functions namespaceUri(), prefix(),
 localName(), and toClarkName() must be passed the \a namePool
 used here, so the \a namePool must remain in scope while the
 accessor functions might be used. However, two instances can
 be compared with \e {==} or \e {!=} and copied without the
 \a namePool.

 The user guarantees that the string components are valid for a
 QName. In particular, the local name, and the prefix (if present),
 must be valid \l {http://www.w3.org/TR/REC-xml-names/#NT-NCName}
 {NCNames}. The function isNCName() can be used to test validity
 of these names. The namespace URI should be an absolute URI.
 QUrl::isRelative() can be used to test whether the namespace URI
 is relative or absolute. Finally, providing a prefix is not valid
 when no namespace URI is provided.

 \a namePool is not copied. Nor is the reference to it retained
 in this instance. This constructor inserts the three strings
 into \a namePool.
 */
QXmlName::QXmlName(QXmlNamePool &namePool,
                   const QString &localName,
                   const QString &namespaceURI,
                   const QString &prefix)
{
    Q_ASSERT_X(prefix.isEmpty() || QXmlUtils::isNCName(prefix), Q_FUNC_INFO,
               "The prefix is invalid, maybe the arguments were mixed up?");
    Q_ASSERT_X(QXmlUtils::isNCName(localName), Q_FUNC_INFO,
               "The local name is invalid, maybe the arguments were mixed up?");

    m_qNameCode = namePool.d->allocateQName(namespaceURI, localName, prefix).code();
}

/*!
  \typedef QXmlName::Code
  \internal

   Stores the \l {QXmlNamePool} {name pool} identifiers for
   the namespace URI, local name, and prefix.
 */

/*!
  Returns true if this QXmlName is not initialized with a
  valid combination of \e {namespace URI}, \e {local name},
  and \e {prefix}.

  A valid local name is always required. The prefix and
  namespace URI can be empty, but if the prefix is not empty,
  the namespace URI must not be empty. Local names and
  prefixes must be valid
  \l {http://www.w3.org/TR/REC-xml-names/#NT-NCName} {NCNames},
  e.g., \e abc.def or \e abc123.
 */
bool QXmlName::isNull() const
{
    return m_qNameCode == InvalidCode;
}

/*!
  Constructs an uninitialized QXmlName. To build
  a valid QXmlName, you normally use the other constructor, which
  takes a \l {QXmlNamePool} {name pool}, namespace URI, local name,
  and prefix as parameters. But you can also use this constructor
  to build a null QXmlName and then assign an existing QXmlName
  to it.

  \sa isNull()
 */
QXmlName::QXmlName() : m_qNameCode(InvalidCode)
{
}

/*!
  \fn QXmlName::QXmlName(const NamespaceCode uri,
                         const LocalNameCode ln,
                         const PrefixCode p = 0)
  \internal
 */

/*!
  \fn QXmlName::hasPrefix() const
  \internal

  Returns true if this QXmlName has a non-empty prefix. If this
  function returns true, hasNamespace() will also return true,
  because a QXmlName can't have a prefix if it doesn't have a
  namespace URI.
 */

/*!
  \fn bool QXmlName::hasNamespace() const
  \internal

  Returns true if this QXmlName has a non-empty namespace URI.
 */

/*!
  \fn Code QXmlName::code() const
  \internal

  Returns the internal code that contains the id codes for the
  local name, prefix and namespace URI. It is opaque when used
  outside QXmlName, but it can be useful when one wants to put
  a QXmlName in a hash, and the prefix is significant.
 */

/*!
  Returns true if this QXmlName is equal to \a other; otherwise false.
  Two QXmlNames are equal if their namespace URIs are the same \e and
  their local names are the same. The prefixes are ignored.

  Note that it is meaningless to compare two instances of QXmlName
  that were created with different \l {QXmlNamePool} {name pools},
  but the attempt is not detected and the behavior is undefined.

  \sa operator!=()
 */
bool QXmlName::operator==(const QXmlName &other) const
{
    return (m_qNameCode & ExpandedNameMask) == (other.m_qNameCode & ExpandedNameMask);
}

/*!
  Returns true if this QXmlName is \e not equal to \a other;
  otherwise false. Two QXmlNames are equal if their namespace
  URIs are the same \e and their local names are the same. They
  are not equal if either their namespace URIs differ or their
  local names differ. Their prefixes are ignored.

  Note that it is meaningless to compare two instances of QXmlName
  that were created with different \l {QXmlNamePool} {name pools},
  but the attempt is not detected and the behavior is undefined.

  \sa operator==()
 */
bool QXmlName::operator!=(const QXmlName &other) const
{
    return !operator==(other);
}

/*!
  \fn bool QXmlName::isLexicallyEqual(const QXmlName &other) const
  \internal

  Returns true if this and \a other are lexically equal. Two
  QXmlNames are lexically equal if their local names are equal
  \e and their prefixes are equal.
 */

/*!
 \fn uint qHash(const QXmlName &name)
 \since 4.4
 \relates QXmlName

 Computes a hash key from the local name and the namespace
 URI in \a name. The prefix in \a name is not used in the computation.
 */
uint qHash(const QXmlName &name)
{
    return name.m_qNameCode & QXmlName::ExpandedNameMask;
}

/*!
 Returns the namespace URI.

 Note that for efficiency, the namespace URI string is not
 stored in the QXmlName but in the \l {QXmlNamePool} that was
 passed to the constructor. Hence, that same \a namePool must
 be passed to this function, so it can be used for looking up
 the namespace URI.
 */
QString QXmlName::namespaceUri(const QXmlNamePool &namePool) const
{
    if(isNull())
        return QString();
    else
        return namePool.d->stringForNamespace(namespaceURI());
}

/*!
 Returns the prefix.

 Note that for efficiency, the prefix string is not stored in
 the QXmlName but in the \l {QXmlNamePool} that was passed to
 the constructor. Hence, that same \a namePool must be passed
 to this function, so it can be used for looking up the prefix.
 */
QString QXmlName::prefix(const QXmlNamePool &namePool) const
{
    if(isNull())
        return QString();
    else
        return namePool.d->stringForPrefix(prefix());
}

/*!
 Returns the local name.

 Note that for efficiency, the local name string is not stored
 in the QXmlName but in the \l {QXmlNamePool} that was passed to
 the constructor. Hence, that same \a namePool must be passed
 to this function, so it can be used for looking up the
 local name.
 */
QString QXmlName::localName(const QXmlNamePool &namePool) const
{
    if(isNull())
        return QString();
    else
        return namePool.d->stringForLocalName(localName());
}

/*!
  Returns this QXmlName formatted as a Clark Name. For example,
  if the local name is \c html, the prefix is \c x, and the
  namespace URI is \c {http://www.w3.org/1999/xhtml/},
  then the Clark Name returned is:

  \code
  {http://www.w3.org/1999/xhtml/}x:html.
  \endcode

  If the local name is \e {MyWidget} and the namespace is empty,
  the Clark Name returned is:

  \code
  MyWidget
  \endcode

  Note that for efficiency, the namespace URI, local name, and
  prefix strings are not stored in the QXmlName but in the
  \l {QXmlNamePool} that was passed to the constructor. Hence,
  that same \a namePool must be passed to this function, so it
  can be used for looking up the three string components.

  This function can be useful for debugging.

 \sa {http://www.jclark.com/xml/xmlns.htm} {XML Namespaces, James Clark}
 \sa fromClarkName()
 */
QString QXmlName::toClarkName(const QXmlNamePool &namePool) const
{
    return namePool.d->toClarkName(*this);
}

/*!
  Assigns \a other to \e this and returns \e this.
 */
QXmlName &QXmlName::operator=(const QXmlName &other)
{
    m_qNameCode = other.m_qNameCode;
    return *this;
}

/*!
 Returns true if \a candidate is an \c NCName. An \c NCName
 is a string that can be used as a name in XML and XQuery,
 e.g., the prefix or local name in an element or attribute,
 or the name of a variable.

 \sa {http://www.w3.org/TR/REC-xml-names/#NT-NCName} {Namespaces in XML 1.0 (Second Edition), [4] NCName}
 */
bool QXmlName::isNCName(const QString &candidate)
{
    return QXmlUtils::isNCName(candidate);
}

/*!
  Converts \a clarkName into a QXmlName, inserts into \a namePool, and
  returns it.

  A clark name is a way to present a full QName with only one string, where
  the namespace cannot contain braces. Here are a couple of examples:

    \table
    \header
        \o Clark Name
        \o Description
    \row
        \o \c html
        \o The local name \c html, in no namespace
    \row
        \o \c {http://www.w3.org/1999/xhtml}html
        \o The local name \c html, in the XHTML namespace
    \row
        \o \c {http://www.w3.org/1999/xhtml}my:html
        \o The local name \c html, in the XHTML namespace, with the prefix \c my
    \endtable

    If the namespace contains braces, the returned value is either invalid or
    has undefined content.

    If \a clarkName is an invalid name, a default constructed QXmlName is
    returned.

  \since 4.5
  \sa toClarkName()
 */
QXmlName QXmlName::fromClarkName(const QString &clarkName,
                                 const QXmlNamePool &namePool)
{
    return namePool.d->fromClarkName(clarkName);
}

/*!
  \typedef QXmlName::LocalNameCode
  \internal
 */

/*!
  \typedef QXmlName::PrefixCode
  \internal
 */

/*!
  \typedef QXmlName::NamespaceCode
  \internal
 */

/*!
  \fn void QXmlName::setLocalName(const LocalNameCode c)
  \internal
*/

/*!
  \fn LocalNameCode QXmlName::localName() const
  \internal
*/

/*!
  \fn PrefixCode QXmlName::prefix() const
  \internal
*/

/*!
  \fn NamespaceCode QXmlName::namespaceURI() const
  \internal
*/

/*!
  \fn void QXmlName::setNamespaceURI(const NamespaceCode c)
  \internal
*/

/*!
  \fn void QXmlName::setPrefix(const PrefixCode c)
  \internal
*/
QT_END_NAMESPACE

