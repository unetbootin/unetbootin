/****************************************************************************
** Meta object code from reading C++ file 'qscriptdebugger_p.h'
**
** Created: Thu Mar 5 20:24:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptdebugger_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptdebugger_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptDebugger[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      27,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   16,   16,   16, 0x08,
      63,   16,   16,   16, 0x08,
      93,   16,   16,   16, 0x08,
     127,   16,   16,   16, 0x08,
     160,   16,   16,   16, 0x08,
     175,   16,   16,   16, 0x08,
     189,   16,   16,   16, 0x08,
     203,   16,   16,   16, 0x08,
     217,   16,   16,   16, 0x08,
     230,   16,   16,   16, 0x08,
     247,   16,   16,   16, 0x08,
     267,   16,   16,   16, 0x08,
     289,   16,   16,   16, 0x08,
     311,   16,   16,   16, 0x08,
     330,   16,   16,   16, 0x08,
     348,   16,   16,   16, 0x08,
     366,   16,   16,   16, 0x08,
     388,   16,   16,   16, 0x08,
     416,  414,   16,   16, 0x08,
     450,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDebugger[] = {
    "QScriptDebugger\0\0stopped()\0started()\0"
    "_q_onLineEntered(QString)\0"
    "_q_onCurrentFrameChanged(int)\0"
    "_q_onCurrentScriptChanged(qint64)\0"
    "_q_onScriptLocationSelected(int)\0"
    "_q_interrupt()\0_q_continue()\0_q_stepInto()\0"
    "_q_stepOver()\0_q_stepOut()\0_q_runToCursor()\0"
    "_q_runToNewScript()\0_q_toggleBreakpoint()\0"
    "_q_clearDebugOutput()\0_q_clearErrorLog()\0"
    "_q_clearConsole()\0_q_findInScript()\0"
    "_q_findNextInScript()\0_q_findPreviousInScript()\0"
    ",\0_q_onFindCodeRequest(QString,int)\0"
    "_q_goToLine()\0"
};

const QMetaObject QScriptDebugger::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QScriptDebugger,
      qt_meta_data_QScriptDebugger, 0 }
};

const QMetaObject *QScriptDebugger::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDebugger::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDebugger))
        return static_cast<void*>(const_cast< QScriptDebugger*>(this));
    return QObject::qt_metacast(_clname);
}

int QScriptDebugger::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stopped(); break;
        case 1: started(); break;
        case 2: d_func()->_q_onLineEntered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: d_func()->_q_onCurrentFrameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->_q_onCurrentScriptChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 5: d_func()->_q_onScriptLocationSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: d_func()->_q_interrupt(); break;
        case 7: d_func()->_q_continue(); break;
        case 8: d_func()->_q_stepInto(); break;
        case 9: d_func()->_q_stepOver(); break;
        case 10: d_func()->_q_stepOut(); break;
        case 11: d_func()->_q_runToCursor(); break;
        case 12: d_func()->_q_runToNewScript(); break;
        case 13: d_func()->_q_toggleBreakpoint(); break;
        case 14: d_func()->_q_clearDebugOutput(); break;
        case 15: d_func()->_q_clearErrorLog(); break;
        case 16: d_func()->_q_clearConsole(); break;
        case 17: d_func()->_q_findInScript(); break;
        case 18: d_func()->_q_findNextInScript(); break;
        case 19: d_func()->_q_findPreviousInScript(); break;
        case 20: d_func()->_q_onFindCodeRequest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: d_func()->_q_goToLine(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void QScriptDebugger::stopped()const
{
    QMetaObject::activate(const_cast< QScriptDebugger *>(this), &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QScriptDebugger::started()const
{
    QMetaObject::activate(const_cast< QScriptDebugger *>(this), &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
