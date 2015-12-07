/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the public API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifndef QDBUSABSTRACTADAPTORPRIVATE_H
#define QDBUSABSTRACTADAPTORPRIVATE_H

#include <qdbusabstractadaptor.h>

#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <QtCore/qhash.h>
#include <QtCore/qreadwritelock.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>
#include "private/qobject_p.h"

#define QCLASSINFO_DBUS_INTERFACE       "D-Bus Interface"
#define QCLASSINFO_DBUS_INTROSPECTION   "D-Bus Introspection"

QT_BEGIN_NAMESPACE

class QDBusAbstractAdaptor;
class QDBusAdaptorConnector;
class QDBusAdaptorManager;
class QDBusConnectionPrivate;

class QDBusAbstractAdaptorPrivate: public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDBusAbstractAdaptor)
public:
    QDBusAbstractAdaptorPrivate() : autoRelaySignals(false) {}
    QString xml;
    bool autoRelaySignals;

    static QString retrieveIntrospectionXml(QDBusAbstractAdaptor *adaptor);
    static void saveIntrospectionXml(QDBusAbstractAdaptor *adaptor, const QString &xml);
};

class QDBusAdaptorConnector: public QObject
{
    Q_OBJECT_FAKE

public: // typedefs
    struct AdaptorData
    {
        const char *interface;
        QDBusAbstractAdaptor *adaptor;

        inline bool operator<(const AdaptorData &other) const
        { return QByteArray(interface) < other.interface; }
        inline bool operator<(const QString &other) const
        { return QLatin1String(interface) < other; }
        inline bool operator<(const QByteArray &other) const
        { return interface < other; }
    };
    typedef QVector<AdaptorData> AdaptorMap;

public: // methods
    explicit QDBusAdaptorConnector(QObject *parent);
    ~QDBusAdaptorConnector();

    void addAdaptor(QDBusAbstractAdaptor *adaptor);
    void connectAllSignals(QObject *object);
    void disconnectAllSignals(QObject *object);
    void relay(QObject *sender, int id, void **);

//public slots:
    void relaySlot(void **);
    void polish();

protected:
//signals:
    void relaySignal(QObject *obj, const QMetaObject *metaObject, int sid, const QVariantList &args);

public: // member variables
    AdaptorMap adaptors;
    bool waitingForPolish : 1;
};

extern QDBusAdaptorConnector *qDBusFindAdaptorConnector(QObject *object);
extern QDBusAdaptorConnector *qDBusCreateAdaptorConnector(QObject *object);

QT_END_NAMESPACE

#endif // QDBUSABSTRACTADAPTORPRIVATE_H
