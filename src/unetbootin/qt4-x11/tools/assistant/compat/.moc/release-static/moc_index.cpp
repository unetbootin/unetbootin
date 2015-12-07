/****************************************************************************
** Meta object code from reading C++ file 'index.h'
**
** Created: Thu Mar 5 20:36:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../index.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'index.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Index[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      29,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Index[] = {
    "Index\0\0indexingProgress(int)\0"
    "setLastWinClosed()\0"
};

const QMetaObject Index::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Index,
      qt_meta_data_Index, 0 }
};

const QMetaObject *Index::metaObject() const
{
    return &staticMetaObject;
}

void *Index::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Index))
        return static_cast<void*>(const_cast< Index*>(this));
    return QObject::qt_metacast(_clname);
}

int Index::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexingProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setLastWinClosed(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Index::indexingProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
