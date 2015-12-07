/****************************************************************************
** Meta object code from reading C++ file 'toolbar_taskmenu.h'
**
** Created: Thu Mar 5 20:52:30 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../taskmenu/toolbar_taskmenu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toolbar_taskmenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ToolBarTaskMenu[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__ToolBarTaskMenu[] = {
    "qdesigner_internal::ToolBarTaskMenu\0"
};

const QMetaObject qdesigner_internal::ToolBarTaskMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ToolBarTaskMenu,
      qt_meta_data_qdesigner_internal__ToolBarTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::ToolBarTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ToolBarTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ToolBarTaskMenu))
        return static_cast<void*>(const_cast< ToolBarTaskMenu*>(this));
    if (!strcmp(_clname, "QDesignerTaskMenuExtension"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< ToolBarTaskMenu*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.TaskMenu"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< ToolBarTaskMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::ToolBarTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__StatusBarTaskMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      39,   38,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__StatusBarTaskMenu[] = {
    "qdesigner_internal::StatusBarTaskMenu\0"
    "\0removeStatusBar()\0"
};

const QMetaObject qdesigner_internal::StatusBarTaskMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__StatusBarTaskMenu,
      qt_meta_data_qdesigner_internal__StatusBarTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::StatusBarTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::StatusBarTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__StatusBarTaskMenu))
        return static_cast<void*>(const_cast< StatusBarTaskMenu*>(this));
    if (!strcmp(_clname, "QDesignerTaskMenuExtension"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< StatusBarTaskMenu*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.TaskMenu"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< StatusBarTaskMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::StatusBarTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: removeStatusBar(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
