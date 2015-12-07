/****************************************************************************
** Meta object code from reading C++ file 'tool_widgeteditor.h'
**
** Created: Thu Mar 5 20:52:14 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../formeditor/tool_widgeteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tool_widgeteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__WidgetEditorTool[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__WidgetEditorTool[] = {
    "qdesigner_internal::WidgetEditorTool\0"
};

const QMetaObject qdesigner_internal::WidgetEditorTool::staticMetaObject = {
    { &QDesignerFormWindowToolInterface::staticMetaObject, qt_meta_stringdata_qdesigner_internal__WidgetEditorTool,
      qt_meta_data_qdesigner_internal__WidgetEditorTool, 0 }
};

const QMetaObject *qdesigner_internal::WidgetEditorTool::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::WidgetEditorTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__WidgetEditorTool))
        return static_cast<void*>(const_cast< WidgetEditorTool*>(this));
    return QDesignerFormWindowToolInterface::qt_metacast(_clname);
}

int qdesigner_internal::WidgetEditorTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerFormWindowToolInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
