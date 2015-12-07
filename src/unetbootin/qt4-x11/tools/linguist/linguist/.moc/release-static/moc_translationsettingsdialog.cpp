/****************************************************************************
** Meta object code from reading C++ file 'translationsettingsdialog.h'
**
** Created: Thu Mar 5 20:56:10 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../translationsettingsdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'translationsettingsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TranslationSettingsDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TranslationSettingsDialog[] = {
    "TranslationSettingsDialog\0\0"
    "on_buttonBox_accepted()\0"
};

const QMetaObject TranslationSettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TranslationSettingsDialog,
      qt_meta_data_TranslationSettingsDialog, 0 }
};

const QMetaObject *TranslationSettingsDialog::metaObject() const
{
    return &staticMetaObject;
}

void *TranslationSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TranslationSettingsDialog))
        return static_cast<void*>(const_cast< TranslationSettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int TranslationSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_buttonBox_accepted(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
