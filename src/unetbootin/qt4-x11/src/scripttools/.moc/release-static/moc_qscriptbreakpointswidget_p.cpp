/****************************************************************************
** Meta object code from reading C++ file 'qscriptbreakpointswidget_p.h'
**
** Created: Thu Mar 5 20:25:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptbreakpointswidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptbreakpointswidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptBreakpointsWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x08,
      45,   25,   25,   25, 0x08,
      67,   25,   25,   25, 0x08,
     102,  100,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScriptBreakpointsWidget[] = {
    "QScriptBreakpointsWidget\0\0_q_newBreakpoint()\0"
    "_q_deleteBreakpoint()\0"
    "_q_onCurrentChanged(QModelIndex)\0,\0"
    "_q_onNewBreakpointRequest(QString,int)\0"
};

const QMetaObject QScriptBreakpointsWidget::staticMetaObject = {
    { &QScriptBreakpointsWidgetInterface::staticMetaObject, qt_meta_stringdata_QScriptBreakpointsWidget,
      qt_meta_data_QScriptBreakpointsWidget, 0 }
};

const QMetaObject *QScriptBreakpointsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptBreakpointsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptBreakpointsWidget))
        return static_cast<void*>(const_cast< QScriptBreakpointsWidget*>(this));
    return QScriptBreakpointsWidgetInterface::qt_metacast(_clname);
}

int QScriptBreakpointsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScriptBreakpointsWidgetInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_newBreakpoint(); break;
        case 1: d_func()->_q_deleteBreakpoint(); break;
        case 2: d_func()->_q_onCurrentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: d_func()->_q_onNewBreakpointRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
