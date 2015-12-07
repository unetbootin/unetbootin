/****************************************************************************
** Meta object code from reading C++ file 'qnetworkaccessftpbackend_p.h'
**
** Created: Fri May 30 22:20:05 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../access/qnetworkaccessftpbackend_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qnetworkaccessftpbackend_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QNetworkAccessFtpBackend[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      33,   26,   25,   25, 0x0a,
      91,   25,   25,   25, 0x0a,
     101,   25,   25,   25, 0x0a,
     126,  116,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QNetworkAccessFtpBackend[] = {
    "QNetworkAccessFtpBackend\0\0object\0"
    "ftpConnectionReady(QNetworkAccessCache::CacheableObject*)\0"
    "ftpDone()\0ftpReadyRead()\0code,text\0"
    "ftpRawCommandReply(int,QString)\0"
};

const QMetaObject QNetworkAccessFtpBackend::staticMetaObject = {
    { &QNetworkAccessBackend::staticMetaObject, qt_meta_stringdata_QNetworkAccessFtpBackend,
      qt_meta_data_QNetworkAccessFtpBackend, 0 }
};

const QMetaObject *QNetworkAccessFtpBackend::metaObject() const
{
    return &staticMetaObject;
}

void *QNetworkAccessFtpBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNetworkAccessFtpBackend))
	return static_cast<void*>(const_cast< QNetworkAccessFtpBackend*>(this));
    return QNetworkAccessBackend::qt_metacast(_clname);
}

int QNetworkAccessFtpBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QNetworkAccessBackend::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: ftpConnectionReady((*reinterpret_cast< QNetworkAccessCache::CacheableObject*(*)>(_a[1]))); break;
        case 1: ftpDone(); break;
        case 2: ftpReadyRead(); break;
        case 3: ftpRawCommandReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
