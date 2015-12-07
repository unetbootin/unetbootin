/****************************************************************************
** Meta object code from reading C++ file 'qacceltreeresourceloader_p.h'
**
** Created: Thu Mar 5 19:20:30 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../acceltree/qacceltreeresourceloader_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qacceltreeresourceloader_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPatternist__NetworkLoop[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      31,   26,   25,   25, 0x0a,
      66,   25,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QPatternist__NetworkLoop[] = {
    "QPatternist::NetworkLoop\0\0code\0"
    "error(QNetworkReply::NetworkError)\0"
    "finished()\0"
};

const QMetaObject QPatternist::NetworkLoop::staticMetaObject = {
    { &QEventLoop::staticMetaObject, qt_meta_stringdata_QPatternist__NetworkLoop,
      qt_meta_data_QPatternist__NetworkLoop, 0 }
};

const QMetaObject *QPatternist::NetworkLoop::metaObject() const
{
    return &staticMetaObject;
}

void *QPatternist::NetworkLoop::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPatternist__NetworkLoop))
        return static_cast<void*>(const_cast< NetworkLoop*>(this));
    return QEventLoop::qt_metacast(_clname);
}

int QPatternist::NetworkLoop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QEventLoop::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 1: finished(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
