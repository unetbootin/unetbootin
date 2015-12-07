/****************************************************************************
** Meta object code from reading C++ file 'sourcecodeview.h'
**
** Created: Thu Mar 5 20:56:04 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../sourcecodeview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sourcecodeview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SourceCodeView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SourceCodeView[] = {
    "SourceCodeView\0\0activated\0setActivated(bool)\0"
};

const QMetaObject SourceCodeView::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_SourceCodeView,
      qt_meta_data_SourceCodeView, 0 }
};

const QMetaObject *SourceCodeView::metaObject() const
{
    return &staticMetaObject;
}

void *SourceCodeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SourceCodeView))
        return static_cast<void*>(const_cast< SourceCodeView*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int SourceCodeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setActivated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
