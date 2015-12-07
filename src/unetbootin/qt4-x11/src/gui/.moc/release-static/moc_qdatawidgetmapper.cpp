/****************************************************************************
** Meta object code from reading C++ file 'qdatawidgetmapper.h'
**
** Created: Thu Mar 5 18:55:41 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qdatawidgetmapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdatawidgetmapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDataWidgetMapper[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   12, // methods
       3,   77, // properties
       1,   89, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      25,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      50,   18,   18,   18, 0x0a,
      64,   18,   59,   18, 0x0a,
      73,   18,   18,   18, 0x0a,
      83,   18,   18,   18, 0x0a,
      92,   18,   18,   18, 0x0a,
     101,   18,   18,   18, 0x0a,
     114,   19,   18,   18, 0x0a,
     135,   19,   18,   18, 0x0a,
     171,  169,   18,   18, 0x08,
     211,   18,   18,   18, 0x08,
     235,  169,   18,   18, 0x08,
     295,   18,   18,   18, 0x08,

 // properties: name, type, flags
     319,  315, 0x02495103,
     348,  332, 0x0009510b,
     373,  360, 0x0009510b,

 // properties: notify_signal_id
       0,
       0,
       0,

 // enums: name, flags, count, data
     360, 0x0,    2,   93,

 // enum data: key, value
     386, uint(QDataWidgetMapper::AutoSubmit),
     397, uint(QDataWidgetMapper::ManualSubmit),

       0        // eod
};

static const char qt_meta_stringdata_QDataWidgetMapper[] = {
    "QDataWidgetMapper\0\0index\0"
    "currentIndexChanged(int)\0revert()\0"
    "bool\0submit()\0toFirst()\0toLast()\0"
    "toNext()\0toPrevious()\0setCurrentIndex(int)\0"
    "setCurrentModelIndex(QModelIndex)\0,\0"
    "_q_dataChanged(QModelIndex,QModelIndex)\0"
    "_q_commitData(QWidget*)\0"
    "_q_closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "_q_modelDestroyed()\0int\0currentIndex\0"
    "Qt::Orientation\0orientation\0SubmitPolicy\0"
    "submitPolicy\0AutoSubmit\0ManualSubmit\0"
};

const QMetaObject QDataWidgetMapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDataWidgetMapper,
      qt_meta_data_QDataWidgetMapper, 0 }
};

const QMetaObject *QDataWidgetMapper::metaObject() const
{
    return &staticMetaObject;
}

void *QDataWidgetMapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDataWidgetMapper))
        return static_cast<void*>(const_cast< QDataWidgetMapper*>(this));
    return QObject::qt_metacast(_clname);
}

int QDataWidgetMapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: revert(); break;
        case 2: { bool _r = submit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: toFirst(); break;
        case 4: toLast(); break;
        case 5: toNext(); break;
        case 6: toPrevious(); break;
        case 7: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: setCurrentModelIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: d_func()->_q_dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 10: d_func()->_q_commitData((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 11: d_func()->_q_closeEditor((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 12: d_func()->_q_modelDestroyed(); break;
        default: ;
        }
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        case 1: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 2: *reinterpret_cast< SubmitPolicy*>(_v) = submitPolicy(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        case 1: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 2: setSubmitPolicy(*reinterpret_cast< SubmitPolicy*>(_v)); break;
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
void QDataWidgetMapper::currentIndexChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
