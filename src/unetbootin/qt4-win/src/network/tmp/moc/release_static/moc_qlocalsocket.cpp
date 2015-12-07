/****************************************************************************
** Meta object code from reading C++ file 'qlocalsocket.h'
**
** Created: Fri May 30 22:19:53 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../socket/qlocalsocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlocalsocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QLocalSocket[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      26,   13,   13,   13, 0x05,
      53,   41,   13,   13, 0x05,
     103,   91,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     148,   13,   13,   13, 0x08,
     162,   13,   13,   13, 0x08,
     176,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QLocalSocket[] = {
    "QLocalSocket\0\0connected()\0disconnected()\0"
    "socketError\0error(QLocalSocket::LocalSocketError)\0"
    "socketState\0stateChanged(QLocalSocket::LocalSocketState)\0"
    "_q_notified()\0_q_canWrite()\0_q_pipeClosed()\0"
};

const QMetaObject QLocalSocket::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_QLocalSocket,
      qt_meta_data_QLocalSocket, 0 }
};

const QMetaObject *QLocalSocket::metaObject() const
{
    return &staticMetaObject;
}

void *QLocalSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QLocalSocket))
	return static_cast<void*>(const_cast< QLocalSocket*>(this));
    return QIODevice::qt_metacast(_clname);
}

int QLocalSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: connected(); break;
        case 1: disconnected(); break;
        case 2: error((*reinterpret_cast< QLocalSocket::LocalSocketError(*)>(_a[1]))); break;
        case 3: stateChanged((*reinterpret_cast< QLocalSocket::LocalSocketState(*)>(_a[1]))); break;
        case 4: d_func()->_q_notified(); break;
        case 5: d_func()->_q_canWrite(); break;
        case 6: d_func()->_q_pipeClosed(); break;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QLocalSocket::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QLocalSocket::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QLocalSocket::error(QLocalSocket::LocalSocketError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QLocalSocket::stateChanged(QLocalSocket::LocalSocketState _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
