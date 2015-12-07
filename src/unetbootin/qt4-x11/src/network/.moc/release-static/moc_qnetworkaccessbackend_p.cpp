/****************************************************************************
** Meta object code from reading C++ file 'qnetworkaccessbackend_p.h'
**
** Created: Thu Mar 5 18:42:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../access/qnetworkaccessbackend_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qnetworkaccessbackend_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QNetworkAccessBackend[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x09,
      51,   34,   22,   22, 0x09,
     105,   94,   22,   22, 0x09,
     169,  164,   22,   22, 0x09,
     209,   22,   22,   22, 0x09,
     239,  227,   22,   22, 0x09,
     273,  266,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_QNetworkAccessBackend[] = {
    "QNetworkAccessBackend\0\0finished()\0"
    "code,errorString\0"
    "error(QNetworkReply::NetworkError,QString)\0"
    "proxy,auth\0"
    "proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)\0"
    "auth\0authenticationRequired(QAuthenticator*)\0"
    "metaDataChanged()\0destination\0"
    "redirectionRequested(QUrl)\0errors\0"
    "sslErrors(QList<QSslError>)\0"
};

const QMetaObject QNetworkAccessBackend::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QNetworkAccessBackend,
      qt_meta_data_QNetworkAccessBackend, 0 }
};

const QMetaObject *QNetworkAccessBackend::metaObject() const
{
    return &staticMetaObject;
}

void *QNetworkAccessBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNetworkAccessBackend))
        return static_cast<void*>(const_cast< QNetworkAccessBackend*>(this));
    return QObject::qt_metacast(_clname);
}

int QNetworkAccessBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished(); break;
        case 1: error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: proxyAuthenticationRequired((*reinterpret_cast< const QNetworkProxy(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 3: authenticationRequired((*reinterpret_cast< QAuthenticator*(*)>(_a[1]))); break;
        case 4: metaDataChanged(); break;
        case 5: redirectionRequested((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 6: sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
