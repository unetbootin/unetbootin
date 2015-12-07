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

#include "index.h"

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QApplication>
#include <QByteArray>
#include <QTextStream>
#include <QtAlgorithms>
#include <QUrl>
#include <QTextCodec>
#include <ctype.h>
#include <QTextDocument>

QT_BEGIN_NAMESPACE

struct Term {
    Term() : frequency(-1) {}
    Term( const QString &t, int f, QVector<Document> l ) : term( t ), frequency( f ), documents( l ) {}
    QString term;
    int frequency;
    QVector<Document>documents;
    bool operator<( const Term &i2 ) const { return frequency < i2.frequency; }
};

QDataStream &operator>>( QDataStream &s, Document &l )
{
    s >> l.docNumber;
    s >> l.frequency;
    return s;
}

QDataStream &operator<<( QDataStream &s, const Document &l )
{
    s << (qint16)l.docNumber;
    s << (qint16)l.frequency;
    return s;
}

Index::Index( const QString &dp, const QString &hp )
    : QObject( 0 ), docPath( dp )
{
    Q_UNUSED(hp);

    alreadyHaveDocList = false;
    lastWindowClosed = false;
    connect( qApp, SIGNAL(lastWindowClosed()),
             this, SLOT(setLastWinClosed()) );
}

Index::Index( const QStringList &dl, const QString &hp )
    : QObject( 0 )
{
    Q_UNUSED(hp);
    docList = dl;
    alreadyHaveDocList = true;
    lastWindowClosed = false;
    connect( qApp, SIGNAL(lastWindowClosed()),
             this, SLOT(setLastWinClosed()) );
}

void Index::setLastWinClosed()
{
    lastWindowClosed = true;
}

void Index::setDictionaryFile( const QString &f )
{
    dictFile = f;
}

void Index::setDocListFile( const QString &f )
{
    docListFile = f;
}

void Index::setDocList( const QStringList &lst )
{
    docList = lst;
}

int Index::makeIndex()
{
    if ( !alreadyHaveDocList )
        setupDocumentList();
    if ( docList.isEmpty() )
        return 1;
    QStringList::Iterator it = docList.begin();
    int steps = docList.count() / 100;
    if ( !steps )
        steps++;
    int prog = 0;
    for ( int i = 0; it != docList.end(); ++it, ++i ) {
        if ( lastWindowClosed ) {
            return -1;
        }
        QUrl url(*it);
        parseDocument( url.toLocalFile(), i );
        if ( i%steps == 0 ) {
            prog++;
            emit indexingProgress( prog );
        }
    }
    return 0;
}

void Index::setupDocumentList()
{
    QDir d( docPath );
    QStringList filters;
    filters.append(QLatin1String("*.html"));
    QStringList lst = d.entryList(filters);
    QStringList::ConstIterator it = lst.constBegin();
    for ( ; it != lst.constEnd(); ++it )
        docList.append( QLatin1String("file:") + docPath + QLatin1String("/") + *it );
}

void Index::insertInDict( const QString &str, int docNum )
{
    if ( str == QLatin1String("amp") || str == QLatin1String("nbsp"))
        return;
    Entry *e = 0;
    if ( dict.count() )
        e = dict[ str ];

    if ( e ) {
        if ( e->documents.last().docNumber != docNum )
            e->documents.append( Document(docNum, 1 ) );
        else
            e->documents.last().frequency++;
    } else {
        dict.insert( str, new Entry( docNum ) );
    }
}

QString Index::getCharsetForDocument(QFile *file)
{
    QTextStream s(file);
    QString contents = s.readAll();

    QString encoding;
    int start = contents.indexOf(QLatin1String("<meta"), 0, Qt::CaseInsensitive);
    if (start > 0) {
        int end = contents.indexOf(QLatin1String(">"), start);
        QString meta = contents.mid(start+5, end-start);
        meta = meta.toLower();
        QRegExp r(QLatin1String("charset=([^\"\\s]+)"));
        if (r.indexIn(meta) != -1) {
            encoding = r.cap(1);        
        }
    }

    file->seek(0);
    if (encoding.isEmpty())
        return QLatin1String("utf-8");
    return encoding;
}

void Index::parseDocument( const QString &filename, int docNum )
{
    QFile file( filename );
    if ( !file.open(QFile::ReadOnly) ) {
        qWarning( "can not open file %s", qPrintable(filename) );
        return;
    }

    QTextStream s(&file);
    QString en = getCharsetForDocument(&file);
    s.setCodec(QTextCodec::codecForName(en.toLatin1().constData()));

    QString text = s.readAll();
    if (text.isNull())
        return;

    bool valid = true;
    const QChar *buf = text.unicode();
    QChar str[64];
    QChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( j < text.length() ) {
        if ( c == QLatin1Char('<') || c == QLatin1Char('&') ) {
            valid = false;
            if ( i > 1 )
                insertInDict( QString(str,i), docNum );
            i = 0;
            c = buf[++j];
            continue;
        }
        if ( ( c == QLatin1Char('>') || c == QLatin1Char(';') ) && !valid ) {
            valid = true;
            c = buf[++j];
            continue;
        }
        if ( !valid ) {
            c = buf[++j];
            continue;
        }
        if ( ( c.isLetterOrNumber() || c == QLatin1Char('_') ) && i < 63 ) {
            str[i] = c.toLower();
            ++i;
        } else {
            if ( i > 1 )
                insertInDict( QString(str,i), docNum );
            i = 0;
        }
        c = buf[++j];
    }
    if ( i > 1 )
        insertInDict( QString(str,i), docNum );
    file.close();
}

