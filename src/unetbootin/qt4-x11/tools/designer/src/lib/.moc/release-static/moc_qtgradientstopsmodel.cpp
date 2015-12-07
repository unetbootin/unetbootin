/****************************************************************************
** Meta object code from reading C++ file 'qtgradientstopsmodel.h'
**
** Created: Thu Mar 5 20:46:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradientstopsmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradientstopsmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientStopsModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      27,   22,   21,   21, 0x05,
      54,   22,   21,   21, 0x05,
      95,   83,   21,   21, 0x05,
     140,  128,   21,   21, 0x05,
     200,  186,   21,   21, 0x05,
     250,  236,   21,   21, 0x05,
     285,   22,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientStopsModel[] = {
    "QtGradientStopsModel\0\0stop\0"
    "stopAdded(QtGradientStop*)\0"
    "stopRemoved(QtGradientStop*)\0stop,newPos\0"
    "stopMoved(QtGradientStop*,qreal)\0"
    "stop1,stop2\0stopsSwapped(QtGradientStop*,QtGradientStop*)\0"
    "stop,newColor\0stopChanged(QtGradientStop*,QColor)\0"
    "stop,selected\0stopSelected(QtGradientStop*,bool)\0"
    "currentStopChanged(QtGradientStop*)\0"
};

const QMetaObject QtGradientStopsModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtGradientStopsModel,
      qt_meta_data_QtGradientStopsModel, 0 }
};

const QMetaObject *QtGradientStopsModel::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientStopsModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientStopsModel))
        return static_cast<void*>(const_cast< QtGradientStopsModel*>(this));
    return QObject::qt_metacast(_clname);
}

int QtGradientStopsModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stopAdded((*reinterpret_cast< QtGradientStop*(*)>(_a[1]))); break;
        case 1: stopRemoved((*reinterpret_cast< QtGradientStop*(*)>(_a[1]))); break;
        case 2: stopMoved((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 3: stopsSwapped((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< QtGradientStop*(*)>(_a[2]))); break;
        case 4: stopChanged((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 5: stopSelected((*reinterpret_cast< QtGradientStop*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: currentStopChanged((*reinterpret_cast< QtGradientStop*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QtGradientStopsModel::stopAdded(QtGradientStop * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtGradientStopsModel::stopRemoved(QtGradientStop * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtGradientStopsModel::stopMoved(QtGradientStop * _t1, qreal _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtGradientStopsModel::stopsSwapped(QtGradientStop * _t1, QtGradientStop * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QtGradientStopsModel::stopChanged(QtGradientStop * _t1, const QColor & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QtGradientStopsModel::stopSelected(QtGradientStop * _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QtGradientStopsModel::currentStopChanged(QtGradientStop * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
