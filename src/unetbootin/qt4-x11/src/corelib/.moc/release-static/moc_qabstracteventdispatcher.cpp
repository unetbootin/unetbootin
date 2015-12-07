/****************************************************************************
** Meta object code from reading C++ file 'qabstracteventdispatcher.h'
**
** Created: Thu Mar 5 18:40:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qabstracteventdispatcher.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstracteventdispatcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractEventDispatcher[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,
      41,   25,   25,   25, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractEventDispatcher[] = {
    "QAbstractEventDispatcher\0\0aboutToBlock()\0"
    "awake()\0"
};

const QMetaObject QAbstractEventDispatcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAbstractEventDispatcher,
      qt_meta_data_QAbstractEventDispatcher, 0 }
};

const QMetaObject *QAbstractEventDispatcher::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractEventDispatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractEventDispatcher))
        return static_cast<void*>(const_cast< QAbstractEventDispatcher*>(this));
    return QObject::qt_metacast(_clname);
}

int QAbstractEventDispatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aboutToBlock(); break;
        case 1: awake(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QAbstractEventDispatcher::aboutToBlock()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QAbstractEventDispatcher::awake()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