void Index::writeDict()
{
    QFile f( dictFile );
    if ( !f.open(QFile::WriteOnly ) )
        return;
    QDataStream s( &f );
    for(QHash<QString, Entry *>::Iterator it = dict.begin(); it != dict.end(); ++it) {
        s << it.key();
        s << it.value()->documents.count();
        s << it.value()->documents;
    }
    f.close();
    writeDocumentList();
}

void Index::writeDocumentList()
{
    QFile f( docListFile );
    if ( !f.open(QFile::WriteOnly ) )
        return;
    QDataStream s( &f );
    s << docList;
}

void Index::readDict()
{
    QFile f( dictFile );
    if ( !f.open(QFile::ReadOnly ) )
        return;

    dict.clear();
    QDataStream s( &f );
    QString key;
    int numOfDocs;
    QVector<Document> docs;
    while ( !s.atEnd() ) {
        s >> key;
        s >> numOfDocs;
        docs.resize(numOfDocs);
        s >> docs;
        dict.insert( key, new Entry( docs ) );
    }
    f.close();
    readDocumentList();
}

void Index::readDocumentList()
{
    QFile f( docListFile );
    if ( !f.open(QFile::ReadOnly ) )
        return;
    QDataStream s( &f );
    s >> docList;
}

QStringList Index::query( const QStringList &terms, const QStringList &termSeq, const QStringList &seqWords )
{
    QList<Term> termList;
    for (QStringList::ConstIterator it = terms.begin(); it != terms.end(); ++it ) {
        Entry *e = 0;
        if ( (*it).contains(QLatin1Char('*')) ) {
            QVector<Document> wcts = setupDummyTerm( getWildcardTerms( *it ) );
            termList.append( Term(QLatin1String("dummy"), wcts.count(), wcts ) );
        } else if ( dict[ *it ] ) {
            e = dict[ *it ];
            termList.append( Term( *it, e->documents.count(), e->documents ) );
        } else {
            return QStringList();
        }
    }
    if ( !termList.count() )
        return QStringList();
    qSort(termList);

    QVector<Document> minDocs = termList.takeFirst().documents;
    for(QList<Term>::Iterator it = termList.begin(); it != termList.end(); ++it) {
        Term *t = &(*it);
        QVector<Document> docs = t->documents;
        for(QVector<Document>::Iterator minDoc_it = minDocs.begin(); minDoc_it != minDocs.end(); ) {
            bool found = false;
            for (QVector<Document>::ConstIterator doc_it = docs.constBegin(); doc_it != docs.constEnd(); ++doc_it ) {
                if ( (*minDoc_it).docNumber == (*doc_it).docNumber ) {
                    (*minDoc_it).frequency += (*doc_it).frequency;
                    found = true;
                    break;
                }
            }
            if ( !found )
                minDoc_it = minDocs.erase( minDoc_it );
            else
                ++minDoc_it;
        }
    }

    QStringList results;
    qSort( minDocs );
    if ( termSeq.isEmpty() ) {
        for(QVector<Document>::Iterator it = minDocs.begin(); it != minDocs.end(); ++it)
            results << docList.at((int)(*it).docNumber);
        return results;
    }

    QString fileName;
    for(QVector<Document>::Iterator it = minDocs.begin(); it != minDocs.end(); ++it) {
        fileName =  docList[ (int)(*it).docNumber ];
        if ( searchForPattern( termSeq, seqWords, fileName ) )
            results << fileName;
    }
    return results;
}

QString Index::getDocumentTitle( const QString &fullFileName )
{
    QUrl url(fullFileName);
    QString fileName = url.toLocalFile();

    if (documentTitleCache.contains(fileName))
        return documentTitleCache.value(fileName);

    QFile file( fileName );
    if ( !file.open( QFile::ReadOnly ) ) {
        qWarning( "cannot open file %s", qPrintable(fileName) );
        return fileName;
    }
    QTextStream s( &file );
    QString text = s.readAll();

    int start = text.indexOf(QLatin1String("<title>"), 0, Qt::CaseInsensitive) + 7;
    int end = text.indexOf(QLatin1String("</title>"), 0, Qt::CaseInsensitive);

    QString title = tr("Untitled");
    if (end - start > 0) {
        title = text.mid(start, end - start);
        if (Qt::mightBeRichText(title)) {
            QTextDocument doc;
            doc.setHtml(title);
            title = doc.toPlainText();
        }
    }
    documentTitleCache.insert(fileName, title);
    return title;
}

