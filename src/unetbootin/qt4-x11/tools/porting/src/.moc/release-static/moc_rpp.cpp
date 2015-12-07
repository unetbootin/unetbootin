/****************************************************************************
** Meta object code from reading C++ file 'rpp.h'
**
** Created: Thu Mar 5 20:38:36 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../rpp.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rpp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Rpp__Preprocessor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      32,   19,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Rpp__Preprocessor[] = {
    "Rpp::Preprocessor\0\0type,message\0"
    "error(QString,QString)\0"
};

const QMetaObject Rpp::Preprocessor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Rpp__Preprocessor,
      qt_meta_data_Rpp__Preprocessor, 0 }
};

const QMetaObject *Rpp::Preprocessor::metaObject() const
{
    return &staticMetaObject;
}

void *Rpp::Preprocessor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Rpp__Preprocessor))
        return static_cast<void*>(const_cast< Preprocessor*>(this));
    return QObject::qt_metacast(_clname);
}

int Rpp::Preprocessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: error((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Rpp::Preprocessor::error(const QString _t1, const QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
