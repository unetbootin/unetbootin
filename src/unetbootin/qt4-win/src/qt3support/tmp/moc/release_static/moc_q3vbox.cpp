/****************************************************************************
** Meta object code from reading C++ file 'q3vbox.h'
**
** Created: Fri May 30 22:41:19 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/q3vbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3vbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3VBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_Q3VBox[] = {
    "Q3VBox\0"
};

const QMetaObject Q3VBox::staticMetaObject = {
    { &Q3HBox::staticMetaObject, qt_meta_stringdata_Q3VBox,
      qt_meta_data_Q3VBox, 0 }
};

const QMetaObject *Q3VBox::metaObject() const
{
    return &staticMetaObject;
}

void *Q3VBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3VBox))
	return static_cast<void*>(const_cast< Q3VBox*>(this));
    return Q3HBox::qt_metacast(_clname);
}

int Q3VBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3HBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
