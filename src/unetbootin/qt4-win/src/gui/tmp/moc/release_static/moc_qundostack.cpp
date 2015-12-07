/****************************************************************************
** Meta object code from reading C++ file 'qundostack.h'
**
** Created: Fri May 30 22:34:22 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../util/qundostack.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qundostack.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QUndoStack[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       2,   70, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x05,
      40,   34,   11,   11, 0x05,
      67,   59,   11,   11, 0x05,
      96,   88,   11,   11, 0x05,
     126,  117,   11,   11, 0x05,
     160,  151,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     185,   11,   11,   11, 0x0a,
     196,   12,   11,   11, 0x0a,
     210,   11,   11,   11, 0x0a,
     217,   11,   11,   11, 0x0a,
     231,  224,   11,   11, 0x0a,
     247,   11,   11,   11, 0x2a,

 // properties: name, type, flags
     224,  259, 0x01095103,
     268,  264, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_QUndoStack[] = {
    "QUndoStack\0\0idx\0indexChanged(int)\0"
    "clean\0cleanChanged(bool)\0canUndo\0"
    "canUndoChanged(bool)\0canRedo\0"
    "canRedoChanged(bool)\0undoText\0"
    "undoTextChanged(QString)\0redoText\0"
    "redoTextChanged(QString)\0setClean()\0"
    "setIndex(int)\0undo()\0redo()\0active\0"
    "setActive(bool)\0setActive()\0bool\0int\0"
    "undoLimit\0"
};

const QMetaObject QUndoStack::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QUndoStack,
      qt_meta_data_QUndoStack, 0 }
};

const QMetaObject *QUndoStack::metaObject() const
{
    return &staticMetaObject;
}

void *QUndoStack::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QUndoStack))
	return static_cast<void*>(const_cast< QUndoStack*>(this));
    return QObject::qt_metacast(_clname);
}

int QUndoStack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: cleanChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: canUndoChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: canRedoChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: undoTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: redoTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: setClean(); break;
        case 7: setIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: undo(); break;
        case 9: redo(); break;
        case 10: setActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: setActive(); break;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isActive(); break;
        case 1: *reinterpret_cast< int*>(_v) = undoLimit(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setActive(*reinterpret_cast< bool*>(_v)); break;
        case 1: setUndoLimit(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QUndoStack::indexChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QUndoStack::cleanChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QUndoStack::canUndoChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QUndoStack::canRedoChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QUndoStack::undoTextChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QUndoStack::redoTextChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
