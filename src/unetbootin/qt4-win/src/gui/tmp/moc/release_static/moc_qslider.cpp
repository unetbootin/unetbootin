/****************************************************************************
** Meta object code from reading C++ file 'qslider.h'
**
** Created: Fri May 30 22:33:48 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qslider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qslider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSlider[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       2,   20, // properties
       1,   26, // enums/sets

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x1a,
      19,    8,    8,    8, 0x1a,

 // properties: name, type, flags
      47,   34, 0x0009510b,
      64,   60, 0x02095103,

 // enums: name, flags, count, data
      34, 0x0,    6,   30,

 // enum data: key, value
      77, uint(QSlider::NoTicks),
      85, uint(QSlider::TicksAbove),
      96, uint(QSlider::TicksLeft),
     106, uint(QSlider::TicksBelow),
     117, uint(QSlider::TicksRight),
     128, uint(QSlider::TicksBothSides),

       0        // eod
};

static const char qt_meta_stringdata_QSlider[] = {
    "QSlider\0\0addStep()\0subtractStep()\0"
    "TickPosition\0tickPosition\0int\0"
    "tickInterval\0NoTicks\0TicksAbove\0"
    "TicksLeft\0TicksBelow\0TicksRight\0"
    "TicksBothSides\0"
};

const QMetaObject QSlider::staticMetaObject = {
    { &QAbstractSlider::staticMetaObject, qt_meta_stringdata_QSlider,
      qt_meta_data_QSlider, 0 }
};

const QMetaObject *QSlider::metaObject() const
{
    return &staticMetaObject;
}

void *QSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSlider))
	return static_cast<void*>(const_cast< QSlider*>(this));
    return QAbstractSlider::qt_metacast(_clname);
}

int QSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addStep(); break;
        case 1: subtractStep(); break;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TickPosition*>(_v) = tickPosition(); break;
        case 1: *reinterpret_cast< int*>(_v) = tickInterval(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTickPosition(*reinterpret_cast< TickPosition*>(_v)); break;
        case 1: setTickInterval(*reinterpret_cast< int*>(_v)); break;
        }
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
QT_END_MOC_NAMESPACE
