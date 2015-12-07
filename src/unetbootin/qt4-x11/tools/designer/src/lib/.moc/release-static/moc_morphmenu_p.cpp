/****************************************************************************
** Meta object code from reading C++ file 'morphmenu_p.h'
**
** Created: Thu Mar 5 20:46:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/morphmenu_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'morphmenu_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__MorphMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      44,   31,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__MorphMenu[] = {
    "qdesigner_internal::MorphMenu\0\0"
    "newClassName\0slotMorph(QString)\0"
};

const QMetaObject qdesigner_internal::MorphMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__MorphMenu,
      qt_meta_data_qdesigner_internal__MorphMenu, 0 }
};

const QMetaObject *qdesigner_internal::MorphMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::MorphMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__MorphMenu))
        return static_cast<void*>(const_cast< MorphMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::MorphMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotMorph((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
