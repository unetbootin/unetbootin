/****************************************************************************
** Meta object code from reading C++ file 'qundoview.h'
**
** Created: Fri May 30 22:34:23 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../util/qundoview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qundoview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QUndoView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       2,   20, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x0a,
      45,   39,   10,   10, 0x0a,

 // properties: name, type, flags
      75,   67, 0x0a095103,
      92,   86, 0x45095103,

       0        // eod
};

static const char qt_meta_stringdata_QUndoView[] = {
    "QUndoView\0\0stack\0setStack(QUndoStack*)\0"
    "group\0setGroup(QUndoGroup*)\0QString\0"
    "emptyLabel\0QIcon\0cleanIcon\0"
};

const QMetaObject QUndoView::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_QUndoView,
      qt_meta_data_QUndoView, 0 }
};

const QMetaObject *QUndoView::metaObject() const
{
    return &staticMetaObject;
}

void *QUndoView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QUndoView))
	return static_cast<void*>(const_cast< QUndoView*>(this));
    return QListView::qt_metacast(_clname);
}

int QUndoView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setStack((*reinterpret_cast< QUndoStack*(*)>(_a[1]))); break;
        case 1: setGroup((*reinterpret_cast< QUndoGroup*(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = emptyLabel(); break;
        case 1: *reinterpret_cast< QIcon*>(_v) = cleanIcon(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setEmptyLabel(*reinterpret_cast< QString*>(_v)); break;
        case 1: setCleanIcon(*reinterpret_cast< QIcon*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
