/****************************************************************************
** Meta object code from reading C++ file 'qcoreapplication.h'
**
** Created: Thu Mar 5 18:40:46 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qcoreapplication.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcoreapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCoreApplication[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       4,   27, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      32,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,   17,   17,   17, 0x0a,

 // properties: name, type, flags
      63,   55, 0x0a095103,
      79,   55, 0x0a095103,
      98,   55, 0x0a095103,
     115,   55, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QCoreApplication[] = {
    "QCoreApplication\0\0aboutToQuit()\0"
    "unixSignal(int)\0quit()\0QString\0"
    "applicationName\0applicationVersion\0"
    "organizationName\0organizationDomain\0"
};

const QMetaObject QCoreApplication::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QCoreApplication,
      qt_meta_data_QCoreApplication, 0 }
};

const QMetaObject *QCoreApplication::metaObject() const
{
    return &staticMetaObject;
}

void *QCoreApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCoreApplication))
        return static_cast<void*>(const_cast< QCoreApplication*>(this));
    return QObject::qt_metacast(_clname);
}

int QCoreApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aboutToQuit(); break;
        case 1: unixSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: quit(); break;
        default: ;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = applicationName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = applicationVersion(); break;
        case 2: *reinterpret_cast< QString*>(_v) = organizationName(); break;
        case 3: *reinterpret_cast< QString*>(_v) = organizationDomain(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setApplicationName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setApplicationVersion(*reinterpret_cast< QString*>(_v)); break;
        case 2: setOrganizationName(*reinterpret_cast< QString*>(_v)); break;
        case 3: setOrganizationDomain(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QCoreApplication::aboutToQuit()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QCoreApplication::unixSignal(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
