/****************************************************************************
** Meta object code from reading C++ file 'qprocess.h'
**
** Created: Fri May 30 22:16:33 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../io/qprocess.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qprocess.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QProcess[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      29,   20,    9,    9, 0x05,
      63,   43,    9,    9, 0x05,
     104,   98,    9,    9, 0x05,
     140,  134,    9,    9, 0x05,
     177,    9,    9,    9, 0x05,
     203,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     228,    9,    9,    9, 0x0a,
     240,    9,    9,    9, 0x0a,
     252,    9,  247,    9, 0x08,
     279,    9,  247,    9, 0x08,
     305,    9,  247,    9, 0x08,
     319,    9,  247,    9, 0x08,
     344,    9,  247,    9, 0x08,
     361,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QProcess[] = {
    "QProcess\0\0started()\0exitCode\0finished(int)\0"
    "exitCode,exitStatus\0"
    "finished(int,QProcess::ExitStatus)\0"
    "error\0error(QProcess::ProcessError)\0"
    "state\0stateChanged(QProcess::ProcessState)\0"
    "readyReadStandardOutput()\0"
    "readyReadStandardError()\0terminate()\0"
    "kill()\0bool\0_q_canReadStandardOutput()\0"
    "_q_canReadStandardError()\0_q_canWrite()\0"
    "_q_startupNotification()\0_q_processDied()\0"
    "_q_notified()\0"
};

const QMetaObject QProcess::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_QProcess,
      qt_meta_data_QProcess, 0 }
};

const QMetaObject *QProcess::metaObject() const
{
    return &staticMetaObject;
}

void *QProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QProcess))
	return static_cast<void*>(const_cast< QProcess*>(this));
    return QIODevice::qt_metacast(_clname);
}

int QProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: started(); break;
        case 1: finished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: finished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 3: error((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 4: stateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1]))); break;
        case 5: readyReadStandardOutput(); break;
        case 6: readyReadStandardError(); break;
        case 7: terminate(); break;
        case 8: kill(); break;
        case 9: { bool _r = d_func()->_q_canReadStandardOutput();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = d_func()->_q_canReadStandardError();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = d_func()->_q_canWrite();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = d_func()->_q_startupNotification();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = d_func()->_q_processDied();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: d_func()->_q_notified(); break;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void QProcess::started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QProcess::finished(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QProcess::finished(int _t1, QProcess::ExitStatus _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QProcess::error(QProcess::ProcessError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QProcess::stateChanged(QProcess::ProcessState _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QProcess::readyReadStandardOutput()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QProcess::readyReadStandardError()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
