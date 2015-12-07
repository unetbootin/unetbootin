/****************************************************************************
** Meta object code from reading C++ file 'q3widgetstack.h'
**
** Created: Thu Mar 5 19:06:02 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/q3widgetstack.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3widgetstack.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3WidgetStack[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      32,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      54,   14,   14,   14, 0x0a,
      71,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Q3WidgetStack[] = {
    "Q3WidgetStack\0\0aboutToShow(int)\0"
    "aboutToShow(QWidget*)\0raiseWidget(int)\0"
    "raiseWidget(QWidget*)\0"
};

const QMetaObject Q3WidgetStack::staticMetaObject = {
    { &Q3Frame::staticMetaObject, qt_meta_stringdata_Q3WidgetStack,
      qt_meta_data_Q3WidgetStack, 0 }
};

const QMetaObject *Q3WidgetStack::metaObject() const
{
    return &staticMetaObject;
}

void *Q3WidgetStack::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3WidgetStack))
        return static_cast<void*>(const_cast< Q3WidgetStack*>(this));
    return Q3Frame::qt_metacast(_clname);
}

int Q3WidgetStack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3Frame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aboutToShow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: aboutToShow((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: raiseWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: raiseWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Q3WidgetStack::aboutToShow(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3WidgetStack::aboutToShow(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
