/****************************************************************************
** Meta object code from reading C++ file 'qdockwidget.h'
**
** Created: Fri May 30 22:27:19 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qdockwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdockwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDockWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       4,   45, // properties
       1,   57, // enums/sets

 // signals: signature, parameters, type, tag, flags
      22,   13,   12,   12, 0x05,
      80,   71,   12,   12, 0x05,
     115,  102,   12,   12, 0x05,
     164,  156,   12,   12, 0x05,
     193,  188,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     233,   12,   12,   12, 0x08,
     253,   12,   12,   12, 0x08,

 // properties: name, type, flags
     278,  273, 0x01095103,
      13,  287, 0x0009510b,
     102,  306, 0x0009510b,
     334,  326, 0x0a095103,

 // enums: name, flags, count, data
     287, 0x1,    8,   61,

 // enum data: key, value
     346, uint(QDockWidget::DockWidgetClosable),
     365, uint(QDockWidget::DockWidgetMovable),
     383, uint(QDockWidget::DockWidgetFloatable),
     403, uint(QDockWidget::DockWidgetVerticalTitleBar),
     430, uint(QDockWidget::DockWidgetFeatureMask),
     452, uint(QDockWidget::AllDockWidgetFeatures),
     474, uint(QDockWidget::NoDockWidgetFeatures),
     495, uint(QDockWidget::Reserved),

       0        // eod
};

static const char qt_meta_stringdata_QDockWidget[] = {
    "QDockWidget\0\0features\0"
    "featuresChanged(QDockWidget::DockWidgetFeatures)\0"
    "topLevel\0topLevelChanged(bool)\0"
    "allowedAreas\0allowedAreasChanged(Qt::DockWidgetAreas)\0"
    "visible\0visibilityChanged(bool)\0area\0"
    "dockLocationChanged(Qt::DockWidgetArea)\0"
    "_q_toggleView(bool)\0_q_toggleTopLevel()\0"
    "bool\0floating\0DockWidgetFeatures\0"
    "Qt::DockWidgetAreas\0QString\0windowTitle\0"
    "DockWidgetClosable\0DockWidgetMovable\0"
    "DockWidgetFloatable\0DockWidgetVerticalTitleBar\0"
    "DockWidgetFeatureMask\0AllDockWidgetFeatures\0"
    "NoDockWidgetFeatures\0Reserved\0"
};

const QMetaObject QDockWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDockWidget,
      qt_meta_data_QDockWidget, 0 }
};

const QMetaObject *QDockWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDockWidget))
	return static_cast<void*>(const_cast< QDockWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: featuresChanged((*reinterpret_cast< QDockWidget::DockWidgetFeatures(*)>(_a[1]))); break;
        case 1: topLevelChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: allowedAreasChanged((*reinterpret_cast< Qt::DockWidgetAreas(*)>(_a[1]))); break;
        case 3: visibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: dockLocationChanged((*reinterpret_cast< Qt::DockWidgetArea(*)>(_a[1]))); break;
        case 5: d_func()->_q_toggleView((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: d_func()->_q_toggleTopLevel(); break;
        }
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isFloating(); break;
        case 1: *reinterpret_cast<int*>(_v) = QFlag(features()); break;
        case 2: *reinterpret_cast< Qt::DockWidgetAreas*>(_v) = allowedAreas(); break;
        case 3: *reinterpret_cast< QString*>(_v) = windowTitle(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFloating(*reinterpret_cast< bool*>(_v)); break;
        case 1: setFeatures(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 2: setAllowedAreas(*reinterpret_cast< Qt::DockWidgetAreas*>(_v)); break;
        case 3: setWindowTitle(*reinterpret_cast< QString*>(_v)); break;
        }
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

// SIGNAL 0
void QDockWidget::featuresChanged(QDockWidget::DockWidgetFeatures _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDockWidget::topLevelChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QDockWidget::allowedAreasChanged(Qt::DockWidgetAreas _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QDockWidget::visibilityChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QDockWidget::dockLocationChanged(Qt::DockWidgetArea _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
