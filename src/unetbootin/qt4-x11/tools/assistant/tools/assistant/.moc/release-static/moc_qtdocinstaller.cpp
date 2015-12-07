/****************************************************************************
** Meta object code from reading C++ file 'qtdocinstaller.h'
**
** Created: Thu Mar 5 20:34:07 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qtdocinstaller.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtdocinstaller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtDocInstaller[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      20,   16,   15,   15, 0x05,
      59,   42,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QtDocInstaller[] = {
    "QtDocInstaller\0\0msg\0errorMessage(QString)\0"
    "newDocsInstalled\0docsInstalled(bool)\0"
};

const QMetaObject QtDocInstaller::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QtDocInstaller,
      qt_meta_data_QtDocInstaller, 0 }
};

const QMetaObject *QtDocInstaller::metaObject() const
{
    return &staticMetaObject;
}

void *QtDocInstaller::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtDocInstaller))
        return static_cast<void*>(const_cast< QtDocInstaller*>(this));
    return QThread::qt_metacast(_clname);
}

int QtDocInstaller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: errorMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: docsInstalled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QtDocInstaller::errorMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtDocInstaller::docsInstalled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
