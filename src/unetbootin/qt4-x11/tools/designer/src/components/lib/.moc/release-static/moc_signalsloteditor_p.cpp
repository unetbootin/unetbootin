/****************************************************************************
** Meta object code from reading C++ file 'signalsloteditor_p.h'
**
** Created: Thu Mar 5 20:52:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../signalsloteditor/signalsloteditor_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signalsloteditor_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ConnectionModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      41,   37,   36,   36, 0x08,
      74,   70,   36,   36, 0x08,
      97,   37,   36,   36, 0x08,
     134,   70,   36,   36, 0x08,
     160,   37,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ConnectionModel[] = {
    "qdesigner_internal::ConnectionModel\0"
    "\0con\0connectionAdded(Connection*)\0idx\0"
    "connectionRemoved(int)\0"
    "aboutToRemoveConnection(Connection*)\0"
    "aboutToAddConnection(int)\0"
    "connectionChanged(Connection*)\0"
};

const QMetaObject qdesigner_internal::ConnectionModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ConnectionModel,
      qt_meta_data_qdesigner_internal__ConnectionModel, 0 }
};

const QMetaObject *qdesigner_internal::ConnectionModel::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ConnectionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ConnectionModel))
        return static_cast<void*>(const_cast< ConnectionModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int qdesigner_internal::ConnectionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: connectionAdded((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        case 1: connectionRemoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: aboutToRemoveConnection((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        case 3: aboutToAddConnection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: connectionChanged((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
