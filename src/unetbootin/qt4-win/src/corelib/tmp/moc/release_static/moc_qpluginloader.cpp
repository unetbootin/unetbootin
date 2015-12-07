/****************************************************************************
** Meta object code from reading C++ file 'qpluginloader.h'
**
** Created: Fri May 30 22:18:27 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugin/qpluginloader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qpluginloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPluginLoader[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   10, // properties
       0,    0, // enums/sets

 // properties: name, type, flags
      22,   14, 0x0a095103,
      51,   31, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_QPluginLoader[] = {
    "QPluginLoader\0QString\0fileName\0"
    "QLibrary::LoadHints\0loadHints\0"
};

static const QMetaObject *qt_meta_extradata_QPluginLoader[] = {
        &QLibrary::staticMetaObject,0
};

const QMetaObject QPluginLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QPluginLoader,
      qt_meta_data_QPluginLoader, qt_meta_extradata_QPluginLoader }
};

const QMetaObject *QPluginLoader::metaObject() const
{
    return &staticMetaObject;
}

void *QPluginLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPluginLoader))
	return static_cast<void*>(const_cast< QPluginLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int QPluginLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = fileName(); break;
        case 1: *reinterpret_cast< QLibrary::LoadHints*>(_v) = loadHints(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFileName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setLoadHints(*reinterpret_cast< QLibrary::LoadHints*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
