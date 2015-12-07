/****************************************************************************
** Meta object code from reading C++ file 'qdesigner.h'
**
** Created: Thu Mar 5 20:53:33 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdesigner.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesigner[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   25,   10,   10, 0x0a,
      63,   10,   10,   10, 0x08,
      76,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDesigner[] = {
    "QDesigner\0\0initialized()\0message\0"
    "showErrorMessage(const char*)\0"
    "initialize()\0callCreateForm()\0"
};

const QMetaObject QDesigner::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_QDesigner,
      qt_meta_data_QDesigner, 0 }
};

const QMetaObject *QDesigner::metaObject() const
{
    return &staticMetaObject;
}

void *QDesigner::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesigner))
        return static_cast<void*>(const_cast< QDesigner*>(this));
    return QApplication::qt_metacast(_clname);
}

int QDesigner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initialized(); break;
        case 1: showErrorMessage((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 2: initialize(); break;
        case 3: callCreateForm(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QDesigner::initialized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
