/****************************************************************************
** Meta object code from reading C++ file 'qtablewidget.h'
**
** Created: Thu Mar 5 18:55:14 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qtablewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtablewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTableWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   12, // methods
       2,  172, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x05,
      50,   14,   13,   13, 0x05,
      81,   14,   13,   13, 0x05,
     118,   14,   13,   13, 0x05,
     151,   14,   13,   13, 0x05,
     182,   14,   13,   13, 0x05,
     230,  213,   13,   13, 0x05,
     286,   13,   13,   13, 0x05,
     320,  309,   13,   13, 0x05,
     341,  309,   13,   13, 0x05,
     362,  309,   13,   13, 0x05,
     389,  309,   13,   13, 0x05,
     412,  309,   13,   13, 0x05,
     433,  309,   13,   13, 0x05,
     506,  454,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     552,  542,   13,   13, 0x0a,
     620,   14,   13,   13, 0x2a,
     662,  658,   13,   13, 0x0a,
     684,  677,   13,   13, 0x0a,
     702,  658,   13,   13, 0x0a,
     717,  677,   13,   13, 0x0a,
     735,   13,   13,   13, 0x0a,
     743,   13,   13,   13, 0x0a,
     765,  759,   13,   13, 0x08,
     797,  759,   13,   13, 0x08,
     829,  759,   13,   13, 0x08,
     867,  759,   13,   13, 0x08,
     901,  759,   13,   13, 0x08,
     933,  759,   13,   13, 0x08,
     982,  965,   13,   13, 0x08,
    1033,   13,   13,   13, 0x08,
    1063, 1043,   13,   13, 0x08,

 // properties: name, type, flags
    1107, 1103, 0x02095103,
    1116, 1103, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_QTableWidget[] = {
    "QTableWidget\0\0item\0itemPressed(QTableWidgetItem*)\0"
    "itemClicked(QTableWidgetItem*)\0"
    "itemDoubleClicked(QTableWidgetItem*)\0"
    "itemActivated(QTableWidgetItem*)\0"
    "itemEntered(QTableWidgetItem*)\0"
    "itemChanged(QTableWidgetItem*)\0"
    "current,previous\0"
    "currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)\0"
    "itemSelectionChanged()\0row,column\0"
    "cellPressed(int,int)\0cellClicked(int,int)\0"
    "cellDoubleClicked(int,int)\0"
    "cellActivated(int,int)\0cellEntered(int,int)\0"
    "cellChanged(int,int)\0"
    "currentRow,currentColumn,previousRow,previousColumn\0"
    "currentCellChanged(int,int,int,int)\0"
    "item,hint\0"
    "scrollToItem(const QTableWidgetItem*,QAbstractItemView::ScrollHint)\0"
    "scrollToItem(const QTableWidgetItem*)\0"
    "row\0insertRow(int)\0column\0insertColumn(int)\0"
    "removeRow(int)\0removeColumn(int)\0"
    "clear()\0clearContents()\0index\0"
    "_q_emitItemPressed(QModelIndex)\0"
    "_q_emitItemClicked(QModelIndex)\0"
    "_q_emitItemDoubleClicked(QModelIndex)\0"
    "_q_emitItemActivated(QModelIndex)\0"
    "_q_emitItemEntered(QModelIndex)\0"
    "_q_emitItemChanged(QModelIndex)\0"
    "previous,current\0"
    "_q_emitCurrentItemChanged(QModelIndex,QModelIndex)\0"
    "_q_sort()\0topLeft,bottomRight\0"
    "_q_dataChanged(QModelIndex,QModelIndex)\0"
    "int\0rowCount\0columnCount\0"
};

const QMetaObject QTableWidget::staticMetaObject = {
    { &QTableView::staticMetaObject, qt_meta_stringdata_QTableWidget,
      qt_meta_data_QTableWidget, 0 }
};

const QMetaObject *QTableWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QTableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTableWidget))
        return static_cast<void*>(const_cast< QTableWidget*>(this));
    return QTableView::qt_metacast(_clname);
}

int QTableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemPressed((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 1: itemClicked((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 2: itemDoubleClicked((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 3: itemActivated((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: itemEntered((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 5: itemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 6: currentItemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTableWidgetItem*(*)>(_a[2]))); break;
        case 7: itemSelectionChanged(); break;
        case 8: cellPressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: cellDoubleClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: cellActivated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: cellEntered((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: currentCellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 15: scrollToItem((*reinterpret_cast< const QTableWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemView::ScrollHint(*)>(_a[2]))); break;
        case 16: scrollToItem((*reinterpret_cast< const QTableWidgetItem*(*)>(_a[1]))); break;
        case 17: insertRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: insertColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: removeRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: removeColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: clear(); break;
        case 22: clearContents(); break;
        case 23: d_func()->_q_emitItemPressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 24: d_func()->_q_emitItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 25: d_func()->_q_emitItemDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 26: d_func()->_q_emitItemActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 27: d_func()->_q_emitItemEntered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 28: d_func()->_q_emitItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 29: d_func()->_q_emitCurrentItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 30: d_func()->_q_sort(); break;
        case 31: d_func()->_q_dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 32;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = rowCount(); break;
        case 1: *reinterpret_cast< int*>(_v) = columnCount(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setRowCount(*reinterpret_cast< int*>(_v)); break;
        case 1: setColumnCount(*reinterpret_cast< int*>(_v)); break;
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
void QTableWidget::itemPressed(QTableWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTableWidget::itemClicked(QTableWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTableWidget::itemDoubleClicked(QTableWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QTableWidget::itemActivated(QTableWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QTableWidget::itemEntered(QTableWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QTableWidget::itemChanged(QTableWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QTableWidget::currentItemChanged(QTableWidgetItem * _t1, QTableWidgetItem * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QTableWidget::itemSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void QTableWidget::cellPressed(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QTableWidget::cellClicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QTableWidget::cellDoubleClicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void QTableWidget::cellActivated(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void QTableWidget::cellEntered(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void QTableWidget::cellChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void QTableWidget::currentCellChanged(int _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}
QT_END_MOC_NAMESPACE
