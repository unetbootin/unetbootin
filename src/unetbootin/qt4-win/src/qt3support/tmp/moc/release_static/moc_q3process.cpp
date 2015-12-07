/****************************************************************************
** Meta object code from reading C++ file 'q3process.h'
**
** Created: Fri May 30 22:41:09 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../other/q3process.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3process.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3Process[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      29,   10,   10,   10, 0x05,
      47,   10,   10,   10, 0x05,
      63,   10,   10,   10, 0x05,
      78,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      95,   10,   10,   10, 0x0a,
     110,   10,   10,   10, 0x0a,
     121,  117,   10,   10, 0x0a,
     146,  117,   10,   10, 0x0a,
     168,   10,   10,   10, 0x0a,
     184,  181,   10,   10, 0x08,
     200,  181,   10,   10, 0x08,
     217,   10,   10,   10, 0x08,
     227,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Q3Process[] = {
    "Q3Process\0\0readyReadStdout()\0"
    "readyReadStderr()\0processExited()\0"
    "wroteToStdin()\0launchFinished()\0"
    "tryTerminate()\0kill()\0buf\0"
    "writeToStdin(QByteArray)\0writeToStdin(QString)\0"
    "closeStdin()\0fd\0socketRead(int)\0"
    "socketWrite(int)\0timeout()\0"
    "closeStdinLaunch()\0"
};

const QMetaObject Q3Process::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3Process,
      qt_meta_data_Q3Process, 0 }
};

const QMetaObject *Q3Process::metaObject() const
{
    return &staticMetaObject;
}

void *Q3Process::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3Process))
	return static_cast<void*>(const_cast< Q3Process*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3Process::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readyReadStdout(); break;
        case 1: readyReadStderr(); break;
        case 2: processExited(); break;
        case 3: wroteToStdin(); break;
        case 4: launchFinished(); break;
        case 5: tryTerminate(); break;
        case 6: kill(); break;
        case 7: writeToStdin((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 8: writeToStdin((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: closeStdin(); break;
        case 10: socketRead((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: socketWrite((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: timeout(); break;
        case 13: closeStdinLaunch(); break;
        }
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Q3Process::readyReadStdout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Q3Process::readyReadStderr()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Q3Process::processExited()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Q3Process::wroteToStdin()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Q3Process::launchFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
