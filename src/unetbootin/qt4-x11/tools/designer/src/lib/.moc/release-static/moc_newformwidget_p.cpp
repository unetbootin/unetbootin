/****************************************************************************
** Meta object code from reading C++ file 'newformwidget_p.h'
**
** Created: Thu Mar 5 20:47:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/newformwidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newformwidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__NewFormWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      40,   35,   34,   34, 0x08,
      95,   86,   34,   34, 0x08,
     163,   35,   34,   34, 0x08,
     211,  207,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__NewFormWidget[] = {
    "qdesigner_internal::NewFormWidget\0\0"
    "item\0on_treeWidget_itemActivated(QTreeWidgetItem*)\0"
    "current,\0"
    "on_treeWidget_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)\0"
    "on_treeWidget_itemPressed(QTreeWidgetItem*)\0"
    "idx\0slotDeviceProfileIndexChanged(int)\0"
};

const QMetaObject qdesigner_internal::NewFormWidget::staticMetaObject = {
    { &QDesignerNewFormWidgetInterface::staticMetaObject, qt_meta_stringdata_qdesigner_internal__NewFormWidget,
      qt_meta_data_qdesigner_internal__NewFormWidget, 0 }
};

const QMetaObject *qdesigner_internal::NewFormWidget::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::NewFormWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__NewFormWidget))
        return static_cast<void*>(const_cast< NewFormWidget*>(this));
    return QDesignerNewFormWidgetInterface::qt_metacast(_clname);
}

int qdesigner_internal::NewFormWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerNewFormWidgetInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_treeWidget_itemActivated((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 1: on_treeWidget_currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 2: on_treeWidget_itemPressed((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 3: slotDeviceProfileIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
