/****************************************************************************
** Meta object code from reading C++ file 'q3sqlform.h'
**
** Created: Thu Mar 5 19:05:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../sql/q3sqlform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3sqlform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3SqlForm[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x0a,
      38,   11,   10,   10, 0x0a,
      59,   10,   10,   10, 0x0a,
      72,   10,   10,   10, 0x0a,
      86,   10,   10,   10, 0x0a,
      94,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Q3SqlForm[] = {
    "Q3SqlForm\0\0widget\0readField(QWidget*)\0"
    "writeField(QWidget*)\0readFields()\0"
    "writeFields()\0clear()\0clearValues()\0"
};

const QMetaObject Q3SqlForm::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3SqlForm,
      qt_meta_data_Q3SqlForm, 0 }
};

const QMetaObject *Q3SqlForm::metaObject() const
{
    return &staticMetaObject;
}

void *Q3SqlForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3SqlForm))
        return static_cast<void*>(const_cast< Q3SqlForm*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3SqlForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readField((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: writeField((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: readFields(); break;
        case 3: writeFields(); break;
        case 4: clear(); break;
        case 5: clearValues(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
