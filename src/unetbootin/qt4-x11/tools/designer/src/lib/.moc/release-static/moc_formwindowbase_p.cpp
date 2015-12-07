/****************************************************************************
** Meta object code from reading C++ file 'formwindowbase_p.h'
**
** Created: Thu Mar 5 20:47:13 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/formwindowbase_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'formwindowbase_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__FormWindowBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      67,   36,   35,   35, 0x0a,
     111,  109,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__FormWindowBase[] = {
    "qdesigner_internal::FormWindowBase\0\0"
    "resourceSet,resourceSetChanged\0"
    "resourceSetActivated(QtResourceSet*,bool)\0"
    "w\0triggerDefaultAction(QWidget*)\0"
};

const QMetaObject qdesigner_internal::FormWindowBase::staticMetaObject = {
    { &QDesignerFormWindowInterface::staticMetaObject, qt_meta_stringdata_qdesigner_internal__FormWindowBase,
      qt_meta_data_qdesigner_internal__FormWindowBase, 0 }
};

const QMetaObject *qdesigner_internal::FormWindowBase::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::FormWindowBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__FormWindowBase))
        return static_cast<void*>(const_cast< FormWindowBase*>(this));
    return QDesignerFormWindowInterface::qt_metacast(_clname);
}

int qdesigner_internal::FormWindowBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerFormWindowInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resourceSetActivated((*reinterpret_cast< QtResourceSet*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: triggerDefaultAction((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
