/****************************************************************************
** Meta object code from reading C++ file 'qmdiarea.h'
**
** Created: Thu Mar 5 18:51:57 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qmdiarea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmdiarea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMdiArea[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       6,   72, // properties
       2,   90, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,   45,    9,    9, 0x0a,
      87,    9,    9,    9, 0x0a,
     104,    9,    9,    9, 0x0a,
     124,    9,    9,    9, 0x0a,
     147,    9,    9,    9, 0x0a,
     168,    9,    9,    9, 0x0a,
     192,    9,    9,    9, 0x0a,
     229,  220,    9,    9, 0x09,
     253,    9,    9,    9, 0x08,
     281,  279,    9,    9, 0x08,
     351,  345,    9,    9, 0x08,

 // properties: name, type, flags
     384,  377, 0x42095103,
     407,  395, 0x0009510b,
     432,  423, 0x0009510b,
     446,  441, 0x01095103,
     480,  459, 0x0009510b,
     513,  489, 0x0009510b,

 // enums: name, flags, count, data
     395, 0x0,    3,   98,
     423, 0x0,    2,  104,

 // enum data: key, value
     525, uint(QMdiArea::CreationOrder),
     539, uint(QMdiArea::StackingOrder),
     553, uint(QMdiArea::ActivationHistoryOrder),
     576, uint(QMdiArea::SubWindowView),
     590, uint(QMdiArea::TabbedView),

       0        // eod
};

static const char qt_meta_stringdata_QMdiArea[] = {
    "QMdiArea\0\0subWindowActivated(QMdiSubWindow*)\0"
    "window\0setActiveSubWindow(QMdiSubWindow*)\0"
    "tileSubWindows()\0cascadeSubWindows()\0"
    "closeActiveSubWindow()\0closeAllSubWindows()\0"
    "activateNextSubWindow()\0"
    "activatePreviousSubWindow()\0viewport\0"
    "setupViewport(QWidget*)\0"
    "_q_deactivateAllWindows()\0,\0"
    "_q_processWindowStateChanged(Qt::WindowStates,Qt::WindowStates)\0"
    "index\0_q_currentTabChanged(int)\0QBrush\0"
    "background\0WindowOrder\0activationOrder\0"
    "ViewMode\0viewMode\0bool\0documentMode\0"
    "QTabWidget::TabShape\0tabShape\0"
    "QTabWidget::TabPosition\0tabPosition\0"
    "CreationOrder\0StackingOrder\0"
    "ActivationHistoryOrder\0SubWindowView\0"
    "TabbedView\0"
};

static const QMetaObject *qt_meta_extradata_QMdiArea[] = {
        &QTabWidget::staticMetaObject,0
};

static const QMetaObjectExtraData qt_meta_extradata2_QMdiArea = {
    qt_meta_extradata_QMdiArea, 0 
};

const QMetaObject QMdiArea::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QMdiArea,
      qt_meta_data_QMdiArea, &qt_meta_extradata2_QMdiArea }
};

const QMetaObject *QMdiArea::metaObject() const
{
    return &staticMetaObject;
}

void *QMdiArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMdiArea))
        return static_cast<void*>(const_cast< QMdiArea*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QMdiArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: subWindowActivated((*reinterpret_cast< QMdiSubWindow*(*)>(_a[1]))); break;
        case 1: setActiveSubWindow((*reinterpret_cast< QMdiSubWindow*(*)>(_a[1]))); break;
        case 2: tileSubWindows(); break;
        case 3: cascadeSubWindows(); break;
        case 4: closeActiveSubWindow(); break;
        case 5: closeAllSubWindows(); break;
        case 6: activateNextSubWindow(); break;
        case 7: activatePreviousSubWindow(); break;
        case 8: setupViewport((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 9: d_func()->_q_deactivateAllWindows(); break;
        case 10: d_func()->_q_processWindowStateChanged((*reinterpret_cast< Qt::WindowStates(*)>(_a[1])),(*reinterpret_cast< Qt::WindowStates(*)>(_a[2]))); break;
        case 11: d_func()->_q_currentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QBrush*>(_v) = background(); break;
        case 1: *reinterpret_cast< WindowOrder*>(_v) = activationOrder(); break;
        case 2: *reinterpret_cast< ViewMode*>(_v) = viewMode(); break;
        case 3: *reinterpret_cast< bool*>(_v) = documentMode(); break;
        case 4: *reinterpret_cast< QTabWidget::TabShape*>(_v) = tabShape(); break;
        case 5: *reinterpret_cast< QTabWidget::TabPosition*>(_v) = tabPosition(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBackground(*reinterpret_cast< QBrush*>(_v)); break;
        case 1: setActivationOrder(*reinterpret_cast< WindowOrder*>(_v)); break;
        case 2: setViewMode(*reinterpret_cast< ViewMode*>(_v)); break;
        case 3: setDocumentMode(*reinterpret_cast< bool*>(_v)); break;
        case 4: setTabShape(*reinterpret_cast< QTabWidget::TabShape*>(_v)); break;
        case 5: setTabPosition(*reinterpret_cast< QTabWidget::TabPosition*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QMdiArea::subWindowActivated(QMdiSubWindow * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
