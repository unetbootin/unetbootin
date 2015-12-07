/****************************************************************************
** Meta object code from reading C++ file 'qscriptedit_p.h'
**
** Created: Thu Mar 5 20:26:09 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../debugging/qscriptedit_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscriptedit_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScriptEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      27,   13,   12,   12, 0x05,
      79,   61,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     113,   12,   12,   12, 0x08,
     138,  136,   12,   12, 0x08,
     165,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScriptEdit[] = {
    "QScriptEdit\0\0lineNumber,on\0"
    "breakpointToggleRequest(int,bool)\0"
    "lineNumber,enable\0breakpointEnableRequest(int,bool)\0"
    "updateExtraAreaWidth()\0,\0"
    "updateExtraArea(QRect,int)\0"
    "highlightCurrentLine()\0"
};

const QMetaObject QScriptEdit::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_QScriptEdit,
      qt_meta_data_QScriptEdit, 0 }
};

const QMetaObject *QScriptEdit::metaObject() const
{
    return &staticMetaObject;
}

void *QScriptEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScriptEdit))
        return static_cast<void*>(const_cast< QScriptEdit*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int QScriptEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: breakpointToggleRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: breakpointEnableRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: updateExtraAreaWidth(); break;
        case 3: updateExtraArea((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: highlightCurrentLine(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QScriptEdit::breakpointToggleRequest(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QScriptEdit::breakpointEnableRequest(int _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
