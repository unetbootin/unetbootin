/****************************************************************************
** Meta object code from reading C++ file 'qfilesystemmodel.h'
**
** Created: Fri May 30 22:30:15 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qfilesystemmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfilesystemmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFileSystemModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       3,   40, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      26,   18,   17,   17, 0x05,
      72,   51,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     124,  109,   17,   17, 0x08,
     165,   17,   17,   17, 0x08,
     195,  189,   17,   17, 0x08,
     280,  258,   17,   17, 0x08,

 // properties: name, type, flags
     318,  313, 0x01095103,
     334,  313, 0x01095103,
     343,  313, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QFileSystemModel[] = {
    "QFileSystemModel\0\0newPath\0"
    "rootPathChanged(QString)\0path,oldName,newName\0"
    "fileRenamed(QString,QString,QString)\0"
    "directory,list\0_q_directoryChanged(QString,QStringList)\0"
    "_q_performDelayedSort()\0path,\0"
    "_q_fileSystemChanged(QString,QList<QPair<QString,QFileInfo> >)\0"
    "fileName,resolvedName\0"
    "_q_resolvedName(QString,QString)\0bool\0"
    "resolveSymlinks\0readOnly\0nameFilterDisables\0"
};

const QMetaObject QFileSystemModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QFileSystemModel,
      qt_meta_data_QFileSystemModel, 0 }
};

const QMetaObject *QFileSystemModel::metaObject() const
{
    return &staticMetaObject;
}

void *QFileSystemModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFileSystemModel))
	return static_cast<void*>(const_cast< QFileSystemModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QFileSystemModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rootPathChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: fileRenamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: d_func()->_q_directoryChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: d_func()->_q_performDelayedSort(); break;
        case 4: d_func()->_q_fileSystemChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QList<QPair<QString,QFileInfo> >(*)>(_a[2]))); break;
        case 5: d_func()->_q_resolvedName((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        }
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = resolveSymlinks(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isReadOnly(); break;
        case 2: *reinterpret_cast< bool*>(_v) = nameFilterDisables(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setResolveSymlinks(*reinterpret_cast< bool*>(_v)); break;
        case 1: setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 2: setNameFilterDisables(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QFileSystemModel::rootPathChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFileSystemModel::fileRenamed(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
