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
