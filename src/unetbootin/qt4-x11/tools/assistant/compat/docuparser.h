/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
