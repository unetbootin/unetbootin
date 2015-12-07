/****************************************************************************
** Meta object code from reading C++ file 'qtgradientstopswidget.h'
**
** Created: Thu Mar 5 20:42:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradientstopswidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradientstopswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientStopsWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   12, // methods
       1,   87, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      28,   23,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   48,   22,   22, 0x08,
      84,   48,   22,   22, 0x08,
     129,  117,   22,   22, 0x08,
     178,  166,   22,   22, 0x08,
     242,  228,   22,   22, 0x08,
     296,  282,   22,   22, 0x08,
     335,   48,   22,   22, 0x08,
     375,   22,   22,   22, 0x08,
     389,   22,   22,   22, 0x08,
     402,   22,   22,   22, 0x08,
     416,   22,   22,   22, 0x08,
     432,   22,   22,   22, 0x08,
     445,   22,   22,   22, 0x08,
     459,   22,   22,   22, 0x08,

 // properties: name, type, flags
     480,  475, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientStopsWidget[] = {
    "QtGradientStopsWidget\0\0zoom\0"
    "zoomChanged(double)\0stop\0"
    "slotStopAdded(QtGradientStop*)\0"
    "slotStopRemoved(QtGradientStop*)\0"
    "stop,newPos\0slotStopMoved(QtGradientStop*,qreal)\0"
    "stop1,stop2\0"
    "slotStopsSwapped(QtGradientStop*,QtGradientStop*)\0"
    "stop,newColor\0slotStopChanged(QtGradientStop*,QColor)\0"
    "stop,selected\0slotStopSelected(QtGradientStop*,bool)\0"
    "slotCurrentStopChanged(QtGradientStop*)\0"
    "slotNewStop()\0slotDelete()\0slotFlipAll()\0"
    "slotSelectAll()\0slotZoomIn()\0slotZoomOut()\0"
    "slotResetZoom()\0bool\0backgroundCheckered\0"
};

const QMetaObject QtGradientStopsWidget::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QtGradientStopsWidget,
      qt_meta_data_QtGradientStopsWidget, 0 }
};

const QMetaObject *QtGradientStopsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientStopsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientStopsWidget))
        return static_cast<void*>(const_cast< QtGradientStopsWidget*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QtGradientStopsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: zoomChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: d_func()->slotStopAdded((*reinterpret_cast< QtGradientStop*(*)>(_a[1]))); break;
        case 2: d_func()->slotStopRemoved((*reinterpret_cast< QtGradientStop*(*)>(_a[1]))); break;
        case 3: d_func()->slotStopMoved((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 4: d_func()->slotStopsSwapped((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< QtGradientStop*(*)>(_a[2]))); break;
        case 5: d_func()->slotStopChanged((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 6: d_func()->slotStopSelected((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: d_func()->slotCurrentStopChanged((*reinterpret_cast< QtGradientStop*(*)>(_a[1]))); break;
        case 8: d_func()->slotNewStop(); break;
        case 9: d_func()->slotDelete(); break;
        case 10: d_func()->slotFlipAll(); break;
        case 11: d_func()->slotSelectAll(); break;
        case 12: d_func()->slotZoomIn(); break;
        case 13: d_func()->slotZoomOut(); break;
        case 14: d_func()->slotResetZoom(); break;
        default: ;
        }
        _id -= 15;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isBackgroundCheckered(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBackgroundCheckered(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QtGradientStopsWidget::zoomChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
