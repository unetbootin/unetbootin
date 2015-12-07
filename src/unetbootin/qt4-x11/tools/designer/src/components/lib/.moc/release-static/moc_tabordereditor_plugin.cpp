/****************************************************************************
** Meta object code from reading C++ file 'tabordereditor_plugin.h'
**
** Created: Thu Mar 5 20:52:41 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../tabordereditor/tabordereditor_plugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tabordereditor_plugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__TabOrderEditorPlugin[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      53,   42,   41,   41, 0x0a,
     108,   42,   41,   41, 0x08,
     153,   42,   41,   41, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__TabOrderEditorPlugin[] = {
    "qdesigner_internal::TabOrderEditorPlugin\0"
    "\0formWindow\0"
    "activeFormWindowChanged(QDesignerFormWindowInterface*)\0"
    "addFormWindow(QDesignerFormWindowInterface*)\0"
    "removeFormWindow(QDesignerFormWindowInterface*)\0"
};

const QMetaObject qdesigner_internal::TabOrderEditorPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__TabOrderEditorPlugin,
      qt_meta_data_qdesigner_internal__TabOrderEditorPlugin, 0 }
};

const QMetaObject *qdesigner_internal::TabOrderEditorPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::TabOrderEditorPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__TabOrderEditorPlugin))
        return static_cast<void*>(const_cast< TabOrderEditorPlugin*>(this));
    if (!strcmp(_clname, "QDesignerFormEditorPluginInterface"))
        return static_cast< QDesignerFormEditorPluginInterface*>(const_cast< TabOrderEditorPlugin*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.QDesignerFormEditorPluginInterface"))
        return static_cast< QDesignerFormEditorPluginInterface*>(const_cast< TabOrderEditorPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::TabOrderEditorPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activeFormWindowChanged((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 1: addFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 2: removeFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
