/****************************************************************************
** Meta object code from reading C++ file 'qiconengineplugin.h'
**
** Created: Fri May 30 22:33:15 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../image/qiconengineplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qiconengineplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QIconEnginePlugin[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QIconEnginePlugin[] = {
    "QIconEnginePlugin\0"
};

const QMetaObject QIconEnginePlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QIconEnginePlugin,
      qt_meta_data_QIconEnginePlugin, 0 }
};

const QMetaObject *QIconEnginePlugin::metaObject() const
{
    return &staticMetaObject;
}

void *QIconEnginePlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QIconEnginePlugin))
	return static_cast<void*>(const_cast< QIconEnginePlugin*>(this));
    if (!strcmp(_clname, "QIconEngineFactoryInterface"))
	return static_cast< QIconEngineFactoryInterface*>(const_cast< QIconEnginePlugin*>(this));
    if (!strcmp(_clname, QIconEngineFactoryInterface_iid))
	return static_cast< QIconEngineFactoryInterface*>(const_cast< QIconEnginePlugin*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.QFactoryInterface"))
	return static_cast< QFactoryInterface*>(static_cast< QIconEngineFactoryInterface*>(const_cast< QIconEnginePlugin*>(this)));
    return QObject::qt_metacast(_clname);
}

int QIconEnginePlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QIconEnginePluginV2[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QIconEnginePluginV2[] = {
    "QIconEnginePluginV2\0"
};

const QMetaObject QIconEnginePluginV2::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QIconEnginePluginV2,
      qt_meta_data_QIconEnginePluginV2, 0 }
};

const QMetaObject *QIconEnginePluginV2::metaObject() const
{
    return &staticMetaObject;
}

void *QIconEnginePluginV2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QIconEnginePluginV2))
	return static_cast<void*>(const_cast< QIconEnginePluginV2*>(this));
    if (!strcmp(_clname, "QIconEngineFactoryInterfaceV2"))
	return static_cast< QIconEngineFactoryInterfaceV2*>(const_cast< QIconEnginePluginV2*>(this));
    if (!strcmp(_clname, QIconEngineFactoryInterfaceV2_iid))
	return static_cast< QIconEngineFactoryInterfaceV2*>(const_cast< QIconEnginePluginV2*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.QFactoryInterface"))
	return static_cast< QFactoryInterface*>(static_cast< QIconEngineFactoryInterfaceV2*>(const_cast< QIconEnginePluginV2*>(this)));
    return QObject::qt_metacast(_clname);
}

int QIconEnginePluginV2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
