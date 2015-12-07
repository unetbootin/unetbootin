/****************************************************************************
** Meta object code from reading C++ file 'qeventdispatcher_glib_p.h'
**
** Created: Thu Mar 5 18:40:49 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qeventdispatcher_glib_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qeventdispatcher_glib_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QEventDispatcherGlib[] = {

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

static const char qt_meta_stringdata_QEventDispatcherGlib[] = {
    "QEventDispatcherGlib\0"
};

const QMetaObject QEventDispatcherGlib::staticMetaObject = {
    { &QAbstractEventDispatcher::staticMetaObject, qt_meta_stringdata_QEventDispatcherGlib,
      qt_meta_data_QEventDispatcherGlib, 0 }
};

const QMetaObject *QEventDispatcherGlib::metaObject() const
{
    return &staticMetaObject;
}

void *QEventDispatcherGlib::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QEventDispatcherGlib))
        return static_cast<void*>(const_cast< QEventDispatcherGlib*>(this));
    return QAbstractEventDispatcher::qt_metacast(_clname);
}

int QEventDispatcherGlib::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractEventDispatcher::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
