/****************************************************************************
** Meta object code from reading C++ file 'qtreewidget.h'
**
** Created: Thu Mar 5 18:55:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qtreewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtreewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTreeWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   12, // methods
       2,  147, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      25,   13,   12,   12, 0x05,
      59,   13,   12,   12, 0x05,
      93,   13,   12,   12, 0x05,
     133,   13,   12,   12, 0x05,
     169,   13,   12,   12, 0x05,
     203,   13,   12,   12, 0x05,
     242,  237,   12,   12, 0x05,
     273,  237,   12,   12, 0x05,
     322,  305,   12,   12, 0x05,
     376,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     409,  399,   12,   12, 0x0a,
     476,  237,   12,   12, 0x2a,
     513,  237,   12,   12, 0x0a,
     548,  237,   12,   12, 0x0a,
     585,   12,   12,   12, 0x0a,
     599,  593,   12,   12, 0x08,
     631,  593,   12,   12, 0x08,
     663,  593,   12,   12, 0x08,
     701,  593,   12,   12, 0x08,
     735,  593,   12,   12, 0x08,
     767,  593,   12,   12, 0x08,
     799,  593,   12,   12, 0x08,
     832,  593,   12,   12, 0x08,
     883,  866,   12,   12, 0x08,
     934,   12,   12,   12, 0x08,
     964,  944,   12,   12, 0x08,
    1024, 1004,   12,   12, 0x08,

 // properties: name, type, flags
    1079, 1075, 0x02095103,
    1091, 1075, 0x02095001,

       0        // eod
};

static const char qt_meta_stringdata_QTreeWidget[] = {
    "QTreeWidget\0\0item,column\0"
    "itemPressed(QTreeWidgetItem*,int)\0"
    "itemClicked(QTreeWidgetItem*,int)\0"
    "itemDoubleClicked(QTreeWidgetItem*,int)\0"
    "itemActivated(QTreeWidgetItem*,int)\0"
    "itemEntered(QTreeWidgetItem*,int)\0"
    "itemChanged(QTreeWidgetItem*,int)\0"
    "item\0itemExpanded(QTreeWidgetItem*)\0"
    "itemCollapsed(QTreeWidgetItem*)\0"
    "current,previous\0"
    "currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)\0"
    "itemSelectionChanged()\0item,hint\0"
    "scrollToItem(const QTreeWidgetItem*,QAbstractItemView::ScrollHint)\0"
    "scrollToItem(const QTreeWidgetItem*)\0"
    "expandItem(const QTreeWidgetItem*)\0"
    "collapseItem(const QTreeWidgetItem*)\0"
    "clear()\0index\0_q_emitItemPressed(QModelIndex)\0"
    "_q_emitItemClicked(QModelIndex)\0"
    "_q_emitItemDoubleClicked(QModelIndex)\0"
    "_q_emitItemActivated(QModelIndex)\0"
    "_q_emitItemEntered(QModelIndex)\0"
    "_q_emitItemChanged(QModelIndex)\0"
    "_q_emitItemExpanded(QModelIndex)\0"
    "_q_emitItemCollapsed(QModelIndex)\0"
    "previous,current\0"
    "_q_emitCurrentItemChanged(QModelIndex,QModelIndex)\0"
    "_q_sort()\0topLeft,bottomRight\0"
    "_q_dataChanged(QModelIndex,QModelIndex)\0"
    "selected,deselected\0"
    "_q_selectionChanged(QItemSelection,QItemSelection)\0"
    "int\0columnCount\0topLevelItemCount\0"
};

const QMetaObject QTreeWidget::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_QTreeWidget,
      qt_meta_data_QTreeWidget, 0 }
};

const QMetaObject *QTreeWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTreeWidget))
        return static_cast<void*>(const_cast< QTreeWidget*>(this));
    return QTreeView::qt_metacast(_clname);
}

int QTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: itemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: itemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: itemActivated((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: itemEntered((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: itemExpanded((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 7: itemCollapsed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 8: currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 9: itemSelectionChanged(); break;
        case 10: scrollToItem((*reinterpret_cast< const QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemView::ScrollHint(*)>(_a[2]))); break;
        case 11: scrollToItem((*reinterpret_cast< const QTreeWidgetItem*(*)>(_a[1]))); break;
        case 12: expandItem((*reinterpret_cast< const QTreeWidgetItem*(*)>(_a[1]))); break;
        case 13: collapseItem((*reinterpret_cast< const QTreeWidgetItem*(*)>(_a[1]))); break;
        case 14: clear(); break;
        case 15: d_func()->_q_emitItemPressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 16: d_func()->_q_emitItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 17: d_func()->_q_emitItemDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 18: d_func()->_q_emitItemActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 19: d_func()->_q_emitItemEntered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 20: d_func()->_q_emitItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 21: d_func()->_q_emitItemExpanded((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 22: d_func()->_q_emitItemCollapsed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 23: d_func()->_q_emitCurrentItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 24: d_func()->_q_sort(); break;
        case 25: d_func()->_q_dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 26: d_func()->_q_selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 27;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = columnCount(); break;
        case 1: *reinterpret_cast< int*>(_v) = topLevelItemCount(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setColumnCount(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QTreeWidget::itemPressed(QTreeWidgetItem * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTreeWidget::itemClicked(QTreeWidgetItem * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTreeWidget::itemDoubleClicked(QTreeWidgetItem * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QTreeWidget::itemActivated(QTreeWidgetItem * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QTreeWidget::itemEntered(QTreeWidgetItem * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QTreeWidget::itemChanged(QTreeWidgetItem * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QTreeWidget::itemExpanded(QTreeWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QTreeWidget::itemCollapsed(QTreeWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QTreeWidget::currentItemChanged(QTreeWidgetItem * _t1, QTreeWidgetItem * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QTreeWidget::itemSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}
QT_END_MOC_NAMESPACE
