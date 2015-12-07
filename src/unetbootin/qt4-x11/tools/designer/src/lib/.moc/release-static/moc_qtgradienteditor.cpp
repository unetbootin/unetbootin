/****************************************************************************
** Meta object code from reading C++ file 'qtgradienteditor.h'
**
** Created: Thu Mar 5 20:43:04 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradienteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradienteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   12, // methods
       4,  137, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      27,   18,   17,   17, 0x05,
      82,   54,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     117,  111,   17,   17, 0x08,
     163,  158,   17,   17, 0x08,
     184,  158,   17,   17, 0x08,
     213,  207,   17,   17, 0x08,
     245,  207,   17,   17, 0x08,
     277,  207,   17,   17, 0x08,
     307,  207,   17,   17, 0x08,
     337,  207,   17,   17, 0x08,
     371,  207,   17,   17, 0x08,
     405,  207,   17,   17, 0x08,
     437,  207,   17,   17, 0x08,
     469,  207,   17,   17, 0x08,
     501,  207,   17,   17, 0x08,
     536,  207,   17,   17, 0x08,
     571,  207,   17,   17, 0x08,
     611,  603,   17,   17, 0x08,
     636,   17,   17,   17, 0x08,
     664,   17,   17,   17, 0x08,
     690,   17,   17,   17, 0x08,
     720,   17,   17,   17, 0x08,
     748,   17,   17,   17, 0x08,
     775,   17,   17,   17, 0x08,
     806,   17,   17,   17, 0x08,

 // properties: name, type, flags
      18,  833, 0x0009510b,
     848,  843, 0x01095103,
     868,  843, 0x01095103,
     883,  843, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientEditor[] = {
    "QtGradientEditor\0\0gradient\0"
    "gradientChanged(QGradient)\0"
    "details,extenstionWidthHint\0"
    "aboutToShowDetails(bool,int)\0stops\0"
    "slotGradientStopsChanged(QGradientStops)\0"
    "type\0slotTypeChanged(int)\0"
    "slotSpreadChanged(int)\0value\0"
    "slotStartLinearXChanged(double)\0"
    "slotStartLinearYChanged(double)\0"
    "slotEndLinearXChanged(double)\0"
    "slotEndLinearYChanged(double)\0"
    "slotCentralRadialXChanged(double)\0"
    "slotCentralRadialYChanged(double)\0"
    "slotFocalRadialXChanged(double)\0"
    "slotFocalRadialYChanged(double)\0"
    "slotRadiusRadialChanged(double)\0"
    "slotCentralConicalXChanged(double)\0"
    "slotCentralConicalYChanged(double)\0"
    "slotAngleConicalChanged(double)\0details\0"
    "slotDetailsChanged(bool)\0"
    "startLinearChanged(QPointF)\0"
    "endLinearChanged(QPointF)\0"
    "centralRadialChanged(QPointF)\0"
    "focalRadialChanged(QPointF)\0"
    "radiusRadialChanged(qreal)\0"
    "centralConicalChanged(QPointF)\0"
    "angleConicalChanged(qreal)\0QGradient\0"
    "bool\0backgroundCheckered\0detailsVisible\0"
    "detailsButtonVisible\0"
};

const QMetaObject QtGradientEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtGradientEditor,
      qt_meta_data_QtGradientEditor, 0 }
};

const QMetaObject *QtGradientEditor::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientEditor))
        return static_cast<void*>(const_cast< QtGradientEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtGradientEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: gradientChanged((*reinterpret_cast< const QGradient(*)>(_a[1]))); break;
        case 1: aboutToShowDetails((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: d_func()->slotGradientStopsChanged((*reinterpret_cast< const QGradientStops(*)>(_a[1]))); break;
        case 3: d_func()->slotTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->slotSpreadChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: d_func()->slotStartLinearXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: d_func()->slotStartLinearYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: d_func()->slotEndLinearXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: d_func()->slotEndLinearYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: d_func()->slotCentralRadialXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: d_func()->slotCentralRadialYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: d_func()->slotFocalRadialXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: d_func()->slotFocalRadialYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: d_func()->slotRadiusRadialChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: d_func()->slotCentralConicalXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: d_func()->slotCentralConicalYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: d_func()->slotAngleConicalChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: d_func()->slotDetailsChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: d_func()->startLinearChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 19: d_func()->endLinearChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 20: d_func()->centralRadialChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 21: d_func()->focalRadialChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 22: d_func()->radiusRadialChanged((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 23: d_func()->centralConicalChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 24: d_func()->angleConicalChanged((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 25;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QGradient*>(_v) = gradient(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isBackgroundCheckered(); break;
        case 2: *reinterpret_cast< bool*>(_v) = detailsVisible(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isDetailsButtonVisible(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setGradient(*reinterpret_cast< QGradient*>(_v)); break;
        case 1: setBackgroundCheckered(*reinterpret_cast< bool*>(_v)); break;
        case 2: setDetailsVisible(*reinterpret_cast< bool*>(_v)); break;
        case 3: setDetailsButtonVisible(*reinterpret_cast< bool*>(_v)); break;
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
void QtGradientEditor::gradientChanged(const QGradient & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtGradientEditor::aboutToShowDetails(bool _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
