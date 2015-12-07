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

#include <QUrl>

#include "qabstractmessagehandler.h"

#include "qcommonnamespaces_p.h"
#include "qexpression_p.h"

#include "qreportcontext_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ReportContext::~ReportContext()
{
}

QString ReportContext::finalizeDescription(const QString &desc)
{
    return QLatin1String("<html xmlns='http://www.w3.org/1999/xhtml/'><body><p>")
           + desc
           + QLatin1String("</p></body></html>");
}

void ReportContext::warning(const QString &description,
                            const QSourceLocation &sourceLocation)
{
    messageHandler()->message(QtWarningMsg, finalizeDescription(description), QUrl(), sourceLocation);
}

void ReportContext::createError(const QString &description,
                                const QtMsgType type,
                                const QUrl &id,
                                const QSourceLocation &sourceLocation) const
{
    messageHandler()->message(type, finalizeDescription(description), id, sourceLocation);
    throw Exception(true);
}

void ReportContext::error(const QString &msg,
                          const ErrorCode code,
                          const QSourceLocation &sourceLocation)
{
    createError(msg, QtFatalMsg,
                QUrl(CommonNamespaces::XPERR + QLatin1Char('#') + codeToString(code)),
                sourceLocation);
}

QSourceLocation ReportContext::lookupSourceLocation(const SourceLocationReflection *const r) const
{
    Q_ASSERT(r);
    const SourceLocationReflection *const actual = r->actualReflection();
    Q_ASSERT(actual);

    const QSourceLocation &sl = actual->sourceLocation();

    if(sl.isNull())
    {
        Q_ASSERT_X(!locationFor(actual).isNull(), Q_FUNC_INFO,
                   qPrintable(QString::fromLatin1("No location is available for: %1").arg(actual->description())));
        return locationFor(actual);
    }
    else
        return sl;
}

void ReportContext::error(const QString &message,
                          const ReportContext::ErrorCode errorCode,
                          const SourceLocationReflection *const reflection)
{
    Q_ASSERT(reflection);
    error(message, errorCode, lookupSourceLocation(reflection));
}

void ReportContext::error(const QString &msg,
                          const QXmlName qname,
                          const SourceLocationReflection *const reflection)
{
    Q_ASSERT(!qname.isNull());
    createError(msg, QtFatalMsg,
                QUrl(namePool()->stringForNamespace(qname.namespaceURI()) + QLatin1Char('#') + namePool()->stringForLocalName(qname.localName())),
                lookupSourceLocation(reflection));
}

QString ReportContext::codeFromURI(const QString &typeURI,
                                   QString &uri)
{
    /* Wouldn't surprise me if this can be done more efficiently. */
    QUrl source(typeURI);

    const QString code(source.fragment());
    source.setFragment(QString());
    uri = source.toString();
    return code;
}

