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

#ifndef Q3ACCEL_H
#define Q3ACCEL_H

#include <QtCore/qobject.h>
#include <QtGui/qkeysequence.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3AccelPrivate;

class Q_COMPAT_EXPORT Q3Accel : public QObject			// accelerator class
{
    Q_OBJECT
public:
    Q3Accel( QWidget *parent, const char *name=0 );
    Q3Accel( QWidget* watch, QObject *parent, const char *name=0 );
    ~Q3Accel();

    bool isEnabled() const;
    void setEnabled( bool );

    uint count() const;

    int insertItem( const QKeySequence& key, int id=-1);
    void removeItem( int id );
    void clear();

    QKeySequence key( int id );
    int findKey( const QKeySequence& key ) const;

    bool isItemEnabled( int id ) const;
    void setItemEnabled( int id, bool enable );

    bool connectItem( int id, const QObject *receiver, const char* member );
    bool disconnectItem( int id, const QObject *receiver, const char* member );

    void repairEventFilter() {}

    void setWhatsThis( int id, const QString& );
    QString whatsThis( int id ) const;
    void setIgnoreWhatsThis( bool );
    bool ignoreWhatsThis() const;

    static QKeySequence shortcutKey( const QString & );
    static QString keyToString(QKeySequence k );
    static QKeySequence stringToKey( const QString & );

Q_SIGNALS:
    void activated( int id );
    void activatedAmbiguously( int id );

private:
    Q3AccelPrivate * d;

private:
    Q_DISABLE_COPY(Q3Accel)
    friend class Q3AccelPrivate;
    friend class Q3AccelManager;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3ACCEL_H
