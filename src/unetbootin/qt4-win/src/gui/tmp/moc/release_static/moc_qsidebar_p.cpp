/****************************************************************************
** Meta object code from reading C++ file 'qsidebar_p.h'
**
** Created: Fri May 30 22:34:01 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qsidebar_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsidebar_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QUrlModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      31,   11,   10,   10, 0x08,
      68,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QUrlModel[] = {
    "QUrlModel\0\0topLeft,bottomRight\0"
    "dataChanged(QModelIndex,QModelIndex)\0"
    "layoutChanged()\0"
};

const QMetaObject QUrlModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_QUrlModel,
      qt_meta_data_QUrlModel, 0 }
};

const QMetaObject *QUrlModel::metaObject() const
{
    return &staticMetaObject;
}

void *QUrlModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QUrlModel))
	return static_cast<void*>(const_cast< QUrlModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int QUrlModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 1: layoutChanged(); break;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_QSidebar[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   10,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   28,    9,    9, 0x08,
      64,   55,    9,    9, 0x08,
      88,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QSidebar[] = {
    "QSidebar\0\0url\0goToUrl(QUrl)\0index\0"
    "clicked(QModelIndex)\0position\0"
    "showContextMenu(QPoint)\0removeEntry()\0"
};

const QMetaObject QSidebar::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_QSidebar,
      qt_meta_data_QSidebar, 0 }
};

const QMetaObject *QSidebar::metaObject() const
{
    return &staticMetaObject;
}

void *QSidebar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSidebar))
	return static_cast<void*>(const_cast< QSidebar*>(this));
    return QListView::qt_metacast(_clname);
}

int QSidebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: goToUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: removeEntry(); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QSidebar::goToUrl(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
