/****************************************************************************
** Meta object code from reading C++ file 'qscriptdebuggerconsolewidgetinterface_p.h'
**
** Created: Thu Mar 5 20:26:10 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptdebuggerconsolewidgetinterface_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptdebuggerconsolewidgetinterface_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptDebuggerConsoleWidgetInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      48,   39,   38,   38, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDebuggerConsoleWidgetInterface[] = {
    "QScriptDebuggerConsoleWidgetInterface\0"
    "\0contents\0lineEntered(QString)\0"
};

const QMetaObject QScriptDebuggerConsoleWidgetInterface::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QScriptDebuggerConsoleWidgetInterface,
      qt_meta_data_QScriptDebuggerConsoleWidgetInterface, 0 }
};

const QMetaObject *QScriptDebuggerConsoleWidgetInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDebuggerConsoleWidgetInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDebuggerConsoleWidgetInterface))
        return static_cast<void*>(const_cast< QScriptDebuggerConsoleWidgetInterface*>(this));
    if (!strcmp(_clname, "QScriptMessageHandlerInterface"))
        return static_cast< QScriptMessageHandlerInterface*>(const_cast< QScriptDebuggerConsoleWidgetInterface*>(this));
    return QWidget::qt_metacast(_clname);
}

int QScriptDebuggerConsoleWidgetInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: lineEntered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QScriptDebuggerConsoleWidgetInterface::lineEntered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