QStringList Index::getWildcardTerms( const QString &term )
{
    QStringList lst;
    QStringList terms = split( term );
    QStringList::Iterator iter;

    for(QHash<QString, Entry*>::Iterator it = dict.begin(); it != dict.end(); ++it) {
        int index = 0;
        bool found = false;
        QString text( it.key() );
        for ( iter = terms.begin(); iter != terms.end(); ++iter ) {
            if ( *iter == QLatin1String("*") ) {
                found = true;
                continue;
            }
            if ( iter == terms.begin() && (*iter)[0] != text[0] ) {
                found = false;
                break;
            }
            index = text.indexOf( *iter, index );
            if ( *iter == terms.last() && index != (int)text.length()-1 ) {
                index = text.lastIndexOf( *iter );
                if ( index != (int)text.length() - (int)(*iter).length() ) {
                    found = false;
                    break;
                }
            }
            if ( index != -1 ) {
                found = true;
                index += (*iter).length();
                continue;
            } else {
                found = false;
                break;
            }
        }
        if ( found )
            lst << text;
    }

    return lst;
}

QStringList Index::split( const QString &str )
{
    QStringList lst;
    int j = 0;
    int i = str.indexOf(QLatin1Char('*'), j );

    if (str.startsWith(QLatin1String("*")))
        lst << QLatin1String("*");

    while ( i != -1 ) {
        if ( i > j && i <= (int)str.length() ) {
            lst << str.mid( j, i - j );
            lst << QLatin1String("*");
        }
        j = i + 1;
        i = str.indexOf(QLatin1Char('*'), j );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
        lst << str.mid( j, l - j + 1 );

    return lst;
}

QVector<Document> Index::setupDummyTerm( const QStringList &terms )
{
    QList<Term> termList;
    for (QStringList::ConstIterator it = terms.begin(); it != terms.end(); ++it) {
        Entry *e = 0;
        if ( dict[ *it ] ) {
            e = dict[ *it ];
            termList.append( Term( *it, e->documents.count(), e->documents ) );
        }
    }
    QVector<Document> maxList(0);
    if ( !termList.count() )
        return maxList;
    qSort(termList);

    maxList = termList.takeLast().documents;
    for(QList<Term>::Iterator it = termList.begin(); it != termList.end(); ++it) {
        Term *t = &(*it);
        QVector<Document> docs = t->documents;
        for (QVector<Document>::iterator docIt = docs.begin(); docIt != docs.end(); ++docIt ) {
            if ( maxList.indexOf( *docIt ) == -1 )
                maxList.append( *docIt );
        }
    }
    return maxList;
}

void Index::buildMiniDict( const QString &str )
{
    if ( miniDict[ str ] )
        miniDict[ str ]->positions.append( wordNum );
    ++wordNum;
}

bool Index::searchForPattern( const QStringList &patterns, const QStringList &words, const QString &fileName )
{
    QUrl url(fileName);
    QString fName = url.toLocalFile();
    QFile file( fName );
    if ( !file.open( QFile::ReadOnly ) ) {
        qWarning( "cannot open file %s", qPrintable(fName) );
        return false;
    }

    wordNum = 3;
    miniDict.clear();
    QStringList::ConstIterator cIt = words.begin();
    for ( ; cIt != words.end(); ++cIt )
        miniDict.insert( *cIt, new PosEntry( 0 ) );

    QTextStream s( &file );
    QString text = s.readAll();
    bool valid = true;
    const QChar *buf = text.unicode();
    QChar str[64];
    QChar c = buf[0];
    int j = 0;
    int i = 0;
    while ( j < text.length() ) {
        if ( c == QLatin1Char('<') || c == QLatin1Char('&') ) {
            valid = false;
            if ( i > 1 )
                buildMiniDict( QString(str,i) );
            i = 0;
            c = buf[++j];
            continue;
        }
        if ( ( c == QLatin1Char('>') || c == QLatin1Char(';') ) && !valid ) {
            valid = true;
            c = buf[++j];
            continue;
        }
        if ( !valid ) {
            c = buf[++j];
            continue;
        }
        if ( ( c.isLetterOrNumber() || c == QLatin1Char('_') ) && i < 63 ) {
            str[i] = c.toLower();
            ++i;
        } else {
            if ( i > 1 )
                buildMiniDict( QString(str,i) );
            i = 0;
        }
        c = buf[++j];
    }
    if ( i > 1 )
        buildMiniDict( QString(str,i) );
    file.close();

    QStringList::ConstIterator patIt = patterns.begin();
    QStringList wordLst;
    QList<uint> a, b;
    QList<uint>::iterator aIt;
    for ( ; patIt != patterns.end(); ++patIt ) {
        wordLst = (*patIt).split(QLatin1Char(' '));
        a = miniDict[ wordLst[0] ]->positions;
        for ( int j = 1; j < (int)wordLst.count(); ++j ) {
            b = miniDict[ wordLst[j] ]->positions;
            aIt = a.begin();
            while ( aIt != a.end() ) {
                if ( b.contains( *aIt + 1 )) {
                    (*aIt)++;
                    ++aIt;
                } else {
                    aIt = a.erase( aIt );
                }
            }
        }
    }
    if ( a.count() )
        return true;
    return false;
}

QT_END_NAMESPACE
