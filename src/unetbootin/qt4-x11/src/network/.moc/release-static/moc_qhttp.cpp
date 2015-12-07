/****************************************************************************
** Meta object code from reading C++ file 'qhttp.h'
**
** Created: Thu Mar 5 18:41:23 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../access/qhttp.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhttp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHttp[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      30,   25,    6,    6, 0x05,
      74,   25,    6,    6, 0x05,
     107,  105,    6,    6, 0x05,
     133,  105,    6,    6, 0x05,
     159,    6,    6,    6, 0x05,
     179,  105,    6,    6, 0x05,
     205,    6,    6,    6, 0x05,
     223,  216,    6,    6, 0x05,
     297,  282,    6,    6, 0x05,
     360,  353,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     388,    6,    6,    6, 0x0a,
     396,    6,    6,    6, 0x0a,
     414,    6,    6,    6, 0x08,
     436,    6,    6,    6, 0x08,
     455,    6,    6,    6, 0x08,
     474,    6,    6,    6, 0x08,
     517,    6,    6,    6, 0x08,
     542,  533,    6,    6, 0x08,
     570,    6,    6,    6, 0x08,
     590,    6,    6,    6, 0x08,
     611,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHttp[] = {
    "QHttp\0\0stateChanged(int)\0resp\0"
    "responseHeaderReceived(QHttpResponseHeader)\0"
    "readyRead(QHttpResponseHeader)\0,\0"
    "dataSendProgress(int,int)\0"
    "dataReadProgress(int,int)\0requestStarted(int)\0"
    "requestFinished(int,bool)\0done(bool)\0"
    "proxy,\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
    "hostname,port,\0"
    "authenticationRequired(QString,quint16,QAuthenticator*)\0"
    "errors\0sslErrors(QList<QSslError>)\0"
    "abort()\0ignoreSslErrors()\0"
    "_q_startNextRequest()\0_q_slotReadyRead()\0"
    "_q_slotConnected()\0"
    "_q_slotError(QAbstractSocket::SocketError)\0"
    "_q_slotClosed()\0numBytes\0"
    "_q_slotBytesWritten(qint64)\0"
    "_q_slotDoFinished()\0_q_slotSendRequest()\0"
    "_q_continuePost()\0"
};

const QMetaObject QHttp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHttp,
      qt_meta_data_QHttp, 0 }
};

const QMetaObject *QHttp::metaObject() const
{
    return &staticMetaObject;
}

void *QHttp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHttp))
        return static_cast<void*>(const_cast< QHttp*>(this));
    return QObject::qt_metacast(_clname);
}

int QHttp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: responseHeaderReceived((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 2: readyRead((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 3: dataSendProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: dataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: requestStarted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: requestFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 9: authenticationRequired((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< QAuthenticator*(*)>(_a[3]))); break;
        case 10: sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 11: abort(); break;
        case 12: ignoreSslErrors(); break;
        case 13: d_func()->_q_startNextRequest(); break;
        case 14: d_func()->_q_slotReadyRead(); break;
        case 15: d_func()->_q_slotConnected(); break;
        case 16: d_func()->_q_slotError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 17: d_func()->_q_slotClosed(); break;
        case 18: d_func()->_q_slotBytesWritten((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 19: d_func()->_q_slotDoFinished(); break;
        case 20: d_func()->_q_slotSendRequest(); break;
        case 21: d_func()->_q_continuePost(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void QHttp::stateChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QHttp::responseHeaderReceived(const QHttpResponseHeader & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QHttp::readyRead(const QHttpResponseHeader & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QHttp::dataSendProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QHttp::dataReadProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QHttp::requestStarted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QHttp::requestFinished(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QHttp::done(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QHttp::proxyAuthenticationRequired(const QNetworkProxy & _t1, QAuthenticator * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QHttp::authenticationRequired(const QString & _t1, quint16 _t2, QAuthenticator * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QHttp::sslErrors(const QList<QSslError> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
