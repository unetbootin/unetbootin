/****************************************************************************
** Meta object code from reading C++ file 'qtabwidget.h'
**
** Created: Thu Mar 5 18:52:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qtabwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTabWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
      10,   72, // properties
       2,  112, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,
      38,   12,   11,   11, 0x05,
      61,   11,   11,   11, 0x15,
      86,   11,   11,   11, 0x15,

 // slots: signature, parameters, type, tag, flags
     104,   12,   11,   11, 0x0a,
     132,  125,   11,   11, 0x0a,
     159,   12,   11,   11, 0x1a,
     181,  179,   11,   11, 0x1a,
     200,  179,   11,   11, 0x1a,
     221,   11,   11,   11, 0x08,
     237,   11,   11,   11, 0x08,
     257,  255,   11,   11, 0x08,

 // properties: name, type, flags
     290,  278, 0x0009510b,
     311,  302, 0x0009510b,
     324,  320, 0x02495103,
     337,  320, 0x02095001,
     349,  343, 0x15095103,
     376,  358, 0x0009510b,
     391,  386, 0x01095103,
     409,  386, 0x01095103,
     422,  386, 0x01095103,
     435,  386, 0x01095103,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

 // enums: name, flags, count, data
     278, 0x0,    4,  120,
     302, 0x0,    2,  128,

 // enum data: key, value
     443, uint(QTabWidget::North),
     449, uint(QTabWidget::South),
     455, uint(QTabWidget::West),
     460, uint(QTabWidget::East),
     465, uint(QTabWidget::Rounded),
     473, uint(QTabWidget::Triangular),

       0        // eod
};

static const char qt_meta_stringdata_QTabWidget[] = {
    "QTabWidget\0\0index\0currentChanged(int)\0"
    "tabCloseRequested(int)\0currentChanged(QWidget*)\0"
    "selected(QString)\0setCurrentIndex(int)\0"
    "widget\0setCurrentWidget(QWidget*)\0"
    "setCurrentPage(int)\0w\0showPage(QWidget*)\0"
    "removePage(QWidget*)\0_q_showTab(int)\0"
    "_q_removeTab(int)\0,\0_q_tabMoved(int,int)\0"
    "TabPosition\0tabPosition\0TabShape\0"
    "tabShape\0int\0currentIndex\0count\0QSize\0"
    "iconSize\0Qt::TextElideMode\0elideMode\0"
    "bool\0usesScrollButtons\0documentMode\0"
    "tabsClosable\0movable\0North\0South\0West\0"
    "East\0Rounded\0Triangular\0"
};

const QMetaObject QTabWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QTabWidget,
      qt_meta_data_QTabWidget, 0 }
};

const QMetaObject *QTabWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QTabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTabWidget))
        return static_cast<void*>(const_cast< QTabWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QTabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: tabCloseRequested((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: currentChanged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 3: selected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setCurrentWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 6: setCurrentPage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: showPage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 8: removePage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 9: d_func()->_q_showTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: d_func()->_q_removeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: d_func()->_q_tabMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< TabPosition*>(_v) = tabPosition(); break;
        case 1: *reinterpret_cast< TabShape*>(_v) = tabShape(); break;
        case 2: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        case 3: *reinterpret_cast< int*>(_v) = count(); break;
        case 4: *reinterpret_cast< QSize*>(_v) = iconSize(); break;
        case 5: *reinterpret_cast< Qt::TextElideMode*>(_v) = elideMode(); break;
        case 6: *reinterpret_cast< bool*>(_v) = usesScrollButtons(); break;
        case 7: *reinterpret_cast< bool*>(_v) = documentMode(); break;
        case 8: *reinterpret_cast< bool*>(_v) = tabsClosable(); break;
        case 9: *reinterpret_cast< bool*>(_v) = isMovable(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTabPosition(*reinterpret_cast< TabPosition*>(_v)); break;
        case 1: setTabShape(*reinterpret_cast< TabShape*>(_v)); break;
        case 2: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        case 4: setIconSize(*reinterpret_cast< QSize*>(_v)); break;
        case 5: setElideMode(*reinterpret_cast< Qt::TextElideMode*>(_v)); break;
        case 6: setUsesScrollButtons(*reinterpret_cast< bool*>(_v)); break;
        case 7: setDocumentMode(*reinterpret_cast< bool*>(_v)); break;
        case 8: setTabsClosable(*reinterpret_cast< bool*>(_v)); break;
        case 9: setMovable(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QTabWidget::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTabWidget::tabCloseRequested(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTabWidget::currentChanged(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QTabWidget::selected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
