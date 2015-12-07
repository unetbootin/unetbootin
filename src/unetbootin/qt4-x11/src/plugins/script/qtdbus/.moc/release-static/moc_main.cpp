/****************************************************************************
** Meta object code from reading C++ file 'main.h'
**
** Created: Thu Mar 5 20:27:57 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../main.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusConnectionConstructor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       2,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   37,   28,   27, 0x0a,
      64,   37,   27,   27, 0x0a,
     120,  107,   91,   27, 0x0a,
     160,  150,   91,   27, 0x0a,

 // properties: name, type, flags
     220,  207, 0x00095009,
     231,  207, 0x00095009,

       0        // eod
};

static const char qt_meta_stringdata_QDBusConnectionConstructor[] = {
    "QDBusConnectionConstructor\0\0QObject*\0"
    "name\0qscript_call(QString)\0"
    "disconnectFromBus(QString)\0QDBusConnection\0"
    "address,name\0connectToBus(QString,QString)\0"
    "type,name\0connectToBus(QDBusConnection::BusType,QString)\0"
    "QScriptValue\0sessionBus\0systemBus\0"
};

const QMetaObject QDBusConnectionConstructor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDBusConnectionConstructor,
      qt_meta_data_QDBusConnectionConstructor, 0 }
};

const QMetaObject *QDBusConnectionConstructor::metaObject() const
{
    return &staticMetaObject;
}

void *QDBusConnectionConstructor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDBusConnectionConstructor))
        return static_cast<void*>(const_cast< QDBusConnectionConstructor*>(this));
    if (!strcmp(_clname, "QScriptable"))
        return static_cast< QScriptable*>(const_cast< QDBusConnectionConstructor*>(this));
    return QObject::qt_metacast(_clname);
}

int QDBusConnectionConstructor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QObject* _r = qscript_call((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        case 1: disconnectFromBus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: { QDBusConnection _r = connectToBus((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusConnection*>(_a[0]) = _r; }  break;
        case 3: { QDBusConnection _r = connectToBus((*reinterpret_cast< QDBusConnection::BusType(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusConnection*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QScriptValue*>(_v) = sessionBus(); break;
        case 1: *reinterpret_cast< QScriptValue*>(_v) = systemBus(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_QScriptDBusConnection[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   12, // methods
       3,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      36,   28,   23,   22, 0x0a,
      93,   68,   55,   22, 0x0a,
     137,  120,   55,   22, 0x2a,
     160,   28,   55,   22, 0x2a,
     191,  179,   23,   22, 0x0a,
     216,  179,   23,   22, 0x0a,
     254,   22,  243,   22, 0x0a,
     276,  266,   22,   22, 0x0a,
     339,  334,   22,   22, 0x2a,
     374,  334,  365,   22, 0x0a,

 // properties: name, type, flags
     410,  402, 0x0a095001,
     422,   23, 0x01095001,
     447,  434, 0x00095009,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDBusConnection[] = {
    "QScriptDBusConnection\0\0bool\0message\0"
    "send(QDBusMessage)\0QDBusMessage\0"
    "message,callMode,timeout\0"
    "call(QDBusMessage,int,int)\0message,callMode\0"
    "call(QDBusMessage,int)\0call(QDBusMessage)\0"
    "serviceName\0registerService(QString)\0"
    "unregisterService(QString)\0QDBusError\0"
    "lastError()\0path,mode\0"
    "unregisterObject(QString,QDBusConnection::UnregisterMode)\0"
    "path\0unregisterObject(QString)\0QObject*\0"
    "objectRegisteredAt(QString)\0QString\0"
    "baseService\0isConnected\0QScriptValue\0"
    "dbusInterface\0"
};

const QMetaObject QScriptDBusConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QScriptDBusConnection,
      qt_meta_data_QScriptDBusConnection, 0 }
};

const QMetaObject *QScriptDBusConnection::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDBusConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDBusConnection))
        return static_cast<void*>(const_cast< QScriptDBusConnection*>(this));
    if (!strcmp(_clname, "QScriptable"))
        return static_cast< QScriptable*>(const_cast< QScriptDBusConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int QScriptDBusConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = send((*reinterpret_cast< const QDBusMessage(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { QDBusMessage _r = call((*reinterpret_cast< const QDBusMessage(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QDBusMessage*>(_a[0]) = _r; }  break;
        case 2: { QDBusMessage _r = call((*reinterpret_cast< const QDBusMessage(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusMessage*>(_a[0]) = _r; }  break;
        case 3: { QDBusMessage _r = call((*reinterpret_cast< const QDBusMessage(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusMessage*>(_a[0]) = _r; }  break;
        case 4: { bool _r = registerService((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = unregisterService((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { QDBusError _r = lastError();
            if (_a[0]) *reinterpret_cast< QDBusError*>(_a[0]) = _r; }  break;
        case 7: unregisterObject((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QDBusConnection::UnregisterMode(*)>(_a[2]))); break;
        case 8: unregisterObject((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: { QObject* _r = objectRegisteredAt((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = baseService(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isConnected(); break;
        case 2: *reinterpret_cast< QScriptValue*>(_v) = dbusInterface(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_QScriptDBusInterfaceConstructor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      74,   46,   33,   32, 0x0a,
     148,  125,   33,   32, 0x2a,
     199,  186,   33,   32, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDBusInterfaceConstructor[] = {
    "QScriptDBusInterfaceConstructor\0\0"
    "QScriptValue\0service,path,interface,conn\0"
    "qscript_call(QString,QString,QString,QScriptValue)\0"
    "service,path,interface\0"
    "qscript_call(QString,QString,QString)\0"
    "service,path\0qscript_call(QString,QString)\0"
};

const QMetaObject QScriptDBusInterfaceConstructor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QScriptDBusInterfaceConstructor,
      qt_meta_data_QScriptDBusInterfaceConstructor, 0 }
};

const QMetaObject *QScriptDBusInterfaceConstructor::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDBusInterfaceConstructor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDBusInterfaceConstructor))
        return static_cast<void*>(const_cast< QScriptDBusInterfaceConstructor*>(this));
    if (!strcmp(_clname, "QScriptable"))
        return static_cast< QScriptable*>(const_cast< QScriptDBusInterfaceConstructor*>(this));
    return QObject::qt_metacast(_clname);
}

int QScriptDBusInterfaceConstructor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QScriptValue _r = qscript_call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QScriptValue(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QScriptValue*>(_a[0]) = _r; }  break;
        case 1: { QScriptValue _r = qscript_call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QScriptValue*>(_a[0]) = _r; }  break;
        case 2: { QScriptValue _r = qscript_call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QScriptValue*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QScriptDBusMessageConstructor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       1,   27, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      64,   44,   31,   30, 0x0a,
     136,  102,   31,   30, 0x0a,
     195,  186,   31,   30, 0x0a,

 // enums: name, flags, count, data
     224, 0x0,    5,   31,

 // enum data: key, value
     236, uint(QScriptDBusMessageConstructor::InvalidMessage),
     251, uint(QScriptDBusMessageConstructor::MethodCallMessage),
     269, uint(QScriptDBusMessageConstructor::ReplyMessage),
     282, uint(QScriptDBusMessageConstructor::ErrorMessage),
     295, uint(QScriptDBusMessageConstructor::SignalMessage),

       0        // eod
};

static const char qt_meta_stringdata_QScriptDBusMessageConstructor[] = {
    "QScriptDBusMessageConstructor\0\0"
    "QDBusMessage\0path,interface,name\0"
    "createSignal(QString,QString,QString)\0"
    "destination,path,interface,method\0"
    "createMethodCall(QString,QString,QString,QString)\0"
    "name,msg\0createError(QString,QString)\0"
    "MessageType\0InvalidMessage\0MethodCallMessage\0"
    "ReplyMessage\0ErrorMessage\0SignalMessage\0"
};

const QMetaObject QScriptDBusMessageConstructor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QScriptDBusMessageConstructor,
      qt_meta_data_QScriptDBusMessageConstructor, 0 }
};

const QMetaObject *QScriptDBusMessageConstructor::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDBusMessageConstructor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDBusMessageConstructor))
        return static_cast<void*>(const_cast< QScriptDBusMessageConstructor*>(this));
    if (!strcmp(_clname, "QScriptable"))
        return static_cast< QScriptable*>(const_cast< QScriptDBusMessageConstructor*>(this));
    return QObject::qt_metacast(_clname);
}

int QScriptDBusMessageConstructor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QDBusMessage _r = createSignal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QDBusMessage*>(_a[0]) = _r; }  break;
        case 1: { QDBusMessage _r = createMethodCall((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QDBusMessage*>(_a[0]) = _r; }  break;
        case 2: { QDBusMessage _r = createError((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusMessage*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
