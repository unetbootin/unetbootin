/****************************************************************************
** Meta object code from reading C++ file 'richtexteditor_p.h'
**
** Created: Thu Mar 5 20:47:04 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/richtexteditor_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'richtexteditor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__RichTextEditorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      51,   42,   41,   41, 0x08,
      72,   41,   41,   41, 0x08,
      90,   41,   41,   41, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__RichTextEditorDialog[] = {
    "qdesigner_internal::RichTextEditorDialog\0"
    "\0newIndex\0tabIndexChanged(int)\0"
    "richTextChanged()\0sourceChanged()\0"
};

const QMetaObject qdesigner_internal::RichTextEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__RichTextEditorDialog,
      qt_meta_data_qdesigner_internal__RichTextEditorDialog, 0 }
};

const QMetaObject *qdesigner_internal::RichTextEditorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::RichTextEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__RichTextEditorDialog))
        return static_cast<void*>(const_cast< RichTextEditorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::RichTextEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tabIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: richTextChanged(); break;
        case 2: sourceChanged(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
