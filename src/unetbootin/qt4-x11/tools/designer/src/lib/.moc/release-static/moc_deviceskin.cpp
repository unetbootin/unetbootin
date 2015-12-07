/****************************************************************************
** Meta object code from reading C++ file 'deviceskin.h'
**
** Created: Thu Mar 5 20:46:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/deviceskin/deviceskin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'deviceskin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceSkin[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      42,   24,   11,   11, 0x05,
      78,   24,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     116,   11,   11,   11, 0x09,
     132,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_DeviceSkin[] = {
    "DeviceSkin\0\0popupMenu()\0code,text,autorep\0"
    "skinKeyPressEvent(int,QString,bool)\0"
    "skinKeyReleaseEvent(int,QString,bool)\0"
    "skinKeyRepeat()\0moveParent()\0"
};

const QMetaObject DeviceSkin::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DeviceSkin,
      qt_meta_data_DeviceSkin, 0 }
};

const QMetaObject *DeviceSkin::metaObject() const
{
    return &staticMetaObject;
}

void *DeviceSkin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceSkin))
        return static_cast<void*>(const_cast< DeviceSkin*>(this));
    return QWidget::qt_metacast(_clname);
}

int DeviceSkin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: popupMenu(); break;
        case 1: skinKeyPressEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: skinKeyReleaseEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: skinKeyRepeat(); break;
        case 4: moveParent(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DeviceSkin::popupMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void DeviceSkin::skinKeyPressEvent(int _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceSkin::skinKeyReleaseEvent(int _t1, const QString & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
