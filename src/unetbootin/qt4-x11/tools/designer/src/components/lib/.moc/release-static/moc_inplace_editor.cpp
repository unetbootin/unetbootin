/****************************************************************************
** Meta object code from reading C++ file 'inplace_editor.h'
**
** Created: Thu Mar 5 20:52:31 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../taskmenu/inplace_editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'inplace_editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__InPlaceEditor[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__InPlaceEditor[] = {
    "qdesigner_internal::InPlaceEditor\0"
};

const QMetaObject qdesigner_internal::InPlaceEditor::staticMetaObject = {
    { &TextPropertyEditor::staticMetaObject, qt_meta_stringdata_qdesigner_internal__InPlaceEditor,
      qt_meta_data_qdesigner_internal__InPlaceEditor, 0 }
};

const QMetaObject *qdesigner_internal::InPlaceEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::InPlaceEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__InPlaceEditor))
        return static_cast<void*>(const_cast< InPlaceEditor*>(this));
    return TextPropertyEditor::qt_metacast(_clname);
}

int qdesigner_internal::InPlaceEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TextPropertyEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__TaskMenuInlineEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   41,   41,   41, 0x0a,
      58,   53,   41,   41, 0x08,
      78,   41,   41,   41, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__TaskMenuInlineEditor[] = {
    "qdesigner_internal::TaskMenuInlineEditor\0"
    "\0editText()\0text\0updateText(QString)\0"
    "updateSelection()\0"
};

const QMetaObject qdesigner_internal::TaskMenuInlineEditor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__TaskMenuInlineEditor,
      qt_meta_data_qdesigner_internal__TaskMenuInlineEditor, 0 }
};

const QMetaObject *qdesigner_internal::TaskMenuInlineEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::TaskMenuInlineEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__TaskMenuInlineEditor))
        return static_cast<void*>(const_cast< TaskMenuInlineEditor*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::TaskMenuInlineEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: editText(); break;
        case 1: updateText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: updateSelection(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
