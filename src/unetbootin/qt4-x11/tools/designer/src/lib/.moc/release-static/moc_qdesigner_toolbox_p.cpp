/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_toolbox_p.h'
**
** Created: Thu Mar 5 20:47:00 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_toolbox_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_toolbox_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QToolBoxHelper[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      36,   15,   15,   15, 0x08,
      46,   15,   15,   15, 0x08,
      61,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QToolBoxHelper[] = {
    "QToolBoxHelper\0\0removeCurrentPage()\0"
    "addPage()\0addPageAfter()\0changeOrder()\0"
};

const QMetaObject QToolBoxHelper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QToolBoxHelper,
      qt_meta_data_QToolBoxHelper, 0 }
};

const QMetaObject *QToolBoxHelper::metaObject() const
{
    return &staticMetaObject;
}

void *QToolBoxHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QToolBoxHelper))
        return static_cast<void*>(const_cast< QToolBoxHelper*>(this));
    return QObject::qt_metacast(_clname);
}

int QToolBoxHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: removeCurrentPage(); break;
        case 1: addPage(); break;
        case 2: addPageAfter(); break;
        case 3: changeOrder(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
