/****************************************************************************
** Meta object code from reading C++ file 'qdbusconnectioninterface.h'
**
** Created: Thu Mar 5 19:07:17 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdbusconnectioninterface.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdbusconnectioninterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusConnectionInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   12, // methods
       1,   97, // properties
       3,  100, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      34,   26,   25,   25, 0x05,
      61,   26,   25,   25, 0x05,
     113,   90,   25,   25, 0x05,
     169,  158,   25,   25, 0x05,
     217,   25,   25,   25, 0x05,
     239,   25,   25,   25, 0x05,
     260,  257,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     326,   25,  302,   25, 0x0a,
     380,  368,  351,   25, 0x0a,
     434,  429,  409,   25, 0x0a,
     456,  368,  351,   25, 0x0a,
     570,  542,  483,   25, 0x0a,
     661,  641,  483,   25, 0x2a,
     706,  368,  483,   25, 0x2a,
     748,  368,  731,   25, 0x0a,
     768,  368,  731,   25, 0x0a,
     805,  429,  788,   25, 0x0a,

 // properties: name, type, flags
     827,  302, 0x00095009,

 // enums: name, flags, count, data
     850, 0x0,    3,  112,
     870, 0x0,    2,  118,
     896, 0x0,    3,  122,

 // enum data: key, value
     917, uint(QDBusConnectionInterface::DontQueueService),
     934, uint(QDBusConnectionInterface::QueueService),
     947, uint(QDBusConnectionInterface::ReplaceExistingService),
     970, uint(QDBusConnectionInterface::DontAllowReplacement),
     991, uint(QDBusConnectionInterface::AllowReplacement),
    1008, uint(QDBusConnectionInterface::ServiceNotRegistered),
    1029, uint(QDBusConnectionInterface::ServiceRegistered),
    1047, uint(QDBusConnectionInterface::ServiceQueued),

       0        // eod
};

static const char qt_meta_stringdata_QDBusConnectionInterface[] = {
    "QDBusConnectionInterface\0\0service\0"
    "serviceRegistered(QString)\0"
    "serviceUnregistered(QString)\0"
    "name,oldOwner,newOwner\0"
    "serviceOwnerChanged(QString,QString,QString)\0"
    "error,call\0callWithCallbackFailed(QDBusError,QDBusMessage)\0"
    "NameAcquired(QString)\0NameLost(QString)\0"
    ",,\0NameOwnerChanged(QString,QString,QString)\0"
    "QDBusReply<QStringList>\0"
    "registeredServiceNames()\0QDBusReply<bool>\0"
    "serviceName\0isServiceRegistered(QString)\0"
    "QDBusReply<QString>\0name\0serviceOwner(QString)\0"
    "unregisterService(QString)\0"
    "QDBusReply<QDBusConnectionInterface::RegisterServiceReply>\0"
    "serviceName,qoption,roption\0"
    "registerService(QString,ServiceQueueOptions,ServiceReplacementOptions)\0"
    "serviceName,qoption\0"
    "registerService(QString,ServiceQueueOptions)\0"
    "registerService(QString)\0QDBusReply<uint>\0"
    "servicePid(QString)\0serviceUid(QString)\0"
    "QDBusReply<void>\0startService(QString)\0"
    "registeredServiceNames\0ServiceQueueOptions\0"
    "ServiceReplacementOptions\0"
    "RegisterServiceReply\0DontQueueService\0"
    "QueueService\0ReplaceExistingService\0"
    "DontAllowReplacement\0AllowReplacement\0"
    "ServiceNotRegistered\0ServiceRegistered\0"
    "ServiceQueued\0"
};

const QMetaObject QDBusConnectionInterface::staticMetaObject = {
    { &QDBusAbstractInterface::staticMetaObject, qt_meta_stringdata_QDBusConnectionInterface,
      qt_meta_data_QDBusConnectionInterface, 0 }
};

const QMetaObject *QDBusConnectionInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QDBusConnectionInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDBusConnectionInterface))
        return static_cast<void*>(const_cast< QDBusConnectionInterface*>(this));
    return QDBusAbstractInterface::qt_metacast(_clname);
}

int QDBusConnectionInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: serviceRegistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: serviceUnregistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: serviceOwnerChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: callWithCallbackFailed((*reinterpret_cast< const QDBusError(*)>(_a[1])),(*reinterpret_cast< const QDBusMessage(*)>(_a[2]))); break;
        case 4: NameAcquired((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: NameLost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: NameOwnerChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 7: { QDBusReply<QStringList> _r = registeredServiceNames();
            if (_a[0]) *reinterpret_cast< QDBusReply<QStringList>*>(_a[0]) = _r; }  break;
        case 8: { QDBusReply<bool> _r = isServiceRegistered((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<bool>*>(_a[0]) = _r; }  break;
        case 9: { QDBusReply<QString> _r = serviceOwner((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<QString>*>(_a[0]) = _r; }  break;
        case 10: { QDBusReply<bool> _r = unregisterService((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<bool>*>(_a[0]) = _r; }  break;
        case 11: { QDBusReply<QDBusConnectionInterface::RegisterServiceReply> _r = registerService((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< ServiceQueueOptions(*)>(_a[2])),(*reinterpret_cast< ServiceReplacementOptions(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QDBusReply<QDBusConnectionInterface::RegisterServiceReply>*>(_a[0]) = _r; }  break;
        case 12: { QDBusReply<QDBusConnectionInterface::RegisterServiceReply> _r = registerService((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< ServiceQueueOptions(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusReply<QDBusConnectionInterface::RegisterServiceReply>*>(_a[0]) = _r; }  break;
        case 13: { QDBusReply<QDBusConnectionInterface::RegisterServiceReply> _r = registerService((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<QDBusConnectionInterface::RegisterServiceReply>*>(_a[0]) = _r; }  break;
        case 14: { QDBusReply<uint> _r = servicePid((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<uint>*>(_a[0]) = _r; }  break;
        case 15: { QDBusReply<uint> _r = serviceUid((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<uint>*>(_a[0]) = _r; }  break;
        case 16: { QDBusReply<void> _r = startService((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusReply<void>*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 17;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QDBusReply<QStringList>*>(_v) = registeredServiceNames(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QDBusConnectionInterface::serviceRegistered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDBusConnectionInterface::serviceUnregistered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QDBusConnectionInterface::serviceOwnerChanged(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QDBusConnectionInterface::callWithCallbackFailed(const QDBusError & _t1, const QDBusMessage & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QDBusConnectionInterface::NameAcquired(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QDBusConnectionInterface::NameLost(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QDBusConnectionInterface::NameOwnerChanged(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
