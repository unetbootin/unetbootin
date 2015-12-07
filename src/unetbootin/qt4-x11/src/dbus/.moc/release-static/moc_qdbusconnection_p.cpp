/****************************************************************************
** Meta object code from reading C++ file 'qdbusconnection_p.h'
**
** Created: Thu Mar 5 19:07:21 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdbusconnection_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdbusconnection_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusConnectionPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      47,   24,   23,   23, 0x05,
     106,   92,   23,   23, 0x05,
     165,  154,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
     202,   23,   23,   23, 0x0a,
     215,   23,   23,   23, 0x0a,
     231,   23,   23,   23, 0x0a,
     250,  248,   23,   23, 0x0a,
     295,  276,   23,   23, 0x0a,
     353,   24,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QDBusConnectionPrivate[] = {
    "QDBusConnectionPrivate\0\0name,oldOwner,newOwner\0"
    "serviceOwnerChanged(QString,QString,QString)\0"
    "error,message\0"
    "callWithCallbackFailed(QDBusError,QDBusMessage)\0"
    "connection\0newServerConnection(QDBusConnection)\0"
    "doDispatch()\0socketRead(int)\0"
    "socketWrite(int)\0o\0objectDestroyed(QObject*)\0"
    "obj,,signalId,args\0"
    "relaySignal(QObject*,const QMetaObject*,int,QVariantList)\0"
    "_q_serviceOwnerChanged(QString,QString,QString)\0"
};

const QMetaObject QDBusConnectionPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDBusConnectionPrivate,
      qt_meta_data_QDBusConnectionPrivate, 0 }
};

const QMetaObject *QDBusConnectionPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *QDBusConnectionPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDBusConnectionPrivate))
        return static_cast<void*>(const_cast< QDBusConnectionPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int QDBusConnectionPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: serviceOwnerChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: callWithCallbackFailed((*reinterpret_cast< const QDBusError(*)>(_a[1])),(*reinterpret_cast< const QDBusMessage(*)>(_a[2]))); break;
        case 2: newServerConnection((*reinterpret_cast< const QDBusConnection(*)>(_a[1]))); break;
        case 3: doDispatch(); break;
        case 4: socketRead((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: socketWrite((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: objectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 7: relaySignal((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< const QMetaObject*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< const QVariantList(*)>(_a[4]))); break;
        case 8: _q_serviceOwnerChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QDBusConnectionPrivate::serviceOwnerChanged(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDBusConnectionPrivate::callWithCallbackFailed(const QDBusError & _t1, const QDBusMessage & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QDBusConnectionPrivate::newServerConnection(const QDBusConnection & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
