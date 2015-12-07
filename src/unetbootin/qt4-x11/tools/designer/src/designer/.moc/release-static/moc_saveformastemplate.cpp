/****************************************************************************
** Meta object code from reading C++ file 'saveformastemplate.h'
**
** Created: Thu Mar 5 20:53:37 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../saveformastemplate.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'saveformastemplate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SaveFormAsTemplate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      33,   29,   19,   19, 0x08,
      67,   57,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SaveFormAsTemplate[] = {
    "SaveFormAsTemplate\0\0accept()\0str\0"
    "updateOKButton(QString)\0itemIndex\0"
    "checkToAddPath(int)\0"
};

const QMetaObject SaveFormAsTemplate::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SaveFormAsTemplate,
      qt_meta_data_SaveFormAsTemplate, 0 }
};

const QMetaObject *SaveFormAsTemplate::metaObject() const
{
    return &staticMetaObject;
}

void *SaveFormAsTemplate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SaveFormAsTemplate))
        return static_cast<void*>(const_cast< SaveFormAsTemplate*>(this));
    return QDialog::qt_metacast(_clname);
}

int SaveFormAsTemplate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: updateOKButton((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: checkToAddPath((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
