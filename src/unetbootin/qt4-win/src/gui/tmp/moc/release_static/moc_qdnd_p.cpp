/****************************************************************************
** Meta object code from reading C++ file 'qdnd_p.h'
**
** Created: Fri May 30 22:33:06 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qdnd_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdnd_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QInternalMimeData[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QInternalMimeData[] = {
    "QInternalMimeData\0"
};

const QMetaObject QInternalMimeData::staticMetaObject = {
    { &QMimeData::staticMetaObject, qt_meta_stringdata_QInternalMimeData,
      qt_meta_data_QInternalMimeData, 0 }
};

const QMetaObject *QInternalMimeData::metaObject() const
{
    return &staticMetaObject;
}

void *QInternalMimeData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QInternalMimeData))
	return static_cast<void*>(const_cast< QInternalMimeData*>(this));
    return QMimeData::qt_metacast(_clname);
}

int QInternalMimeData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMimeData::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QDropData[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QDropData[] = {
    "QDropData\0"
};

const QMetaObject QDropData::staticMetaObject = {
    { &QInternalMimeData::staticMetaObject, qt_meta_stringdata_QDropData,
      qt_meta_data_QDropData, 0 }
};

const QMetaObject *QDropData::metaObject() const
{
    return &staticMetaObject;
}

void *QDropData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDropData))
	return static_cast<void*>(const_cast< QDropData*>(this));
    return QInternalMimeData::qt_metacast(_clname);
}

int QDropData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QInternalMimeData::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QDragManager[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QDragManager[] = {
    "QDragManager\0"
};

const QMetaObject QDragManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDragManager,
      qt_meta_data_QDragManager, 0 }
};

const QMetaObject *QDragManager::metaObject() const
{
    return &staticMetaObject;
}

void *QDragManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDragManager))
	return static_cast<void*>(const_cast< QDragManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QDragManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
