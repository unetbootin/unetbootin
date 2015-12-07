/****************************************************************************
** Meta object code from reading C++ file 'qtablewidget_p.h'
**
** Created: Thu Mar 5 18:57:38 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qtablewidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtablewidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTableWidgetMimeData[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QTableWidgetMimeData[] = {
    "QTableWidgetMimeData\0"
};

const QMetaObject QTableWidgetMimeData::staticMetaObject = {
    { &QMimeData::staticMetaObject, qt_meta_stringdata_QTableWidgetMimeData,
      qt_meta_data_QTableWidgetMimeData, 0 }
};

const QMetaObject *QTableWidgetMimeData::metaObject() const
{
    return &staticMetaObject;
}

void *QTableWidgetMimeData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTableWidgetMimeData))
        return static_cast<void*>(const_cast< QTableWidgetMimeData*>(this));
    return QMimeData::qt_metacast(_clname);
}

int QTableWidgetMimeData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMimeData::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QTableModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QTableModel[] = {
    "QTableModel\0"
};

const QMetaObject QTableModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_QTableModel,
      qt_meta_data_QTableModel, 0 }
};

const QMetaObject *QTableModel::metaObject() const
{
    return &staticMetaObject;
}

void *QTableModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTableModel))
        return static_cast<void*>(const_cast< QTableModel*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int QTableModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
