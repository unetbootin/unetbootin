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
