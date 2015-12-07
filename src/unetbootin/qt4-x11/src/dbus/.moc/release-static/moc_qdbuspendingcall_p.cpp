/****************************************************************************
** Meta object code from reading C++ file 'qdbuspendingcall_p.h'
**
** Created: Thu Mar 5 19:07:23 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdbuspendingcall_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdbuspendingcall_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusPendingCallWatcherHelper[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x05,
      46,   42,   30,   30, 0x05,
      76,   66,   30,   30, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QDBusPendingCallWatcherHelper[] = {
    "QDBusPendingCallWatcherHelper\0\0"
    "finished()\0msg\0reply(QDBusMessage)\0"
    "error,msg\0error(QDBusError,QDBusMessage)\0"
};

const QMetaObject QDBusPendingCallWatcherHelper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDBusPendingCallWatcherHelper,
      qt_meta_data_QDBusPendingCallWatcherHelper, 0 }
};

const QMetaObject *QDBusPendingCallWatcherHelper::metaObject() const
{
    return &staticMetaObject;
}

void *QDBusPendingCallWatcherHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDBusPendingCallWatcherHelper))
        return static_cast<void*>(const_cast< QDBusPendingCallWatcherHelper*>(this));
    return QObject::qt_metacast(_clname);
}

int QDBusPendingCallWatcherHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished(); break;
        case 1: reply((*reinterpret_cast< const QDBusMessage(*)>(_a[1]))); break;
        case 2: error((*reinterpret_cast< const QDBusError(*)>(_a[1])),(*reinterpret_cast< const QDBusMessage(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QDBusPendingCallWatcherHelper::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QDBusPendingCallWatcherHelper::reply(const QDBusMessage & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QDBusPendingCallWatcherHelper::error(const QDBusError & _t1, const QDBusMessage & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
