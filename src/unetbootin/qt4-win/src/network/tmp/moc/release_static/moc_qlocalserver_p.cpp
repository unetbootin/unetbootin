/****************************************************************************
** Meta object code from reading C++ file 'qlocalserver_p.h'
**
** Created: Fri May 30 22:20:12 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../socket/qlocalserver_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlocalserver_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QLocalServerThread[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      30,   20,   19,   19, 0x05,
      66,   48,   19,   19, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QLocalServerThread[] = {
    "QLocalServerThread\0\0newSocket\0"
    "connected(HANDLE)\0error,errorString\0"
    "error(QAbstractSocket::SocketError,QString)\0"
};

const QMetaObject QLocalServerThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QLocalServerThread,
      qt_meta_data_QLocalServerThread, 0 }
};

const QMetaObject *QLocalServerThread::metaObject() const
{
    return &staticMetaObject;
}

void *QLocalServerThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QLocalServerThread))
	return static_cast<void*>(const_cast< QLocalServerThread*>(this));
    return QThread::qt_metacast(_clname);
}

int QLocalServerThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: connected((*reinterpret_cast< HANDLE(*)>(_a[1]))); break;
        case 1: error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QLocalServerThread::connected(HANDLE _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QLocalServerThread::error(QAbstractSocket::SocketError _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
