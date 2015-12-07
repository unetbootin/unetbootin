/****************************************************************************
** Meta object code from reading C++ file 'qthread.h'
**
** Created: Thu Mar 5 18:40:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../thread/qthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QThread[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      19,    8,    8,    8, 0x05,
      30,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,    8,    8,    8, 0x0a,
      59,    8,    8,    8, 0x2a,
      67,    8,    8,    8, 0x0a,
      79,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QThread[] = {
    "QThread\0\0started()\0finished()\0"
    "terminated()\0start(Priority)\0start()\0"
    "terminate()\0quit()\0"
};

const QMetaObject QThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QThread,
      qt_meta_data_QThread, 0 }
};

const QMetaObject *QThread::metaObject() const
{
    return &staticMetaObject;
}

void *QThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QThread))
        return static_cast<void*>(const_cast< QThread*>(this));
    return QObject::qt_metacast(_clname);
}

int QThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: started(); break;
        case 1: finished(); break;
        case 2: terminated(); break;
        case 3: start((*reinterpret_cast< Priority(*)>(_a[1]))); break;
        case 4: start(); break;
        case 5: terminate(); break;
        case 6: quit(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QThread::started()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QThread::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QThread::terminated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
