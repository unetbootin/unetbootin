/****************************************************************************
** Meta object code from reading C++ file 'qabstractsocketengine_p.h'
**
** Created: Fri May 30 22:20:07 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../socket/qabstractsocketengine_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractsocketengine_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractSocketEngine[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,
      42,   22,   22,   22, 0x0a,
      62,   22,   22,   22, 0x0a,
     106,   86,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractSocketEngine[] = {
    "QAbstractSocketEngine\0\0readNotification()\0"
    "writeNotification()\0exceptionNotification()\0"
    "proxy,authenticator\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
};

const QMetaObject QAbstractSocketEngine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAbstractSocketEngine,
      qt_meta_data_QAbstractSocketEngine, 0 }
};

const QMetaObject *QAbstractSocketEngine::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractSocketEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractSocketEngine))
	return static_cast<void*>(const_cast< QAbstractSocketEngine*>(this));
    return QObject::qt_metacast(_clname);
}

int QAbstractSocketEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readNotification(); break;
        case 1: writeNotification(); break;
        case 2: exceptionNotification(); break;
        case 3: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
