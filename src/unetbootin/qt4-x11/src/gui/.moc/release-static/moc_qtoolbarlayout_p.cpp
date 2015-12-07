/****************************************************************************
** Meta object code from reading C++ file 'qtoolbarlayout_p.h'
**
** Created: Thu Mar 5 18:57:26 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qtoolbarlayout_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtoolbarlayout_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QToolBarLayout[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QToolBarLayout[] = {
    "QToolBarLayout\0\0b\0setExpanded(bool)\0"
};

const QMetaObject QToolBarLayout::staticMetaObject = {
    { &QLayout::staticMetaObject, qt_meta_stringdata_QToolBarLayout,
      qt_meta_data_QToolBarLayout, 0 }
};

const QMetaObject *QToolBarLayout::metaObject() const
{
    return &staticMetaObject;
}

void *QToolBarLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QToolBarLayout))
        return static_cast<void*>(const_cast< QToolBarLayout*>(this));
    return QLayout::qt_metacast(_clname);
}

int QToolBarLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setExpanded((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
