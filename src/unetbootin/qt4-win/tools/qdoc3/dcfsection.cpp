/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include "dcfsection.h"
#include "htmlgenerator.h"

QT_BEGIN_NAMESPACE

void appendDcfSubSection( DcfSection *dcfSect, const DcfSection& sub )
{
    dcfSect->subsections.append( sub );
}

void appendDcfSubSections( DcfSection *dcfSect, const QList<DcfSection>& subs )
{
    dcfSect->subsections += subs;
}

void generateDcfSubSections( QString indent, QTextStream& out, const DcfSection& sect )
{
    QList<DcfSection>::const_iterator ss = sect.subsections.constBegin();
    while ( ss != sect.subsections.constEnd() ) {
	out << indent << "<section ref=\"" << HtmlGenerator::protect((*ss).ref)
            << "\" title=\"" << HtmlGenerator::protect((*ss).title) << "\"";
	if ((*ss).keywords.isEmpty() && (*ss).subsections.isEmpty()) {
	    out << "/>\n";
	} else {
	    out << ">\n";
	    QString indentIndent = indent + "    ";
	    QList<QPair<QString, QString> >::const_iterator k = (*ss).keywords.constBegin();
	    while ( k != (*ss).keywords.constEnd() ) {
		out << indentIndent << "<keyword ref=\"" << (*k).second << "\">"
                    << HtmlGenerator::protect((*k).first) << "</keyword>\n";
		++k;
	    }

            generateDcfSubSections( indentIndent, out, *ss );
	    out << indent << "</section>\n";
	}
	++ss;
    }
    out.flush();
}

void generateDcfSections( const DcfSection& rootSect, const QString& fileName,
			  const QString& /* category */ )
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return ;

    QTextStream out(&file);

    QString icon = QFileInfo(fileName).baseName() + ".png";

    out << "<!DOCTYPE DCF>\n";
    out << "<DCF ref=\"" << HtmlGenerator::protect(rootSect.ref);
    if (icon != "qmake.png")
	out << "\" icon=\"" << HtmlGenerator::protect(icon);
    out << "\" imagedir=\"../../gif\" title=\"" << HtmlGenerator::protect(rootSect.title) +
	      "\">\n";

    generateDcfSubSections( "", out, rootSect );

    out << "</DCF>\n";
    out.flush();
}

QT_END_NAMESPACE
