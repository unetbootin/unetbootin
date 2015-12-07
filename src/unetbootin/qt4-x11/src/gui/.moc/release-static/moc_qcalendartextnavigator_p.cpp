/****************************************************************************
** Meta object code from reading C++ file 'qcalendartextnavigator_p.h'
**
** Created: Thu Mar 5 18:57:12 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qcalendartextnavigator_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcalendartextnavigator_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCalendarTextNavigator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      29,   24,   23,   23, 0x05,
      48,   23,   23,   23, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QCalendarTextNavigator[] = {
    "QCalendarTextNavigator\0\0date\0"
    "dateChanged(QDate)\0editingFinished()\0"
};

const QMetaObject QCalendarTextNavigator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QCalendarTextNavigator,
      qt_meta_data_QCalendarTextNavigator, 0 }
};

const QMetaObject *QCalendarTextNavigator::metaObject() const
{
    return &staticMetaObject;
}

void *QCalendarTextNavigator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCalendarTextNavigator))
        return static_cast<void*>(const_cast< QCalendarTextNavigator*>(this));
    return QObject::qt_metacast(_clname);
}

int QCalendarTextNavigator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dateChanged((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 1: editingFinished(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QCalendarTextNavigator::dateChanged(const QDate & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QCalendarTextNavigator::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
