/****************************************************************************
** Meta object code from reading C++ file 'qdbusabstractinterface.h'
**
** Created: Thu Mar 5 19:06:35 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdbusabstractinterface.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdbusabstractinterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusAbstractInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      27,   24,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDBusAbstractInterface[] = {
    "QDBusAbstractInterface\0\0,,\0"
    "_q_serviceOwnerChanged(QString,QString,QString)\0"
};

const QMetaObject QDBusAbstractInterface::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDBusAbstractInterface,
      qt_meta_data_QDBusAbstractInterface, 0 }
};

const QMetaObject *QDBusAbstractInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QDBusAbstractInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDBusAbstractInterface))
        return static_cast<void*>(const_cast< QDBusAbstractInterface*>(this));
    return QObject::qt_metacast(_clname);
}

int QDBusAbstractInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_serviceOwnerChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
