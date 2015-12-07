/****************************************************************************
** Meta object code from reading C++ file 'qlcdnumber.h'
**
** Created: Fri May 30 22:33:43 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qlcdnumber.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlcdnumber.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QLCDNumber[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   10, // methods
       6,   55, // properties
       2,   73, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,   23,   11,   11, 0x0a,
      48,   44,   11,   11, 0x0a,
      61,   44,   11,   11, 0x0a,
      77,   11,   11,   11, 0x0a,
      90,   11,   11,   11, 0x0a,
     103,   11,   11,   11, 0x0a,
     116,   11,   11,   11, 0x0a,
     129,   11,   11,   11, 0x0a,

 // properties: name, type, flags
     161,  156, 0x01095103,
     183,  179, 0x02095103,
     198,  193, 0x0009510b,
     216,  203, 0x0009510b,
     236,  229, 0x06095003,
     242,  179, 0x02095003,

 // enums: name, flags, count, data
     193, 0x0,    4,   81,
     203, 0x0,    3,   89,

 // enum data: key, value
     251, uint(QLCDNumber::Hex),
     255, uint(QLCDNumber::Dec),
     259, uint(QLCDNumber::Oct),
     263, uint(QLCDNumber::Bin),
     267, uint(QLCDNumber::Outline),
     275, uint(QLCDNumber::Filled),
     282, uint(QLCDNumber::Flat),

       0        // eod
};

static const char qt_meta_stringdata_QLCDNumber[] = {
    "QLCDNumber\0\0overflow()\0str\0display(QString)\0"
    "num\0display(int)\0display(double)\0"
    "setHexMode()\0setDecMode()\0setOctMode()\0"
    "setBinMode()\0setSmallDecimalPoint(bool)\0"
    "bool\0smallDecimalPoint\0int\0numDigits\0"
    "Mode\0mode\0SegmentStyle\0segmentStyle\0"
    "double\0value\0intValue\0Hex\0Dec\0Oct\0Bin\0"
    "Outline\0Filled\0Flat\0"
};

const QMetaObject QLCDNumber::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QLCDNumber,
      qt_meta_data_QLCDNumber, 0 }
};

const QMetaObject *QLCDNumber::metaObject() const
{
    return &staticMetaObject;
}

void *QLCDNumber::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QLCDNumber))
	return static_cast<void*>(const_cast< QLCDNumber*>(this));
    return QFrame::qt_metacast(_clname);
}

int QLCDNumber::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: overflow(); break;
        case 1: display((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: display((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: display((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: setHexMode(); break;
        case 5: setDecMode(); break;
        case 6: setOctMode(); break;
        case 7: setBinMode(); break;
        case 8: setSmallDecimalPoint((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = smallDecimalPoint(); break;
        case 1: *reinterpret_cast< int*>(_v) = numDigits(); break;
        case 2: *reinterpret_cast< Mode*>(_v) = mode(); break;
        case 3: *reinterpret_cast< SegmentStyle*>(_v) = segmentStyle(); break;
        case 4: *reinterpret_cast< double*>(_v) = value(); break;
        case 5: *reinterpret_cast< int*>(_v) = intValue(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSmallDecimalPoint(*reinterpret_cast< bool*>(_v)); break;
        case 1: setNumDigits(*reinterpret_cast< int*>(_v)); break;
        case 2: setMode(*reinterpret_cast< Mode*>(_v)); break;
        case 3: setSegmentStyle(*reinterpret_cast< SegmentStyle*>(_v)); break;
        case 4: display(*reinterpret_cast< double*>(_v)); break;
        case 5: display(*reinterpret_cast< int*>(_v)); break;
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

// SIGNAL 0
void QLCDNumber::overflow()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
