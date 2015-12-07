/****************************************************************************
** Meta object code from reading C++ file 'qaccessibleplugin.h'
**
** Created: Fri May 30 22:34:05 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../accessible/qaccessibleplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qaccessibleplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAccessiblePlugin[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QAccessiblePlugin[] = {
    "QAccessiblePlugin\0"
};

const QMetaObject QAccessiblePlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAccessiblePlugin,
      qt_meta_data_QAccessiblePlugin, 0 }
};

const QMetaObject *QAccessiblePlugin::metaObject() const
{
    return &staticMetaObject;
}

void *QAccessiblePlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAccessiblePlugin))
	return static_cast<void*>(const_cast< QAccessiblePlugin*>(this));
    if (!strcmp(_clname, "QAccessibleFactoryInterface"))
	return static_cast< QAccessibleFactoryInterface*>(const_cast< QAccessiblePlugin*>(this));
    if (!strcmp(_clname, QAccessibleFactoryInterface_iid))
	return static_cast< QAccessibleFactoryInterface*>(const_cast< QAccessiblePlugin*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.QFactoryInterface"))
	return static_cast< QFactoryInterface*>(static_cast< QAccessibleFactoryInterface*>(const_cast< QAccessiblePlugin*>(this)));
    return QObject::qt_metacast(_clname);
}

int QAccessiblePlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
