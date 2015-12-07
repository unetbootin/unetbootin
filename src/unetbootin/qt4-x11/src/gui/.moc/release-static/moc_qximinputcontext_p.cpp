/****************************************************************************
** Meta object code from reading C++ file 'qximinputcontext_p.h'
**
** Created: Thu Mar 5 18:57:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../inputmethod/qximinputcontext_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qximinputcontext_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QXIMInputContext[] = {

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

static const char qt_meta_stringdata_QXIMInputContext[] = {
    "QXIMInputContext\0"
};

const QMetaObject QXIMInputContext::staticMetaObject = {
    { &QInputContext::staticMetaObject, qt_meta_stringdata_QXIMInputContext,
      qt_meta_data_QXIMInputContext, 0 }
};

const QMetaObject *QXIMInputContext::metaObject() const
{
    return &staticMetaObject;
}

void *QXIMInputContext::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QXIMInputContext))
        return static_cast<void*>(const_cast< QXIMInputContext*>(this));
    return QInputContext::qt_metacast(_clname);
}

int QXIMInputContext::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QInputContext::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
