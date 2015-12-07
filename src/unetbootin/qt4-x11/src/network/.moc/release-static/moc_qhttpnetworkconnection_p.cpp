/****************************************************************************
** Meta object code from reading C++ file 'qhttpnetworkconnection_p.h'
**
** Created: Thu Mar 5 18:41:29 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../access/qhttpnetworkconnection_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhttpnetworkconnection_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHttpNetworkConnection[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      31,   24,   23,   23, 0x05,
      90,   59,   23,   23, 0x05,
     199,  179,   23,   23, 0x25,
     291,  258,   23,   23, 0x05,
     403,  381,   23,   23, 0x25,
     480,  463,   23,   23, 0x05,
     533,  523,   23,   23, 0x25,

 // slots: signature, parameters, type, tag, flags
     568,   23,   23,   23, 0x08,
     592,   23,   23,   23, 0x08,
     607,   23,   23,   23, 0x08,
     625,   23,   23,   23, 0x08,
     647,   23,   23,   23, 0x08,
     674,   23,   23,   23, 0x08,
     689,   23,   23,   23, 0x08,
     730,  728,   23,   23, 0x08,
     792,   23,   23,   23, 0x08,
     817,   23,   23,   23, 0x08,
     844,   23,   23,   23, 0x08,
     859,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHttpNetworkConnection[] = {
    "QHttpNetworkConnection\0\0errors\0"
    "sslErrors(QList<QSslError>)\0"
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
    "_q_dataReadyReadNoBuffer()\0_q_encrypted()\0"
    "_q_sslErrors(QList<QSslError>)\0"
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
        case 0: sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 1: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2])),(*reinterpret_cast< const QHttpNetworkConnection*(*)>(_a[3]))); break;
        case 2: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 3: authenticationRequired((*reinterpret_cast< const QHttpNetworkRequest(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2])),(*reinterpret_cast< const QHttpNetworkConnection*(*)>(_a[3]))); break;
        case 4: authenticationRequired((*reinterpret_cast< const QHttpNetworkRequest(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 5: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 7: d_func()->_q_bytesWritten((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 8: d_func()->_q_readyRead(); break;
        case 9: d_func()->_q_disconnected(); break;
        case 10: d_func()->_q_startNextRequest(); break;
        case 11: d_func()->_q_restartPendingRequest(); break;
        case 12: d_func()->_q_connected(); break;
        case 13: d_func()->_q_error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 14: d_func()->_q_proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 15: d_func()->_q_dataReadyReadBuffer(); break;
        case 16: d_func()->_q_dataReadyReadNoBuffer(); break;
        case 17: d_func()->_q_encrypted(); break;
        case 18: d_func()->_q_sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void QHttpNetworkConnection::sslErrors(const QList<QSslError> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QHttpNetworkConnection::proxyAuthenticationRequired(const QNetworkProxy & _t1, QAuthenticator * _t2, const QHttpNetworkConnection * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, 2, _a);
}

// SIGNAL 3
void QHttpNetworkConnection::authenticationRequired(const QHttpNetworkRequest & _t1, QAuthenticator * _t2, const QHttpNetworkConnection * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, 4, _a);
}

// SIGNAL 5
void QHttpNetworkConnection::error(QNetworkReply::NetworkError _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, 6, _a);
}
static const uint qt_meta_data_QHttpNetworkReply[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      26,   19,   18,   18, 0x05,
      54,   18,   18,   18, 0x05,
      66,   18,   18,   18, 0x05,
      94,   77,   18,   18, 0x05,
     159,  149,   18,   18, 0x25,
     206,   18,   18,   18, 0x05,
     233,  222,   18,   18, 0x05,
     259,  222,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QHttpNetworkReply[] = {
    "QHttpNetworkReply\0\0errors\0"
    "sslErrors(QList<QSslError>)\0readyRead()\0"
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
        case 0: sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 1: readyRead(); break;
        case 2: finished(); break;
        case 3: finishedWithError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: finishedWithError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 5: headerChanged(); break;
        case 6: dataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: dataSendProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void QHttpNetworkReply::sslErrors(const QList<QSslError> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QHttpNetworkReply::readyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QHttpNetworkReply::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QHttpNetworkReply::finishedWithError(QNetworkReply::NetworkError _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, 4, _a);
}

// SIGNAL 5
void QHttpNetworkReply::headerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QHttpNetworkReply::dataReadProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QHttpNetworkReply::dataSendProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
