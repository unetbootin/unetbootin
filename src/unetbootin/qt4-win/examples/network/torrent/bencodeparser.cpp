/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include "bencodeparser.h"

#include <QList>
#include <QMetaType>

BencodeParser::BencodeParser()
{
}

bool BencodeParser::parse(const QByteArray &content)
{
    if (content.isEmpty()) {
        errString = QString("No content");
        return false;
    }

    this->content = content;
    index = 0;
    infoStart = 0;
    infoLength = 0;
    return getDictionary(&dictionaryValue);
}

QString BencodeParser::errorString() const
{
    return errString;
}

QMap<QByteArray, QVariant> BencodeParser::dictionary() const
{
    return dictionaryValue;
}

QByteArray BencodeParser::infoSection() const
{
    return content.mid(infoStart, infoLength);
}

bool BencodeParser::getByteString(QByteArray *byteString)
{
    const int contentSize = content.size();
    int size = -1;
    do {
	char c = content.at(index);
	if (c < '0' || c > '9') {
	    if (size == -1)
		return false;
	    if (c != ':') {
		errString = QString("Unexpected character at pos %1: %2")
		    .arg(index).arg(c);
		return false;
	    }
	    ++index;
	    break;
	}
	if (size == -1)
	    size = 0;
	size *= 10;
	size += c - '0';
    } while (++index < contentSize);

    if (byteString)
	*byteString = content.mid(index, size);
    index += size;
    return true;
}

bool BencodeParser::getInteger(qint64 *integer)
{
    const int contentSize = content.size();
    if (content.at(index) != 'i')
	return false;

    ++index;
    qint64 num = -1;
    bool negative = false;

    do {
	char c = content.at(index);
	if (c < '0' || c > '9') {
	    if (num == -1) {
		if (c != '-' || negative)
		    return false;
		negative = true;
		continue;
	    } else {
		if (c != 'e') {
		    errString = QString("Unexpected character at pos %1: %2")
			.arg(index).arg(c);
		    return false;
		}
		++index;
		break;
	    }
	}
	if (num == -1)
	    num = 0;
	num *= 10;
	num += c - '0';
    } while (++index < contentSize);

    if (integer)
	*integer = negative ? -num : num;
    return true;
}

bool BencodeParser::getList(QList<QVariant> *list)
{
    const int contentSize = content.size();
    if (content.at(index) != 'l')
	return false;

    QList<QVariant> tmp;    
    ++index;

    do {
	if (content.at(index) == 'e') {
	    ++index;
	    break;
	}

	qint64 number;
	QByteArray byteString;
	QList<QVariant> tmpList;
	QMap<QByteArray, QVariant> dictionary;

	if (getInteger(&number))
	    tmp << number;
	else if (getByteString(&byteString))
	    tmp << byteString;
	else if (getList(&tmpList))
	    tmp << tmpList;
	else if (getDictionary(&dictionary))
	    tmp << qVariantFromValue<QMap<QByteArray, QVariant> >(dictionary);
	else {
	    errString = QString("error at index %1").arg(index);
	    return false;
	}
    } while (index < contentSize);

    if (list)
	*list = tmp;
    return true;
}

bool BencodeParser::getDictionary(QMap<QByteArray, QVariant> *dictionary)
{
    const int contentSize = content.size();
    if (content.at(index) != 'd')
	return false;

    QMap<QByteArray, QVariant> tmp;    
    ++index;

    do {
	if (content.at(index) == 'e') {
	    ++index;
	    break;
	}

	QByteArray key;
	if (!getByteString(&key))
	    break;

	if (key == "info")
	  infoStart = index;

	qint64 number;
	QByteArray byteString;
	QList<QVariant> tmpList;
	QMap<QByteArray, QVariant> dictionary;

	if (getInteger(&number))
	    tmp.insert(key, number);
	else if (getByteString(&byteString))
	    tmp.insert(key, byteString);
	else if (getList(&tmpList))
	    tmp.insert(key, tmpList);
	else if (getDictionary(&dictionary))
	    tmp.insert(key, qVariantFromValue<QMap<QByteArray, QVariant> >(dictionary));
	else {
	    errString = QString("error at index %1").arg(index);
	    return false;
	}

	if (key == "info")
	  infoLength = index - infoStart;

    } while (index < contentSize);

    if (dictionary)
	*dictionary = tmp;
    return true;
}
