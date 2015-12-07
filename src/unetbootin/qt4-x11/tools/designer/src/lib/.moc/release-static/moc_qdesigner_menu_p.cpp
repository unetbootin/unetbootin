/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_menu_p.h'
**
** Created: Thu Mar 5 20:47:08 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_menu_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_menu_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      34,   14,   14,   14, 0x08,
      61,   14,   14,   14, 0x08,
      82,   14,   14,   14, 0x08,
     102,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerMenu[] = {
    "QDesignerMenu\0\0slotAddSeparator()\0"
    "slotRemoveSelectedAction()\0"
    "slotShowSubMenuNow()\0slotDeactivateNow()\0"
    "slotAdjustSizeNow()\0"
};

const QMetaObject QDesignerMenu::staticMetaObject = {
    { &QMenu::staticMetaObject, qt_meta_stringdata_QDesignerMenu,
      qt_meta_data_QDesignerMenu, 0 }
};

const QMetaObject *QDesignerMenu::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerMenu))
        return static_cast<void*>(const_cast< QDesignerMenu*>(this));
    return QMenu::qt_metacast(_clname);
}

int QDesignerMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotAddSeparator(); break;
        case 1: slotRemoveSelectedAction(); break;
        case 2: slotShowSubMenuNow(); break;
        case 3: slotDeactivateNow(); break;
        case 4: slotAdjustSizeNow(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
