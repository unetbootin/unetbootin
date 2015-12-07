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

#ifndef INDEX_H
#define INDEX_H

#include <QStringList>
#include <QHash>
#include <QDataStream>
#include <QObject>
#include <QList>
#include <QFile>
#include <QVector>

QT_BEGIN_NAMESPACE

struct Document {
    Document( int d, int f ) : docNumber( d ), frequency( f ) {}
    Document() : docNumber( -1 ), frequency( 0 ) {}
    bool operator==( const Document &doc ) const {
	return docNumber == doc.docNumber;
    }
    bool operator<( const Document &doc ) const {
	return frequency > doc.frequency;
    }
    bool operator<=( const Document &doc ) const {
	return frequency >= doc.frequency;
    }
    bool operator>( const Document &doc ) const {
	return frequency < doc.frequency;
    }
    qint16 docNumber;
    qint16 frequency;
};

QDataStream &operator>>( QDataStream &s, Document &l );
QDataStream &operator<<( QDataStream &s, const Document &l );

class Index : public QObject
{
    Q_OBJECT
public:
    struct Entry {
	    Entry( int d ) { documents.append( Document( d, 1 ) ); }
	    Entry( QVector<Document> l ) : documents( l ) {}
	    QVector<Document> documents;
    };
    struct PosEntry {
	    PosEntry( int p ) { positions.append( p ); }
	    QList<uint> positions;
    };

    Index( const QString &dp, const QString &hp );
    Index( const QStringList &dl, const QString &hp );
    void writeDict();
    void readDict();
    int makeIndex();
    QStringList query( const QStringList&, const QStringList&, const QStringList& );
    QString getDocumentTitle( const QString& );
    void setDictionaryFile( const QString& );
    void setDocListFile( const QString& );
    void setDocList( const QStringList & );

signals:
    void indexingProgress( int );

private slots:
    void setLastWinClosed();

private:
    void setupDocumentList();
    void parseDocument( const QString&, int );
    void insertInDict( const QString&, int );
    void writeDocumentList();
    void readDocumentList();
    QStringList getWildcardTerms( const QString& );
    QStringList split( const QString& );
    QVector<Document> setupDummyTerm( const QStringList& );
    bool searchForPattern( const QStringList&, const QStringList&, const QString& );
    void buildMiniDict( const QString& );
    QString getCharsetForDocument(QFile *);
    QStringList docList;
    QHash<QString, Entry*> dict;
    QHash<QString, PosEntry*> miniDict;
    uint wordNum;
    QString docPath;
    QString dictFile, docListFile;
    bool alreadyHaveDocList;
    bool lastWindowClosed;
    QHash<QString, QString> documentTitleCache;
};

#endif

QT_END_NAMESPACE
