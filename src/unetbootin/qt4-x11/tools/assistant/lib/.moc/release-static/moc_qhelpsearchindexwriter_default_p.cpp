/****************************************************************************
** Meta object code from reading C++ file 'qhelpsearchindexwriter_default_p.h'
**
** Created: Thu Mar 5 20:32:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpsearchindexwriter_default_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpsearchindexwriter_default_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qt__fulltextsearch__std__QHelpSearchIndexWriter[] = {

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
      67,   48,   48,   48, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_qt__fulltextsearch__std__QHelpSearchIndexWriter[] = {
    "qt::fulltextsearch::std::QHelpSearchIndexWriter\0"
    "\0indexingStarted()\0indexingFinished()\0"
};

const QMetaObject qt::fulltextsearch::std::QHelpSearchIndexWriter::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_qt__fulltextsearch__std__QHelpSearchIndexWriter,
      qt_meta_data_qt__fulltextsearch__std__QHelpSearchIndexWriter, 0 }
};

const QMetaObject *qt::fulltextsearch::std::QHelpSearchIndexWriter::metaObject() const
{
    return &staticMetaObject;
}

void *qt::fulltextsearch::std::QHelpSearchIndexWriter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qt__fulltextsearch__std__QHelpSearchIndexWriter))
        return static_cast<void*>(const_cast< QHelpSearchIndexWriter*>(this));
    return QThread::qt_metacast(_clname);
}

int qt::fulltextsearch::std::QHelpSearchIndexWriter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexingStarted(); break;
        case 1: indexingFinished(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void qt::fulltextsearch::std::QHelpSearchIndexWriter::indexingStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void qt::fulltextsearch::std::QHelpSearchIndexWriter::indexingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
