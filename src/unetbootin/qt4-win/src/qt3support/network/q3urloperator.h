/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3URLOPERATOR_H
#define Q3URLOPERATOR_H

#include <QtCore/qobject.h>
#include <Qt3Support/q3url.h>
#include <Qt3Support/q3ptrlist.h>
#include <Qt3Support/q3networkprotocol.h>
#include <QtCore/qstringlist.h> // QString->QStringList conversion

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_NETWORKPROTOCOL

class QUrlInfo;
class Q3UrlOperatorPrivate;
class Q3NetworkProtocol;

class Q_COMPAT_EXPORT Q3UrlOperator : public QObject, public Q3Url
{
    Q_OBJECT
    friend class Q3NetworkProtocol;

public:
    Q3UrlOperator();
    Q3UrlOperator( const QString &urL );
    Q3UrlOperator( const Q3UrlOperator& url );
    Q3UrlOperator( const Q3UrlOperator& url, const QString& relUrl, bool checkSlash = false );
    virtual ~Q3UrlOperator();

    virtual void setPath( const QString& path );
    virtual bool cdUp();

    virtual const Q3NetworkOperation *listChildren();
    virtual const Q3NetworkOperation *mkdir( const QString &dirname );
    virtual const Q3NetworkOperation *remove( const QString &filename );
    virtual const Q3NetworkOperation *rename( const QString &oldname, const QString &newname );
    virtual const Q3NetworkOperation *get( const QString &location = QString() );
    virtual const Q3NetworkOperation *put( const QByteArray &data, const QString &location = QString()  );
    virtual Q3PtrList<Q3NetworkOperation> copy( const QString &from, const QString &to, bool move = false, bool toPath = true );
    virtual void copy( const QStringList &files, const QString &dest, bool move = false );
    virtual bool isDir( bool *ok = 0 );

    virtual void setNameFilter( const QString &nameFilter );
    QString nameFilter() const;

    virtual QUrlInfo info( const QString &entry ) const;

    Q3UrlOperator& operator=( const Q3UrlOperator &url );
    Q3UrlOperator& operator=( const QString &url );

    virtual void stop();

Q_SIGNALS:
    void newChildren( const Q3ValueList<QUrlInfo> &, Q3NetworkOperation *res );
    void finished( Q3NetworkOperation *res );
    void start( Q3NetworkOperation *res );
    void createdDirectory( const QUrlInfo &, Q3NetworkOperation *res );
    void removed( Q3NetworkOperation *res );
    void itemChanged( Q3NetworkOperation *res );
    void data( const QByteArray &, Q3NetworkOperation *res );
    void dataTransferProgress( int bytesDone, int bytesTotal, Q3NetworkOperation *res );
    void startedNextCopy( const Q3PtrList<Q3NetworkOperation> &lst );
    void connectionStateChanged( int state, const QString &data );

protected:
    void reset();
    bool parse( const QString& url );
    virtual bool checkValid();
    virtual void clearEntries();
    void getNetworkProtocol();
    void deleteNetworkProtocol();

private Q_SLOTS:
    const Q3NetworkOperation *startOperation( Q3NetworkOperation *op );
    void copyGotData( const QByteArray &data, Q3NetworkOperation *op );
    void continueCopy( Q3NetworkOperation *op );
    void finishedCopy();
    void addEntry( const Q3ValueList<QUrlInfo> &i );
    void slotItemChanged( Q3NetworkOperation *op );

private:
    void deleteOperation( Q3NetworkOperation *op );

    Q3UrlOperatorPrivate *d;
};

#endif // QT_NO_NETWORKPROTOCOL

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3URLOPERATOR_H
