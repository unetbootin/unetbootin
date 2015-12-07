/****************************************************************************
** Meta object code from reading C++ file 'qscriptdebuggercodeviewinterface_p.h'
**
** Created: Thu Mar 5 20:26:20 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptdebuggercodeviewinterface_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptdebuggercodeviewinterface_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptDebuggerCodeViewInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      48,   34,   33,   33, 0x05,
     100,   82,   33,   33, 0x05,
     154,  134,   33,   33, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QScriptDebuggerCodeViewInterface[] = {
    "QScriptDebuggerCodeViewInterface\0\0"
    "lineNumber,on\0breakpointToggleRequest(int,bool)\0"
    "lineNumber,enable\0breakpointEnableRequest(int,bool)\0"
    "pos,lineNumber,path\0"
    "toolTipRequest(QPoint,int,QStringList)\0"
};

const QMetaObject QScriptDebuggerCodeViewInterface::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QScriptDebuggerCodeViewInterface,
      qt_meta_data_QScriptDebuggerCodeViewInterface, 0 }
};

const QMetaObject *QScriptDebuggerCodeViewInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptDebuggerCodeViewInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptDebuggerCodeViewInterface))
        return static_cast<void*>(const_cast< QScriptDebuggerCodeViewInterface*>(this));
    return QWidget::qt_metacast(_clname);
}

int QScriptDebuggerCodeViewInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: breakpointToggleRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: breakpointEnableRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: toolTipRequest((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QScriptDebuggerCodeViewInterface::breakpointToggleRequest(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QScriptDebuggerCodeViewInterface::breakpointEnableRequest(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QScriptDebuggerCodeViewInterface::toolTipRequest(const QPoint & _t1, int _t2, const QStringList & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
