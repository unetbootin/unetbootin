/****************************************************************************
** Meta object code from reading C++ file 'qstylesheetstyle_p.h'
**
** Created: Thu Mar 5 18:49:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../styles/qstylesheetstyle_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qstylesheetstyle_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QStyleSheetStyle[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      48,   24,   18,   17, 0x09,
     143,  126,   18,   17, 0x29,
     219,  206,   18,   17, 0x29,
     310,  266,  262,   17, 0x09,
     477,  440,  262,   17, 0x29,
     622,  592,  262,   17, 0x29,
     717,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QStyleSheetStyle[] = {
    "QStyleSheetStyle\0\0QIcon\0standardIcon,opt,widget\0"
    "standardIconImplementation(StandardPixmap,const QStyleOption*,const QW"
    "idget*)\0"
    "standardIcon,opt\0"
    "standardIconImplementation(StandardPixmap,const QStyleOption*)\0"
    "standardIcon\0standardIconImplementation(StandardPixmap)\0"
    "int\0control1,control2,orientation,option,widget\0"
    "layoutSpacingImplementation(QSizePolicy::ControlType,QSizePolicy::Cont"
    "rolType,Qt::Orientation,const QStyleOption*,const QWidget*)\0"
    "control1,control2,orientation,option\0"
    "layoutSpacingImplementation(QSizePolicy::ControlType,QSizePolicy::Cont"
    "rolType,Qt::Orientation,const QStyleOption*)\0"
    "control1,control2,orientation\0"
    "layoutSpacingImplementation(QSizePolicy::ControlType,QSizePolicy::Cont"
    "rolType,Qt::Orientation)\0"
    "widgetDestroyed(QObject*)\0"
};

const QMetaObject QStyleSheetStyle::staticMetaObject = {
    { &QWindowsStyle::staticMetaObject, qt_meta_stringdata_QStyleSheetStyle,
      qt_meta_data_QStyleSheetStyle, 0 }
};

const QMetaObject *QStyleSheetStyle::metaObject() const
{
    return &staticMetaObject;
}

void *QStyleSheetStyle::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStyleSheetStyle))
        return static_cast<void*>(const_cast< QStyleSheetStyle*>(this));
    return QWindowsStyle::qt_metacast(_clname);
}

int QStyleSheetStyle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWindowsStyle::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QIcon _r = standardIconImplementation((*reinterpret_cast< StandardPixmap(*)>(_a[1])),(*reinterpret_cast< const QStyleOption*(*)>(_a[2])),(*reinterpret_cast< const QWidget*(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QIcon*>(_a[0]) = _r; }  break;
        case 1: { QIcon _r = standardIconImplementation((*reinterpret_cast< StandardPixmap(*)>(_a[1])),(*reinterpret_cast< const QStyleOption*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QIcon*>(_a[0]) = _r; }  break;
        case 2: { QIcon _r = standardIconImplementation((*reinterpret_cast< StandardPixmap(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QIcon*>(_a[0]) = _r; }  break;
        case 3: { int _r = layoutSpacingImplementation((*reinterpret_cast< QSizePolicy::ControlType(*)>(_a[1])),(*reinterpret_cast< QSizePolicy::ControlType(*)>(_a[2])),(*reinterpret_cast< Qt::Orientation(*)>(_a[3])),(*reinterpret_cast< const QStyleOption*(*)>(_a[4])),(*reinterpret_cast< const QWidget*(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: { int _r = layoutSpacingImplementation((*reinterpret_cast< QSizePolicy::ControlType(*)>(_a[1])),(*reinterpret_cast< QSizePolicy::ControlType(*)>(_a[2])),(*reinterpret_cast< Qt::Orientation(*)>(_a[3])),(*reinterpret_cast< const QStyleOption*(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = layoutSpacingImplementation((*reinterpret_cast< QSizePolicy::ControlType(*)>(_a[1])),(*reinterpret_cast< QSizePolicy::ControlType(*)>(_a[2])),(*reinterpret_cast< Qt::Orientation(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: widgetDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
