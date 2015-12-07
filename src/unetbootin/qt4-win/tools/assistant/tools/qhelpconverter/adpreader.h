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

#ifndef ADPREADER_H
#define ADPREADER_H

#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtXml/QXmlStreamReader>

QT_BEGIN_NAMESPACE

struct ContentItem {
    ContentItem(const QString &t, const QString &r, int d)
	: title(t), reference(r), depth(d) {}
    QString title;
    QString reference;
    int depth;    
};

struct KeywordItem {
    KeywordItem(const QString &k, const QString &r)
	: keyword(k), reference(r) {}
    QString keyword;
    QString reference;
};

class AdpReader : public QXmlStreamReader
{
public:
    void readData(const QByteArray &contents);
    QList<ContentItem> contents() const;
    QList<KeywordItem> keywords() const;
    QSet<QString> files() const;

    QMap<QString, QString> properties() const;

private:
    void readProject();
    void readProfile();
    void readDCF();
    void addFile(const QString &file);

    QMap<QString, QString> m_properties;
    QList<ContentItem> m_contents;
    QList<KeywordItem> m_keywords;
    QSet<QString> m_files;
};

QT_END_NAMESPACE

#endif
