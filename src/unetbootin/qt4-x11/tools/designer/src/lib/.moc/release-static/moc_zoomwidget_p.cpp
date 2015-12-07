/****************************************************************************
** Meta object code from reading C++ file 'zoomwidget_p.h'
**
** Created: Thu Mar 5 20:47:16 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/zoomwidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'zoomwidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ZoomMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   47,   29,   29, 0x0a,
      68,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ZoomMenu[] = {
    "qdesigner_internal::ZoomMenu\0\0"
    "zoomChanged(int)\0percent\0setZoom(int)\0"
    "slotZoomMenu(QAction*)\0"
};

const QMetaObject qdesigner_internal::ZoomMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ZoomMenu,
      qt_meta_data_qdesigner_internal__ZoomMenu, 0 }
};

const QMetaObject *qdesigner_internal::ZoomMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ZoomMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ZoomMenu))
        return static_cast<void*>(const_cast< ZoomMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::ZoomMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: zoomChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setZoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: slotZoomMenu((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ZoomMenu::zoomChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__ZoomView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       3,   22, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      38,   30,   29,   29, 0x0a,
      61,   51,   29,   29, 0x0a,

 // properties: name, type, flags
      89,   85, 0x02095103,
      99,   94, 0x01095103,
     122,   94, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ZoomView[] = {
    "qdesigner_internal::ZoomView\0\0percent\0"
    "setZoom(int)\0globalPos\0showContextMenu(QPoint)\0"
    "int\0zoom\0bool\0zoomContextMenuEnabled\0"
    "autoScrollSuppressed\0"
};

const QMetaObject qdesigner_internal::ZoomView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ZoomView,
      qt_meta_data_qdesigner_internal__ZoomView, 0 }
};

const QMetaObject *qdesigner_internal::ZoomView::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ZoomView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ZoomView))
        return static_cast<void*>(const_cast< ZoomView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int qdesigner_internal::ZoomView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setZoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = zoom(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isZoomContextMenuEnabled(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isAutoScrollSuppressed(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setZoom(*reinterpret_cast< int*>(_v)); break;
        case 1: setZoomContextMenuEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setAutoScrollSuppressed(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__ZoomWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       2,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      32,   31,   31,   31, 0x0a,

 // properties: name, type, flags
      44,   39, 0x01095103,
      73,   39, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ZoomWidget[] = {
    "qdesigner_internal::ZoomWidget\0\0dump()\0"
    "bool\0widgetZoomContextMenuEnabled\0"
    "itemAcceptDrops\0"
};

const QMetaObject qdesigner_internal::ZoomWidget::staticMetaObject = {
    { &ZoomView::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ZoomWidget,
      qt_meta_data_qdesigner_internal__ZoomWidget, 0 }
};

const QMetaObject *qdesigner_internal::ZoomWidget::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ZoomWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ZoomWidget))
        return static_cast<void*>(const_cast< ZoomWidget*>(this));
    return ZoomView::qt_metacast(_clname);
}

int qdesigner_internal::ZoomWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ZoomView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dump(); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isWidgetZoomContextMenuEnabled(); break;
        case 1: *reinterpret_cast< bool*>(_v) = itemAcceptDrops(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setWidgetZoomContextMenuEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 1: setItemAcceptDrops(*reinterpret_cast< bool*>(_v)); break;
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
