/****************************************************************************
** Meta object code from reading C++ file 'qtgradientmanager.h'
**
** Created: Thu Mar 5 20:46:50 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradientmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradientmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      31,   19,   18,   18, 0x05,
      73,   64,   18,   18, 0x05,
     121,  106,   18,   18, 0x05,
     159,  156,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     192,   19,  184,   18, 0x0a,
     223,   64,   18,   18, 0x0a,
     255,  106,   18,   18, 0x0a,
     289,  156,   18,   18, 0x0a,
     313,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientManager[] = {
    "QtGradientManager\0\0id,gradient\0"
    "gradientAdded(QString,QGradient)\0"
    "id,newId\0gradientRenamed(QString,QString)\0"
    "id,newGradient\0gradientChanged(QString,QGradient)\0"
    "id\0gradientRemoved(QString)\0QString\0"
    "addGradient(QString,QGradient)\0"
    "renameGradient(QString,QString)\0"
    "changeGradient(QString,QGradient)\0"
    "removeGradient(QString)\0clear()\0"
};

const QMetaObject QtGradientManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtGradientManager,
      qt_meta_data_QtGradientManager, 0 }
};

const QMetaObject *QtGradientManager::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientManager))
        return static_cast<void*>(const_cast< QtGradientManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QtGradientManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: gradientAdded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QGradient(*)>(_a[2]))); break;
        case 1: gradientRenamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: gradientChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QGradient(*)>(_a[2]))); break;
        case 3: gradientRemoved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: { QString _r = addGradient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QGradient(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 5: renameGradient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: changeGradient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QGradient(*)>(_a[2]))); break;
        case 7: removeGradient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: clear(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QtGradientManager::gradientAdded(const QString & _t1, const QGradient & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtGradientManager::gradientRenamed(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtGradientManager::gradientChanged(const QString & _t1, const QGradient & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtGradientManager::gradientRemoved(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
