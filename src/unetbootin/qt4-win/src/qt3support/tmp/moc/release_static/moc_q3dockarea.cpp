/****************************************************************************
** Meta object code from reading C++ file 'q3dockarea.h'
**
** Created: Fri May 30 22:41:17 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/q3dockarea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3dockarea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3DockAreaLayout[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_Q3DockAreaLayout[] = {
    "Q3DockAreaLayout\0"
};

const QMetaObject Q3DockAreaLayout::staticMetaObject = {
    { &QLayout::staticMetaObject, qt_meta_stringdata_Q3DockAreaLayout,
      qt_meta_data_Q3DockAreaLayout, 0 }
};

const QMetaObject *Q3DockAreaLayout::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DockAreaLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DockAreaLayout))
	return static_cast<void*>(const_cast< Q3DockAreaLayout*>(this));
    return QLayout::qt_metacast(_clname);
}

int Q3DockAreaLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Q3DockArea[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       4,   15, // properties
       1,   27, // enums/sets

 // slots: signature, parameters, type, tag, flags
      25,   12,   11,   11, 0x0a,

 // properties: name, type, flags
      54,   38, 0x00095009,
      70,   66, 0x02095001,
      81,   76, 0x01095001,
     102,   87, 0x00095009,

 // enums: name, flags, count, data
      87, 0x0,    2,   31,

 // enum data: key, value
     117, uint(Q3DockArea::Normal),
     124, uint(Q3DockArea::Reverse),

       0        // eod
};

static const char qt_meta_stringdata_Q3DockArea[] = {
    "Q3DockArea\0\0keepNewLines\0lineUp(bool)\0"
    "Qt::Orientation\0orientation\0int\0count\0"
    "bool\0empty\0HandlePosition\0handlePosition\0"
    "Normal\0Reverse\0"
};

const QMetaObject Q3DockArea::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Q3DockArea,
      qt_meta_data_Q3DockArea, 0 }
};

const QMetaObject *Q3DockArea::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DockArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DockArea))
	return static_cast<void*>(const_cast< Q3DockArea*>(this));
    return QWidget::qt_metacast(_clname);
}

int Q3DockArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: lineUp((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 1: *reinterpret_cast< int*>(_v) = count(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isEmpty(); break;
        case 3: *reinterpret_cast< HandlePosition*>(_v) = handlePosition(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
