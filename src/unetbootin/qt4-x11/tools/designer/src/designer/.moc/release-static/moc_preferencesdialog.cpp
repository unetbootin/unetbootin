/****************************************************************************
** Meta object code from reading C++ file 'preferencesdialog.h'
**
** Created: Thu Mar 5 20:53:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../preferencesdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'preferencesdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PreferencesDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,
      34,   18,   18,   18, 0x08,
      49,   18,   18,   18, 0x08,
      70,   61,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PreferencesDialog[] = {
    "PreferencesDialog\0\0slotAccepted()\0"
    "slotRejected()\0slotApply()\0modified\0"
    "slotUiModeChanged(bool)\0"
};

const QMetaObject PreferencesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PreferencesDialog,
      qt_meta_data_PreferencesDialog, 0 }
};

const QMetaObject *PreferencesDialog::metaObject() const
{
    return &staticMetaObject;
}

void *PreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PreferencesDialog))
        return static_cast<void*>(const_cast< PreferencesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotAccepted(); break;
        case 1: slotRejected(); break;
        case 2: slotApply(); break;
        case 3: slotUiModeChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
