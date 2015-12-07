/****************************************************************************
** Meta object code from reading C++ file 'qsignalmapper.h'
**
** Created: Fri May 30 22:17:47 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qsignalmapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsignalmapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSignalMapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      27,   14,   14,   14, 0x05,
      43,   14,   14,   14, 0x05,
      60,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   14,   14,   14, 0x0a,
      90,   83,   14,   14, 0x0a,
     104,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QSignalMapper[] = {
    "QSignalMapper\0\0mapped(int)\0mapped(QString)\0"
    "mapped(QWidget*)\0mapped(QObject*)\0"
    "map()\0sender\0map(QObject*)\0"
    "_q_senderDestroyed()\0"
};

const QMetaObject QSignalMapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QSignalMapper,
      qt_meta_data_QSignalMapper, 0 }
};

const QMetaObject *QSignalMapper::metaObject() const
{
    return &staticMetaObject;
}

void *QSignalMapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSignalMapper))
	return static_cast<void*>(const_cast< QSignalMapper*>(this));
    return QObject::qt_metacast(_clname);
}

int QSignalMapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: mapped((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: mapped((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: mapped((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: mapped((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 4: map(); break;
        case 5: map((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 6: d_func()->_q_senderDestroyed(); break;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QSignalMapper::mapped(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QSignalMapper::mapped(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QSignalMapper::mapped(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QSignalMapper::mapped(QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
