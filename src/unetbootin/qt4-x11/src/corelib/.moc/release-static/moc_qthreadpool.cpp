/****************************************************************************
** Meta object code from reading C++ file 'qthreadpool.h'
**
** Created: Thu Mar 5 18:40:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../concurrent/qthreadpool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qthreadpool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QThreadPool[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      16,   12, 0x02095103,
      30,   12, 0x02095103,
      45,   12, 0x02095001,

       0        // eod
};

static const char qt_meta_stringdata_QThreadPool[] = {
    "QThreadPool\0int\0expiryTimeout\0"
    "maxThreadCount\0activeThreadCount\0"
};

const QMetaObject QThreadPool::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QThreadPool,
      qt_meta_data_QThreadPool, 0 }
};

const QMetaObject *QThreadPool::metaObject() const
{
    return &staticMetaObject;
}

void *QThreadPool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QThreadPool))
        return static_cast<void*>(const_cast< QThreadPool*>(this));
    return QObject::qt_metacast(_clname);
}

int QThreadPool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = expiryTimeout(); break;
        case 1: *reinterpret_cast< int*>(_v) = maxThreadCount(); break;
        case 2: *reinterpret_cast< int*>(_v) = activeThreadCount(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setExpiryTimeout(*reinterpret_cast< int*>(_v)); break;
        case 1: setMaxThreadCount(*reinterpret_cast< int*>(_v)); break;
        }
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
QT_END_MOC_NAMESPACE
