/****************************************************************************
** Meta object code from reading C++ file 'textpropertyeditor_p.h'
**
** Created: Thu Mar 5 20:47:10 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/textpropertyeditor_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textpropertyeditor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__TextPropertyEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       1,   52, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      45,   40,   39,   39, 0x05,
      66,   39,   39,   39, 0x05,

 // slots: signature, parameters, type, tag, flags
      84,   40,   39,   39, 0x0a,
     101,   39,   39,   39, 0x0a,
     113,   39,   39,   39, 0x0a,
     121,   40,   39,   39, 0x08,
     146,   39,   39,   39, 0x08,
     163,   39,   39,   39, 0x08,

 // properties: name, type, flags
      40,  185, 0x0a195103,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__TextPropertyEditor[] = {
    "qdesigner_internal::TextPropertyEditor\0"
    "\0text\0textChanged(QString)\0editingFinished()\0"
    "setText(QString)\0selectAll()\0clear()\0"
    "slotTextChanged(QString)\0slotTextEdited()\0"
    "slotEditingFinished()\0QString\0"
};

const QMetaObject qdesigner_internal::TextPropertyEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__TextPropertyEditor,
      qt_meta_data_qdesigner_internal__TextPropertyEditor, 0 }
};

const QMetaObject *qdesigner_internal::TextPropertyEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::TextPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__TextPropertyEditor))
        return static_cast<void*>(const_cast< TextPropertyEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::TextPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: editingFinished(); break;
        case 2: setText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: selectAll(); break;
        case 4: clear(); break;
        case 5: slotTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: slotTextEdited(); break;
        case 7: slotEditingFinished(); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void qdesigner_internal::TextPropertyEditor::textChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::TextPropertyEditor::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
