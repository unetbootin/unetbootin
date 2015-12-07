/****************************************************************************
** Meta object code from reading C++ file 'qnetworkreply.h'
**
** Created: Fri May 30 22:20:06 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../access/qnetworkreply.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qnetworkreply.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QNetworkReply[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       1,   40, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      33,   14,   14,   14, 0x05,
      44,   14,   14,   14, 0x05,
     100,   79,   14,   14, 0x05,
     155,  130,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     187,   14,   14,   14, 0x0a,

 // enums: name, flags, count, data
     205, 0x0,   22,   44,

 // enum data: key, value
     218, uint(QNetworkReply::NoError),
     226, uint(QNetworkReply::ConnectionRefusedError),
     249, uint(QNetworkReply::RemoteHostClosedError),
     271, uint(QNetworkReply::HostNotFoundError),
     289, uint(QNetworkReply::TimeoutError),
     302, uint(QNetworkReply::OperationCanceledError),
     325, uint(QNetworkReply::SslHandshakeFailedError),
     349, uint(QNetworkReply::UnknownNetworkError),
     369, uint(QNetworkReply::ProxyConnectionRefusedError),
     397, uint(QNetworkReply::ProxyConnectionClosedError),
     424, uint(QNetworkReply::ProxyNotFoundError),
     443, uint(QNetworkReply::ProxyTimeoutError),
     461, uint(QNetworkReply::ProxyAuthenticationRequiredError),
     494, uint(QNetworkReply::UnknownProxyError),
     512, uint(QNetworkReply::ContentAccessDenied),
     532, uint(QNetworkReply::ContentOperationNotPermittedError),
     566, uint(QNetworkReply::ContentNotFoundError),
     587, uint(QNetworkReply::AuthenticationRequiredError),
     615, uint(QNetworkReply::UnknownContentError),
     635, uint(QNetworkReply::ProtocolUnknownError),
     656, uint(QNetworkReply::ProtocolInvalidOperationError),
     686, uint(QNetworkReply::ProtocolFailure),

       0        // eod
};

static const char qt_meta_stringdata_QNetworkReply[] = {
    "QNetworkReply\0\0metaDataChanged()\0"
    "finished()\0error(QNetworkReply::NetworkError)\0"
    "bytesSent,bytesTotal\0uploadProgress(qint64,qint64)\0"
    "bytesReceived,bytesTotal\0"
    "downloadProgress(qint64,qint64)\0"
    "ignoreSslErrors()\0NetworkError\0NoError\0"
    "ConnectionRefusedError\0RemoteHostClosedError\0"
    "HostNotFoundError\0TimeoutError\0"
    "OperationCanceledError\0SslHandshakeFailedError\0"
    "UnknownNetworkError\0ProxyConnectionRefusedError\0"
    "ProxyConnectionClosedError\0"
    "ProxyNotFoundError\0ProxyTimeoutError\0"
    "ProxyAuthenticationRequiredError\0"
    "UnknownProxyError\0ContentAccessDenied\0"
    "ContentOperationNotPermittedError\0"
    "ContentNotFoundError\0AuthenticationRequiredError\0"
    "UnknownContentError\0ProtocolUnknownError\0"
    "ProtocolInvalidOperationError\0"
    "ProtocolFailure\0"
};

const QMetaObject QNetworkReply::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_QNetworkReply,
      qt_meta_data_QNetworkReply, 0 }
};

const QMetaObject *QNetworkReply::metaObject() const
{
    return &staticMetaObject;
}

void *QNetworkReply::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNetworkReply))
	return static_cast<void*>(const_cast< QNetworkReply*>(this));
    return QIODevice::qt_metacast(_clname);
}

int QNetworkReply::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: metaDataChanged(); break;
        case 1: finished(); break;
        case 2: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 3: uploadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 4: downloadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 5: ignoreSslErrors(); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QNetworkReply::metaDataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QNetworkReply::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QNetworkReply::error(QNetworkReply::NetworkError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QNetworkReply::uploadProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QNetworkReply::downloadProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
