/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#ifndef DOCUPARSER_H
#define DOCUPARSER_H

#include <QList>
#include <QMap>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>
#include <QXmlParseException>

QT_BEGIN_NAMESPACE

class Profile;

struct ContentItem {
    ContentItem()
	: title( QString() ), reference( QString() ), depth( 0 ) {}
    ContentItem( const QString &t, const QString &r, int d )
	: title( t ), reference( r ), depth( d ) {}
    QString title;
    QString reference;
    int depth;
    Q_DUMMY_COMPARISON_OPERATOR(ContentItem)
};

QDataStream &operator>>( QDataStream &s, ContentItem &ci );
QDataStream &operator<<( QDataStream &s, const ContentItem &ci );

struct IndexItem {
    IndexItem( const QString &k, const QString &r )
	: keyword( k ), reference( r ) {}
    QString keyword;
    QString reference;
};



class DocuParser : public QXmlDefaultHandler
{
public:
    enum ParserVersion { Qt310, Qt320 };
    // Since We don't want problems with documentation
    // from version to version, this string stores the correct
    // version string to save documents.
    static const QString DocumentKey;

    static DocuParser *createParser( const QString &fileName );

    virtual bool parse( QFile *file );
    
    QList<ContentItem> getContentItems();
    QList<IndexItem*> getIndexItems();

    QString errorProtocol() const;
    QString contentsURL() const { return conURL; }

    virtual ParserVersion parserVersion() const = 0;
    virtual void addTo( Profile *p ) = 0;

    QString fileName() const { return fname; }
    void setFileName( const QString &file ) { fname = file; }

protected:
    QString absolutify( const QString &input, bool makeUrl = true ) const;
    
    QString contentRef, indexRef, errorProt, conURL;
    QString docTitle, title, iconName;
    QList<ContentItem> contentList;
    QList<IndexItem*> indexList;
    QString fname;
};


class DocuParser310 : public DocuParser
{
public:
    enum States{ StateInit, StateContent, StateSect, StateKeyword };
    
    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );
    bool characters( const QString & );
    bool fatalError( const QXmlParseException& exception );

    virtual ParserVersion parserVersion() const { return Qt310; }
    virtual void addTo( Profile *p );
    
private:
    States state;
    int depth;
};


class DocuParser320 : public DocuParser
{
public:
    enum States { StateInit, StateDocRoot, StateProfile, StateProperty,
		  StateContent, StateSect, StateKeyword };

    DocuParser320();    
    
    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );
    bool characters( const QString & );
    bool fatalError( const QXmlParseException& exception );

    virtual ParserVersion parserVersion() const { return Qt320; }
    virtual void addTo( Profile *p );
    Profile *profile() const { return prof; }    

private:
    
    States state;
    int depth;
    int docfileCounter;
    QString propertyValue;
    QString propertyName;
    Profile *prof;
};

QT_END_NAMESPACE

#endif // DOCUPARSER_H
