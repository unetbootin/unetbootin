/****************************************************************************
** Meta object code from reading C++ file 'qitemselectionmodel.h'
**
** Created: Thu Mar 5 18:55:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qitemselectionmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qitemselectionmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QItemSelectionModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   12, // methods
       0,    0, // properties
       1,   92, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      41,   21,   20,   20, 0x05,
     106,   89,   20,   20, 0x05,
     146,   89,   20,   20, 0x05,
     189,   89,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
     249,  235,   20,   20, 0x0a,
     314,  235,   20,   20, 0x0a,
     388,  370,   20,   20, 0x0a,
     447,   20,   20,   20, 0x0a,
     455,   20,   20,   20, 0x0a,
     463,   20,   20,   20, 0x0a,
     483,  480,   20,   20, 0x08,
     531,  480,   20,   20, 0x08,
     576,  480,   20,   20, 0x08,
     625,  480,   20,   20, 0x08,
     671,   20,   20,   20, 0x08,
     699,   20,   20,   20, 0x08,

 // enums: name, flags, count, data
     718, 0x1,   11,   96,

 // enum data: key, value
     733, uint(QItemSelectionModel::NoUpdate),
     742, uint(QItemSelectionModel::Clear),
     748, uint(QItemSelectionModel::Select),
     755, uint(QItemSelectionModel::Deselect),
     764, uint(QItemSelectionModel::Toggle),
     771, uint(QItemSelectionModel::Current),
     779, uint(QItemSelectionModel::Rows),
     784, uint(QItemSelectionModel::Columns),
     792, uint(QItemSelectionModel::SelectCurrent),
     806, uint(QItemSelectionModel::ToggleCurrent),
     820, uint(QItemSelectionModel::ClearAndSelect),

       0        // eod
};

static const char qt_meta_stringdata_QItemSelectionModel[] = {
    "QItemSelectionModel\0\0selected,deselected\0"
    "selectionChanged(QItemSelection,QItemSelection)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "currentRowChanged(QModelIndex,QModelIndex)\0"
    "currentColumnChanged(QModelIndex,QModelIndex)\0"
    "index,command\0"
    "setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)\0"
    "select(QModelIndex,QItemSelectionModel::SelectionFlags)\0"
    "selection,command\0"
    "select(QItemSelection,QItemSelectionModel::SelectionFlags)\0"
    "clear()\0reset()\0clearSelection()\0,,\0"
    "_q_columnsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_rowsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_columnsAboutToBeInserted(QModelIndex,int,int)\0"
    "_q_rowsAboutToBeInserted(QModelIndex,int,int)\0"
    "_q_layoutAboutToBeChanged()\0"
    "_q_layoutChanged()\0SelectionFlags\0"
    "NoUpdate\0Clear\0Select\0Deselect\0Toggle\0"
    "Current\0Rows\0Columns\0SelectCurrent\0"
    "ToggleCurrent\0ClearAndSelect\0"
};

const QMetaObject QItemSelectionModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QItemSelectionModel,
      qt_meta_data_QItemSelectionModel, 0 }
};

const QMetaObject *QItemSelectionModel::metaObject() const
{
    return &staticMetaObject;
}

void *QItemSelectionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QItemSelectionModel))
        return static_cast<void*>(const_cast< QItemSelectionModel*>(this));
    return QObject::qt_metacast(_clname);
}

int QItemSelectionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 1: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 2: currentRowChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 3: currentColumnChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 4: setCurrentIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< QItemSelectionModel::SelectionFlags(*)>(_a[2]))); break;
        case 5: select((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< QItemSelectionModel::SelectionFlags(*)>(_a[2]))); break;
        case 6: select((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< QItemSelectionModel::SelectionFlags(*)>(_a[2]))); break;
        case 7: clear(); break;
        case 8: reset(); break;
        case 9: clearSelection(); break;
        case 10: d_func()->_q_columnsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: d_func()->_q_rowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 12: d_func()->_q_columnsAboutToBeInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 13: d_func()->_q_rowsAboutToBeInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 14: d_func()->_q_layoutAboutToBeChanged(); break;
        case 15: d_func()->_q_layoutChanged(); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void QItemSelectionModel::selectionChanged(const QItemSelection & _t1, const QItemSelection & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QItemSelectionModel::currentChanged(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QItemSelectionModel::currentRowChanged(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QItemSelectionModel::currentColumnChanged(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
