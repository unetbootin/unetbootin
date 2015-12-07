/****************************************************************************
** Meta object code from reading C++ file 'qiodevice.h'
**
** Created: Thu Mar 5 18:40:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../io/qiodevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qiodevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QIODevice[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      29,   23,   10,   10, 0x05,
      50,   10,   10,   10, 0x05,
      65,   10,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QIODevice[] = {
    "QIODevice\0\0readyRead()\0bytes\0"
    "bytesWritten(qint64)\0aboutToClose()\0"
    "readChannelFinished()\0"
};

const QMetaObject QIODevice::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QIODevice,
      qt_meta_data_QIODevice, 0 }
};

const QMetaObject *QIODevice::metaObject() const
{
    return &staticMetaObject;
}

void *QIODevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QIODevice))
        return static_cast<void*>(const_cast< QIODevice*>(this));
    return QObject::qt_metacast(_clname);
}

int QIODevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readyRead(); break;
        case 1: bytesWritten((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 2: aboutToClose(); break;
        case 3: readChannelFinished(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QIODevice::readyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QIODevice::bytesWritten(qint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QIODevice::aboutToClose()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QIODevice::readChannelFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
