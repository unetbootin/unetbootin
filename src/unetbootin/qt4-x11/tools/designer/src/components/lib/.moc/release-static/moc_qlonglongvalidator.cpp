/****************************************************************************
** Meta object code from reading C++ file 'qlonglongvalidator.h'
**
** Created: Thu Mar 5 20:51:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../propertyeditor/qlonglongvalidator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlonglongvalidator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__QLongLongValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      49,   39, 0x04095103,
      56,   39, 0x04095103,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__QLongLongValidator[] = {
    "qdesigner_internal::QLongLongValidator\0"
    "qlonglong\0bottom\0top\0"
};

const QMetaObject qdesigner_internal::QLongLongValidator::staticMetaObject = {
    { &QValidator::staticMetaObject, qt_meta_stringdata_qdesigner_internal__QLongLongValidator,
      qt_meta_data_qdesigner_internal__QLongLongValidator, 0 }
};

const QMetaObject *qdesigner_internal::QLongLongValidator::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::QLongLongValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__QLongLongValidator))
        return static_cast<void*>(const_cast< QLongLongValidator*>(this));
    return QValidator::qt_metacast(_clname);
}

int qdesigner_internal::QLongLongValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qlonglong*>(_v) = bottom(); break;
        case 1: *reinterpret_cast< qlonglong*>(_v) = top(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBottom(*reinterpret_cast< qlonglong*>(_v)); break;
        case 1: setTop(*reinterpret_cast< qlonglong*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__QULongLongValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      51,   40, 0x05095103,
      58,   40, 0x05095103,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__QULongLongValidator[] = {
    "qdesigner_internal::QULongLongValidator\0"
    "qulonglong\0bottom\0top\0"
};

const QMetaObject qdesigner_internal::QULongLongValidator::staticMetaObject = {
    { &QValidator::staticMetaObject, qt_meta_stringdata_qdesigner_internal__QULongLongValidator,
      qt_meta_data_qdesigner_internal__QULongLongValidator, 0 }
};

const QMetaObject *qdesigner_internal::QULongLongValidator::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::QULongLongValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__QULongLongValidator))
        return static_cast<void*>(const_cast< QULongLongValidator*>(this));
    return QValidator::qt_metacast(_clname);
}

int qdesigner_internal::QULongLongValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qulonglong*>(_v) = bottom(); break;
        case 1: *reinterpret_cast< qulonglong*>(_v) = top(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBottom(*reinterpret_cast< qulonglong*>(_v)); break;
        case 1: setTop(*reinterpret_cast< qulonglong*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
