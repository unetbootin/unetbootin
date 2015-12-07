/****************************************************************************
** Meta object code from reading C++ file 'signalsloteditor_plugin.h'
**
** Created: Thu Mar 5 20:52:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../signalsloteditor/signalsloteditor_plugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signalsloteditor_plugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__SignalSlotEditorPlugin[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      55,   44,   43,   43, 0x0a,
     110,   44,   43,   43, 0x08,
     155,   44,   43,   43, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__SignalSlotEditorPlugin[] = {
    "qdesigner_internal::SignalSlotEditorPlugin\0"
    "\0formWindow\0"
    "activeFormWindowChanged(QDesignerFormWindowInterface*)\0"
    "addFormWindow(QDesignerFormWindowInterface*)\0"
    "removeFormWindow(QDesignerFormWindowInterface*)\0"
};

const QMetaObject qdesigner_internal::SignalSlotEditorPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__SignalSlotEditorPlugin,
      qt_meta_data_qdesigner_internal__SignalSlotEditorPlugin, 0 }
};

const QMetaObject *qdesigner_internal::SignalSlotEditorPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::SignalSlotEditorPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__SignalSlotEditorPlugin))
        return static_cast<void*>(const_cast< SignalSlotEditorPlugin*>(this));
    if (!strcmp(_clname, "QDesignerFormEditorPluginInterface"))
        return static_cast< QDesignerFormEditorPluginInterface*>(const_cast< SignalSlotEditorPlugin*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.QDesignerFormEditorPluginInterface"))
        return static_cast< QDesignerFormEditorPluginInterface*>(const_cast< SignalSlotEditorPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::SignalSlotEditorPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
