/****************************************************************************
** Meta object code from reading C++ file 'finddialog.h'
**
** Created: Thu Mar 5 20:55:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../finddialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'finddialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FindDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      52,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     104,   11,   11,   11, 0x08,
     119,   11,   11,   11, 0x08,
     139,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FindDialog[] = {
    "FindDialog\0\0text,where,matchCase,ignoreAccelerators\0"
    "findNext(QString,DataModel::FindLocation,bool,bool)\0"
    "emitFindNext()\0verifyText(QString)\0"
    "find()\0"
};

const QMetaObject FindDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FindDialog,
      qt_meta_data_FindDialog, 0 }
};

const QMetaObject *FindDialog::metaObject() const
{
    return &staticMetaObject;
}

void *FindDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FindDialog))
        return static_cast<void*>(const_cast< FindDialog*>(this));
    if (!strcmp(_clname, "Ui::FindDialog"))
        return static_cast< Ui::FindDialog*>(const_cast< FindDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int FindDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: findNext((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< DataModel::FindLocation(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 1: emitFindNext(); break;
        case 2: verifyText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: find(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void FindDialog::findNext(const QString & _t1, DataModel::FindLocation _t2, bool _t3, bool _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
