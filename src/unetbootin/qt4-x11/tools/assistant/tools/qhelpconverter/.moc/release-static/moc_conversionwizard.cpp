/****************************************************************************
** Meta object code from reading C++ file 'conversionwizard.h'
**
** Created: Thu Mar 5 20:35:21 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../conversionwizard.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'conversionwizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConversionWizard[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   18,   17,   17, 0x08,
      45,   38,   17,   17, 0x08,
      60,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ConversionWizard[] = {
    "ConversionWizard\0\0id\0pageChanged(int)\0"
    "toggle\0showHelp(bool)\0convert()\0"
};

const QMetaObject ConversionWizard::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_ConversionWizard,
      qt_meta_data_ConversionWizard, 0 }
};

const QMetaObject *ConversionWizard::metaObject() const
{
    return &staticMetaObject;
}

void *ConversionWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConversionWizard))
        return static_cast<void*>(const_cast< ConversionWizard*>(this));
    return QWizard::qt_metacast(_clname);
}

int ConversionWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: pageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: showHelp((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: convert(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
