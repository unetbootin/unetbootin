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

#include "qcommonnamespaces_p.h"
#include "qnamepool_p.h"

#include "qexternalenvironment_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/* The goal is to support serialization, backwards compatibility, import feature
 * and full axis featur, but not schema awareness. */

const xsDecimal ExternalEnvironment::XSLVersion                        (1.20);
const QString   ExternalEnvironment::Vendor                            (QLatin1String("The Patternist Team"));
const QUrl      ExternalEnvironment::VendorURL                         (QLatin1String("http://svg.kde.org/"));
const QString   ExternalEnvironment::ProductName                       (QLatin1String("Patternist"));
const QString   ExternalEnvironment::ProductVersion                    (QLatin1String("0.1"));
const bool      ExternalEnvironment::IsSchemaAware                     (false);
const bool      ExternalEnvironment::SupportsSerialization             (false);
const bool      ExternalEnvironment::SupportsBackwardsCompatibility    (false);

QString ExternalEnvironment::toString(const bool value)
{
    return value ? QLatin1String("yes") : QLatin1String("no");
}

QString ExternalEnvironment::retrieveProperty(const QXmlName name)
{
    if(name.namespaceURI() != StandardNamespaces::xslt)
        return QString();

    switch(name.localName())
    {
        case StandardLocalNames::version:
            return QString::number(ExternalEnvironment::XSLVersion);
        case StandardLocalNames::vendor:
            return ExternalEnvironment::Vendor;
        case StandardLocalNames::vendor_url:
            return QString(ExternalEnvironment::VendorURL.toString());
        case StandardLocalNames::product_name:
            return ExternalEnvironment::ProductName;
        case StandardLocalNames::product_version:
            return ExternalEnvironment::ProductVersion;
        case StandardLocalNames::is_schema_aware:
            return toString(ExternalEnvironment::IsSchemaAware);
        case StandardLocalNames::supports_serialization:
            return toString(ExternalEnvironment::SupportsSerialization);
        case StandardLocalNames::supports_backwards_compatibility:
            return toString(ExternalEnvironment::SupportsBackwardsCompatibility);
        default:
            return QString();
    }
}

QT_END_NAMESPACE
