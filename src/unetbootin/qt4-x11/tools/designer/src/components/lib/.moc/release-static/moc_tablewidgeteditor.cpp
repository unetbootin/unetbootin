/****************************************************************************
** Meta object code from reading C++ file 'tablewidgeteditor.h'
**
** Created: Thu Mar 5 20:52:37 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../taskmenu/tablewidgeteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tablewidgeteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__TableWidgetEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      63,   39,   38,   38, 0x08,
     119,  114,   38,   38, 0x08,
     169,  165,   38,   38, 0x08,
     214,  203,   38,   38, 0x08,
     260,  165,   38,   38, 0x08,
     294,  165,   38,   38, 0x08,
     327,  165,   38,   38, 0x08,
     360,  165,   38,   38, 0x08,
     395,  165,   38,   38, 0x08,
     426,  203,   38,   38, 0x08,
     469,  165,   38,   38, 0x08,
     500,  165,   38,   38, 0x08,
     530,  165,   38,   38, 0x08,
     560,  165,   38,   38, 0x08,
     592,   38,   38,   38, 0x08,
     616,   38,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__TableWidgetEditor[] = {
    "qdesigner_internal::TableWidgetEditor\0"
    "\0currentRow,currnetCol,,\0"
    "on_tableWidget_currentCellChanged(int,int,int,int)\0"
    "item\0on_tableWidget_itemChanged(QTableWidgetItem*)\0"
    "idx\0on_columnEditor_indexChanged(int)\0"
    "idx,role,v\0on_columnEditor_itemChanged(int,int,QVariant)\0"
    "on_columnEditor_itemInserted(int)\0"
    "on_columnEditor_itemDeleted(int)\0"
    "on_columnEditor_itemMovedUp(int)\0"
    "on_columnEditor_itemMovedDown(int)\0"
    "on_rowEditor_indexChanged(int)\0"
    "on_rowEditor_itemChanged(int,int,QVariant)\0"
    "on_rowEditor_itemInserted(int)\0"
    "on_rowEditor_itemDeleted(int)\0"
    "on_rowEditor_itemMovedUp(int)\0"
    "on_rowEditor_itemMovedDown(int)\0"
    "togglePropertyBrowser()\0cacheReloaded()\0"
};

const QMetaObject qdesigner_internal::TableWidgetEditor::staticMetaObject = {
    { &AbstractItemEditor::staticMetaObject, qt_meta_stringdata_qdesigner_internal__TableWidgetEditor,
      qt_meta_data_qdesigner_internal__TableWidgetEditor, 0 }
};

const QMetaObject *qdesigner_internal::TableWidgetEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::TableWidgetEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__TableWidgetEditor))
        return static_cast<void*>(const_cast< TableWidgetEditor*>(this));
    return AbstractItemEditor::qt_metacast(_clname);
}

int qdesigner_internal::TableWidgetEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractItemEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_tableWidget_currentCellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: on_tableWidget_itemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 2: on_columnEditor_indexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_columnEditor_itemChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 4: on_columnEditor_itemInserted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_columnEditor_itemDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: on_columnEditor_itemMovedUp((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: on_columnEditor_itemMovedDown((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: on_rowEditor_indexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: on_rowEditor_itemChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 10: on_rowEditor_itemInserted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: on_rowEditor_itemDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: on_rowEditor_itemMovedUp((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: on_rowEditor_itemMovedDown((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: togglePropertyBrowser(); break;
        case 15: cacheReloaded(); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
