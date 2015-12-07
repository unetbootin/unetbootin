/****************************************************************************
** Meta object code from reading C++ file 'identifierpage.h'
**
** Created: Thu Mar 5 20:35:30 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../identifierpage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'identifierpage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_IdentifierPage[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      24,   16,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_IdentifierPage[] = {
    "IdentifierPage\0\0checked\0setupButtons(bool)\0"
};

const QMetaObject IdentifierPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_IdentifierPage,
      qt_meta_data_IdentifierPage, 0 }
};

const QMetaObject *IdentifierPage::metaObject() const
{
    return &staticMetaObject;
}

void *IdentifierPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IdentifierPage))
        return static_cast<void*>(const_cast< IdentifierPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int IdentifierPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setupButtons((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
