/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
