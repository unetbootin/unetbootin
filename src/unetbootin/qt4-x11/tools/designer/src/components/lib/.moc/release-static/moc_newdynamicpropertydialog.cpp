/****************************************************************************
** Meta object code from reading C++ file 'newdynamicpropertydialog.h'
**
** Created: Thu Mar 5 20:51:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../propertyeditor/newdynamicpropertydialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newdynamicpropertydialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__NewDynamicPropertyDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      50,   46,   45,   45, 0x08,
      91,   45,   45,   45, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__NewDynamicPropertyDialog[] = {
    "qdesigner_internal::NewDynamicPropertyDialog\0"
    "\0btn\0on_m_buttonBox_clicked(QAbstractButton*)\0"
    "nameChanged(QString)\0"
};

const QMetaObject qdesigner_internal::NewDynamicPropertyDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__NewDynamicPropertyDialog,
      qt_meta_data_qdesigner_internal__NewDynamicPropertyDialog, 0 }
};

const QMetaObject *qdesigner_internal::NewDynamicPropertyDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::NewDynamicPropertyDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__NewDynamicPropertyDialog))
        return static_cast<void*>(const_cast< NewDynamicPropertyDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::NewDynamicPropertyDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_m_buttonBox_clicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 1: nameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
