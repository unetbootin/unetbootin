/****************************************************************************
** Meta object code from reading C++ file 'qlocalserver.h'
**
** Created: Fri May 30 22:19:55 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../socket/qlocalserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlocalserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QLocalServer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   30,   13,   13, 0x08,
      59,   13,   13,   13, 0x08,
      99,   81,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QLocalServer[] = {
    "QLocalServer\0\0newConnection()\0handle\0"
    "_q_openSocket(HANDLE)\0_q_stoppedListening()\0"
    "error,errorString\0"
    "_q_setError(QAbstractSocket::SocketError,QString)\0"
};

const QMetaObject QLocalServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QLocalServer,
      qt_meta_data_QLocalServer, 0 }
};

const QMetaObject *QLocalServer::metaObject() const
{
    return &staticMetaObject;
}

void *QLocalServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QLocalServer))
	return static_cast<void*>(const_cast< QLocalServer*>(this));
    return QObject::qt_metacast(_clname);
}

int QLocalServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newConnection(); break;
        case 1: d_func()->_q_openSocket((*reinterpret_cast< HANDLE(*)>(_a[1]))); break;
        case 2: d_func()->_q_stoppedListening(); break;
        case 3: d_func()->_q_setError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QLocalServer::newConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
