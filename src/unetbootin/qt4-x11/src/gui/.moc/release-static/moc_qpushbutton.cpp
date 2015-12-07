/****************************************************************************
** Meta object code from reading C++ file 'qpushbutton.h'
**
** Created: Thu Mar 5 18:52:20 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qpushbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qpushbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPushButton[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       3,   22, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      24,   12,   12,   12, 0x08,

 // properties: name, type, flags
      47,   42, 0x01095103,
      59,   42, 0x01095103,
      67,   42, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QPushButton[] = {
    "QPushButton\0\0showMenu()\0_q_popupPressed()\0"
    "bool\0autoDefault\0default\0flat\0"
};

const QMetaObject QPushButton::staticMetaObject = {
    { &QAbstractButton::staticMetaObject, qt_meta_stringdata_QPushButton,
      qt_meta_data_QPushButton, 0 }
};

const QMetaObject *QPushButton::metaObject() const
{
    return &staticMetaObject;
}

void *QPushButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPushButton))
        return static_cast<void*>(const_cast< QPushButton*>(this));
    return QAbstractButton::qt_metacast(_clname);
}

int QPushButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showMenu(); break;
        case 1: d_func()->_q_popupPressed(); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = autoDefault(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isDefault(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isFlat(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAutoDefault(*reinterpret_cast< bool*>(_v)); break;
        case 1: setDefault(*reinterpret_cast< bool*>(_v)); break;
        case 2: setFlat(*reinterpret_cast< bool*>(_v)); break;
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
