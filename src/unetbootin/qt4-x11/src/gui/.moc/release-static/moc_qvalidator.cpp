/****************************************************************************
** Meta object code from reading C++ file 'qvalidator.h'
**
** Created: Thu Mar 5 18:57:27 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qvalidator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qvalidator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QValidator[] = {
    "QValidator\0"
};

const QMetaObject QValidator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QValidator,
      qt_meta_data_QValidator, 0 }
};

const QMetaObject *QValidator::metaObject() const
{
    return &staticMetaObject;
}

void *QValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QValidator))
        return static_cast<void*>(const_cast< QValidator*>(this));
    return QObject::qt_metacast(_clname);
}

int QValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QIntValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      18,   14, 0x02095103,
      25,   14, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_QIntValidator[] = {
    "QIntValidator\0int\0bottom\0top\0"
};

const QMetaObject QIntValidator::staticMetaObject = {
    { &QValidator::staticMetaObject, qt_meta_stringdata_QIntValidator,
      qt_meta_data_QIntValidator, 0 }
};

const QMetaObject *QIntValidator::metaObject() const
{
    return &staticMetaObject;
}

void *QIntValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QIntValidator))
        return static_cast<void*>(const_cast< QIntValidator*>(this));
    return QValidator::qt_metacast(_clname);
}

int QIntValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = bottom(); break;
        case 1: *reinterpret_cast< int*>(_v) = top(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBottom(*reinterpret_cast< int*>(_v)); break;
        case 1: setTop(*reinterpret_cast< int*>(_v)); break;
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
static const uint qt_meta_data_QDoubleValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      24,   17, 0x06095103,
      31,   17, 0x06095103,
      39,   35, 0x02095103,
      57,   48, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_QDoubleValidator[] = {
    "QDoubleValidator\0double\0bottom\0top\0"
    "int\0decimals\0Notation\0notation\0"
};

const QMetaObject QDoubleValidator::staticMetaObject = {
    { &QValidator::staticMetaObject, qt_meta_stringdata_QDoubleValidator,
      qt_meta_data_QDoubleValidator, 0 }
};

const QMetaObject *QDoubleValidator::metaObject() const
{
    return &staticMetaObject;
}

void *QDoubleValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDoubleValidator))
        return static_cast<void*>(const_cast< QDoubleValidator*>(this));
    return QValidator::qt_metacast(_clname);
}

int QDoubleValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = bottom(); break;
        case 1: *reinterpret_cast< double*>(_v) = top(); break;
        case 2: *reinterpret_cast< int*>(_v) = decimals(); break;
        case 3: *reinterpret_cast< Notation*>(_v) = notation(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBottom(*reinterpret_cast< double*>(_v)); break;
        case 1: setTop(*reinterpret_cast< double*>(_v)); break;
        case 2: setDecimals(*reinterpret_cast< int*>(_v)); break;
        case 3: setNotation(*reinterpret_cast< Notation*>(_v)); break;
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
static const uint qt_meta_data_QRegExpValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      25,   17, 0x1b095103,

       0        // eod
};

static const char qt_meta_stringdata_QRegExpValidator[] = {
    "QRegExpValidator\0QRegExp\0regExp\0"
};

const QMetaObject QRegExpValidator::staticMetaObject = {
    { &QValidator::staticMetaObject, qt_meta_stringdata_QRegExpValidator,
      qt_meta_data_QRegExpValidator, 0 }
};

const QMetaObject *QRegExpValidator::metaObject() const
{
    return &staticMetaObject;
}

void *QRegExpValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QRegExpValidator))
        return static_cast<void*>(const_cast< QRegExpValidator*>(this));
    return QValidator::qt_metacast(_clname);
}

int QRegExpValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QRegExp*>(_v) = regExp(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setRegExp(*reinterpret_cast< QRegExp*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
