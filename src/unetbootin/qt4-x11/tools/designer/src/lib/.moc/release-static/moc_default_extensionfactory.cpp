/****************************************************************************
** Meta object code from reading C++ file 'default_extensionfactory.h'
**
** Created: Thu Mar 5 20:46:36 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../extension/default_extensionfactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'default_extensionfactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QExtensionFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   19,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QExtensionFactory[] = {
    "QExtensionFactory\0\0object\0"
    "objectDestroyed(QObject*)\0"
};

const QMetaObject QExtensionFactory::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QExtensionFactory,
      qt_meta_data_QExtensionFactory, 0 }
};

const QMetaObject *QExtensionFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QExtensionFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QExtensionFactory))
        return static_cast<void*>(const_cast< QExtensionFactory*>(this));
    if (!strcmp(_clname, "QAbstractExtensionFactory"))
        return static_cast< QAbstractExtensionFactory*>(const_cast< QExtensionFactory*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.QAbstractExtensionFactory"))
        return static_cast< QAbstractExtensionFactory*>(const_cast< QExtensionFactory*>(this));
    return QObject::qt_metacast(_clname);
}

int QExtensionFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: objectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
