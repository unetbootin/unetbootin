/****************************************************************************
** Meta object code from reading C++ file 'qscriptdebuggercodefinderwidget_p.h'
**
** Created: Thu Mar 5 20:25:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptdebuggercodefinderwidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptdebuggercodefinderwidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptDebuggerCodeFinderWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x08,
      52,   32,   32,   32, 0x08,
      78,   32,   32,   32, 0x08,
      88,   32,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDebuggerCodeFinderWidget[] = {
    "QScriptDebuggerCodeFinderWidget\0\0"
    "_q_updateButtons()\0_q_onTextChanged(QString)\0"
    "_q_next()\0_q_previous()\0"
};

const QMetaObject QScriptDebuggerCodeFinderWidget::staticMetaObject = {
    { &QScriptDebuggerCodeFinderWidgetInterface::staticMetaObject, qt_meta_stringdata_QScriptDebuggerCodeFinderWidget,
      qt_meta_data_QScriptDebuggerCodeFinderWidget, 0 }
};

const QMetaObject *QScriptDebuggerCodeFinderWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDebuggerCodeFinderWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDebuggerCodeFinderWidget))
        return static_cast<void*>(const_cast< QScriptDebuggerCodeFinderWidget*>(this));
    return QScriptDebuggerCodeFinderWidgetInterface::qt_metacast(_clname);
}

int QScriptDebuggerCodeFinderWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScriptDebuggerCodeFinderWidgetInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_updateButtons(); break;
        case 1: d_func()->_q_onTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: d_func()->_q_next(); break;
        case 3: d_func()->_q_previous(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
