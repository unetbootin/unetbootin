/****************************************************************************
** Meta object code from reading C++ file 'q3signal.h'
**
** Created: Fri May 30 22:41:04 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../tools/q3signal.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3signal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3Signal[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      27,    9,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Q3Signal[] = {
    "Q3Signal\0\0signal(QVariant)\0intSignal(int)\0"
};

const QMetaObject Q3Signal::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3Signal,
      qt_meta_data_Q3Signal, 0 }
};

const QMetaObject *Q3Signal::metaObject() const
{
    return &staticMetaObject;
}

void *Q3Signal::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3Signal))
	return static_cast<void*>(const_cast< Q3Signal*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3Signal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signal((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 1: intSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Q3Signal::signal(const QVariant & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3Signal::intSignal(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
