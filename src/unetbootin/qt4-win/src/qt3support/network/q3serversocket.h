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

#ifndef Q3SERVERSOCKET_H
#define Q3SERVERSOCKET_H

#include <QtCore/qobject.h>
#include <QtNetwork/qhostaddress.h>
#include <Qt3Support/q3socketdevice.h> // ### remove or keep for users' convenience?

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

class Q3ServerSocketPrivate;

class Q_COMPAT_EXPORT Q3ServerSocket : public QObject
{
    Q_OBJECT
public:
    Q3ServerSocket( Q_UINT16 port, int backlog = 1,
		   QObject *parent=0, const char *name=0 );
    Q3ServerSocket( const QHostAddress & address, Q_UINT16 port, int backlog = 1,
		   QObject *parent=0, const char *name=0 );
    Q3ServerSocket( QObject *parent=0, const char *name=0 );
    virtual ~Q3ServerSocket();

    bool ok() const;

    Q_UINT16 port() const ;

    int socket() const ;
    virtual void setSocket( int socket );

    QHostAddress address() const ;

    virtual void newConnection( int socket ) = 0;

protected:
    Q3SocketDevice *socketDevice();

private Q_SLOTS:
    void incomingConnection( int socket );

private:
    Q3ServerSocketPrivate *d;
    void init( const QHostAddress & address, Q_UINT16 port, int backlog );
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3SERVERSOCKET_H
