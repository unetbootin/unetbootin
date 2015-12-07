/****************************************************************************
** Meta object code from reading C++ file 'qhelpsearchengine.h'
**
** Created: Thu Mar 5 20:32:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpsearchengine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpsearchengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHelpSearchEngine[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      37,   18,   18,   18, 0x05,
      56,   18,   18,   18, 0x05,
      80,   75,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     103,   18,   18,   18, 0x0a,
     126,   18,   18,   18, 0x0a,
     153,  143,   18,   18, 0x0a,
     185,   18,   18,   18, 0x0a,
     203,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHelpSearchEngine[] = {
    "QHelpSearchEngine\0\0indexingStarted()\0"
    "indexingFinished()\0searchingStarted()\0"
    "hits\0searchingFinished(int)\0"
    "reindexDocumentation()\0cancelIndexing()\0"
    "queryList\0search(QList<QHelpSearchQuery>)\0"
    "cancelSearching()\0indexDocumentation()\0"
};

const QMetaObject QHelpSearchEngine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHelpSearchEngine,
      qt_meta_data_QHelpSearchEngine, 0 }
};

const QMetaObject *QHelpSearchEngine::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpSearchEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpSearchEngine))
        return static_cast<void*>(const_cast< QHelpSearchEngine*>(this));
    return QObject::qt_metacast(_clname);
}

int QHelpSearchEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexingStarted(); break;
        case 1: indexingFinished(); break;
        case 2: searchingStarted(); break;
        case 3: searchingFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: reindexDocumentation(); break;
        case 5: cancelIndexing(); break;
        case 6: search((*reinterpret_cast< const QList<QHelpSearchQuery>(*)>(_a[1]))); break;
        case 7: cancelSearching(); break;
        case 8: indexDocumentation(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QHelpSearchEngine::indexingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QHelpSearchEngine::indexingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QHelpSearchEngine::searchingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QHelpSearchEngine::searchingFinished(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
