/****************************************************************************
** Meta object code from reading C++ file 'stringlisteditor.h'
**
** Created: Thu Mar 5 20:51:50 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../propertyeditor/stringlisteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stringlisteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__StringListEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      38,   37,   37,   37, 0x08,
      60,   37,   37,   37, 0x08,
      84,   37,   37,   37, 0x08,
     107,   37,   37,   37, 0x08,
     138,  133,   37,   37, 0x08,
     188,  171,   37,   37, 0x08,
     233,   37,   37,   37, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__StringListEditor[] = {
    "qdesigner_internal::StringListEditor\0"
    "\0on_upButton_clicked()\0on_downButton_clicked()\0"
    "on_newButton_clicked()\0on_deleteButton_clicked()\0"
    "text\0on_valueEdit_textEdited(QString)\0"
    "current,previous\0"
    "currentIndexChanged(QModelIndex,QModelIndex)\0"
    "currentValueChanged()\0"
};

const QMetaObject qdesigner_internal::StringListEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__StringListEditor,
      qt_meta_data_qdesigner_internal__StringListEditor, 0 }
};

const QMetaObject *qdesigner_internal::StringListEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::StringListEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__StringListEditor))
        return static_cast<void*>(const_cast< StringListEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::StringListEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_upButton_clicked(); break;
        case 1: on_downButton_clicked(); break;
        case 2: on_newButton_clicked(); break;
        case 3: on_deleteButton_clicked(); break;
        case 4: on_valueEdit_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: currentIndexChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 6: currentValueChanged(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
