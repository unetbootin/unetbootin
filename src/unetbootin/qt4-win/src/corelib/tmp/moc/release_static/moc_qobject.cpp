/****************************************************************************
** Meta object code from reading C++ file 'qobject.h'
**
** Created: Fri May 30 22:17:42 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qobject.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qobject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QObject[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       1,   30, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      29,    8,    8,    8, 0x25,

 // slots: signature, parameters, type, tag, flags
      41,    8,    8,    8, 0x0a,
      55,    8,    8,    8, 0x08,

 // properties: name, type, flags
      90,   82, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QObject[] = {
    "QObject\0\0destroyed(QObject*)\0destroyed()\0"
    "deleteLater()\0_q_reregisterTimers(void*)\0"
    "QString\0objectName\0"
};

const QMetaObject QObject::staticMetaObject = {
    { 0, qt_meta_stringdata_QObject,
      qt_meta_data_QObject, 0 }
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
