/****************************************************************************
** Meta object code from reading C++ file 'qimageiohandler.h'
**
** Created: Thu Mar 5 18:57:00 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../image/qimageiohandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qimageiohandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QImageIOPlugin[] = {

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

static const char qt_meta_stringdata_QImageIOPlugin[] = {
    "QImageIOPlugin\0"
};

const QMetaObject QImageIOPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QImageIOPlugin,
      qt_meta_data_QImageIOPlugin, 0 }
};

const QMetaObject *QImageIOPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *QImageIOPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QImageIOPlugin))
        return static_cast<void*>(const_cast< QImageIOPlugin*>(this));
    if (!strcmp(_clname, "QImageIOHandlerFactoryInterface"))
        return static_cast< QImageIOHandlerFactoryInterface*>(const_cast< QImageIOPlugin*>(this));
    if (!strcmp(_clname, QImageIOHandlerFactoryInterface_iid))
        return static_cast< QImageIOHandlerFactoryInterface*>(const_cast< QImageIOPlugin*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.QFactoryInterface"))
        return static_cast< QFactoryInterface*>(static_cast< QImageIOHandlerFactoryInterface*>(const_cast< QImageIOPlugin*>(this)));
    return QObject::qt_metacast(_clname);
}

int QImageIOPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
