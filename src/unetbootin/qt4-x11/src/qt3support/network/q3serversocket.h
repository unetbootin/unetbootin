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
