/****************************************************************************
** Meta object code from reading C++ file 'qstandarditemmodel.h'
**
** Created: Thu Mar 5 18:55:34 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qstandarditemmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qstandarditemmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QStandardItemModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       1,   22, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      25,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      73,   53,   19,   19, 0x08,

 // properties: name, type, flags
     121,  117, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_QStandardItemModel[] = {
    "QStandardItemModel\0\0item\0"
    "itemChanged(QStandardItem*)\0"
    "topLeft,bottomRight\0"
    "_q_emitItemChanged(QModelIndex,QModelIndex)\0"
    "int\0sortRole\0"
};

const QMetaObject QStandardItemModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QStandardItemModel,
      qt_meta_data_QStandardItemModel, 0 }
};

const QMetaObject *QStandardItemModel::metaObject() const
{
    return &staticMetaObject;
}

void *QStandardItemModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStandardItemModel))
        return static_cast<void*>(const_cast< QStandardItemModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QStandardItemModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemChanged((*reinterpret_cast< QStandardItem*(*)>(_a[1]))); break;
        case 1: d_func()->_q_emitItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = sortRole(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSortRole(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QStandardItemModel::itemChanged(QStandardItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
