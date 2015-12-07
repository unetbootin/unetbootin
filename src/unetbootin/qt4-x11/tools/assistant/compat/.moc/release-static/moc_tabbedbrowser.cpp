/****************************************************************************
** Meta object code from reading C++ file 'tabbedbrowser.h'
**
** Created: Thu Mar 5 20:36:49 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tabbedbrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tabbedbrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TabbedBrowser[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x05,
      47,   42,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   14,   14,   14, 0x0a,
      81,   14,   14,   14, 0x0a,
      91,   14,   14,   14, 0x0a,
     106,  102,   14,   14, 0x0a,
     125,   14,   14,   14, 0x0a,
     134,   14,   14,   14, 0x0a,
     141,   14,   14,   14, 0x0a,
     151,   14,   14,   14, 0x0a,
     169,  165,   14,   14, 0x0a,
     185,   14,   14,   14, 0x0a,
     194,   14,   14,   14, 0x0a,
     210,  204,   14,   14, 0x0a,
     231,   14,   14,   14, 0x0a,
     240,   14,   14,   14, 0x0a,
     260,  256,   14,   14, 0x0a,
     288,   14,   14,   14, 0x0a,
     296,   14,   14,   14, 0x0a,
     303,   14,   14,   14, 0x0a,
     314,   14,   14,   14, 0x0a,
     330,   14,   14,   14, 0x0a,
     337,   14,   14,   14, 0x0a,
     348,   14,   14,   14, 0x0a,
     381,  363,   14,   14, 0x08,
     414,  405,   14,   14, 0x28,
     433,   14,   14,   14, 0x28,
     451,  447,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TabbedBrowser[] = {
    "TabbedBrowser\0\0count\0tabCountChanged(int)\0"
    "link\0browserUrlChanged(QString)\0init()\0"
    "forward()\0backward()\0ref\0setSource(QString)\0"
    "reload()\0home()\0nextTab()\0previousTab()\0"
    "lnk\0newTab(QString)\0zoomIn()\0zoomOut()\0"
    "title\0updateTitle(QString)\0newTab()\0"
    "transferFocus()\0win\0initHelpWindow(HelpWindow*)\0"
    "setup()\0copy()\0closeTab()\0sourceChanged()\0"
    "find()\0findNext()\0findPrevious()\0"
    ",forward,backward\0find(QString,bool,bool)\0"
    ",forward\0find(QString,bool)\0find(QString)\0"
    "pos\0openTabMenu(QPoint)\0"
};

const QMetaObject TabbedBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TabbedBrowser,
      qt_meta_data_TabbedBrowser, 0 }
};

const QMetaObject *TabbedBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *TabbedBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TabbedBrowser))
        return static_cast<void*>(const_cast< TabbedBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int TabbedBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tabCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: browserUrlChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: init(); break;
        case 3: forward(); break;
        case 4: backward(); break;
        case 5: setSource((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: reload(); break;
        case 7: home(); break;
        case 8: nextTab(); break;
        case 9: previousTab(); break;
        case 10: newTab((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: zoomIn(); break;
        case 12: zoomOut(); break;
        case 13: updateTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: newTab(); break;
        case 15: transferFocus(); break;
        case 16: initHelpWindow((*reinterpret_cast< HelpWindow*(*)>(_a[1]))); break;
        case 17: setup(); break;
        case 18: copy(); break;
        case 19: closeTab(); break;
        case 20: sourceChanged(); break;
        case 21: find(); break;
        case 22: findNext(); break;
        case 23: findPrevious(); break;
        case 24: find((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 25: find((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 26: find((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 27: openTabMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void TabbedBrowser::tabCountChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TabbedBrowser::browserUrlChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
