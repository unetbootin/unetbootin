/****************************************************************************
** Meta object code from reading C++ file 'q3networkprotocol.h'
**
** Created: Fri May 30 22:40:46 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../network/q3networkprotocol.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3networkprotocol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3NetworkProtocol[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      24,   19,   18,   18, 0x05,
      72,   61,   18,   18, 0x05,
     112,  108,   18,   18, 0x05,
     142,  108,   18,   18, 0x05,
     169,   19,   18,   18, 0x05,
     224,   19,   18,   18, 0x05,
     263,   19,   18,   18, 0x05,
     310,  108,   18,   18, 0x05,
     339,  108,   18,   18, 0x05,
     397,  372,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     451,  447,   18,   18, 0x08,
     493,   18,   18,   18, 0x08,
     509,  504,   18,   18, 0x08,
     555,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Q3NetworkProtocol[] = {
    "Q3NetworkProtocol\0\0,res\0"
    "data(QByteArray,Q3NetworkOperation*)\0"
    "state,data\0connectionStateChanged(int,QString)\0"
    "res\0finished(Q3NetworkOperation*)\0"
    "start(Q3NetworkOperation*)\0"
    "newChildren(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)\0"
    "newChild(QUrlInfo,Q3NetworkOperation*)\0"
    "createdDirectory(QUrlInfo,Q3NetworkOperation*)\0"
    "removed(Q3NetworkOperation*)\0"
    "itemChanged(Q3NetworkOperation*)\0"
    "bytesDone,bytesTotal,res\0"
    "dataTransferProgress(int,int,Q3NetworkOperation*)\0"
    "old\0processNextOperation(Q3NetworkOperation*)\0"
    "startOps()\0i,op\0"
    "emitNewChildren(QUrlInfo,Q3NetworkOperation*)\0"
    "removeMe()\0"
};

const QMetaObject Q3NetworkProtocol::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3NetworkProtocol,
      qt_meta_data_Q3NetworkProtocol, 0 }
};

const QMetaObject *Q3NetworkProtocol::metaObject() const
{
    return &staticMetaObject;
}

void *Q3NetworkProtocol::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3NetworkProtocol))
	return static_cast<void*>(const_cast< Q3NetworkProtocol*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3NetworkProtocol::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: data((*reinterpret_cast< const QByteArray(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 1: connectionStateChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: finished((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 3: start((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 4: newChildren((*reinterpret_cast< const Q3ValueList<QUrlInfo>(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 5: newChild((*reinterpret_cast< const QUrlInfo(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 6: createdDirectory((*reinterpret_cast< const QUrlInfo(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 7: removed((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 8: itemChanged((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 9: dataTransferProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[3]))); break;
        case 10: processNextOperation((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 11: startOps(); break;
        case 12: emitNewChildren((*reinterpret_cast< const QUrlInfo(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 13: removeMe(); break;
        }
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Q3NetworkProtocol::data(const QByteArray & _t1, Q3NetworkOperation * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3NetworkProtocol::connectionStateChanged(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3NetworkProtocol::finished(Q3NetworkOperation * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3NetworkProtocol::start(Q3NetworkOperation * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3NetworkProtocol::newChildren(const Q3ValueList<QUrlInfo> & _t1, Q3NetworkOperation * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3NetworkProtocol::newChild(const QUrlInfo & _t1, Q3NetworkOperation * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3NetworkProtocol::createdDirectory(const QUrlInfo & _t1, Q3NetworkOperation * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Q3NetworkProtocol::removed(Q3NetworkOperation * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Q3NetworkProtocol::itemChanged(Q3NetworkOperation * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Q3NetworkProtocol::dataTransferProgress(int _t1, int _t2, Q3NetworkOperation * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
static const uint qt_meta_data_Q3NetworkOperation[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Q3NetworkOperation[] = {
    "Q3NetworkOperation\0\0deleteMe()\0"
};

const QMetaObject Q3NetworkOperation::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3NetworkOperation,
      qt_meta_data_Q3NetworkOperation, 0 }
};

const QMetaObject *Q3NetworkOperation::metaObject() const
{
    return &staticMetaObject;
}

void *Q3NetworkOperation::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3NetworkOperation))
	return static_cast<void*>(const_cast< Q3NetworkOperation*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3NetworkOperation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: deleteMe(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
