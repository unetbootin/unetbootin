/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QFile>

#include "qhcpwriter.h"

QT_BEGIN_NAMESPACE

QhcpWriter::QhcpWriter()
{
    setAutoFormatting(true);
}

void QhcpWriter::setHelpProjectFile(const QString &qhpFile)
{
    m_qhpFile = qhpFile;    
}

void QhcpWriter::setProperties(const QMap<QString, QString> props)
{
    m_properties = props;
}

void QhcpWriter::setTitlePath(const QString &path)
{
    m_titlePath = path;
}

bool QhcpWriter::writeFile(const QString &fileName)
{
    QFile out(fileName);
    if (!out.open(QIODevice::WriteOnly))
        return false;
    
    setDevice(&out);
    writeStartDocument();
    writeStartElement(QLatin1String("QHelpCollectionProject"));
    writeAttribute(QLatin1String("version"), QLatin1String("1.0"));
    writeAssistantSettings();
    writeDocuments();
    writeEndDocument();
    return true;
}

void QhcpWriter::writeAssistantSettings()
{
    if (m_properties.isEmpty())
        return;

    writeStartElement(QLatin1String("assistant"));

    if (m_properties.contains(QLatin1String("title")))
        writeTextElement(QLatin1String("title"), m_properties.value(QLatin1String("title")));
    if (m_properties.contains(QLatin1String("applicationicon")))
        writeTextElement(QLatin1String("applicationIcon"),
            m_properties.value(QLatin1String("applicationicon")));
    if (m_properties.contains(QLatin1String("startpage")))
        writeTextElement(QLatin1String("startPage"), m_titlePath + QLatin1String("/")
            + m_properties.value(QLatin1String("startpage")));
    if (m_properties.contains(QLatin1String("aboutmenutext"))) {
        writeStartElement(QLatin1String("aboutMenuText"));
        writeTextElement(QLatin1String("text"),
            m_properties.value(QLatin1String("aboutmenutext")));
        writeEndElement();
    }
    if (m_properties.contains(QLatin1String("abouturl"))) {
        writeStartElement(QLatin1String("aboutDialog"));
        writeTextElement(QLatin1String("file"), m_properties.value(QLatin1String("abouturl")));
        writeEndElement();
    }
    if (m_properties.contains(QLatin1String("name"))) {
        writeTextElement(QLatin1String("cacheDirectory"),
            QLatin1String(".") + m_properties.value(QLatin1String("name")));
    }

    writeEndElement();
}

void QhcpWriter::writeDocuments()
{
    if (m_qhpFile.isEmpty())
        return;

    QString out = m_qhpFile;
    int i = out.indexOf(QLatin1Char('.'));
    if (i > -1)
        out = out.left(i);
    out.append(QLatin1String(".qch"));

    writeStartElement(QLatin1String("docFiles"));
    
    writeStartElement(QLatin1String("generate"));
    writeStartElement(QLatin1String("file"));
    writeTextElement(QLatin1String("input"), m_qhpFile);
    writeTextElement(QLatin1String("output"), out);
    writeEndElement();
    writeEndElement();

    writeStartElement(QLatin1String("register"));
    writeTextElement(QLatin1String("file"), out);
    writeEndElement();

    writeEndElement();
}

QT_END_NAMESPACE
