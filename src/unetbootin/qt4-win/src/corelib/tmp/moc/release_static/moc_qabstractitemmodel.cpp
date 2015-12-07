/****************************************************************************
** Meta object code from reading C++ file 'qabstractitemmodel.h'
**
** Created: Fri May 30 22:18:29 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qabstractitemmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractitemmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractItemModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      40,   20,   19,   19, 0x05,
     100,   77,   19,   19, 0x05,
     143,   19,   19,   19, 0x05,
     159,   19,   19,   19, 0x05,
     202,  184,   19,   19, 0x05,
     245,  184,   19,   19, 0x05,
     279,  184,   19,   19, 0x05,
     321,  184,   19,   19, 0x05,
     354,  184,   19,   19, 0x05,
     400,  184,   19,   19, 0x05,
     437,  184,   19,   19, 0x05,
     482,  184,   19,   19, 0x05,
     518,   19,   19,   19, 0x05,
     540,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     558,   19,  553,   19, 0x0a,
     567,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractItemModel[] = {
    "QAbstractItemModel\0\0topLeft,bottomRight\0"
    "dataChanged(QModelIndex,QModelIndex)\0"
    "orientation,first,last\0"
    "headerDataChanged(Qt::Orientation,int,int)\0"
    "layoutChanged()\0layoutAboutToBeChanged()\0"
    "parent,first,last\0"
    "rowsAboutToBeInserted(QModelIndex,int,int)\0"
    "rowsInserted(QModelIndex,int,int)\0"
    "rowsAboutToBeRemoved(QModelIndex,int,int)\0"
    "rowsRemoved(QModelIndex,int,int)\0"
    "columnsAboutToBeInserted(QModelIndex,int,int)\0"
    "columnsInserted(QModelIndex,int,int)\0"
    "columnsAboutToBeRemoved(QModelIndex,int,int)\0"
    "columnsRemoved(QModelIndex,int,int)\0"
    "modelAboutToBeReset()\0modelReset()\0"
    "bool\0submit()\0revert()\0"
};

const QMetaObject QAbstractItemModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAbstractItemModel,
      qt_meta_data_QAbstractItemModel, 0 }
};

const QMetaObject *QAbstractItemModel::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractItemModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractItemModel))
	return static_cast<void*>(const_cast< QAbstractItemModel*>(this));
    return QObject::qt_metacast(_clname);
}

int QAbstractItemModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 1: headerDataChanged((*reinterpret_cast< Qt::Orientation(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: layoutChanged(); break;
        case 3: layoutAboutToBeChanged(); break;
        case 4: rowsAboutToBeInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: rowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 7: rowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: columnsAboutToBeInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 9: columnsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 10: columnsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: columnsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 12: modelAboutToBeReset(); break;
        case 13: modelReset(); break;
        case 14: { bool _r = submit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: revert(); break;
        }
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void QAbstractItemModel::dataChanged(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QAbstractItemModel::headerDataChanged(Qt::Orientation _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QAbstractItemModel::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QAbstractItemModel::layoutAboutToBeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void QAbstractItemModel::rowsAboutToBeInserted(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QAbstractItemModel::rowsInserted(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QAbstractItemModel::rowsAboutToBeRemoved(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QAbstractItemModel::rowsRemoved(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QAbstractItemModel::columnsAboutToBeInserted(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QAbstractItemModel::columnsInserted(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QAbstractItemModel::columnsAboutToBeRemoved(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void QAbstractItemModel::columnsRemoved(const QModelIndex & _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void QAbstractItemModel::modelAboutToBeReset()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void QAbstractItemModel::modelReset()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}
static const uint qt_meta_data_QAbstractTableModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QAbstractTableModel[] = {
    "QAbstractTableModel\0"
};

const QMetaObject QAbstractTableModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QAbstractTableModel,
      qt_meta_data_QAbstractTableModel, 0 }
};

const QMetaObject *QAbstractTableModel::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractTableModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractTableModel))
	return static_cast<void*>(const_cast< QAbstractTableModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QAbstractTableModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QAbstractListModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QAbstractListModel[] = {
    "QAbstractListModel\0"
};

const QMetaObject QAbstractListModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QAbstractListModel,
      qt_meta_data_QAbstractListModel, 0 }
};

const QMetaObject *QAbstractListModel::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractListModel))
	return static_cast<void*>(const_cast< QAbstractListModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QAbstractListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
