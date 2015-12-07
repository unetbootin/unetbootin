/****************************************************************************
** Meta object code from reading C++ file 'propertyeditor.h'
**
** Created: Thu Mar 5 20:51:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../propertyeditor/propertyeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'propertyeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PropertyEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      45,   36,   35,   35, 0x08,
     117,   76,   35,   35, 0x08,
     168,  161,   35,   35, 0x08,
     196,  161,   35,   35, 0x08,
     229,   35,   35,   35, 0x08,
     262,  257,   35,   35, 0x08,
     286,  280,   35,   35, 0x08,
     305,   35,   35,   35, 0x08,
     352,  344,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PropertyEditor[] = {
    "qdesigner_internal::PropertyEditor\0\0"
    "property\0slotResetProperty(QtProperty*)\0"
    "property,value,enableSubPropertyHandling\0"
    "slotValueChanged(QtProperty*,QVariant,bool)\0"
    "action\0slotViewTriggered(QAction*)\0"
    "slotAddDynamicProperty(QAction*)\0"
    "slotRemoveDynamicProperty()\0sort\0"
    "slotSorting(bool)\0color\0slotColoring(bool)\0"
    "slotCurrentItemChanged(QtBrowserItem*)\0"
    "pattern\0setFilter(QString)\0"
};

const QMetaObject qdesigner_internal::PropertyEditor::staticMetaObject = {
    { &QDesignerPropertyEditor::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PropertyEditor,
      qt_meta_data_qdesigner_internal__PropertyEditor, 0 }
};

const QMetaObject *qdesigner_internal::PropertyEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PropertyEditor))
        return static_cast<void*>(const_cast< PropertyEditor*>(this));
    return QDesignerPropertyEditor::qt_metacast(_clname);
}

int qdesigner_internal::PropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerPropertyEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotResetProperty((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 1: slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: slotViewTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: slotAddDynamicProperty((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 4: slotRemoveDynamicProperty(); break;
        case 5: slotSorting((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: slotColoring((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: slotCurrentItemChanged((*reinterpret_cast< QtBrowserItem*(*)>(_a[1]))); break;
        case 8: setFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
