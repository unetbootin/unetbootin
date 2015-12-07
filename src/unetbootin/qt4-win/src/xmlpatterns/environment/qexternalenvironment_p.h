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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_ExternalEvironment_H
#define Patternist_ExternalEvironment_H

#include <QString>
#include <QUrl>

#include "qprimitives_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains static values and functions related to retrieving
     * information about the implementation.
     *
     * This is notably used for supporting XSL-T's <tt>fn:system-property()</tt> function.
     *
     * The values are stored in native formats, and their string representation,
     * as defined in the specification, can be accessed via retrieveProperty().
     *
     * @see <a href="http://www.w3.org/TR/xslt20/#system-property">XSL Transformations
     * (XSLT) Version 2.0, 16.6.5 system-property</a>
     * @author Frans Englich <fenglich@trolltech.com>
      */
    class ExternalEnvironment
    {
    public:
        /**
         * The supported XSL-T version.
         *
         * This value is currently not 2.0 even though Patternist is intended to
         * be an XSL-T 2.0 processor. This is because Patternist is not comformant.
         *
         * @see <a href="http://www.w3.org/TR/xslt20/#system-property">The Note paragraph
         * at the very end of XSL Transformations (XSLT) Version 2.0,
         * 16.6.5 system-property</a>
         */
        static const xsDecimal XSLVersion;

        static const QString Vendor;
        static const QUrl VendorURL;
        static const QString ProductName;
        static const QString ProductVersion;
        static const bool IsSchemaAware;
        static const bool SupportsSerialization;
        static const bool SupportsBackwardsCompatibility;

        /**
         * Returns a string representation for @p property as defined
         * for the system properties in "XSL Transformations (XSLT)
         * Version 2.0, 16.6.5 system-property". Hence, this function
         * handles only the properties specified in the XSL namespace, and returns
         * an empty string if an unrecognized property is asked for.
         */
        static QString retrieveProperty(const QXmlName name);

    private:
        static inline QString toString(const bool);
        /**
         * This class is not supposed to be instantiated. No implementation
         * is provided for this constructor.
         */
        ExternalEnvironment();
        Q_DISABLE_COPY(ExternalEnvironment)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
