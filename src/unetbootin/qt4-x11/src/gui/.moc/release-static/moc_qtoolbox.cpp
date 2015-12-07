/****************************************************************************
** Meta object code from reading C++ file 'qtoolbox.h'
**
** Created: Thu Mar 5 18:52:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qtoolbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtoolbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QToolBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       2,   37, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   10,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   10,    9,    9, 0x0a,
      64,   57,    9,    9, 0x0a,
      91,    9,    9,    9, 0x08,
     110,    9,    9,    9, 0x08,

 // properties: name, type, flags
     143,  139, 0x02495103,
     156,  139, 0x02095001,

 // properties: notify_signal_id
       0,
       0,

       0        // eod
};

static const char qt_meta_stringdata_QToolBox[] = {
    "QToolBox\0\0index\0currentChanged(int)\0"
    "setCurrentIndex(int)\0widget\0"
    "setCurrentWidget(QWidget*)\0"
    "_q_buttonClicked()\0_q_widgetDestroyed(QObject*)\0"
    "int\0currentIndex\0count\0"
};

const QMetaObject QToolBox::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QToolBox,
      qt_meta_data_QToolBox, 0 }
};

const QMetaObject *QToolBox::metaObject() const
{
    return &staticMetaObject;
}

void *QToolBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QToolBox))
        return static_cast<void*>(const_cast< QToolBox*>(this));
    return QFrame::qt_metacast(_clname);
}

int QToolBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setCurrentWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: d_func()->_q_buttonClicked(); break;
        case 4: d_func()->_q_widgetDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        case 1: *reinterpret_cast< int*>(_v) = count(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QToolBox::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
