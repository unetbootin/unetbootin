/****************************************************************************
** Meta object code from reading C++ file 'qobjectcleanuphandler.h'
**
** Created: Thu Mar 5 18:40:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qobjectcleanuphandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qobjectcleanuphandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QObjectCleanupHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QObjectCleanupHandler[] = {
    "QObjectCleanupHandler\0\0objectDestroyed(QObject*)\0"
};

const QMetaObject QObjectCleanupHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QObjectCleanupHandler,
      qt_meta_data_QObjectCleanupHandler, 0 }
};

const QMetaObject *QObjectCleanupHandler::metaObject() const
{
    return &staticMetaObject;
}

void *QObjectCleanupHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QObjectCleanupHandler))
        return static_cast<void*>(const_cast< QObjectCleanupHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int QObjectCleanupHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
