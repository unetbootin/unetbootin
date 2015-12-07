/****************************************************************************
** Meta object code from reading C++ file 'qnetworkreplyimpl_p.h'
**
** Created: Thu Mar 5 18:41:50 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../access/qnetworkreplyimpl_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qnetworkreplyimpl_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QNetworkReplyImpl[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,
      39,   18,   18,   18, 0x0a,
      60,   18,   18,   18, 0x0a,
      91,   18,   18,   18, 0x0a,
     110,   18,   18,   18, 0x0a,

 // methods: signature, parameters, type, tag, flags
     157,   18,  139,   18, 0x02,
     204,  190,   18,   18, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_QNetworkReplyImpl[] = {
    "QNetworkReplyImpl\0\0_q_startOperation()\0"
    "_q_sourceReadyRead()\0"
    "_q_sourceReadChannelFinished()\0"
    "_q_copyReadyRead()\0_q_copyReadChannelFinished()\0"
    "QSslConfiguration\0sslConfigurationImplementation()\0"
    "configuration\0"
    "setSslConfigurationImplementation(QSslConfiguration)\0"
};

const QMetaObject QNetworkReplyImpl::staticMetaObject = {
    { &QNetworkReply::staticMetaObject, qt_meta_stringdata_QNetworkReplyImpl,
      qt_meta_data_QNetworkReplyImpl, 0 }
};

const QMetaObject *QNetworkReplyImpl::metaObject() const
{
    return &staticMetaObject;
}

void *QNetworkReplyImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QNetworkReplyImpl))
        return static_cast<void*>(const_cast< QNetworkReplyImpl*>(this));
    return QNetworkReply::qt_metacast(_clname);
}

int QNetworkReplyImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QNetworkReply::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_startOperation(); break;
        case 1: d_func()->_q_sourceReadyRead(); break;
        case 2: d_func()->_q_sourceReadChannelFinished(); break;
        case 3: d_func()->_q_copyReadyRead(); break;
        case 4: d_func()->_q_copyReadChannelFinished(); break;
        case 5: { QSslConfiguration _r = sslConfigurationImplementation();
            if (_a[0]) *reinterpret_cast< QSslConfiguration*>(_a[0]) = _r; }  break;
        case 6: setSslConfigurationImplementation((*reinterpret_cast< const QSslConfiguration(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
