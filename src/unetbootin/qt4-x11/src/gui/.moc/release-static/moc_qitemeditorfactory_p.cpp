/****************************************************************************
** Meta object code from reading C++ file 'qitemeditorfactory_p.h'
**
** Created: Thu Mar 5 18:57:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qitemeditorfactory_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qitemeditorfactory_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QExpandingLineEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QExpandingLineEdit[] = {
    "QExpandingLineEdit\0\0resizeToContents()\0"
};

const QMetaObject QExpandingLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_QExpandingLineEdit,
      qt_meta_data_QExpandingLineEdit, 0 }
};

const QMetaObject *QExpandingLineEdit::metaObject() const
{
    return &staticMetaObject;
}

void *QExpandingLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QExpandingLineEdit))
        return static_cast<void*>(const_cast< QExpandingLineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int QExpandingLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resizeToContents(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
