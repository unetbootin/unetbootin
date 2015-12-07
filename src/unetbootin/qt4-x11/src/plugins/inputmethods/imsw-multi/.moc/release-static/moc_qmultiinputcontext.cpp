/****************************************************************************
** Meta object code from reading C++ file 'qmultiinputcontext.h'
**
** Created: Thu Mar 5 20:28:21 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qmultiinputcontext.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmultiinputcontext.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMultiInputContext[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_QMultiInputContext[] = {
    "QMultiInputContext\0\0changeSlave(QAction*)\0"
};

const QMetaObject QMultiInputContext::staticMetaObject = {
    { &QInputContext::staticMetaObject, qt_meta_stringdata_QMultiInputContext,
      qt_meta_data_QMultiInputContext, 0 }
};

const QMetaObject *QMultiInputContext::metaObject() const
{
    return &staticMetaObject;
}

void *QMultiInputContext::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMultiInputContext))
        return static_cast<void*>(const_cast< QMultiInputContext*>(this));
    return QInputContext::qt_metacast(_clname);
}

int QMultiInputContext::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QInputContext::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changeSlave((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
