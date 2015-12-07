/****************************************************************************
** Meta object code from reading C++ file 'qsqlrelationaltablemodel.h'
**
** Created: Thu Mar 5 18:58:17 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../models/qsqlrelationaltablemodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsqlrelationaltablemodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSqlRelationalTableModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      30,   26,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QSqlRelationalTableModel[] = {
    "QSqlRelationalTableModel\0\0row\0"
    "revertRow(int)\0"
};

const QMetaObject QSqlRelationalTableModel::staticMetaObject = {
    { &QSqlTableModel::staticMetaObject, qt_meta_stringdata_QSqlRelationalTableModel,
      qt_meta_data_QSqlRelationalTableModel, 0 }
};

const QMetaObject *QSqlRelationalTableModel::metaObject() const
{
    return &staticMetaObject;
}

void *QSqlRelationalTableModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSqlRelationalTableModel))
        return static_cast<void*>(const_cast< QSqlRelationalTableModel*>(this));
    return QSqlTableModel::qt_metacast(_clname);
}

int QSqlRelationalTableModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSqlTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: revertRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
