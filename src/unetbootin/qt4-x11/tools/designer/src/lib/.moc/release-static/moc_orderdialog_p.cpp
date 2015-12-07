/****************************************************************************
** Meta object code from reading C++ file 'orderdialog_p.h'
**
** Created: Thu Mar 5 20:47:08 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/orderdialog_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'orderdialog_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__OrderDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x08,
      55,   32,   32,   32, 0x08,
      83,   79,   32,   32, 0x08,
     118,   32,   32,   32, 0x08,
     146,   32,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__OrderDialog[] = {
    "qdesigner_internal::OrderDialog\0\0"
    "on_upButton_clicked()\0on_downButton_clicked()\0"
    "row\0on_pageList_currentRowChanged(int)\0"
    "slotEnableButtonsAfterDnD()\0slotReset()\0"
};

const QMetaObject qdesigner_internal::OrderDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__OrderDialog,
      qt_meta_data_qdesigner_internal__OrderDialog, 0 }
};

const QMetaObject *qdesigner_internal::OrderDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::OrderDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__OrderDialog))
        return static_cast<void*>(const_cast< OrderDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::OrderDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_upButton_clicked(); break;
        case 1: on_downButton_clicked(); break;
        case 2: on_pageList_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: slotEnableButtonsAfterDnD(); break;
        case 4: slotReset(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
