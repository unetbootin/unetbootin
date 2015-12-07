/****************************************************************************
** Meta object code from reading C++ file 'q3progressbar.h'
**
** Created: Thu Mar 5 19:06:00 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/q3progressbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3progressbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3ProgressBar[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       5,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      34,   23,   14,   14, 0x0a,
      62,   53,   14,   14, 0x0a,
      99,   79,   14,   14, 0x0a,

 // properties: name, type, flags
      23,  120, 0x02095103,
      53,  120, 0x02095103,
     132,  124, 0x0a095001,
     152,  147, 0x01095103,
     168,  147, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_Q3ProgressBar[] = {
    "Q3ProgressBar\0\0reset()\0totalSteps\0"
    "setTotalSteps(int)\0progress\0"
    "setProgress(int)\0progress,totalSteps\0"
    "setProgress(int,int)\0int\0QString\0"
    "progressString\0bool\0centerIndicator\0"
    "percentageVisible\0"
};

const QMetaObject Q3ProgressBar::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Q3ProgressBar,
      qt_meta_data_Q3ProgressBar, 0 }
};

const QMetaObject *Q3ProgressBar::metaObject() const
{
    return &staticMetaObject;
}

void *Q3ProgressBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3ProgressBar))
        return static_cast<void*>(const_cast< Q3ProgressBar*>(this));
    return QFrame::qt_metacast(_clname);
}

int Q3ProgressBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reset(); break;
        case 1: setTotalSteps((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = totalSteps(); break;
        case 1: *reinterpret_cast< int*>(_v) = progress(); break;
        case 2: *reinterpret_cast< QString*>(_v) = progressString(); break;
        case 3: *reinterpret_cast< bool*>(_v) = centerIndicator(); break;
        case 4: *reinterpret_cast< bool*>(_v) = percentageVisible(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTotalSteps(*reinterpret_cast< int*>(_v)); break;
        case 1: setProgress(*reinterpret_cast< int*>(_v)); break;
        case 3: setCenterIndicator(*reinterpret_cast< bool*>(_v)); break;
        case 4: setPercentageVisible(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
