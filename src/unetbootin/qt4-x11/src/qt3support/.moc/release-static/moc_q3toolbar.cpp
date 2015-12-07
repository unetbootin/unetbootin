/****************************************************************************
** Meta object code from reading C++ file 'q3toolbar.h'
**
** Created: Thu Mar 5 19:06:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/q3toolbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3toolbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3ToolBar[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       1,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,

 // properties: name, type, flags
      33,   25, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_Q3ToolBar[] = {
    "Q3ToolBar\0\0createPopup()\0QString\0label\0"
};

const QMetaObject Q3ToolBar::staticMetaObject = {
    { &Q3DockWindow::staticMetaObject, qt_meta_stringdata_Q3ToolBar,
      qt_meta_data_Q3ToolBar, 0 }
};

const QMetaObject *Q3ToolBar::metaObject() const
{
    return &staticMetaObject;
}

void *Q3ToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3ToolBar))
        return static_cast<void*>(const_cast< Q3ToolBar*>(this));
    return Q3DockWindow::qt_metacast(_clname);
}

int Q3ToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DockWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: createPopup(); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = label(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setLabel(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
