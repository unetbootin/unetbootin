/****************************************************************************
** Meta object code from reading C++ file 'qdial.h'
**
** Created: Fri May 30 22:33:40 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qdial.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdial.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDial[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       4,   35, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x15,
      27,   21,    6,    6, 0x15,
      42,    6,    6,    6, 0x15,

 // slots: signature, parameters, type, tag, flags
      65,   57,    6,    6, 0x0a,
      92,   89,    6,    6, 0x0a,

 // properties: name, type, flags
     115,  110, 0x01095103,
     128,  124, 0x02095001,
     144,  138, 0x06095103,
     156,  110, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QDial[] = {
    "QDial\0\0dialPressed()\0value\0dialMoved(int)\0"
    "dialReleased()\0visible\0setNotchesVisible(bool)\0"
    "on\0setWrapping(bool)\0bool\0wrapping\0"
    "int\0notchSize\0qreal\0notchTarget\0"
    "notchesVisible\0"
};

const QMetaObject QDial::staticMetaObject = {
    { &QAbstractSlider::staticMetaObject, qt_meta_stringdata_QDial,
      qt_meta_data_QDial, 0 }
};

const QMetaObject *QDial::metaObject() const
{
    return &staticMetaObject;
}

void *QDial::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDial))
	return static_cast<void*>(const_cast< QDial*>(this));
    return QAbstractSlider::qt_metacast(_clname);
}

int QDial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dialPressed(); break;
        case 1: dialMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: dialReleased(); break;
        case 3: setNotchesVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: setWrapping((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = wrapping(); break;
        case 1: *reinterpret_cast< int*>(_v) = notchSize(); break;
        case 2: *reinterpret_cast< qreal*>(_v) = notchTarget(); break;
        case 3: *reinterpret_cast< bool*>(_v) = notchesVisible(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setWrapping(*reinterpret_cast< bool*>(_v)); break;
        case 2: setNotchTarget(*reinterpret_cast< qreal*>(_v)); break;
        case 3: setNotchesVisible(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QDial::dialPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QDial::dialMoved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QDial::dialReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
