/****************************************************************************
** Meta object code from reading C++ file 'qabstracturiresolver.h'
**
** Created: Fri May 30 22:53:16 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../api/qabstracturiresolver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstracturiresolver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractUriResolver[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QAbstractUriResolver[] = {
    "QAbstractUriResolver\0"
};

const QMetaObject QAbstractUriResolver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAbstractUriResolver,
      qt_meta_data_QAbstractUriResolver, 0 }
};

const QMetaObject *QAbstractUriResolver::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractUriResolver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractUriResolver))
	return static_cast<void*>(const_cast< QAbstractUriResolver*>(this));
    return QObject::qt_metacast(_clname);
}

int QAbstractUriResolver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
