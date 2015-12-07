/****************************************************************************
** Meta object code from reading C++ file 'qabstractitemdelegate.h'
**
** Created: Thu Mar 5 18:57:35 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qabstractitemdelegate.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractitemdelegate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractItemDelegate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      30,   23,   22,   22, 0x05,
      63,   51,   22,   22, 0x05,
     120,   23,   22,   22, 0x25,
     142,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     200,  176,  171,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractItemDelegate[] = {
    "QAbstractItemDelegate\0\0editor\0"
    "commitData(QWidget*)\0editor,hint\0"
    "closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "closeEditor(QWidget*)\0"
    "sizeHintChanged(QModelIndex)\0bool\0"
    "event,view,option,index\0"
    "helpEvent(QHelpEvent*,QAbstractItemView*,QStyleOptionViewItem,QModelIn"
    "dex)\0"
};

const QMetaObject QAbstractItemDelegate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAbstractItemDelegate,
      qt_meta_data_QAbstractItemDelegate, 0 }
};

const QMetaObject *QAbstractItemDelegate::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractItemDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractItemDelegate))
        return static_cast<void*>(const_cast< QAbstractItemDelegate*>(this));
    return QObject::qt_metacast(_clname);
}

int QAbstractItemDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: commitData((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: closeEditor((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 2: closeEditor((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: sizeHintChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: { bool _r = helpEvent((*reinterpret_cast< QHelpEvent*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemView*(*)>(_a[2])),(*reinterpret_cast< const QStyleOptionViewItem(*)>(_a[3])),(*reinterpret_cast< const QModelIndex(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QAbstractItemDelegate::commitData(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QAbstractItemDelegate::closeEditor(QWidget * _t1, QAbstractItemDelegate::EndEditHint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, 2, _a);
}

// SIGNAL 3
void QAbstractItemDelegate::sizeHintChanged(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
