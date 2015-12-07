/****************************************************************************
** Meta object code from reading C++ file 'qabstractproxymodel.h'
**
** Created: Fri May 30 22:31:40 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qabstractproxymodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractproxymodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractProxyModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractProxyModel[] = {
    "QAbstractProxyModel\0\0_q_sourceModelDestroyed()\0"
};

const QMetaObject QAbstractProxyModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QAbstractProxyModel,
      qt_meta_data_QAbstractProxyModel, 0 }
};

const QMetaObject *QAbstractProxyModel::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractProxyModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractProxyModel))
	return static_cast<void*>(const_cast< QAbstractProxyModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QAbstractProxyModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_sourceModelDestroyed(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
