/****************************************************************************
** Meta object code from reading C++ file 'signalsloteditor_tool.h'
**
** Created: Thu Mar 5 20:52:00 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../signalsloteditor/signalsloteditor_tool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signalsloteditor_tool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__SignalSlotEditorTool[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__SignalSlotEditorTool[] = {
    "qdesigner_internal::SignalSlotEditorTool\0"
};

const QMetaObject qdesigner_internal::SignalSlotEditorTool::staticMetaObject = {
    { &QDesignerFormWindowToolInterface::staticMetaObject, qt_meta_stringdata_qdesigner_internal__SignalSlotEditorTool,
      qt_meta_data_qdesigner_internal__SignalSlotEditorTool, 0 }
};

const QMetaObject *qdesigner_internal::SignalSlotEditorTool::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::SignalSlotEditorTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__SignalSlotEditorTool))
        return static_cast<void*>(const_cast< SignalSlotEditorTool*>(this));
    return QDesignerFormWindowToolInterface::qt_metacast(_clname);
}

int qdesigner_internal::SignalSlotEditorTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerFormWindowToolInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
