/****************************************************************************
** Meta object code from reading C++ file 'scripterrordialog_p.h'
**
** Created: Thu Mar 5 20:47:12 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/scripterrordialog_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scripterrordialog_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ScriptErrorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ScriptErrorDialog[] = {
    "qdesigner_internal::ScriptErrorDialog\0"
};

const QMetaObject qdesigner_internal::ScriptErrorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ScriptErrorDialog,
      qt_meta_data_qdesigner_internal__ScriptErrorDialog, 0 }
};

const QMetaObject *qdesigner_internal::ScriptErrorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ScriptErrorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ScriptErrorDialog))
        return static_cast<void*>(const_cast< ScriptErrorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::ScriptErrorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
