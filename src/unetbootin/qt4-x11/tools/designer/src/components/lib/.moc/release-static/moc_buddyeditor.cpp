/****************************************************************************
** Meta object code from reading C++ file 'buddyeditor.h'
**
** Created: Thu Mar 5 20:52:21 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../buddyeditor/buddyeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'buddyeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__BuddyEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x0a,
      54,   52,   32,   32, 0x0a,
      78,   32,   32,   32, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__BuddyEditor[] = {
    "qdesigner_internal::BuddyEditor\0\0"
    "updateBackground()\0w\0widgetRemoved(QWidget*)\0"
    "autoBuddy()\0"
};

const QMetaObject qdesigner_internal::BuddyEditor::staticMetaObject = {
    { &ConnectionEdit::staticMetaObject, qt_meta_stringdata_qdesigner_internal__BuddyEditor,
      qt_meta_data_qdesigner_internal__BuddyEditor, 0 }
};

const QMetaObject *qdesigner_internal::BuddyEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::BuddyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__BuddyEditor))
        return static_cast<void*>(const_cast< BuddyEditor*>(this));
    return ConnectionEdit::qt_metacast(_clname);
}

int qdesigner_internal::BuddyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ConnectionEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateBackground(); break;
        case 1: widgetRemoved((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: autoBuddy(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
