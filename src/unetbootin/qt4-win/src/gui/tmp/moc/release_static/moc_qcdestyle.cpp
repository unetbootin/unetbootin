/****************************************************************************
** Meta object code from reading C++ file 'qcdestyle.h'
**
** Created: Fri May 30 22:33:27 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../styles/qcdestyle.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcdestyle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCDEStyle[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      41,   17,   11,   10, 0x09,
     136,  119,   11,   10, 0x29,
     212,  199,   11,   10, 0x29,

       0        // eod
};

static const char qt_meta_stringdata_QCDEStyle[] = {
    "QCDEStyle\0\0QIcon\0standardIcon,opt,widget\0"
    "standardIconImplementation(StandardPixmap,const QStyleOption*,const QW"
    "idget*)\0"
    "standardIcon,opt\0"
    "standardIconImplementation(StandardPixmap,const QStyleOption*)\0"
    "standardIcon\0standardIconImplementation(StandardPixmap)\0"
};

const QMetaObject QCDEStyle::staticMetaObject = {
    { &QMotifStyle::staticMetaObject, qt_meta_stringdata_QCDEStyle,
      qt_meta_data_QCDEStyle, 0 }
};

const QMetaObject *QCDEStyle::metaObject() const
{
    return &staticMetaObject;
}

void *QCDEStyle::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCDEStyle))
	return static_cast<void*>(const_cast< QCDEStyle*>(this));
    return QMotifStyle::qt_metacast(_clname);
}

int QCDEStyle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMotifStyle::qt_metacall(_c, _id, _a);
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
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
