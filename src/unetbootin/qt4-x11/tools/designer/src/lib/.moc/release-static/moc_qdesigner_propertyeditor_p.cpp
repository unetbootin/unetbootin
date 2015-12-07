/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_propertyeditor_p.h'
**
** Created: Thu Mar 5 20:46:55 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_propertyeditor_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_propertyeditor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__QDesignerPropertyEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      82,   45,   44,   44, 0x05,
     131,  126,   44,   44, 0x05,
     165,  154,   44,   44, 0x05,
     202,  126,   44,   44, 0x05,
     233,   44,   44,   44, 0x05,
     248,   44,   44,   44, 0x05,

 // slots: signature, parameters, type, tag, flags
     263,   44,   44,   44, 0x0a,
     285,   44,   44,   44, 0x0a,
     312,  154,   44,   44, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__QDesignerPropertyEditor[] = {
    "qdesigner_internal::QDesignerPropertyEditor\0"
    "\0name,value,enableSubPropertyHandling\0"
    "propertyValueChanged(QString,QVariant,bool)\0"
    "name\0resetProperty(QString)\0name,value\0"
    "addDynamicProperty(QString,QVariant)\0"
    "removeDynamicProperty(QString)\0"
    "editorOpened()\0editorClosed()\0"
    "updatePropertySheet()\0reloadResourceProperties()\0"
    "slotPropertyChanged(QString,QVariant)\0"
};

const QMetaObject qdesigner_internal::QDesignerPropertyEditor::staticMetaObject = {
    { &QDesignerPropertyEditorInterface::staticMetaObject, qt_meta_stringdata_qdesigner_internal__QDesignerPropertyEditor,
      qt_meta_data_qdesigner_internal__QDesignerPropertyEditor, 0 }
};

const QMetaObject *qdesigner_internal::QDesignerPropertyEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::QDesignerPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__QDesignerPropertyEditor))
        return static_cast<void*>(const_cast< QDesignerPropertyEditor*>(this));
    return QDesignerPropertyEditorInterface::qt_metacast(_clname);
}

int qdesigner_internal::QDesignerPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerPropertyEditorInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: propertyValueChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: resetProperty((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: addDynamicProperty((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 3: removeDynamicProperty((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: editorOpened(); break;
        case 5: editorClosed(); break;
        case 6: updatePropertySheet(); break;
        case 7: reloadResourceProperties(); break;
        case 8: slotPropertyChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::QDesignerPropertyEditor::propertyValueChanged(const QString & _t1, const QVariant & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::QDesignerPropertyEditor::resetProperty(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::QDesignerPropertyEditor::addDynamicProperty(const QString & _t1, const QVariant & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qdesigner_internal::QDesignerPropertyEditor::removeDynamicProperty(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void qdesigner_internal::QDesignerPropertyEditor::editorOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void qdesigner_internal::QDesignerPropertyEditor::editorClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
