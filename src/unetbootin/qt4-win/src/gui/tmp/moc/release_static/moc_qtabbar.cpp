/****************************************************************************
** Meta object code from reading C++ file 'qtabbar.h'
**
** Created: Fri May 30 22:28:41 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qtabbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtabbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTabBar[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       7,   35, // properties
       1,   56, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,    9,    8,    8, 0x05,
      35,    8,    8,    8, 0x15,

 // slots: signature, parameters, type, tag, flags
      49,    9,    8,    8, 0x0a,
      70,    9,    8,    8, 0x1a,
      89,    8,    8,    8, 0x08,

 // properties: name, type, flags
     111,  105, 0x0009510b,
     121,  117, 0x02095103,
     134,  117, 0x02095001,
     145,  140, 0x01095103,
     160,  154, 0x15095103,
     187,  169, 0x0009510b,
     197,  140, 0x01095103,

 // enums: name, flags, count, data
     105, 0x0,    8,   60,

 // enum data: key, value
     215, uint(QTabBar::RoundedNorth),
     228, uint(QTabBar::RoundedSouth),
     241, uint(QTabBar::RoundedWest),
     253, uint(QTabBar::RoundedEast),
     265, uint(QTabBar::TriangularNorth),
     281, uint(QTabBar::TriangularSouth),
     297, uint(QTabBar::TriangularWest),
     312, uint(QTabBar::TriangularEast),

       0        // eod
};

static const char qt_meta_stringdata_QTabBar[] = {
    "QTabBar\0\0index\0currentChanged(int)\0"
    "selected(int)\0setCurrentIndex(int)\0"
    "setCurrentTab(int)\0_q_scrollTabs()\0"
    "Shape\0shape\0int\0currentIndex\0count\0"
    "bool\0drawBase\0QSize\0iconSize\0"
    "Qt::TextElideMode\0elideMode\0"
    "usesScrollButtons\0RoundedNorth\0"
    "RoundedSouth\0RoundedWest\0RoundedEast\0"
    "TriangularNorth\0TriangularSouth\0"
    "TriangularWest\0TriangularEast\0"
};

const QMetaObject QTabBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QTabBar,
      qt_meta_data_QTabBar, 0 }
};

const QMetaObject *QTabBar::metaObject() const
{
    return &staticMetaObject;
}

void *QTabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTabBar))
	return static_cast<void*>(const_cast< QTabBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int QTabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setCurrentTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->_q_scrollTabs(); break;
        }
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Shape*>(_v) = shape(); break;
        case 1: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        case 2: *reinterpret_cast< int*>(_v) = count(); break;
        case 3: *reinterpret_cast< bool*>(_v) = drawBase(); break;
        case 4: *reinterpret_cast< QSize*>(_v) = iconSize(); break;
        case 5: *reinterpret_cast< Qt::TextElideMode*>(_v) = elideMode(); break;
        case 6: *reinterpret_cast< bool*>(_v) = usesScrollButtons(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setShape(*reinterpret_cast< Shape*>(_v)); break;
        case 1: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        case 3: setDrawBase(*reinterpret_cast< bool*>(_v)); break;
        case 4: setIconSize(*reinterpret_cast< QSize*>(_v)); break;
        case 5: setElideMode(*reinterpret_cast< Qt::TextElideMode*>(_v)); break;
        case 6: setUsesScrollButtons(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QTabBar::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTabBar::selected(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
