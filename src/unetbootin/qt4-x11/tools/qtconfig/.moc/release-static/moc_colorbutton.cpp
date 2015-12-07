/****************************************************************************
** Meta object code from reading C++ file 'colorbutton.h'
**
** Created: Thu Mar 5 20:59:27 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../colorbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colorbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ColorButton[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ColorButton[] = {
    "ColorButton\0\0colorChanged(QColor)\0"
    "changeColor()\0"
};

const QMetaObject ColorButton::staticMetaObject = {
    { &QAbstractButton::staticMetaObject, qt_meta_stringdata_ColorButton,
      qt_meta_data_ColorButton, 0 }
};

const QMetaObject *ColorButton::metaObject() const
{
    return &staticMetaObject;
}

void *ColorButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorButton))
        return static_cast<void*>(const_cast< ColorButton*>(this));
    return QAbstractButton::qt_metacast(_clname);
}

int ColorButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: changeColor(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ColorButton::colorChanged(const QColor & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
