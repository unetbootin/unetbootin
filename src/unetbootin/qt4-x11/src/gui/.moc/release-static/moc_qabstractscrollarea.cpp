/****************************************************************************
** Meta object code from reading C++ file 'qabstractscrollarea.h'
**
** Created: Thu Mar 5 18:52:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qabstractscrollarea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractscrollarea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractScrollArea[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       2,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      30,   21,   20,   20, 0x09,
      54,   20,   20,   20, 0x08,
      69,   20,   20,   20, 0x08,
      84,   20,   20,   20, 0x08,

 // properties: name, type, flags
     130,  110, 0x0009510b,
     154,  110, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractScrollArea[] = {
    "QAbstractScrollArea\0\0viewport\0"
    "setupViewport(QWidget*)\0_q_hslide(int)\0"
    "_q_vslide(int)\0_q_showOrHideScrollBars()\0"
    "Qt::ScrollBarPolicy\0verticalScrollBarPolicy\0"
    "horizontalScrollBarPolicy\0"
};

const QMetaObject QAbstractScrollArea::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QAbstractScrollArea,
      qt_meta_data_QAbstractScrollArea, 0 }
};

const QMetaObject *QAbstractScrollArea::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractScrollArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractScrollArea))
        return static_cast<void*>(const_cast< QAbstractScrollArea*>(this));
    return QFrame::qt_metacast(_clname);
}

int QAbstractScrollArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setupViewport((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: d_func()->_q_hslide((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: d_func()->_q_vslide((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: d_func()->_q_showOrHideScrollBars(); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::ScrollBarPolicy*>(_v) = verticalScrollBarPolicy(); break;
        case 1: *reinterpret_cast< Qt::ScrollBarPolicy*>(_v) = horizontalScrollBarPolicy(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVerticalScrollBarPolicy(*reinterpret_cast< Qt::ScrollBarPolicy*>(_v)); break;
        case 1: setHorizontalScrollBarPolicy(*reinterpret_cast< Qt::ScrollBarPolicy*>(_v)); break;
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
QT_END_MOC_NAMESPACE
