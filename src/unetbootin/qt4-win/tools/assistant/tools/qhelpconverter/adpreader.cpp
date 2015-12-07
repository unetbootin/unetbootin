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

#include "adpreader.h"

QT_BEGIN_NAMESPACE

void AdpReader::readData(const QByteArray &contents)
{
    clear();
    m_contents.clear();
    m_keywords.clear();
    m_properties.clear();
    m_files.clear();
    addData(contents);
	while (!atEnd()) {
		readNext();
		if (isStartElement()) {
            if (name().toString().toLower() == QLatin1String("assistantconfig")
                && attributes().value(QLatin1String("version")) == QLatin1String("3.2.0")) {
                readProject();
            } else if (name().toString().toLower() == QLatin1String("dcf")) {
                QString ref = attributes().value(QLatin1String("ref")).toString();
                addFile(ref);
                m_contents.append(ContentItem(attributes().value(QLatin1String("title")).toString(),
                    ref, 0));
                readDCF();
            } else {
                raiseError();
            }
		}
	}
}

QList<ContentItem> AdpReader::contents() const
{
    return m_contents;
}

QList<KeywordItem> AdpReader::keywords() const
{
    return m_keywords;
}

QSet<QString> AdpReader::files() const
{
    return m_files;
}

QMap<QString, QString> AdpReader::properties() const
{
    return m_properties;
}

void AdpReader::readProject()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            QString s = name().toString().toLower();
            if (s == QLatin1String("profile")) {
                readProfile();
            } else if (s == QLatin1String("dcf")) {
                QString ref = attributes().value(QLatin1String("ref")).toString();
                addFile(ref);
                m_contents.append(ContentItem(attributes().value(QLatin1String("title")).toString(),
                    ref, 0));
                readDCF();
            } else {
                raiseError();
            }
        }
    }
}

void AdpReader::readProfile()
{
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name().toString().toLower() == QLatin1String("property")) {
                QString prop = attributes().value(QLatin1String("name")).toString().toLower();
                m_properties[prop] = readElementText();
            } else {
                raiseError();
            }
        } else if (isEndElement()) {
            break;
        }
    }
}

void AdpReader::readDCF()
{
    int depth = 0;
    while (!atEnd()) {
        readNext();
        QString str = name().toString().toLower();
        if (isStartElement()) {
            if (str == QLatin1String("section")) {
                QString ref = attributes().value(QLatin1String("ref")).toString();
                addFile(ref);
                m_contents.append(ContentItem(attributes().value(QLatin1String("title")).toString(),
                    ref, ++depth));
            } else if (str == QLatin1String("keyword")) {
                QString ref = attributes().value(QLatin1String("ref")).toString();
                addFile(ref);
                m_keywords.append(KeywordItem(readElementText(), ref));
            } else {
                raiseError();
            }
        } else if (isEndElement()) {
            if (str == QLatin1String("section"))
                --depth;
            else if (str == QLatin1String("dcf"))
                break;
        }
    }
}

void AdpReader::addFile(const QString &file)
{
    QString s = file;
    if (s.startsWith(QLatin1String("./")))
        s = s.mid(2);
    int i = s.indexOf(QLatin1Char('#'));
    if (i > -1)
        s = s.left(i);
    if (!m_files.contains(s))
        m_files.insert(s);
}

QT_END_NAMESPACE
