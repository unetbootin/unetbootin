/****************************************************************************
** Meta object code from reading C++ file 'qhelpcollectionhandler_p.h'
**
** Created: Thu Mar 5 20:32:38 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpcollectionhandler_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpcollectionhandler_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHelpCollectionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      28,   24,   23,   23, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QHelpCollectionHandler[] = {
    "QHelpCollectionHandler\0\0msg\0error(QString)\0"
};

const QMetaObject QHelpCollectionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHelpCollectionHandler,
      qt_meta_data_QHelpCollectionHandler, 0 }
};

const QMetaObject *QHelpCollectionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpCollectionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpCollectionHandler))
        return static_cast<void*>(const_cast< QHelpCollectionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int QHelpCollectionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QHelpCollectionHandler::error(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
