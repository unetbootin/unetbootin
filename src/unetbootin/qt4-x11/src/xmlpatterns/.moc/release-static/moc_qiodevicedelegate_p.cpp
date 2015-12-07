/****************************************************************************
** Meta object code from reading C++ file 'qiodevicedelegate_p.h'
**
** Created: Thu Mar 5 19:20:31 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../api/qiodevicedelegate_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qiodevicedelegate_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPatternist__QIODeviceDelegate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      32,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QPatternist__QIODeviceDelegate[] = {
    "QPatternist::QIODeviceDelegate\0\0"
    "networkTimeout()\0"
};

const QMetaObject QPatternist::QIODeviceDelegate::staticMetaObject = {
    { &QNetworkReply::staticMetaObject, qt_meta_stringdata_QPatternist__QIODeviceDelegate,
      qt_meta_data_QPatternist__QIODeviceDelegate, 0 }
};

const QMetaObject *QPatternist::QIODeviceDelegate::metaObject() const
{
    return &staticMetaObject;
}

void *QPatternist::QIODeviceDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPatternist__QIODeviceDelegate))
        return static_cast<void*>(const_cast< QIODeviceDelegate*>(this));
    return QNetworkReply::qt_metacast(_clname);
}

int QPatternist::QIODeviceDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QNetworkReply::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: networkTimeout(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
