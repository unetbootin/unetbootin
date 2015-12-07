/****************************************************************************
** Meta object code from reading C++ file 'qnetworkaccessmanager.h'
**
** Created: Thu Mar 5 18:41:34 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../access/qnetworkaccessmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qnetworkaccessmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QNetworkAccessManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      43,   23,   22,   22, 0x05,
     122,  102,   22,   22, 0x05,
     183,  177,   22,   22, 0x05,
     221,  208,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     264,   22,   22,   22, 0x08,
     283,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QNetworkAccessManager[] = {
    "QNetworkAccessManager\0\0proxy,authenticator\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
    "reply,authenticator\0"
    "authenticationRequired(QNetworkReply*,QAuthenticator*)\0"
    "reply\0finished(QNetworkReply*)\0"
    "reply,errors\0sslErrors(QNetworkReply*,QList<QSslError>)\0"
    "_q_replyFinished()\0"
    "_q_replySslErrors(QList<QSslError>)\0"
};

const QMetaObject QNetworkAccessManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QNetworkAccessManager,
      qt_meta_data_QNetworkAccessManager, 0 }
};

const QMetaObject *QNetworkAccessManager::metaObject() const
{
    return &staticMetaObject;
}

void *QNetworkAccessManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNetworkAccessManager))
        return static_cast<void*>(const_cast< QNetworkAccessManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QNetworkAccessManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 1: authenticationRequired((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 2: finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: sslErrors((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        case 4: d_func()->_q_replyFinished(); break;
        case 5: d_func()->_q_replySslErrors((*reinterpret_cast< QList<QSslError>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QNetworkAccessManager::proxyAuthenticationRequired(const QNetworkProxy & _t1, QAuthenticator * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QNetworkAccessManager::authenticationRequired(QNetworkReply * _t1, QAuthenticator * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QNetworkAccessManager::finished(QNetworkReply * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QNetworkAccessManager::sslErrors(QNetworkReply * _t1, const QList<QSslError> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
