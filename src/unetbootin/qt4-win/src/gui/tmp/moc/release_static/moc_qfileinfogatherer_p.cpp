/****************************************************************************
** Meta object code from reading C++ file 'qfileinfogatherer_p.h'
**
** Created: Fri May 30 22:34:02 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qfileinfogatherer_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfileinfogatherer_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFileInfoGatherer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      37,   19,   18,   18, 0x05,
     109,   87,   18,   18, 0x05,
     167,  145,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     211,  197,   18,   18, 0x0a,
     236,  225,   18,   18, 0x0a,
     287,  282,   18,   18, 0x0a,
     314,  307,   18,   18, 0x0a,
     344,   18,  339,   18, 0x0a,
     371,  362,   18,   18, 0x0a,
     426,   18,  407,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QFileInfoGatherer[] = {
    "QFileInfoGatherer\0\0directory,updates\0"
    "updates(QString,QList<QPair<QString,QFileInfo> >)\0"
    "directory,listOfFiles\0"
    "newListOfFiles(QString,QStringList)\0"
    "fileName,resolvedName\0"
    "nameResolved(QString,QString)\0"
    "directoryPath\0list(QString)\0path,files\0"
    "fetchExtendedInformation(QString,QStringList)\0"
    "path\0updateFile(QString)\0enable\0"
    "setResolveSymlinks(bool)\0bool\0"
    "resolveSymlinks()\0provider\0"
    "setIconProvider(QFileIconProvider*)\0"
    "QFileIconProvider*\0iconProvider()\0"
};

const QMetaObject QFileInfoGatherer::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QFileInfoGatherer,
      qt_meta_data_QFileInfoGatherer, 0 }
};

const QMetaObject *QFileInfoGatherer::metaObject() const
{
    return &staticMetaObject;
}

void *QFileInfoGatherer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFileInfoGatherer))
	return static_cast<void*>(const_cast< QFileInfoGatherer*>(this));
    return QThread::qt_metacast(_clname);
}

int QFileInfoGatherer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updates((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QList<QPair<QString,QFileInfo> >(*)>(_a[2]))); break;
        case 1: newListOfFiles((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 2: nameResolved((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: list((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: fetchExtendedInformation((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 5: updateFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: setResolveSymlinks((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: { bool _r = resolveSymlinks();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: setIconProvider((*reinterpret_cast< QFileIconProvider*(*)>(_a[1]))); break;
        case 9: { QFileIconProvider* _r = iconProvider();
            if (_a[0]) *reinterpret_cast< QFileIconProvider**>(_a[0]) = _r; }  break;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QFileInfoGatherer::updates(const QString & _t1, const QList<QPair<QString,QFileInfo> > & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFileInfoGatherer::newListOfFiles(const QString & _t1, const QStringList & _t2)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(const_cast< QFileInfoGatherer *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFileInfoGatherer::nameResolved(const QString & _t1, const QString & _t2)const
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(const_cast< QFileInfoGatherer *>(this), &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
