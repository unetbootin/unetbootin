/****************************************************************************
** Meta object code from reading C++ file 'qcolumnview.h'
**
** Created: Thu Mar 5 18:55:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qcolumnview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcolumnview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QColumnView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       1,   37, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      69,   52,   12,   12, 0x09,
     109,   12,   12,   12, 0x08,
     127,   12,   12,   12, 0x08,
     152,   12,   12,   12, 0x08,

 // properties: name, type, flags
     181,  176, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QColumnView[] = {
    "QColumnView\0\0index\0updatePreviewWidget(QModelIndex)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "_q_gripMoved(int)\0_q_changeCurrentColumn()\0"
    "_q_clicked(QModelIndex)\0bool\0"
    "resizeGripsVisible\0"
};

const QMetaObject QColumnView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_QColumnView,
      qt_meta_data_QColumnView, 0 }
};

const QMetaObject *QColumnView::metaObject() const
{
    return &staticMetaObject;
}

void *QColumnView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QColumnView))
        return static_cast<void*>(const_cast< QColumnView*>(this));
    return QAbstractItemView::qt_metacast(_clname);
}

int QColumnView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updatePreviewWidget((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 2: d_func()->_q_gripMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: d_func()->_q_changeCurrentColumn(); break;
        case 4: d_func()->_q_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = resizeGripsVisible(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setResizeGripsVisible(*reinterpret_cast< bool*>(_v)); break;
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
void QColumnView::updatePreviewWidget(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
