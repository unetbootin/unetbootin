/****************************************************************************
** Meta object code from reading C++ file 'qscriptengine.h'
**
** Created: Fri May 30 22:36:12 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../qscriptengine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptEngine[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScriptEngine[] = {
    "QScriptEngine\0\0exception\0"
    "signalHandlerException(QScriptValue)\0"
    "_q_objectDestroyed(QObject*)\0"
};

const QMetaObject QScriptEngine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QScriptEngine,
      qt_meta_data_QScriptEngine, 0 }
};

const QMetaObject *QScriptEngine::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptEngine))
	return static_cast<void*>(const_cast< QScriptEngine*>(this));
    return QObject::qt_metacast(_clname);
}

int QScriptEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signalHandlerException((*reinterpret_cast< const QScriptValue(*)>(_a[1]))); break;
        case 1: d_func()->_q_objectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QScriptEngine::signalHandlerException(const QScriptValue & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
