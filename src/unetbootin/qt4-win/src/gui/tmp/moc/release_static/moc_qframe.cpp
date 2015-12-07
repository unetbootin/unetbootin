/****************************************************************************
** Meta object code from reading C++ file 'qframe.h'
**
** Created: Fri May 30 22:33:42 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qframe.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qframe.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFrame[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       6,   10, // properties
       2,   28, // enums/sets

 // properties: name, type, flags
      13,    7, 0x0009510b,
      31,   24, 0x0009510b,
      47,   43, 0x02095103,
      57,   43, 0x02095103,
      70,   43, 0x02095001,
      87,   81, 0x13094103,

 // enums: name, flags, count, data
       7, 0x0,    7,   36,
      24, 0x0,    3,   50,

 // enum data: key, value
      97, uint(QFrame::NoFrame),
     105, uint(QFrame::Box),
     109, uint(QFrame::Panel),
     115, uint(QFrame::WinPanel),
     124, uint(QFrame::HLine),
     130, uint(QFrame::VLine),
     136, uint(QFrame::StyledPanel),
     148, uint(QFrame::Plain),
     154, uint(QFrame::Raised),
     161, uint(QFrame::Sunken),

       0        // eod
};

static const char qt_meta_stringdata_QFrame[] = {
    "QFrame\0Shape\0frameShape\0Shadow\0"
    "frameShadow\0int\0lineWidth\0midLineWidth\0"
    "frameWidth\0QRect\0frameRect\0NoFrame\0"
    "Box\0Panel\0WinPanel\0HLine\0VLine\0"
    "StyledPanel\0Plain\0Raised\0Sunken\0"
};

const QMetaObject QFrame::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QFrame,
      qt_meta_data_QFrame, 0 }
};

const QMetaObject *QFrame::metaObject() const
{
    return &staticMetaObject;
}

void *QFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFrame))
	return static_cast<void*>(const_cast< QFrame*>(this));
    return QWidget::qt_metacast(_clname);
}

int QFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Shape*>(_v) = frameShape(); break;
        case 1: *reinterpret_cast< Shadow*>(_v) = frameShadow(); break;
        case 2: *reinterpret_cast< int*>(_v) = lineWidth(); break;
        case 3: *reinterpret_cast< int*>(_v) = midLineWidth(); break;
        case 4: *reinterpret_cast< int*>(_v) = frameWidth(); break;
        case 5: *reinterpret_cast< QRect*>(_v) = frameRect(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFrameShape(*reinterpret_cast< Shape*>(_v)); break;
        case 1: setFrameShadow(*reinterpret_cast< Shadow*>(_v)); break;
        case 2: setLineWidth(*reinterpret_cast< int*>(_v)); break;
        case 3: setMidLineWidth(*reinterpret_cast< int*>(_v)); break;
        case 5: setFrameRect(*reinterpret_cast< QRect*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
