/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QUrl>

#include "qabstracturiresolver.h"

QT_BEGIN_NAMESPACE

/*!
  \class QAbstractUriResolver
  \brief The QAbstractUriResolver class provides a callback interface that is used to rewrite one URI as another.
  \since 4.4
  \reentrant
  \ingroup xml-tools

  Uniform Resource Identifiers, URIs, are strings that identify
  or name a resource. That URIs are not more specific than that, makes
  them versatelite, global identifiers.

  However, in some circumstances it can be useful to turn a URI that
  identifies something logical, into a URI that locates something physical(an URL),
  or to simply rewrite one URI into a completely different. QAbstractUriResolver
  offers this functionality through its resolve() function.

  For instance, one could write a QAbstractUriResolver subclass that rewrites \c urn:isbn:0-345-33973-8
  into an actual file: \c file:///books/returnOfTheKing.doc. Or a web browser could disallow
  certain URIs, as part of protecting the user's private files from malicious scripts.

  \sa {http://en.wikipedia.org/wiki/Uniform_Resource_Identifier} {Wikipedia, Uniform Resource Identifier}
*/

/*!
  Constructs a QAbstractUriResolver instance.

  The argument \a parent is sent to the QObject's constructor.
 */
QAbstractUriResolver::QAbstractUriResolver(QObject *parent) : QObject(parent)
{
}

/*!
  Destructs this QAbstractUriResolver instance.
 */
QAbstractUriResolver::~QAbstractUriResolver()
{
}

/*!
    \fn QUrl QAbstractUriResolver::resolve(const QUrl &relative,
                                           const QUrl &baseURI) const = 0;

    Given \a relative and \a baseURI, returns the URI that actually should be used.

    \a baseURI is the URI that the caller would use for resolving \a relative
    into an absolute URI.

    The caller guarantees that \a baseURI is valid and absolute, and that \a relative is
    valid or empty and may be relative or absolute.

    The implementation guarantees that the returned QUrl is absolute or a
    default constructed QUrl. In other cases, effects are undefined. If a default
    constructed QUrl is returned, it signals that this QAbstractUriResolver did not accept
    the URI requested to be resolved.

    This means if this QAbstractUriResolvers has no particular task to do with the URI, it
    should simply return \a relative resolved against \a baseURI, which is done by:

    \snippet doc/src/snippets/code/src.xmlpatterns.api.qabstracturiresolver.cpp 0

  \sa QUrl::isRelative(), QUrl::isValid()
 */

QT_END_NAMESPACE

