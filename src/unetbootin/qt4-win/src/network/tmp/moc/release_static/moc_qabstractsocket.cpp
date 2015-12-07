/****************************************************************************
** Meta object code from reading C++ file 'qabstractsocket.h'
**
** Created: Fri May 30 22:19:48 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../socket/qabstractsocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractsocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractSocket[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      29,   16,   16,   16, 0x05,
      41,   16,   16,   16, 0x05,
      56,   16,   16,   16, 0x05,
      99,   16,   16,   16, 0x05,
     155,  135,   16,   16, 0x05,
     214,   16,   16,   16, 0x15,
     233,   16,   16,   16, 0x15,

 // slots: signature, parameters, type, tag, flags
     275,  256,   16,   16, 0x09,
     343,  329,   16,   16, 0x29,
     388,   16,   16,   16, 0x09,
     423,   16,   16,   16, 0x08,
     449,   16,   16,   16, 0x08,
     479,   16,   16,   16, 0x08,
     507,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractSocket[] = {
    "QAbstractSocket\0\0hostFound()\0connected()\0"
    "disconnected()\0"
    "stateChanged(QAbstractSocket::SocketState)\0"
    "error(QAbstractSocket::SocketError)\0"
    "proxy,authenticator\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
    "connectionClosed()\0delayedCloseFinished()\0"
    "hostName,port,mode\0"
    "connectToHostImplementation(QString,quint16,OpenMode)\0"
    "hostName,port\0"
    "connectToHostImplementation(QString,quint16)\0"
    "disconnectFromHostImplementation()\0"
    "_q_connectToNextAddress()\0"
    "_q_startConnecting(QHostInfo)\0"
    "_q_abortConnectionAttempt()\0"
    "_q_testConnection()\0"
};

const QMetaObject QAbstractSocket::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_QAbstractSocket,
      qt_meta_data_QAbstractSocket, 0 }
};

const QMetaObject *QAbstractSocket::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractSocket))
	return static_cast<void*>(const_cast< QAbstractSocket*>(this));
    return QIODevice::qt_metacast(_clname);
}

int QAbstractSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: hostFound(); break;
        case 1: connected(); break;
        case 2: disconnected(); break;
        case 3: stateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 4: error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 5: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 6: connectionClosed(); break;
        case 7: delayedCloseFinished(); break;
        case 8: connectToHostImplementation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< OpenMode(*)>(_a[3]))); break;
        case 9: connectToHostImplementation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        case 10: disconnectFromHostImplementation(); break;
        case 11: d_func()->_q_connectToNextAddress(); break;
        case 12: d_func()->_q_startConnecting((*reinterpret_cast< const QHostInfo(*)>(_a[1]))); break;
        case 13: d_func()->_q_abortConnectionAttempt(); break;
        case 14: d_func()->_q_testConnection(); break;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void QAbstractSocket::hostFound()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QAbstractSocket::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QAbstractSocket::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QAbstractSocket::stateChanged(QAbstractSocket::SocketState _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QAbstractSocket::error(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QAbstractSocket::proxyAuthenticationRequired(const QNetworkProxy & _t1, QAuthenticator * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QAbstractSocket::connectionClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void QAbstractSocket::delayedCloseFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}
QT_END_MOC_NAMESPACE
