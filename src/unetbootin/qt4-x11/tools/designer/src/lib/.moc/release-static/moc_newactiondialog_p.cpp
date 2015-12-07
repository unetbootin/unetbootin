/****************************************************************************
** Meta object code from reading C++ file 'newactiondialog_p.h'
**
** Created: Thu Mar 5 20:47:09 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/newactiondialog_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newactiondialog_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__NewActionDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   37,   36,   36, 0x08,
      80,   37,   36,   36, 0x08,
     118,   36,   36,   36, 0x08,
     136,   36,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__NewActionDialog[] = {
    "qdesigner_internal::NewActionDialog\0"
    "\0text\0on_editActionText_textEdited(QString)\0"
    "on_editObjectName_textEdited(QString)\0"
    "slotEditToolTip()\0slotResetKeySequence()\0"
};

const QMetaObject qdesigner_internal::NewActionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__NewActionDialog,
      qt_meta_data_qdesigner_internal__NewActionDialog, 0 }
};

const QMetaObject *qdesigner_internal::NewActionDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::NewActionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__NewActionDialog))
        return static_cast<void*>(const_cast< NewActionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::NewActionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_editActionText_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: on_editObjectName_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: slotEditToolTip(); break;
        case 3: slotResetKeySequence(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
