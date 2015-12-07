/****************************************************************************
** Meta object code from reading C++ file 'newform.h'
**
** Created: Thu Mar 5 20:53:37 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../newform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NewForm[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      13,    9,    8,    8, 0x08,
      52,    8,    8,    8, 0x08,
      88,   71,    8,    8, 0x08,
     121,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NewForm[] = {
    "NewForm\0\0btn\0slotButtonBoxClicked(QAbstractButton*)\0"
    "recentFileChosen()\0templateSelected\0"
    "slotCurrentTemplateChanged(bool)\0"
    "slotTemplateActivated()\0"
};

const QMetaObject NewForm::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NewForm,
      qt_meta_data_NewForm, 0 }
};

const QMetaObject *NewForm::metaObject() const
{
    return &staticMetaObject;
}

void *NewForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewForm))
        return static_cast<void*>(const_cast< NewForm*>(this));
    return QDialog::qt_metacast(_clname);
}

int NewForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotButtonBoxClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 1: recentFileChosen(); break;
        case 2: slotCurrentTemplateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: slotTemplateActivated(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
