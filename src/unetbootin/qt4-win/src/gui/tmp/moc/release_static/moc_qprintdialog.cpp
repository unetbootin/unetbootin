/****************************************************************************
** Meta object code from reading C++ file 'qprintdialog.h'
**
** Created: Fri May 30 22:29:38 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qprintdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qprintdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPrintDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QPrintDialog[] = {
    "QPrintDialog\0\0_q_chbPrintLastFirstToggled(bool)\0"
};

const QMetaObject QPrintDialog::staticMetaObject = {
    { &QAbstractPrintDialog::staticMetaObject, qt_meta_stringdata_QPrintDialog,
      qt_meta_data_QPrintDialog, 0 }
};

const QMetaObject *QPrintDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QPrintDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPrintDialog))
	return static_cast<void*>(const_cast< QPrintDialog*>(this));
    return QAbstractPrintDialog::qt_metacast(_clname);
}

int QPrintDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractPrintDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_chbPrintLastFirstToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
