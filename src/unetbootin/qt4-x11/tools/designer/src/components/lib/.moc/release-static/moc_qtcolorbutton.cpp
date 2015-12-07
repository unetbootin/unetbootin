/****************************************************************************
** Meta object code from reading C++ file 'qtcolorbutton.h'
**
** Created: Thu Mar 5 20:48:55 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../shared/qtgradienteditor/qtcolorbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtcolorbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtColorButton[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       1,   27, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   15,   14,   14, 0x0a,
      59,   14,   14,   14, 0x08,

 // properties: name, type, flags
      80,   75, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QtColorButton[] = {
    "QtColorButton\0\0color\0colorChanged(QColor)\0"
    "setColor(QColor)\0slotEditColor()\0bool\0"
    "backgroundCheckered\0"
};

const QMetaObject QtColorButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_QtColorButton,
      qt_meta_data_QtColorButton, 0 }
};

const QMetaObject *QtColorButton::metaObject() const
{
    return &staticMetaObject;
}

void *QtColorButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtColorButton))
        return static_cast<void*>(const_cast< QtColorButton*>(this));
    return QToolButton::qt_metacast(_clname);
}

int QtColorButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: setColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 2: d_func()->slotEditColor(); break;
        default: ;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isBackgroundCheckered(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBackgroundCheckered(*reinterpret_cast< bool*>(_v)); break;
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
void QtColorButton::colorChanged(const QColor & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
