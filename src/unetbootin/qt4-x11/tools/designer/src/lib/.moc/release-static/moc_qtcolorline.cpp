/****************************************************************************
** Meta object code from reading C++ file 'qtcolorline.h'
**
** Created: Thu Mar 5 20:46:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtcolorline.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtcolorline.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtColorLine[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       7,   22, // properties
       1,   43, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   13,   12,   12, 0x0a,

 // properties: name, type, flags
      13,   57, 0x43095103,
      68,   64, 0x02095103,
      83,   64, 0x02095103,
     102,   97, 0x01095103,
     107,   97, 0x01095103,
     142,  127, 0x0009510b,
     173,  157, 0x0009510b,

 // enums: name, flags, count, data
     127, 0x0,    7,   47,

 // enum data: key, value
     185, uint(QtColorLine::Red),
     189, uint(QtColorLine::Green),
     195, uint(QtColorLine::Blue),
     200, uint(QtColorLine::Hue),
     204, uint(QtColorLine::Saturation),
     215, uint(QtColorLine::Value),
     221, uint(QtColorLine::Alpha),

       0        // eod
};

static const char qt_meta_stringdata_QtColorLine[] = {
    "QtColorLine\0\0color\0colorChanged(QColor)\0"
    "setColor(QColor)\0QColor\0int\0indicatorSpace\0"
    "indicatorSize\0bool\0flip\0backgroundCheckered\0"
    "ColorComponent\0colorComponent\0"
    "Qt::Orientation\0orientation\0Red\0Green\0"
    "Blue\0Hue\0Saturation\0Value\0Alpha\0"
};

const QMetaObject QtColorLine::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtColorLine,
      qt_meta_data_QtColorLine, 0 }
};

const QMetaObject *QtColorLine::metaObject() const
{
    return &staticMetaObject;
}

void *QtColorLine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtColorLine))
        return static_cast<void*>(const_cast< QtColorLine*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtColorLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: setColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = color(); break;
        case 1: *reinterpret_cast< int*>(_v) = indicatorSpace(); break;
        case 2: *reinterpret_cast< int*>(_v) = indicatorSize(); break;
        case 3: *reinterpret_cast< bool*>(_v) = flip(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isBackgroundCheckered(); break;
        case 5: *reinterpret_cast< ColorComponent*>(_v) = colorComponent(); break;
        case 6: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: setIndicatorSpace(*reinterpret_cast< int*>(_v)); break;
        case 2: setIndicatorSize(*reinterpret_cast< int*>(_v)); break;
        case 3: setFlip(*reinterpret_cast< bool*>(_v)); break;
        case 4: setBackgroundCheckered(*reinterpret_cast< bool*>(_v)); break;
        case 5: setColorComponent(*reinterpret_cast< ColorComponent*>(_v)); break;
        case 6: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QtColorLine::colorChanged(const QColor & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
