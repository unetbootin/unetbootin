/****************************************************************************
** Meta object code from reading C++ file 'qobject.h'
**
** Created: Thu Mar 5 18:40:08 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qobject.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qobject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QObject[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       1,   32, // properties
       0,    0, // enums/sets
       2,   35, // constructors

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      29,    8,    8,    8, 0x25,

 // slots: signature, parameters, type, tag, flags
      41,    8,    8,    8, 0x0a,
      55,    8,    8,    8, 0x08,

 // properties: name, type, flags
      90,   82, 0x0a095103,

 // constructors: signature, parameters, type, tag, flags
     108,  101,    8,    8, 0x0e,
     126,    8,    8,    8, 0x2e,

       0        // eod
};

static const char qt_meta_stringdata_QObject[] = {
    "QObject\0\0destroyed(QObject*)\0destroyed()\0"
    "deleteLater()\0_q_reregisterTimers(void*)\0"
    "QString\0objectName\0parent\0QObject(QObject*)\0"
    "QObject()\0"
};

static int QObject_qt_static_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QObject *_r = new QObject((*reinterpret_cast< QObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { QObject *_r = new QObject();
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        }
        _id -= 2;
        return _id;
    }
    if (_id < 0)
        return _id;
    return _id;
}

static const QMetaObjectExtraData qt_meta_extradata2_QObject = {
    0, QObject_qt_static_metacall 
};

const QMetaObject QObject::staticMetaObject = {
    { 0, qt_meta_stringdata_QObject,
      qt_meta_data_QObject, &qt_meta_extradata2_QObject }
};

const QMetaObject *QObject::metaObject() const
{
    return &staticMetaObject;
}

void *QObject::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QObject))
        return static_cast<void*>(const_cast< QObject*>(this));
    return 0;
}

int QObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: destroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 1: destroyed(); break;
        case 2: deleteLater(); break;
        case 3: d_func()->_q_reregisterTimers((*reinterpret_cast< void*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = objectName(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setObjectName(*reinterpret_cast< QString*>(_v)); break;
        }
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
void QObject::destroyed(QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, 1, _a);
}
QT_END_MOC_NAMESPACE
