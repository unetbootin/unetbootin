/****************************************************************************
** Meta object code from reading C++ file 'qabstractslider.h'
**
** Created: Fri May 30 22:33:33 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qabstractslider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractslider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractSlider[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
      11,   50, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      23,   17,   16,   16, 0x05,
      41,   16,   16,   16, 0x05,
      66,   57,   16,   16, 0x05,
      83,   16,   16,   16, 0x05,
     108,  100,   16,   16, 0x05,
     137,  130,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     158,   16,   16,   16, 0x0a,
     172,   16,   16,   16, 0x0a,

 // properties: name, type, flags
     208,  204, 0x02095103,
     216,  204, 0x02095103,
     224,  204, 0x02095103,
     235,  204, 0x02095103,
      17,  204, 0x02195103,
     244,  204, 0x02095103,
     264,  259, 0x01095103,
     289,  273, 0x0009510b,
     301,  259, 0x01095103,
     320,  259, 0x01095103,
     337,  259, 0x01094103,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractSlider[] = {
    "QAbstractSlider\0\0value\0valueChanged(int)\0"
    "sliderPressed()\0position\0sliderMoved(int)\0"
    "sliderReleased()\0min,max\0rangeChanged(int,int)\0"
    "action\0actionTriggered(int)\0setValue(int)\0"
    "setOrientation(Qt::Orientation)\0int\0"
    "minimum\0maximum\0singleStep\0pageStep\0"
    "sliderPosition\0bool\0tracking\0"
    "Qt::Orientation\0orientation\0"
    "invertedAppearance\0invertedControls\0"
    "sliderDown\0"
};

const QMetaObject QAbstractSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QAbstractSlider,
      qt_meta_data_QAbstractSlider, 0 }
};

const QMetaObject *QAbstractSlider::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractSlider))
	return static_cast<void*>(const_cast< QAbstractSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int QAbstractSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: sliderPressed(); break;
        case 2: sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: sliderReleased(); break;
        case 4: rangeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: actionTriggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setOrientation((*reinterpret_cast< Qt::Orientation(*)>(_a[1]))); break;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = minimum(); break;
        case 1: *reinterpret_cast< int*>(_v) = maximum(); break;
        case 2: *reinterpret_cast< int*>(_v) = singleStep(); break;
        case 3: *reinterpret_cast< int*>(_v) = pageStep(); break;
        case 4: *reinterpret_cast< int*>(_v) = value(); break;
        case 5: *reinterpret_cast< int*>(_v) = sliderPosition(); break;
        case 6: *reinterpret_cast< bool*>(_v) = hasTracking(); break;
        case 7: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 8: *reinterpret_cast< bool*>(_v) = invertedAppearance(); break;
        case 9: *reinterpret_cast< bool*>(_v) = invertedControls(); break;
        case 10: *reinterpret_cast< bool*>(_v) = isSliderDown(); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMinimum(*reinterpret_cast< int*>(_v)); break;
        case 1: setMaximum(*reinterpret_cast< int*>(_v)); break;
        case 2: setSingleStep(*reinterpret_cast< int*>(_v)); break;
        case 3: setPageStep(*reinterpret_cast< int*>(_v)); break;
        case 4: setValue(*reinterpret_cast< int*>(_v)); break;
        case 5: setSliderPosition(*reinterpret_cast< int*>(_v)); break;
        case 6: setTracking(*reinterpret_cast< bool*>(_v)); break;
        case 7: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 8: setInvertedAppearance(*reinterpret_cast< bool*>(_v)); break;
        case 9: setInvertedControls(*reinterpret_cast< bool*>(_v)); break;
        case 10: setSliderDown(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QAbstractSlider::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QAbstractSlider::sliderPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QAbstractSlider::sliderMoved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QAbstractSlider::sliderReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void QAbstractSlider::rangeChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QAbstractSlider::actionTriggered(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
