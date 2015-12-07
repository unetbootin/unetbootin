/****************************************************************************
** Meta object code from reading C++ file 'promotiontaskmenu_p.h'
**
** Created: Thu Mar 5 20:47:11 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/promotiontaskmenu_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'promotiontaskmenu_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PromotionTaskMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      55,   39,   38,   38, 0x08,
      90,   38,   38,   38, 0x08,
     119,   38,   38,   38, 0x08,
     145,   38,   38,   38, 0x08,
     165,   38,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PromotionTaskMenu[] = {
    "qdesigner_internal::PromotionTaskMenu\0"
    "\0customClassName\0slotPromoteToCustomWidget(QString)\0"
    "slotDemoteFromCustomWidget()\0"
    "slotEditPromotedWidgets()\0slotEditPromoteTo()\0"
    "slotEditSignalsSlots()\0"
};

const QMetaObject qdesigner_internal::PromotionTaskMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PromotionTaskMenu,
      qt_meta_data_qdesigner_internal__PromotionTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::PromotionTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PromotionTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PromotionTaskMenu))
        return static_cast<void*>(const_cast< PromotionTaskMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::PromotionTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotPromoteToCustomWidget((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: slotDemoteFromCustomWidget(); break;
        case 2: slotEditPromotedWidgets(); break;
        case 3: slotEditPromoteTo(); break;
        case 4: slotEditSignalsSlots(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
