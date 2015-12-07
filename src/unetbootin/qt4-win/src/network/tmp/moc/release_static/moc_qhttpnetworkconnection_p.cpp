/****************************************************************************
** Meta object code from reading C++ file 'qhttpnetworkconnection_p.h'
**
** Created: Fri May 30 22:19:07 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../access/qhttpnetworkconnection_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhttpnetworkconnection_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHttpNetworkConnection[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      55,   24,   23,   23, 0x05,
     164,  144,   23,   23, 0x25,
     256,  223,   23,   23, 0x05,
     368,  346,   23,   23, 0x25,
     445,  428,   23,   23, 0x05,
     498,  488,   23,   23, 0x25,

 // slots: signature, parameters, type, tag, flags
     533,   23,   23,   23, 0x08,
     557,   23,   23,   23, 0x08,
     572,   23,   23,   23, 0x08,
     590,   23,   23,   23, 0x08,
     612,   23,   23,   23, 0x08,
     639,   23,   23,   23, 0x08,
     654,   23,   23,   23, 0x08,
     695,  693,   23,   23, 0x08,
     757,   23,   23,   23, 0x08,
     782,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHttpNetworkConnection[] = {
    "QHttpNetworkConnection\0\0"
    "proxy,authenticator,connection\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*,const QHttpN"
    "etworkConnection*)\0"
    "proxy,authenticator\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
    "request,authenticator,connection\0"
    "authenticationRequired(QHttpNetworkRequest,QAuthenticator*,const QHttp"
    "NetworkConnection*)\0"
    "request,authenticator\0"
    "authenticationRequired(QHttpNetworkRequest,QAuthenticator*)\0"
    "errorCode,detail\0"
    "error(QNetworkReply::NetworkError,QString)\0"
    "errorCode\0error(QNetworkReply::NetworkError)\0"
    "_q_bytesWritten(qint64)\0_q_readyRead()\0"
    "_q_disconnected()\0_q_startNextRequest()\0"
    "_q_restartPendingRequest()\0_q_connected()\0"
    "_q_error(QAbstractSocket::SocketError)\0"
    ",\0_q_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
    "_q_dataReadyReadBuffer()\0"
    "_q_dataReadyReadNoBuffer()\0"
};

const QMetaObject QHttpNetworkConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHttpNetworkConnection,
      qt_meta_data_QHttpNetworkConnection, 0 }
};

const QMetaObject *QHttpNetworkConnection::metaObject() const
{
    return &staticMetaObject;
}

void *QHttpNetworkConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHttpNetworkConnection))
	return static_cast<void*>(const_cast< QHttpNetworkConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int QHttpNetworkConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2])),(*reinterpret_cast< const QHttpNetworkConnection*(*)>(_a[3]))); break;
        case 1: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 2: authenticationRequired((*reinterpret_cast< const QHttpNetworkRequest(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2])),(*reinterpret_cast< const QHttpNetworkConnection*(*)>(_a[3]))); break;
        case 3: authenticationRequired((*reinterpret_cast< const QHttpNetworkRequest(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 4: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 6: d_func()->_q_bytesWritten((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 7: d_func()->_q_readyRead(); break;
        case 8: d_func()->_q_disconnected(); break;
        case 9: d_func()->_q_startNextRequest(); break;
        case 10: d_func()->_q_restartPendingRequest(); break;
        case 11: d_func()->_q_connected(); break;
        case 12: d_func()->_q_error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 13: d_func()->_q_proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 14: d_func()->_q_dataReadyReadBuffer(); break;
        case 15: d_func()->_q_dataReadyReadNoBuffer(); break;
        }
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void QHttpNetworkConnection::proxyAuthenticationRequired(const QNetworkProxy & _t1, QAuthenticator * _t2, const QHttpNetworkConnection * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, 1, _a);
}

// SIGNAL 2
void QHttpNetworkConnection::authenticationRequired(const QHttpNetworkRequest & _t1, QAuthenticator * _t2, const QHttpNetworkConnection * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, 3, _a);
}

// SIGNAL 4
void QHttpNetworkConnection::error(QNetworkReply::NetworkError _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, 5, _a);
}
static const uint qt_meta_data_QHttpNetworkReply[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      31,   18,   18,   18, 0x05,
      59,   42,   18,   18, 0x05,
     124,  114,   18,   18, 0x25,
     171,   18,   18,   18, 0x05,
     198,  187,   18,   18, 0x05,
     224,  187,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QHttpNetworkReply[] = {
    "QHttpNetworkReply\0\0readyRead()\0"
    "finished()\0errorCode,detail\0"
    "finishedWithError(QNetworkReply::NetworkError,QString)\0"
    "errorCode\0finishedWithError(QNetworkReply::NetworkError)\0"
    "headerChanged()\0done,total\0"
    "dataReadProgress(int,int)\0"
    "dataSendProgress(int,int)\0"
};

const QMetaObject QHttpNetworkReply::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHttpNetworkReply,
      qt_meta_data_QHttpNetworkReply, 0 }
};

const QMetaObject *QHttpNetworkReply::metaObject() const
{
    return &staticMetaObject;
}

void *QHttpNetworkReply::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHttpNetworkReply))
	return static_cast<void*>(const_cast< QHttpNetworkReply*>(this));
    if (!strcmp(_clname, "QHttpNetworkHeader"))
	return static_cast< QHttpNetworkHeader*>(const_cast< QHttpNetworkReply*>(this));
    return QObject::qt_metacast(_clname);
}

int QHttpNetworkReply::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readyRead(); break;
        case 1: finished(); break;
        case 2: finishedWithError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: finishedWithError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 4: headerChanged(); break;
        case 5: dataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: dataSendProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QHttpNetworkReply::readyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QHttpNetworkReply::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QHttpNetworkReply::finishedWithError(QNetworkReply::NetworkError _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, 3, _a);
}

// SIGNAL 4
void QHttpNetworkReply::headerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QHttpNetworkReply::dataReadProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QHttpNetworkReply::dataSendProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
