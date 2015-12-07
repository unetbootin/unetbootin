/****************************************************************************
** Meta object code from reading C++ file 'actionrepository_p.h'
**
** Created: Thu Mar 5 20:47:05 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/actionrepository_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'actionrepository_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ActionModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      45,   33,   32,   32, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ActionModel[] = {
    "qdesigner_internal::ActionModel\0\0"
    "path,action\0resourceImageDropped(QString,QAction*)\0"
};

const QMetaObject qdesigner_internal::ActionModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ActionModel,
      qt_meta_data_qdesigner_internal__ActionModel, 0 }
};

const QMetaObject *qdesigner_internal::ActionModel::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ActionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ActionModel))
        return static_cast<void*>(const_cast< ActionModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int qdesigner_internal::ActionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resourceImageDropped((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QAction*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ActionModel::resourceImageDropped(const QString & _t1, QAction * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__ActionTreeView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      43,   36,   35,   35, 0x05,
     100,   93,   35,   35, 0x05,
     125,   93,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
     150,  145,   35,   35, 0x0a,
     183,  166,   35,   35, 0x09,
     223,   35,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ActionTreeView[] = {
    "qdesigner_internal::ActionTreeView\0\0"
    "event,\0contextMenuRequested(QContextMenuEvent*,QAction*)\0"
    "action\0currentChanged(QAction*)\0"
    "activated(QAction*)\0text\0filter(QString)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "slotActivated(QModelIndex)\0"
};

const QMetaObject qdesigner_internal::ActionTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ActionTreeView,
      qt_meta_data_qdesigner_internal__ActionTreeView, 0 }
};

const QMetaObject *qdesigner_internal::ActionTreeView::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ActionTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ActionTreeView))
        return static_cast<void*>(const_cast< ActionTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int qdesigner_internal::ActionTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contextMenuRequested((*reinterpret_cast< QContextMenuEvent*(*)>(_a[1])),(*reinterpret_cast< QAction*(*)>(_a[2]))); break;
        case 1: currentChanged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: activated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: filter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 5: slotActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ActionTreeView::contextMenuRequested(QContextMenuEvent * _t1, QAction * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::ActionTreeView::currentChanged(QAction * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::ActionTreeView::activated(QAction * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_qdesigner_internal__ActionListView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      43,   36,   35,   35, 0x05,
     100,   93,   35,   35, 0x05,
     125,   93,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
     150,  145,   35,   35, 0x0a,
     183,  166,   35,   35, 0x09,
     223,   35,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ActionListView[] = {
    "qdesigner_internal::ActionListView\0\0"
    "event,\0contextMenuRequested(QContextMenuEvent*,QAction*)\0"
    "action\0currentChanged(QAction*)\0"
    "activated(QAction*)\0text\0filter(QString)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "slotActivated(QModelIndex)\0"
};

const QMetaObject qdesigner_internal::ActionListView::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ActionListView,
      qt_meta_data_qdesigner_internal__ActionListView, 0 }
};

const QMetaObject *qdesigner_internal::ActionListView::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ActionListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ActionListView))
        return static_cast<void*>(const_cast< ActionListView*>(this));
    return QListView::qt_metacast(_clname);
}

int qdesigner_internal::ActionListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contextMenuRequested((*reinterpret_cast< QContextMenuEvent*(*)>(_a[1])),(*reinterpret_cast< QAction*(*)>(_a[2]))); break;
        case 1: currentChanged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: activated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: filter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 5: slotActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ActionListView::contextMenuRequested(QContextMenuEvent * _t1, QAction * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::ActionListView::currentChanged(QAction * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::ActionListView::activated(QAction * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_qdesigner_internal__ActionView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      39,   32,   31,   31, 0x05,
      96,   89,   31,   31, 0x05,
     121,   89,   31,   31, 0x05,
     161,  141,   31,   31, 0x05,
     221,  209,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
     265,  260,   31,   31, 0x0a,
     281,   31,   31,   31, 0x0a,
     293,   31,   31,   31, 0x0a,
     310,   89,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ActionView[] = {
    "qdesigner_internal::ActionView\0\0event,\0"
    "contextMenuRequested(QContextMenuEvent*,QAction*)\0"
    "action\0currentChanged(QAction*)\0"
    "activated(QAction*)\0selected,deselected\0"
    "selectionChanged(QItemSelection,QItemSelection)\0"
    "data,action\0resourceImageDropped(QString,QAction*)\0"
    "text\0filter(QString)\0selectAll()\0"
    "clearSelection()\0slotCurrentChanged(QAction*)\0"
};

const QMetaObject qdesigner_internal::ActionView::staticMetaObject = {
    { &QStackedWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ActionView,
      qt_meta_data_qdesigner_internal__ActionView, 0 }
};

const QMetaObject *qdesigner_internal::ActionView::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ActionView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ActionView))
        return static_cast<void*>(const_cast< ActionView*>(this));
    return QStackedWidget::qt_metacast(_clname);
}

int qdesigner_internal::ActionView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contextMenuRequested((*reinterpret_cast< QContextMenuEvent*(*)>(_a[1])),(*reinterpret_cast< QAction*(*)>(_a[2]))); break;
        case 1: currentChanged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: activated((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 4: resourceImageDropped((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QAction*(*)>(_a[2]))); break;
        case 5: filter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: selectAll(); break;
        case 7: clearSelection(); break;
        case 8: slotCurrentChanged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ActionView::contextMenuRequested(QContextMenuEvent * _t1, QAction * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::ActionView::currentChanged(QAction * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::ActionView::activated(QAction * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qdesigner_internal::ActionView::selectionChanged(const QItemSelection & _t1, const QItemSelection & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void qdesigner_internal::ActionView::resourceImageDropped(const QString & _t1, QAction * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
static const uint qt_meta_data_qdesigner_internal__ActionRepositoryMimeData[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ActionRepositoryMimeData[] = {
    "qdesigner_internal::ActionRepositoryMimeData\0"
};

const QMetaObject qdesigner_internal::ActionRepositoryMimeData::staticMetaObject = {
    { &QMimeData::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ActionRepositoryMimeData,
      qt_meta_data_qdesigner_internal__ActionRepositoryMimeData, 0 }
};

const QMetaObject *qdesigner_internal::ActionRepositoryMimeData::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ActionRepositoryMimeData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ActionRepositoryMimeData))
        return static_cast<void*>(const_cast< ActionRepositoryMimeData*>(this));
    return QMimeData::qt_metacast(_clname);
}

int qdesigner_internal::ActionRepositoryMimeData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMimeData::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
