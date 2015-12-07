/****************************************************************************
** Meta object code from reading C++ file 'qtabwidget.h'
**
** Created: Fri May 30 22:28:45 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qtabwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTabWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       7,   60, // properties
       2,   81, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,
      38,   11,   11,   11, 0x15,
      63,   11,   11,   11, 0x15,

 // slots: signature, parameters, type, tag, flags
      81,   12,   11,   11, 0x0a,
     109,  102,   11,   11, 0x0a,
     136,   12,   11,   11, 0x1a,
     158,  156,   11,   11, 0x1a,
     177,  156,   11,   11, 0x1a,
     198,   11,   11,   11, 0x08,
     214,   11,   11,   11, 0x08,

 // properties: name, type, flags
     244,  232, 0x0009510b,
     265,  256, 0x0009510b,
     278,  274, 0x02095103,
     291,  274, 0x02095001,
     303,  297, 0x15095103,
     330,  312, 0x0009510b,
     345,  340, 0x01095103,

 // enums: name, flags, count, data
     232, 0x0,    4,   89,
     256, 0x0,    2,   97,

 // enum data: key, value
     363, uint(QTabWidget::North),
     369, uint(QTabWidget::South),
     375, uint(QTabWidget::West),
     380, uint(QTabWidget::East),
     385, uint(QTabWidget::Rounded),
     393, uint(QTabWidget::Triangular),

       0        // eod
};

static const char qt_meta_stringdata_QTabWidget[] = {
    "QTabWidget\0\0index\0currentChanged(int)\0"
    "currentChanged(QWidget*)\0selected(QString)\0"
    "setCurrentIndex(int)\0widget\0"
    "setCurrentWidget(QWidget*)\0"
    "setCurrentPage(int)\0w\0showPage(QWidget*)\0"
    "removePage(QWidget*)\0_q_showTab(int)\0"
    "_q_removeTab(int)\0TabPosition\0tabPosition\0"
    "TabShape\0tabShape\0int\0currentIndex\0"
    "count\0QSize\0iconSize\0Qt::TextElideMode\0"
    "elideMode\0bool\0usesScrollButtons\0North\0"
    "South\0West\0East\0Rounded\0Triangular\0"
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
        case 1: currentChanged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: selected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setCurrentWidget((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 5: setCurrentPage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: showPage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 7: removePage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 8: d_func()->_q_showTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: d_func()->_q_removeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 10;
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
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTabPosition(*reinterpret_cast< TabPosition*>(_v)); break;
        case 1: setTabShape(*reinterpret_cast< TabShape*>(_v)); break;
        case 2: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
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
void QTabWidget::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTabWidget::currentChanged(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTabWidget::selected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
