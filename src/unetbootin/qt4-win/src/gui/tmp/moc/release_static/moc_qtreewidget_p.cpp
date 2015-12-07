/****************************************************************************
** Meta object code from reading C++ file 'qtreewidget_p.h'
**
** Created: Fri May 30 22:34:12 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qtreewidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtreewidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTreeModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QTreeModel[] = {
    "QTreeModel\0\0itemsSorted()\0"
};

const QMetaObject QTreeModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QTreeModel,
      qt_meta_data_QTreeModel, 0 }
};

const QMetaObject *QTreeModel::metaObject() const
{
    return &staticMetaObject;
}

void *QTreeModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTreeModel))
	return static_cast<void*>(const_cast< QTreeModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QTreeModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemsSorted(); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QTreeModel::itemsSorted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
