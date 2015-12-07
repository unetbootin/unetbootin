/****************************************************************************
** Meta object code from reading C++ file 'qstackedwidget.h'
**
** Created: Fri May 30 22:33:51 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qstackedwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qstackedwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QStackedWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       2,   30, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      42,   36,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,   36,   15,   15, 0x0a,
      84,   82,   15,   15, 0x0a,

 // properties: name, type, flags
     115,  111, 0x02095103,
     128,  111, 0x02095001,

       0        // eod
};

static const char qt_meta_stringdata_QStackedWidget[] = {
    "QStackedWidget\0\0currentChanged(int)\0"
    "index\0widgetRemoved(int)\0setCurrentIndex(int)\0"
    "w\0setCurrentWidget(QWidget*)\0int\0"
    "currentIndex\0count\0"
};

const QMetaObject QStackedWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QStackedWidget,
      qt_meta_data_QStackedWidget, 0 }
};

const QMetaObject *QStackedWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QStackedWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStackedWidget))
	return static_cast<void*>(const_cast< QStackedWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int QStackedWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: widgetRemoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setCurrentWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        }
        _id -= 4;
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
void QStackedWidget::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QStackedWidget::widgetRemoved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
