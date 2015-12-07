/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_taskmenu_p.h'
**
** Created: Thu Mar 5 20:46:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_taskmenu_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_taskmenu_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__QDesignerTaskMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      39,   38,   38,   38, 0x08,
      58,   38,   38,   38, 0x08,
      74,   38,   38,   38, 0x08,
      92,   38,   38,   38, 0x08,
     111,   38,   38,   38, 0x08,
     127,   38,   38,   38, 0x08,
     140,   38,   38,   38, 0x08,
     158,   38,   38,   38, 0x08,
     176,   38,   38,   38, 0x08,
     191,   38,   38,   38, 0x08,
     214,   38,   38,   38, 0x08,
     237,  235,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__QDesignerTaskMenu[] = {
    "qdesigner_internal::QDesignerTaskMenu\0"
    "\0changeObjectName()\0changeToolTip()\0"
    "changeWhatsThis()\0changeStyleSheet()\0"
    "createMenuBar()\0addToolBar()\0"
    "createStatusBar()\0removeStatusBar()\0"
    "changeScript()\0containerFakeMethods()\0"
    "slotNavigateToSlot()\0a\0applySize(QAction*)\0"
};

const QMetaObject qdesigner_internal::QDesignerTaskMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__QDesignerTaskMenu,
      qt_meta_data_qdesigner_internal__QDesignerTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::QDesignerTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::QDesignerTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__QDesignerTaskMenu))
        return static_cast<void*>(const_cast< QDesignerTaskMenu*>(this));
    if (!strcmp(_clname, "QDesignerTaskMenuExtension"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< QDesignerTaskMenu*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.TaskMenu"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< QDesignerTaskMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::QDesignerTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changeObjectName(); break;
        case 1: changeToolTip(); break;
        case 2: changeWhatsThis(); break;
        case 3: changeStyleSheet(); break;
        case 4: createMenuBar(); break;
        case 5: addToolBar(); break;
        case 6: createStatusBar(); break;
        case 7: removeStatusBar(); break;
        case 8: changeScript(); break;
        case 9: containerFakeMethods(); break;
        case 10: slotNavigateToSlot(); break;
        case 11: applySize((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
