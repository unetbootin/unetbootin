/****************************************************************************
** Meta object code from reading C++ file 'centralwidget.h'
**
** Created: Thu Mar 5 20:33:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../centralwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'centralwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FindWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      23,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FindWidget[] = {
    "FindWidget\0\0findNext()\0findPrevious()\0"
    "updateButtons()\0"
};

const QMetaObject FindWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FindWidget,
      qt_meta_data_FindWidget, 0 }
};

const QMetaObject *FindWidget::metaObject() const
{
    return &staticMetaObject;
}

void *FindWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FindWidget))
        return static_cast<void*>(const_cast< FindWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int FindWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: findNext(); break;
        case 1: findPrevious(); break;
        case 2: updateButtons(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FindWidget::findNext()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void FindWidget::findPrevious()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
static const uint qt_meta_data_CentralWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      42,   38,   14,   14, 0x05,
      66,   62,   14,   14, 0x05,
      91,   86,   14,   14, 0x05,
     122,  112,   14,   14, 0x05,
     145,  112,   14,   14, 0x05,
     179,  169,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     211,   14,   14,   14, 0x0a,
     220,   14,   14,   14, 0x0a,
     230,   14,   14,   14, 0x0a,
     241,   14,   14,   14, 0x0a,
     252,   14,   14,   14, 0x0a,
     264,   14,   14,   14, 0x0a,
     279,   14,   14,   14, 0x0a,
     294,   14,   14,   14, 0x0a,
     310,   14,   14,   14, 0x0a,
     327,   14,   14,   14, 0x0a,
     335,   14,   14,   14, 0x0a,
     347,   14,   14,   14, 0x0a,
     362,   14,   14,   14, 0x0a,
     382,   62,   14,   14, 0x0a,
     407,  398,   14,   14, 0x0a,
     437,   62,   14,   14, 0x2a,
     466,  461,   14,   14, 0x0a,
     503,   14,  491,   14, 0x0a,
     517,   14,   14,   14, 0x0a,
     524,   14,   14,   14, 0x0a,
     534,   14,   14,   14, 0x0a,
     545,   14,   14,   14, 0x08,
     554,   14,   14,   14, 0x08,
     565,   62,   14,   14, 0x08,
     589,  583,   14,   14, 0x08,
     619,  613,   14,   14, 0x08,
     657,  649,   14,   14, 0x08,
     681,   62,   14,   14, 0x08,
     707,   62,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CentralWidget[] = {
    "CentralWidget\0\0currentViewerChanged()\0"
    "yes\0copyAvailable(bool)\0url\0"
    "sourceChanged(QUrl)\0link\0highlighted(QString)\0"
    "available\0forwardAvailable(bool)\0"
    "backwardAvailable(bool)\0title,url\0"
    "addNewBookmark(QString,QString)\0"
    "zoomIn()\0zoomOut()\0findNext()\0nextPage()\0"
    "resetZoom()\0previousPage()\0findPrevious()\0"
    "copySelection()\0showTextSearch()\0"
    "print()\0pageSetup()\0printPreview()\0"
    "updateBrowserFont()\0setSource(QUrl)\0"
    "url,zoom\0setSourceInNewTab(QUrl,qreal)\0"
    "setSourceInNewTab(QUrl)\0text\0"
    "findCurrentText(QString)\0HelpViewer*\0"
    "newEmptyTab()\0home()\0forward()\0"
    "backward()\0newTab()\0closeTab()\0"
    "setTabTitle(QUrl)\0index\0currentPageChanged(int)\0"
    "point\0showTabBarContextMenu(QPoint)\0"
    "printer\0printPreview(QPrinter*)\0"
    "setSourceFromSearch(QUrl)\0"
    "setSourceFromSearchInNewTab(QUrl)\0"
};

const QMetaObject CentralWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CentralWidget,
      qt_meta_data_CentralWidget, 0 }
};

const QMetaObject *CentralWidget::metaObject() const
{
    return &staticMetaObject;
}

void *CentralWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CentralWidget))
        return static_cast<void*>(const_cast< CentralWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CentralWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentViewerChanged(); break;
        case 1: copyAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: sourceChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 3: highlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: forwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: backwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: addNewBookmark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: zoomIn(); break;
        case 8: zoomOut(); break;
        case 9: findNext(); break;
        case 10: nextPage(); break;
        case 11: resetZoom(); break;
        case 12: previousPage(); break;
        case 13: findPrevious(); break;
        case 14: copySelection(); break;
        case 15: showTextSearch(); break;
        case 16: print(); break;
        case 17: pageSetup(); break;
        case 18: printPreview(); break;
        case 19: updateBrowserFont(); break;
        case 20: setSource((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 21: setSourceInNewTab((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 22: setSourceInNewTab((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 23: findCurrentText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 24: { HelpViewer* _r = newEmptyTab();
            if (_a[0]) *reinterpret_cast< HelpViewer**>(_a[0]) = _r; }  break;
        case 25: home(); break;
        case 26: forward(); break;
        case 27: backward(); break;
        case 28: newTab(); break;
        case 29: closeTab(); break;
        case 30: setTabTitle((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 31: currentPageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: showTabBarContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 33: printPreview((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        case 34: setSourceFromSearch((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 35: setSourceFromSearchInNewTab((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 36;
    }
    return _id;
}

// SIGNAL 0
void CentralWidget::currentViewerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CentralWidget::copyAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CentralWidget::sourceChanged(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CentralWidget::highlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CentralWidget::forwardAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CentralWidget::backwardAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CentralWidget::addNewBookmark(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
