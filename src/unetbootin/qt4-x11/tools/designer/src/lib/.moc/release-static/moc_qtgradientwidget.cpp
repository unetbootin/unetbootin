/****************************************************************************
** Meta object code from reading C++ file 'qtgradientwidget.h'
**
** Created: Thu Mar 5 20:46:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradientwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradientwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       1,   52, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      24,   18,   17,   17, 0x05,
      52,   18,   17,   17, 0x05,
      78,   18,   17,   17, 0x05,
     108,   18,   17,   17, 0x05,
     143,  136,   17,   17, 0x05,
     170,   18,   17,   17, 0x05,
     207,  201,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     240,  234,   17,   17, 0x0a,

 // properties: name, type, flags
     278,  273, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientWidget[] = {
    "QtGradientWidget\0\0point\0"
    "startLinearChanged(QPointF)\0"
    "endLinearChanged(QPointF)\0"
    "centralRadialChanged(QPointF)\0"
    "focalRadialChanged(QPointF)\0radius\0"
    "radiusRadialChanged(qreal)\0"
    "centralConicalChanged(QPointF)\0angle\0"
    "angleConicalChanged(qreal)\0stops\0"
    "setGradientStops(QGradientStops)\0bool\0"
    "backgroundCheckered\0"
};

const QMetaObject QtGradientWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtGradientWidget,
      qt_meta_data_QtGradientWidget, 0 }
};

const QMetaObject *QtGradientWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientWidget))
        return static_cast<void*>(const_cast< QtGradientWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtGradientWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startLinearChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 1: endLinearChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 2: centralRadialChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 3: focalRadialChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 4: radiusRadialChanged((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 5: centralConicalChanged((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 6: angleConicalChanged((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 7: setGradientStops((*reinterpret_cast< const QGradientStops(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
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
void QtGradientWidget::startLinearChanged(const QPointF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtGradientWidget::endLinearChanged(const QPointF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtGradientWidget::centralRadialChanged(const QPointF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtGradientWidget::focalRadialChanged(const QPointF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QtGradientWidget::radiusRadialChanged(qreal _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QtGradientWidget::centralConicalChanged(const QPointF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QtGradientWidget::angleConicalChanged(qreal _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
