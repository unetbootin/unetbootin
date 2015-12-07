/****************************************************************************
** Meta object code from reading C++ file 'q3groupbox.h'
**
** Created: Fri May 30 22:41:24 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/q3groupbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3groupbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3GroupBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       8,   10, // properties
       1,   34, // enums/sets

 // properties: name, type, flags
      27,   11, 0x0009410b,
      43,   39, 0x02094103,
      57,   51, 0x13094103,
      78,   67, 0x0009510b,
      89,   67, 0x0009510b,
     101,   39, 0x02095103,
     111,   39, 0x02095103,
     124,   39, 0x02095103,

 // enums: name, flags, count, data
      67, 0x0,   18,   38,

 // enum data: key, value
     131, uint(Q3GroupBox::Box),
     135, uint(Q3GroupBox::Sunken),
     142, uint(Q3GroupBox::Plain),
     148, uint(Q3GroupBox::Raised),
     155, uint(Q3GroupBox::MShadow),
     163, uint(Q3GroupBox::NoFrame),
     171, uint(Q3GroupBox::Panel),
     177, uint(Q3GroupBox::StyledPanel),
     189, uint(Q3GroupBox::HLine),
     195, uint(Q3GroupBox::VLine),
     201, uint(Q3GroupBox::WinPanel),
     210, uint(Q3GroupBox::ToolBarPanel),
     223, uint(Q3GroupBox::MenuBarPanel),
     236, uint(Q3GroupBox::PopupPanel),
     247, uint(Q3GroupBox::LineEditPanel),
     261, uint(Q3GroupBox::TabWidgetPanel),
     276, uint(Q3GroupBox::GroupBoxPanel),
     290, uint(Q3GroupBox::MShape),

       0        // eod
};

static const char qt_meta_stringdata_Q3GroupBox[] = {
    "Q3GroupBox\0Qt::Orientation\0orientation\0"
    "int\0columns\0QRect\0frameRect\0FrameShape\0"
    "frameShape\0frameShadow\0lineWidth\0"
    "midLineWidth\0margin\0Box\0Sunken\0Plain\0"
    "Raised\0MShadow\0NoFrame\0Panel\0StyledPanel\0"
    "HLine\0VLine\0WinPanel\0ToolBarPanel\0"
    "MenuBarPanel\0PopupPanel\0LineEditPanel\0"
    "TabWidgetPanel\0GroupBoxPanel\0MShape\0"
};

const QMetaObject Q3GroupBox::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_Q3GroupBox,
      qt_meta_data_Q3GroupBox, 0 }
};

const QMetaObject *Q3GroupBox::metaObject() const
{
    return &staticMetaObject;
}

void *Q3GroupBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3GroupBox))
	return static_cast<void*>(const_cast< Q3GroupBox*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int Q3GroupBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 1: *reinterpret_cast< int*>(_v) = columns(); break;
        case 2: *reinterpret_cast< QRect*>(_v) = frameRect(); break;
        case 3: *reinterpret_cast< FrameShape*>(_v) = frameShape(); break;
        case 4: *reinterpret_cast< FrameShape*>(_v) = frameShadow(); break;
        case 5: *reinterpret_cast< int*>(_v) = lineWidth(); break;
        case 6: *reinterpret_cast< int*>(_v) = midLineWidth(); break;
        case 7: *reinterpret_cast< int*>(_v) = margin(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 1: setColumns(*reinterpret_cast< int*>(_v)); break;
        case 2: setFrameRect(*reinterpret_cast< QRect*>(_v)); break;
        case 3: setFrameShape(*reinterpret_cast< FrameShape*>(_v)); break;
        case 4: setFrameShadow(*reinterpret_cast< FrameShape*>(_v)); break;
        case 5: setLineWidth(*reinterpret_cast< int*>(_v)); break;
        case 6: setMidLineWidth(*reinterpret_cast< int*>(_v)); break;
        case 7: setMargin(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