QString ReportContext::codeToString(const ReportContext::ErrorCode code)
{
    const char *result = 0;

    switch(code)
    {
        /* Alphabetically. */
        case FOAR0001: result = "FOAR0001"; break;
        case FOAR0002: result = "FOAR0002"; break;
        case FOCA0001: result = "FOCA0001"; break;
        case FOCA0002: result = "FOCA0002"; break;
        case FOCA0003: result = "FOCA0003"; break;
        case FOCA0005: result = "FOCA0005"; break;
        case FOCA0006: result = "FOCA0006"; break;
        case FOCH0001: result = "FOCH0001"; break;
        case FOCH0002: result = "FOCH0002"; break;
        case FOCH0003: result = "FOCH0003"; break;
        case FOCH0004: result = "FOCH0004"; break;
        case FODC0001: result = "FODC0001"; break;
        case FODC0002: result = "FODC0002"; break;
        case FODC0003: result = "FODC0003"; break;
        case FODC0004: result = "FODC0004"; break;
        case FODC0005: result = "FODC0005"; break;
        case FODT0001: result = "FODT0001"; break;
        case FODT0002: result = "FODT0002"; break;
        case FODT0003: result = "FODT0003"; break;
        case FOER0000: result = "FOER0000"; break;
        case FONS0004: result = "FONS0004"; break;
        case FONS0005: result = "FONS0005"; break;
        case FORG0001: result = "FORG0001"; break;
        case FORG0002: result = "FORG0002"; break;
        case FORG0003: result = "FORG0003"; break;
        case FORG0004: result = "FORG0004"; break;
        case FORG0005: result = "FORG0005"; break;
        case FORG0006: result = "FORG0006"; break;
        case FORG0008: result = "FORG0008"; break;
        case FORG0009: result = "FORG0009"; break;
        case FORX0001: result = "FORX0001"; break;
        case FORX0002: result = "FORX0002"; break;
        case FORX0003: result = "FORX0003"; break;
        case FORX0004: result = "FORX0004"; break;
        case FOTY0012: result = "FOTY0012"; break;
        case SENR0001: result = "SENR0001"; break;
        case SEPM0004: result = "SEPM0004"; break;
        case SEPM0009: result = "SEPM0009"; break;
        case SEPM0010: result = "SEPM0010"; break;
        case SEPM0016: result = "SEPM0016"; break;
        case SERE0003: result = "SERE0003"; break;
        case SERE0005: result = "SERE0005"; break;
        case SERE0006: result = "SERE0006"; break;
        case SERE0008: result = "SERE0008"; break;
        case SERE0012: result = "SERE0012"; break;
        case SERE0014: result = "SERE0014"; break;
        case SERE0015: result = "SERE0015"; break;
        case SESU0007: result = "SESU0007"; break;
        case SESU0011: result = "SESU0011"; break;
        case SESU0013: result = "SESU0013"; break;
        case XPDY0002: result = "XPDY0002"; break;
        case XPDY0021: result = "XPDY0021"; break;
        case XPDY0050: result = "XPDY0050"; break;
        case XPST0001: result = "XPST0001"; break;
        case XPST0003: result = "XPST0003"; break;
        case XPST0005: result = "XPST0005"; break;
        case XPST0008: result = "XPST0008"; break;
        case XPST0010: result = "XPST0010"; break;
        case XPST0017: result = "XPST0017"; break;
        case XPST0051: result = "XPST0051"; break;
        case XPST0080: result = "XPST0080"; break;
        case XPST0081: result = "XPST0081"; break;
        case XPST0083: result = "XPST0083"; break;
        case XPTY0004: result = "XPTY0004"; break;
        case XPTY0006: result = "XPTY0006"; break;
        case XPTY0007: result = "XPTY0007"; break;
        case XPTY0018: result = "XPTY0018"; break;
        case XPTY0019: result = "XPTY0019"; break;
        case XPTY0020: result = "XPTY0020"; break;
        case XQDY0025: result = "XQDY0025"; break;
        case XQDY0026: result = "XQDY0026"; break;
        case XQDY0027: result = "XQDY0027"; break;
        case XQDY0029: result = "XQDY0029"; break;
        case XQDY0041: result = "XQDY0041"; break;
        case XQDY0044: result = "XQDY0044"; break;
        case XQDY0052: result = "XQDY0052"; break;
        case XQDY0061: result = "XQDY0061"; break;
        case XQDY0062: result = "XQDY0062"; break;
        case XQDY0064: result = "XQDY0064"; break;
        case XQDY0072: result = "XQDY0072"; break;
        case XQDY0074: result = "XQDY0074"; break;
        case XQDY0084: result = "XQDY0084"; break;
        case XQDY0091: result = "XQDY0091"; break;
        case XQDY0092: result = "XQDY0092"; break;
        case XQST0009: result = "XQST0009"; break;
        case XQST0012: result = "XQST0012"; break;
        case XQST0013: result = "XQST0013"; break;
        case XQST0014: result = "XQST0014"; break;
        case XQST0015: result = "XQST0015"; break;
        case XQST0016: result = "XQST0016"; break;
        case XQST0022: result = "XQST0022"; break;
        case XQST0031: result = "XQST0031"; break;
        case XQST0032: result = "XQST0032"; break;
        case XQST0033: result = "XQST0033"; break;
        case XQST0034: result = "XQST0034"; break;
        case XQST0035: result = "XQST0035"; break;
        case XQST0036: result = "XQST0036"; break;
        case XQST0037: result = "XQST0037"; break;
        case XQST0038: result = "XQST0038"; break;
        case XQST0039: result = "XQST0039"; break;
        case XQST0040: result = "XQST0040"; break;
        case XQST0042: result = "XQST0042"; break;
        case XQST0043: result = "XQST0043"; break;
        case XQST0045: result = "XQST0045"; break;
        case XQST0046: result = "XQST0046"; break;
        case XQST0047: result = "XQST0047"; break;
        case XQST0048: result = "XQST0048"; break;
        case XQST0049: result = "XQST0049"; break;
        case XQST0053: result = "XQST0053"; break;
        case XQST0054: result = "XQST0054"; break;
        case XQST0055: result = "XQST0055"; break;
        case XQST0056: result = "XQST0056"; break;
        case XQST0057: result = "XQST0057"; break;
        case XQST0058: result = "XQST0058"; break;
        case XQST0059: result = "XQST0059"; break;
        case XQST0060: result = "XQST0060"; break;
        case XQST0063: result = "XQST0063"; break;
        case XQST0065: result = "XQST0065"; break;
        case XQST0066: result = "XQST0066"; break;
        case XQST0067: result = "XQST0067"; break;
        case XQST0068: result = "XQST0068"; break;
        case XQST0069: result = "XQST0069"; break;
        case XQST0070: result = "XQST0070"; break;
        case XQST0071: result = "XQST0071"; break;
        case XQST0073: result = "XQST0073"; break;
        case XQST0075: result = "XQST0075"; break;
        case XQST0076: result = "XQST0076"; break;
        case XQST0077: result = "XQST0077"; break;
        case XQST0078: result = "XQST0078"; break;
        case XQST0079: result = "XQST0079"; break;
        case XQST0082: result = "XQST0082"; break;
        case XQST0085: result = "XQST0085"; break;
        case XQST0087: result = "XQST0087"; break;
        case XQST0088: result = "XQST0088"; break;
        case XQST0089: result = "XQST0089"; break;
        case XQST0090: result = "XQST0090"; break;
        case XQST0093: result = "XQST0093"; break;
        case XQTY0023: result = "XQTY0023"; break;
        case XQTY0024: result = "XQTY0024"; break;
        case XQTY0028: result = "XQTY0028"; break;
        case XQTY0030: result = "XQTY0030"; break;
        case XQTY0086: result = "XQTY0086"; break;
        case XTDE0030: result = "XTDE0030"; break;
        case XTDE0040: result = "XTDE0040"; break;
        case XTDE0045: result = "XTDE0045"; break;
        case XTDE0047: result = "XTDE0047"; break;
        case XTDE0050: result = "XTDE0050"; break;
        case XTDE0060: result = "XTDE0060"; break;
        case XTDE0160: result = "XTDE0160"; break;
        case XTDE0290: result = "XTDE0290"; break;
        case XTDE0410: result = "XTDE0410"; break;
        case XTDE0420: result = "XTDE0420"; break;
        case XTDE0430: result = "XTDE0430"; break;
        case XTDE0440: result = "XTDE0440"; break;
        case XTDE0485: result = "XTDE0485"; break;
        case XTDE0560: result = "XTDE0560"; break;
        case XTDE0610: result = "XTDE0610"; break;
        case XTDE0640: result = "XTDE0640"; break;
        case XTDE0700: result = "XTDE0700"; break;
        case XTDE0820: result = "XTDE0820"; break;
        case XTDE0830: result = "XTDE0830"; break;
        case XTDE0835: result = "XTDE0835"; break;
        case XTDE0850: result = "XTDE0850"; break;
        case XTDE0855: result = "XTDE0855"; break;
        case XTDE0860: result = "XTDE0860"; break;
        case XTDE0865: result = "XTDE0865"; break;
        case XTDE0890: result = "XTDE0890"; break;
        case XTDE0905: result = "XTDE0905"; break;
        case XTDE0920: result = "XTDE0920"; break;
        case XTDE0925: result = "XTDE0925"; break;
        case XTDE0930: result = "XTDE0930"; break;
        case XTDE0980: result = "XTDE0980"; break;
        case XTDE1030: result = "XTDE1030"; break;
        case XTDE1035: result = "XTDE1035"; break;
        case XTDE1110: result = "XTDE1110"; break;
        case XTDE1140: result = "XTDE1140"; break;
        case XTDE1145: result = "XTDE1145"; break;
        case XTDE1150: result = "XTDE1150"; break;
        case XTDE1170: result = "XTDE1170"; break;
        case XTDE1190: result = "XTDE1190"; break;
        case XTDE1200: result = "XTDE1200"; break;
        case XTDE1260: result = "XTDE1260"; break;
        case XTDE1270: result = "XTDE1270"; break;
        case XTDE1280: result = "XTDE1280"; break;
        case XTDE1310: result = "XTDE1310"; break;
        case XTDE1340: result = "XTDE1340"; break;
        case XTDE1350: result = "XTDE1350"; break;
        case XTDE1360: result = "XTDE1360"; break;
        case XTDE1370: result = "XTDE1370"; break;
        case XTDE1380: result = "XTDE1380"; break;
        case XTDE1390: result = "XTDE1390"; break;
        case XTDE1400: result = "XTDE1400"; break;
        case XTDE1420: result = "XTDE1420"; break;
        case XTDE1425: result = "XTDE1425"; break;
        case XTDE1428: result = "XTDE1428"; break;
        case XTDE1440: result = "XTDE1440"; break;
        case XTDE1450: result = "XTDE1450"; break;
        case XTDE1460: result = "XTDE1460"; break;
        case XTDE1480: result = "XTDE1480"; break;
        case XTDE1490: result = "XTDE1490"; break;
        case XTDE1665: result = "XTDE1665"; break;
        case XTMM9000: result = "XTMM9000"; break;
        case XTRE0270: result = "XTRE0270"; break;
        case XTRE0540: result = "XTRE0540"; break;
        case XTRE0795: result = "XTRE0795"; break;
        case XTRE1160: result = "XTRE1160"; break;
        case XTRE1495: result = "XTRE1495"; break;
        case XTRE1500: result = "XTRE1500"; break;
        case XTRE1620: result = "XTRE1620"; break;
        case XTRE1630: result = "XTRE1630"; break;
        case XTSE0010: result = "XTSE0010"; break;
        case XTSE0020: result = "XTSE0020"; break;
        case XTSE0080: result = "XTSE0080"; break;
        case XTSE0090: result = "XTSE0090"; break;
        case XTSE0110: result = "XTSE0110"; break;
        case XTSE0120: result = "XTSE0120"; break;
        case XTSE0125: result = "XTSE0125"; break;
        case XTSE0130: result = "XTSE0130"; break;
        case XTSE0150: result = "XTSE0150"; break;
        case XTSE0165: result = "XTSE0165"; break;
        case XTSE0170: result = "XTSE0170"; break;
        case XTSE0180: result = "XTSE0180"; break;
        case XTSE0190: result = "XTSE0190"; break;
        case XTSE0200: result = "XTSE0200"; break;
        case XTSE0210: result = "XTSE0210"; break;
        case XTSE0215: result = "XTSE0215"; break;
        case XTSE0220: result = "XTSE0220"; break;
        case XTSE0260: result = "XTSE0260"; break;
        case XTSE0265: result = "XTSE0265"; break;
        case XTSE0280: result = "XTSE0280"; break;
        case XTSE0340: result = "XTSE0340"; break;
        case XTSE0350: result = "XTSE0350"; break;
        case XTSE0370: result = "XTSE0370"; break;
        case XTSE0500: result = "XTSE0500"; break;
        case XTSE0530: result = "XTSE0530"; break;
        case XTSE0550: result = "XTSE0550"; break;
        case XTSE0580: result = "XTSE0580"; break;
        case XTSE0620: result = "XTSE0620"; break;
        case XTSE0630: result = "XTSE0630"; break;
        case XTSE0650: result = "XTSE0650"; break;
        case XTSE0660: result = "XTSE0660"; break;
        case XTSE0670: result = "XTSE0670"; break;
        case XTSE0680: result = "XTSE0680"; break;
        case XTSE0690: result = "XTSE0690"; break;
        case XTSE0710: result = "XTSE0710"; break;
        case XTSE0720: result = "XTSE0720"; break;
        case XTSE0740: result = "XTSE0740"; break;
        case XTSE0760: result = "XTSE0760"; break;
        case XTSE0770: result = "XTSE0770"; break;
        case XTSE0805: result = "XTSE0805"; break;
        case XTSE0808: result = "XTSE0808"; break;
        case XTSE0809: result = "XTSE0809"; break;
        case XTSE0810: result = "XTSE0810"; break;
        case XTSE0812: result = "XTSE0812"; break;
        case XTSE0840: result = "XTSE0840"; break;
        case XTSE0870: result = "XTSE0870"; break;
        case XTSE0880: result = "XTSE0880"; break;
        case XTSE0910: result = "XTSE0910"; break;
        case XTSE0940: result = "XTSE0940"; break;
        case XTSE0975: result = "XTSE0975"; break;
        case XTSE1015: result = "XTSE1015"; break;
        case XTSE1017: result = "XTSE1017"; break;
        case XTSE1040: result = "XTSE1040"; break;
        case XTSE1060: result = "XTSE1060"; break;
        case XTSE1070: result = "XTSE1070"; break;
        case XTSE1080: result = "XTSE1080"; break;
        case XTSE1090: result = "XTSE1090"; break;
        case XTSE1130: result = "XTSE1130"; break;
        case XTSE1205: result = "XTSE1205"; break;
        case XTSE1210: result = "XTSE1210"; break;
        case XTSE1220: result = "XTSE1220"; break;
        case XTSE1290: result = "XTSE1290"; break;
        case XTSE1295: result = "XTSE1295"; break;
        case XTSE1300: result = "XTSE1300"; break;
        case XTSE1430: result = "XTSE1430"; break;
        case XTSE1505: result = "XTSE1505"; break;
        case XTSE1520: result = "XTSE1520"; break;
        case XTSE1530: result = "XTSE1530"; break;
        case XTSE1560: result = "XTSE1560"; break;
        case XTSE1570: result = "XTSE1570"; break;
        case XTSE1580: result = "XTSE1580"; break;
        case XTSE1590: result = "XTSE1590"; break;
        case XTSE1600: result = "XTSE1600"; break;
        case XTSE1650: result = "XTSE1650"; break;
        case XTSE1660: result = "XTSE1660"; break;
        case XTTE0505: result = "XTTE0505"; break;
        case XTTE0510: result = "XTTE0510"; break;
        case XTTE0520: result = "XTTE0520"; break;
        case XTTE0570: result = "XTTE0570"; break;
        case XTTE0590: result = "XTTE0590"; break;
        case XTTE0600: result = "XTTE0600"; break;
        case XTTE0780: result = "XTTE0780"; break;
        case XTTE0790: result = "XTTE0790"; break;
        case XTTE0950: result = "XTTE0950"; break;
        case XTTE0990: result = "XTTE0990"; break;
        case XTTE1000: result = "XTTE1000"; break;
        case XTTE1020: result = "XTTE1020"; break;
        case XTTE1100: result = "XTTE1100"; break;
        case XTTE1120: result = "XTTE1120"; break;
        case XTTE1510: result = "XTTE1510"; break;
        case XTTE1512: result = "XTTE1512"; break;
        case XTTE1515: result = "XTTE1515"; break;
        case XTTE1540: result = "XTTE1540"; break;
        case XTTE1545: result = "XTTE1545"; break;
        case XTTE1550: result = "XTTE1550"; break;
        case XTTE1555: result = "XTTE1555"; break;
    }

    Q_ASSERT_X(result, Q_FUNC_INFO, "Unknown enum value.");
    return QLatin1String(result);
}

QUrl ReportContext::resolveURI(const QUrl &relative,
                               const QUrl &baseURI) const
{
    Q_ASSERT_X(!baseURI.isRelative(), Q_FUNC_INFO,
               "The base URI passed from the engine wasn't absolute.");

    const QAbstractUriResolver *const resolver(uriResolver());

    if(resolver)
    {
        const QUrl final(resolver->resolve(relative, baseURI));
        Q_ASSERT_X(final.isValid() || final.isEmpty(), Q_FUNC_INFO,
                   "The QAbstractUriResolver must return a valid URI.");
        Q_ASSERT_X(!final.isRelative(), Q_FUNC_INFO,
                   "The QAbstractUriResolver must return an absolute URI.");
        return final;
    }
    else
        return baseURI.resolved(relative);
}

QT_END_NAMESPACE
