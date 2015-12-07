/****************************************************************************
** Meta object code from reading C++ file 'rpptreeevaluator.h'
**
** Created: Thu Mar 5 20:38:37 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../rpptreeevaluator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rpptreeevaluator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Rpp__RppTreeEvaluator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      62,   23,   22,   22, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Rpp__RppTreeEvaluator[] = {
    "Rpp::RppTreeEvaluator\0\0"
    "includee,includer,filename,includeType\0"
    "includeCallback(Rpp::Source*&,const Rpp::Source*,QString,Rpp::RppTreeE"
    "valuator::IncludeType)\0"
};

const QMetaObject Rpp::RppTreeEvaluator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Rpp__RppTreeEvaluator,
      qt_meta_data_Rpp__RppTreeEvaluator, 0 }
};

const QMetaObject *Rpp::RppTreeEvaluator::metaObject() const
{
    return &staticMetaObject;
}

void *Rpp::RppTreeEvaluator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Rpp__RppTreeEvaluator))
        return static_cast<void*>(const_cast< RppTreeEvaluator*>(this));
    if (!strcmp(_clname, "RppTreeWalker"))
        return static_cast< RppTreeWalker*>(const_cast< RppTreeEvaluator*>(this));
    return QObject::qt_metacast(_clname);
}

int Rpp::RppTreeEvaluator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: includeCallback((*reinterpret_cast< Rpp::Source*(*)>(_a[1])),(*reinterpret_cast< const Rpp::Source*(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< Rpp::RppTreeEvaluator::IncludeType(*)>(_a[4]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Rpp::RppTreeEvaluator::includeCallback(Rpp::Source * & _t1, const Rpp::Source * _t2, const QString & _t3, Rpp::RppTreeEvaluator::IncludeType _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
