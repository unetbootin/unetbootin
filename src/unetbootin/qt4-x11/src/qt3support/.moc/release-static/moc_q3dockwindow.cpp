/****************************************************************************
** Meta object code from reading C++ file 'q3dockwindow.h'
**
** Created: Thu Mar 5 19:05:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/q3dockwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3dockwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3DockWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
      10,   52, // properties
       2,   82, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   14,   13,   13, 0x05,
      54,   52,   13,   13, 0x05,
      88,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     114,  112,   13,   13, 0x0a,
     131,   13,   13,   13, 0x0a,
     140,   13,   13,   13, 0x0a,
     147,   14,   13,   13, 0x0a,
     179,   13,   13,   13, 0x08,

 // properties: name, type, flags
     199,  195, 0x02095103,
     214,  209, 0x01095103,
     228,  209, 0x01095103,
     242,  209, 0x01095103,
     266,  209, 0x01095103,
     288,  209, 0x01095001,
     300,  209, 0x01095103,
     308,  209, 0x01095103,
     321,  195, 0x02095103,
     334,  328, 0x00095009,

 // enums: name, flags, count, data
     328, 0x0,    2,   90,
     340, 0x0,    4,   94,

 // enum data: key, value
     350, uint(Q3DockWindow::InDock),
     357, uint(Q3DockWindow::OutsideDock),
     369, uint(Q3DockWindow::Never),
     375, uint(Q3DockWindow::Docked),
     382, uint(Q3DockWindow::Undocked),
     391, uint(Q3DockWindow::Always),

       0        // eod
};

static const char qt_meta_stringdata_Q3DockWindow[] = {
    "Q3DockWindow\0\0o\0orientationChanged(Qt::Orientation)\0"
    "p\0placeChanged(Q3DockWindow::Place)\0"
    "visibilityChanged(bool)\0w\0undock(QWidget*)\0"
    "undock()\0dock()\0setOrientation(Qt::Orientation)\0"
    "toggleVisible()\0int\0closeMode\0bool\0"
    "resizeEnabled\0movingEnabled\0"
    "horizontallyStretchable\0verticallyStretchable\0"
    "stretchable\0newLine\0opaqueMoving\0"
    "offset\0Place\0place\0CloseMode\0InDock\0"
    "OutsideDock\0Never\0Docked\0Undocked\0"
    "Always\0"
};

const QMetaObject Q3DockWindow::staticMetaObject = {
    { &Q3Frame::staticMetaObject, qt_meta_stringdata_Q3DockWindow,
      qt_meta_data_Q3DockWindow, 0 }
};

const QMetaObject *Q3DockWindow::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DockWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DockWindow))
        return static_cast<void*>(const_cast< Q3DockWindow*>(this));
    return Q3Frame::qt_metacast(_clname);
}

int Q3DockWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3Frame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: orientationChanged((*reinterpret_cast< Qt::Orientation(*)>(_a[1]))); break;
        case 1: placeChanged((*reinterpret_cast< Q3DockWindow::Place(*)>(_a[1]))); break;
        case 2: visibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: undock((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 4: undock(); break;
        case 5: dock(); break;
        case 6: setOrientation((*reinterpret_cast< Qt::Orientation(*)>(_a[1]))); break;
        case 7: toggleVisible(); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = closeMode(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isResizeEnabled(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isMovingEnabled(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isHorizontallyStretchable(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isVerticallyStretchable(); break;
        case 5: *reinterpret_cast< bool*>(_v) = isStretchable(); break;
        case 6: *reinterpret_cast< bool*>(_v) = newLine(); break;
        case 7: *reinterpret_cast< bool*>(_v) = opaqueMoving(); break;
        case 8: *reinterpret_cast< int*>(_v) = offset(); break;
        case 9: *reinterpret_cast< Place*>(_v) = place(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCloseMode(*reinterpret_cast< int*>(_v)); break;
        case 1: setResizeEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setMovingEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setHorizontallyStretchable(*reinterpret_cast< bool*>(_v)); break;
        case 4: setVerticallyStretchable(*reinterpret_cast< bool*>(_v)); break;
        case 6: setNewLine(*reinterpret_cast< bool*>(_v)); break;
        case 7: setOpaqueMoving(*reinterpret_cast< bool*>(_v)); break;
        case 8: setOffset(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3DockWindow::orientationChanged(Qt::Orientation _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3DockWindow::placeChanged(Q3DockWindow::Place _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3DockWindow::visibilityChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
