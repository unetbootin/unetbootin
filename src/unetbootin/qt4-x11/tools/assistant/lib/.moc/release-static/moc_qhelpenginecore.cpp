/****************************************************************************
** Meta object code from reading C++ file 'qhelpenginecore.h'
**
** Created: Thu Mar 5 20:32:27 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpenginecore.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpenginecore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHelpEngineCore[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       3,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      32,   16,   16,   16, 0x05,
      58,   48,   16,   16, 0x05,
      92,   88,   16,   16, 0x05,

 // properties: name, type, flags
     114,  109, 0x01095103,
     137,  129, 0x0a095103,
     152,  129, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QHelpEngineCore[] = {
    "QHelpEngineCore\0\0setupStarted()\0"
    "setupFinished()\0newFilter\0"
    "currentFilterChanged(QString)\0msg\0"
    "warning(QString)\0bool\0autoSaveFilter\0"
    "QString\0collectionFile\0currentFilter\0"
};

const QMetaObject QHelpEngineCore::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHelpEngineCore,
      qt_meta_data_QHelpEngineCore, 0 }
};

const QMetaObject *QHelpEngineCore::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpEngineCore::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpEngineCore))
        return static_cast<void*>(const_cast< QHelpEngineCore*>(this));
    return QObject::qt_metacast(_clname);
}

int QHelpEngineCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setupStarted(); break;
        case 1: setupFinished(); break;
        case 2: currentFilterChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: warning((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = autoSaveFilter(); break;
        case 1: *reinterpret_cast< QString*>(_v) = collectionFile(); break;
        case 2: *reinterpret_cast< QString*>(_v) = currentFilter(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAutoSaveFilter(*reinterpret_cast< bool*>(_v)); break;
        case 1: setCollectionFile(*reinterpret_cast< QString*>(_v)); break;
        case 2: setCurrentFilter(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QHelpEngineCore::setupStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QHelpEngineCore::setupFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QHelpEngineCore::currentFilterChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QHelpEngineCore::warning(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
