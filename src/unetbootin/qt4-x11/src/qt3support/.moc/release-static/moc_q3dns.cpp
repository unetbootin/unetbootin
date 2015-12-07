/****************************************************************************
** Meta object code from reading C++ file 'q3dns.h'
**
** Created: Thu Mar 5 19:06:11 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../network/q3dns.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3dns.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3Dns[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      22,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Q3Dns[] = {
    "Q3Dns\0\0resultsReady()\0startQuery()\0"
};

const QMetaObject Q3Dns::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3Dns,
      qt_meta_data_Q3Dns, 0 }
};

const QMetaObject *Q3Dns::metaObject() const
{
    return &staticMetaObject;
}

void *Q3Dns::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3Dns))
        return static_cast<void*>(const_cast< Q3Dns*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3Dns::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resultsReady(); break;
        case 1: startQuery(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Q3Dns::resultsReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_Q3DnsSocket[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      26,   12,   12,   12, 0x08,
      39,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Q3DnsSocket[] = {
    "Q3DnsSocket\0\0cleanCache()\0retransmit()\0"
    "answer()\0"
};

const QMetaObject Q3DnsSocket::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3DnsSocket,
      qt_meta_data_Q3DnsSocket, 0 }
};

const QMetaObject *Q3DnsSocket::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DnsSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DnsSocket))
        return static_cast<void*>(const_cast< Q3DnsSocket*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3DnsSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cleanCache(); break;
        case 1: retransmit(); break;
        case 2: answer(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
