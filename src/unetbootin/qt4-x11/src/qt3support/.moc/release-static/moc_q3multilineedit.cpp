/****************************************************************************
** Meta object code from reading C++ file 'q3multilineedit.h'
**
** Created: Thu Mar 5 19:06:09 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../text/q3multilineedit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3multilineedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3MultiLineEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       5,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,

 // properties: name, type, flags
      32,   28, 0x02095001,
      46,   41, 0x01095001,
      58,   41, 0x01095001,
      78,   64, 0x0009510b,
      88,   41, 0x01094103,

       0        // eod
};

static const char qt_meta_stringdata_Q3MultiLineEdit[] = {
    "Q3MultiLineEdit\0\0deselect()\0int\0"
    "numLines\0bool\0atBeginning\0atEnd\0"
    "Qt::Alignment\0alignment\0edited\0"
};

const QMetaObject Q3MultiLineEdit::staticMetaObject = {
    { &Q3TextEdit::staticMetaObject, qt_meta_stringdata_Q3MultiLineEdit,
      qt_meta_data_Q3MultiLineEdit, 0 }
};

const QMetaObject *Q3MultiLineEdit::metaObject() const
{
    return &staticMetaObject;
}

void *Q3MultiLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3MultiLineEdit))
        return static_cast<void*>(const_cast< Q3MultiLineEdit*>(this));
    return Q3TextEdit::qt_metacast(_clname);
}

int Q3MultiLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3TextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: deselect(); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = numLines(); break;
        case 1: *reinterpret_cast< bool*>(_v) = atBeginning(); break;
        case 2: *reinterpret_cast< bool*>(_v) = atEnd(); break;
        case 3: *reinterpret_cast< Qt::Alignment*>(_v) = alignment(); break;
        case 4: *reinterpret_cast< bool*>(_v) = edited(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 3: setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 4: setEdited(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
