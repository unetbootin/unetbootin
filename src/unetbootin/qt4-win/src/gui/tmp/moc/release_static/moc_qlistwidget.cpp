/****************************************************************************
** Meta object code from reading C++ file 'qlistwidget.h'
**
** Created: Fri May 30 22:31:23 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qlistwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlistwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QListWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   10, // methods
       3,  120, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x05,
      48,   13,   12,   12, 0x05,
      78,   13,   12,   12, 0x05,
     114,   13,   12,   12, 0x05,
     146,   13,   12,   12, 0x05,
     176,   13,   12,   12, 0x05,
     223,  206,   12,   12, 0x05,
     289,  277,   12,   12, 0x05,
     328,  317,   12,   12, 0x05,
     351,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     384,  374,   12,   12, 0x0a,
     451,   13,   12,   12, 0x2a,
     488,   12,   12,   12, 0x0a,
     502,  496,   12,   12, 0x08,
     534,  496,   12,   12, 0x08,
     566,  496,   12,   12, 0x08,
     604,  496,   12,   12, 0x08,
     638,  496,   12,   12, 0x08,
     670,  496,   12,   12, 0x08,
     719,  702,   12,   12, 0x08,
     770,   12,   12,   12, 0x08,
     800,  780,   12,   12, 0x08,

 // properties: name, type, flags
     844,  840, 0x02095001,
     317,  840, 0x02195103,
     855,  850, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QListWidget[] = {
    "QListWidget\0\0item\0itemPressed(QListWidgetItem*)\0"
    "itemClicked(QListWidgetItem*)\0"
    "itemDoubleClicked(QListWidgetItem*)\0"
    "itemActivated(QListWidgetItem*)\0"
    "itemEntered(QListWidgetItem*)\0"
    "itemChanged(QListWidgetItem*)\0"
    "current,previous\0"
    "currentItemChanged(QListWidgetItem*,QListWidgetItem*)\0"
    "currentText\0currentTextChanged(QString)\0"
    "currentRow\0currentRowChanged(int)\0"
    "itemSelectionChanged()\0item,hint\0"
    "scrollToItem(const QListWidgetItem*,QAbstractItemView::ScrollHint)\0"
    "scrollToItem(const QListWidgetItem*)\0"
    "clear()\0index\0_q_emitItemPressed(QModelIndex)\0"
    "_q_emitItemClicked(QModelIndex)\0"
    "_q_emitItemDoubleClicked(QModelIndex)\0"
    "_q_emitItemActivated(QModelIndex)\0"
    "_q_emitItemEntered(QModelIndex)\0"
    "_q_emitItemChanged(QModelIndex)\0"
    "previous,current\0"
    "_q_emitCurrentItemChanged(QModelIndex,QModelIndex)\0"
    "_q_sort()\0topLeft,bottomRight\0"
    "_q_dataChanged(QModelIndex,QModelIndex)\0"
    "int\0count\0bool\0sortingEnabled\0"
};

const QMetaObject QListWidget::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_QListWidget,
      qt_meta_data_QListWidget, 0 }
};

const QMetaObject *QListWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QListWidget))
	return static_cast<void*>(const_cast< QListWidget*>(this));
    return QListView::qt_metacast(_clname);
}

int QListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemPressed((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 1: itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 2: itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: itemEntered((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: itemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 7: currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: itemSelectionChanged(); break;
        case 10: scrollToItem((*reinterpret_cast< const QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemView::ScrollHint(*)>(_a[2]))); break;
        case 11: scrollToItem((*reinterpret_cast< const QListWidgetItem*(*)>(_a[1]))); break;
        case 12: clear(); break;
        case 13: d_func()->_q_emitItemPressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 14: d_func()->_q_emitItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 15: d_func()->_q_emitItemDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 16: d_func()->_q_emitItemActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 17: d_func()->_q_emitItemEntered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 18: d_func()->_q_emitItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 19: d_func()->_q_emitCurrentItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 20: d_func()->_q_sort(); break;
        case 21: d_func()->_q_dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        }
        _id -= 22;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = count(); break;
        case 1: *reinterpret_cast< int*>(_v) = currentRow(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isSortingEnabled(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setCurrentRow(*reinterpret_cast< int*>(_v)); break;
        case 2: setSortingEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QListWidget::itemPressed(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QListWidget::itemClicked(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QListWidget::itemDoubleClicked(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QListWidget::itemActivated(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QListWidget::itemEntered(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QListWidget::itemChanged(QListWidgetItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QListWidget::currentItemChanged(QListWidgetItem * _t1, QListWidgetItem * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QListWidget::currentTextChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QListWidget::currentRowChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QListWidget::itemSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}
QT_END_MOC_NAMESPACE
