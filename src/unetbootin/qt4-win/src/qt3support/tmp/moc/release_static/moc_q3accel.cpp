/****************************************************************************
** Meta object code from reading C++ file 'q3accel.h'
**
** Created: Fri May 30 22:41:08 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../other/q3accel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3accel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3Accel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,    9,    8,    8, 0x05,
      27,    9,    8,    8, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Q3Accel[] = {
    "Q3Accel\0\0id\0activated(int)\0"
    "activatedAmbiguously(int)\0"
};

const QMetaObject Q3Accel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3Accel,
      qt_meta_data_Q3Accel, 0 }
};

const QMetaObject *Q3Accel::metaObject() const
{
    return &staticMetaObject;
}

void *Q3Accel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3Accel))
	return static_cast<void*>(const_cast< Q3Accel*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3Accel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: activatedAmbiguously((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Q3Accel::activated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3Accel::activatedAmbiguously(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
