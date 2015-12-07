/****************************************************************************
** Meta object code from reading C++ file 'spacer_widget_p.h'
**
** Created: Thu Mar 5 20:46:50 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/spacer_widget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spacer_widget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Spacer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      15,    7, 0x0a095003,
      42,   26, 0x0009510b,
      74,   54, 0x0009510b,
      89,   83, 0x15095003,

 // enums: name, flags, count, data

 // enum data: key, value

       0        // eod
};

static const char qt_meta_stringdata_Spacer[] = {
    "Spacer\0QString\0spacerName\0Qt::Orientation\0"
    "orientation\0QSizePolicy::Policy\0"
    "sizeType\0QSize\0sizeHint\0"
};

static const QMetaObject *qt_meta_extradata_Spacer[] = {
        &QSizePolicy::staticMetaObject,0
};

static const QMetaObjectExtraData qt_meta_extradata2_Spacer = {
    qt_meta_extradata_Spacer, 0 
};

const QMetaObject Spacer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Spacer,
      qt_meta_data_Spacer, &qt_meta_extradata2_Spacer }
};

const QMetaObject *Spacer::metaObject() const
{
    return &staticMetaObject;
}

void *Spacer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Spacer))
        return static_cast<void*>(const_cast< Spacer*>(this));
    return QWidget::qt_metacast(_clname);
}

int Spacer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = objectName(); break;
        case 1: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 2: *reinterpret_cast< QSizePolicy::Policy*>(_v) = sizeType(); break;
        case 3: *reinterpret_cast< QSize*>(_v) = sizeHintProperty(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setObjectName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 2: setSizeType(*reinterpret_cast< QSizePolicy::Policy*>(_v)); break;
        case 3: setSizeHintProperty(*reinterpret_cast< QSize*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
