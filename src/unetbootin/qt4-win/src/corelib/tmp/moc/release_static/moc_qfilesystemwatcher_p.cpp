/****************************************************************************
** Meta object code from reading C++ file 'qfilesystemwatcher_p.h'
**
** Created: Fri May 30 22:18:24 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../io/qfilesystemwatcher_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfilesystemwatcher_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFileSystemWatcherEngine[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      39,   26,   25,   25, 0x05,
      65,   26,   25,   25, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QFileSystemWatcherEngine[] = {
    "QFileSystemWatcherEngine\0\0path,removed\0"
    "fileChanged(QString,bool)\0"
    "directoryChanged(QString,bool)\0"
};

const QMetaObject QFileSystemWatcherEngine::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QFileSystemWatcherEngine,
      qt_meta_data_QFileSystemWatcherEngine, 0 }
};

const QMetaObject *QFileSystemWatcherEngine::metaObject() const
{
    return &staticMetaObject;
}

void *QFileSystemWatcherEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFileSystemWatcherEngine))
	return static_cast<void*>(const_cast< QFileSystemWatcherEngine*>(this));
    return QThread::qt_metacast(_clname);
}

int QFileSystemWatcherEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: directoryChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QFileSystemWatcherEngine::fileChanged(const QString & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFileSystemWatcherEngine::directoryChanged(const QString & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
