/****************************************************************************
** Meta object code from reading C++ file 'q3ftp.h'
**
** Created: Fri May 30 22:41:45 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../network/q3ftp.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3ftp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3Ftp[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      25,    6,    6,    6, 0x05,
      44,    6,    6,    6, 0x05,
      58,   56,    6,    6, 0x05,
      88,   56,    6,    6, 0x05,
     117,    6,    6,    6, 0x05,
     137,   56,    6,    6, 0x05,
     163,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     174,    6,    6,    6, 0x0a,
     182,    6,    6,    6, 0x08,
     201,    6,    6,    6, 0x08,
     221,   56,    6,    6, 0x08,
     242,    6,    6,    6, 0x08,
     262,   56,    6,    6, 0x08,
     286,    6,    6,    6, 0x08,
     307,    6,    6,    6, 0x08,
     320,    6,    6,    6, 0x08,
     340,   56,    6,    6, 0x08,
     372,    6,    6,    6, 0x08,
     386,    6,    6,    6, 0x09,
     398,    6,    6,    6, 0x09,
     410,    6,    6,    6, 0x09,
     419,    6,    6,    6, 0x09,
     435,    6,    6,    6, 0x09,
     451,    6,    6,    6, 0x09,
     464,    6,    6,    6, 0x09,
     487,  480,    6,    6, 0x09,
     509,    6,    6,    6, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_Q3Ftp[] = {
    "Q3Ftp\0\0stateChanged(int)\0listInfo(QUrlInfo)\0"
    "readyRead()\0,\0dataTransferProgress(int,int)\0"
    "rawCommandReply(int,QString)\0"
    "commandStarted(int)\0commandFinished(int,bool)\0"
    "done(bool)\0abort()\0startNextCommand()\0"
    "piFinished(QString)\0piError(int,QString)\0"
    "piConnectState(int)\0piFtpReply(int,QString)\0"
    "npListInfo(QUrlInfo)\0npDone(bool)\0"
    "npStateChanged(int)\0npDataTransferProgress(int,int)\0"
    "npReadyRead()\0hostFound()\0connected()\0"
    "closed()\0dataHostFound()\0dataConnected()\0"
    "dataClosed()\0dataReadyRead()\0nbytes\0"
    "dataBytesWritten(int)\0error(int)\0"
};

const QMetaObject Q3Ftp::staticMetaObject = {
    { &Q3NetworkProtocol::staticMetaObject, qt_meta_stringdata_Q3Ftp,
      qt_meta_data_Q3Ftp, 0 }
};

const QMetaObject *Q3Ftp::metaObject() const
{
    return &staticMetaObject;
}

void *Q3Ftp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3Ftp))
	return static_cast<void*>(const_cast< Q3Ftp*>(this));
    return Q3NetworkProtocol::qt_metacast(_clname);
}

int Q3Ftp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3NetworkProtocol::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: listInfo((*reinterpret_cast< const QUrlInfo(*)>(_a[1]))); break;
        case 2: readyRead(); break;
        case 3: dataTransferProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: rawCommandReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: commandStarted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: commandFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 7: done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: abort(); break;
        case 9: startNextCommand(); break;
        case 10: piFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: piError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 12: piConnectState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: piFtpReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: npListInfo((*reinterpret_cast< const QUrlInfo(*)>(_a[1]))); break;
        case 15: npDone((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: npStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: npDataTransferProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 18: npReadyRead(); break;
        case 19: hostFound(); break;
        case 20: connected(); break;
        case 21: closed(); break;
        case 22: dataHostFound(); break;
        case 23: dataConnected(); break;
        case 24: dataClosed(); break;
        case 25: dataReadyRead(); break;
        case 26: dataBytesWritten((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: error((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void Q3Ftp::stateChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3Ftp::listInfo(const QUrlInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3Ftp::readyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Q3Ftp::dataTransferProgress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3Ftp::rawCommandReply(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3Ftp::commandStarted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3Ftp::commandFinished(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Q3Ftp::done(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
