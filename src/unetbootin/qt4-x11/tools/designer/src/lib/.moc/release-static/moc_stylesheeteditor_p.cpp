/****************************************************************************
** Meta object code from reading C++ file 'stylesheeteditor_p.h'
**
** Created: Thu Mar 5 20:47:09 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/stylesheeteditor_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stylesheeteditor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__StyleSheetEditor[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__StyleSheetEditor[] = {
    "qdesigner_internal::StyleSheetEditor\0"
};

const QMetaObject qdesigner_internal::StyleSheetEditor::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_qdesigner_internal__StyleSheetEditor,
      qt_meta_data_qdesigner_internal__StyleSheetEditor, 0 }
};

const QMetaObject *qdesigner_internal::StyleSheetEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::StyleSheetEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__StyleSheetEditor))
        return static_cast<void*>(const_cast< StyleSheetEditor*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int qdesigner_internal::StyleSheetEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__StyleSheetEditorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      44,   43,   43,   43, 0x08,
      69,   65,   43,   43, 0x08,
     111,  102,   43,   43, 0x08,
     136,  102,   43,   43, 0x08,
     161,  102,   43,   43, 0x08,
     183,   43,   43,   43, 0x08,
     197,   43,   43,   43, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__StyleSheetEditorDialog[] = {
    "qdesigner_internal::StyleSheetEditorDialog\0"
    "\0validateStyleSheet()\0pos\0"
    "slotContextMenuRequested(QPoint)\0"
    "property\0slotAddResource(QString)\0"
    "slotAddGradient(QString)\0slotAddColor(QString)\0"
    "slotAddFont()\0slotRequestHelp()\0"
};

const QMetaObject qdesigner_internal::StyleSheetEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__StyleSheetEditorDialog,
      qt_meta_data_qdesigner_internal__StyleSheetEditorDialog, 0 }
};

const QMetaObject *qdesigner_internal::StyleSheetEditorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::StyleSheetEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__StyleSheetEditorDialog))
        return static_cast<void*>(const_cast< StyleSheetEditorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::StyleSheetEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: validateStyleSheet(); break;
        case 1: slotContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: slotAddResource((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: slotAddGradient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: slotAddColor((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: slotAddFont(); break;
        case 6: slotRequestHelp(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__StyleSheetPropertyEditorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      52,   51,   51,   51, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__StyleSheetPropertyEditorDialog[] = {
    "qdesigner_internal::StyleSheetPropertyEditorDialog\0"
    "\0applyStyleSheet()\0"
};

const QMetaObject qdesigner_internal::StyleSheetPropertyEditorDialog::staticMetaObject = {
    { &StyleSheetEditorDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__StyleSheetPropertyEditorDialog,
      qt_meta_data_qdesigner_internal__StyleSheetPropertyEditorDialog, 0 }
};

const QMetaObject *qdesigner_internal::StyleSheetPropertyEditorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::StyleSheetPropertyEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__StyleSheetPropertyEditorDialog))
        return static_cast<void*>(const_cast< StyleSheetPropertyEditorDialog*>(this));
    return StyleSheetEditorDialog::qt_metacast(_clname);
}

int qdesigner_internal::StyleSheetPropertyEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StyleSheetEditorDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: applyStyleSheet(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
