/****************************************************************************
** Meta object code from reading C++ file 'qscriptdebuggercodewidget_p.h'
**
** Created: Thu Mar 5 20:25:38 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptdebuggercodewidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptdebuggercodewidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptDebuggerCodeWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      29,   27,   26,   26, 0x08,
      68,   27,   26,   26, 0x08,
     110,  107,   26,   26, 0x08,
     164,  107,   26,   26, 0x08,
     210,   27,   26,   26, 0x08,
     263,   26,   26,   26, 0x08,
     285,  107,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDebuggerCodeWidget[] = {
    "QScriptDebuggerCodeWidget\0\0,\0"
    "_q_onBreakpointToggleRequest(int,bool)\0"
    "_q_onBreakpointEnableRequest(int,bool)\0"
    ",,\0_q_onBreakpointsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_onBreakpointsInserted(QModelIndex,int,int)\0"
    "_q_onBreakpointsDataChanged(QModelIndex,QModelIndex)\0"
    "_q_onScriptsChanged()\0"
    "_q_onToolTipRequest(QPoint,int,QStringList)\0"
};

const QMetaObject QScriptDebuggerCodeWidget::staticMetaObject = {
    { &QScriptDebuggerCodeWidgetInterface::staticMetaObject, qt_meta_stringdata_QScriptDebuggerCodeWidget,
      qt_meta_data_QScriptDebuggerCodeWidget, 0 }
};

const QMetaObject *QScriptDebuggerCodeWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDebuggerCodeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDebuggerCodeWidget))
        return static_cast<void*>(const_cast< QScriptDebuggerCodeWidget*>(this));
    return QScriptDebuggerCodeWidgetInterface::qt_metacast(_clname);
}

int QScriptDebuggerCodeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScriptDebuggerCodeWidgetInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_onBreakpointToggleRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: d_func()->_q_onBreakpointEnableRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: d_func()->_q_onBreakpointsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: d_func()->_q_onBreakpointsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: d_func()->_q_onBreakpointsDataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 5: d_func()->_q_onScriptsChanged(); break;
        case 6: d_func()->_q_onToolTipRequest((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
