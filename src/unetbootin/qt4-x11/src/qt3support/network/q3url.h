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

#ifndef Q3URL_H
#define Q3URL_H

#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_URL

class Q3UrlPrivate;

class Q_COMPAT_EXPORT Q3Url
{
public:
    Q3Url();
    Q3Url( const QString& url );
    Q3Url( const Q3Url& url );
    Q3Url( const Q3Url& url, const QString& relUrl, bool checkSlash = false );
    virtual ~Q3Url();

    QString protocol() const;
    virtual void setProtocol( const QString& protocol );

    QString user() const;
    virtual void setUser( const QString& user );
    bool hasUser() const;

    QString password() const;
    virtual void setPassword( const QString& pass );
    bool hasPassword() const;

    QString host() const;
    virtual void setHost( const QString& user );
    bool hasHost() const;

    int port() const;
    virtual void setPort( int port );
    bool hasPort() const;

    QString path( bool correct = true ) const;
    virtual void setPath( const QString& path );
    bool hasPath() const;

    virtual void setEncodedPathAndQuery( const QString& enc );
    QString encodedPathAndQuery();

    virtual void setQuery( const QString& txt );
    QString query() const;

    QString ref() const;
    virtual void setRef( const QString& txt );
    bool hasRef() const;

    bool isValid() const;
    bool isLocalFile() const;

    virtual void addPath( const QString& path );
    virtual void setFileName( const QString& txt );

    QString fileName() const;
    QString dirPath() const;

    Q3Url& operator=( const Q3Url& url );
    Q3Url& operator=( const QString& url );

    bool operator==( const Q3Url& url ) const;
    bool operator==( const QString& url ) const;

    static void decode( QString& url );
    static void encode( QString& url );

    operator QString() const;
    virtual QString toString( bool encodedPath = false, bool forcePrependProtocol = true ) const;

    virtual bool cdUp();

    static bool isRelativeUrl( const QString &url );

protected:
    virtual void reset();
    virtual bool parse( const QString& url );

private:
    Q3UrlPrivate *d;

};

#endif // QT_NO_URL

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3URL_H
