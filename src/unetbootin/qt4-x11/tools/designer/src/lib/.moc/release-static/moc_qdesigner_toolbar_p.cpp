/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_toolbar_p.h'
**
** Created: Thu Mar 5 20:47:06 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_toolbar_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_toolbar_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ToolBarEventFilter[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      40,   39,   39,   39, 0x08,
      67,   39,   39,   39, 0x08,
      87,   39,   39,   39, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ToolBarEventFilter[] = {
    "qdesigner_internal::ToolBarEventFilter\0"
    "\0slotRemoveSelectedAction()\0"
    "slotRemoveToolBar()\0slotInsertSeparator()\0"
};

const QMetaObject qdesigner_internal::ToolBarEventFilter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ToolBarEventFilter,
      qt_meta_data_qdesigner_internal__ToolBarEventFilter, 0 }
};

const QMetaObject *qdesigner_internal::ToolBarEventFilter::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ToolBarEventFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ToolBarEventFilter))
        return static_cast<void*>(const_cast< ToolBarEventFilter*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::ToolBarEventFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotRemoveSelectedAction(); break;
        case 1: slotRemoveToolBar(); break;
        case 2: slotInsertSeparator(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
