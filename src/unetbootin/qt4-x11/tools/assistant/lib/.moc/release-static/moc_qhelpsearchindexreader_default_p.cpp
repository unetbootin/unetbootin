/****************************************************************************
** Meta object code from reading C++ file 'qhelpsearchindexreader_default_p.h'
**
** Created: Thu Mar 5 20:32:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpsearchindexreader_default_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpsearchindexreader_default_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qt__fulltextsearch__std__QHelpSearchIndexReader[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      49,   48,   48,   48, 0x05,
      73,   68,   48,   48, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_qt__fulltextsearch__std__QHelpSearchIndexReader[] = {
    "qt::fulltextsearch::std::QHelpSearchIndexReader\0"
    "\0searchingStarted()\0hits\0"
    "searchingFinished(int)\0"
};

const QMetaObject qt::fulltextsearch::std::QHelpSearchIndexReader::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_qt__fulltextsearch__std__QHelpSearchIndexReader,
      qt_meta_data_qt__fulltextsearch__std__QHelpSearchIndexReader, 0 }
};

const QMetaObject *qt::fulltextsearch::std::QHelpSearchIndexReader::metaObject() const
{
    return &staticMetaObject;
}

void *qt::fulltextsearch::std::QHelpSearchIndexReader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qt__fulltextsearch__std__QHelpSearchIndexReader))
        return static_cast<void*>(const_cast< QHelpSearchIndexReader*>(this));
    return QThread::qt_metacast(_clname);
}

int qt::fulltextsearch::std::QHelpSearchIndexReader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: searchingStarted(); break;
        case 1: searchingFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void qt::fulltextsearch::std::QHelpSearchIndexReader::searchingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void qt::fulltextsearch::std::QHelpSearchIndexReader::searchingFinished(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
