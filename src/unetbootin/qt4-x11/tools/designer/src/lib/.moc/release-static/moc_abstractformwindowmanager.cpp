/****************************************************************************
** Meta object code from reading C++ file 'abstractformwindowmanager.h'
**
** Created: Thu Mar 5 20:46:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../sdk/abstractformwindowmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstractformwindowmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerFormWindowManagerInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      48,   37,   36,   36, 0x05,
      95,   37,   36,   36, 0x05,
     144,   37,   36,   36, 0x05,

 // slots: signature, parameters, type, tag, flags
     199,   37,   36,   36, 0x0a,
     244,   37,   36,   36, 0x0a,
     292,   37,   36,   36, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerFormWindowManagerInterface[] = {
    "QDesignerFormWindowManagerInterface\0"
    "\0formWindow\0formWindowAdded(QDesignerFormWindowInterface*)\0"
    "formWindowRemoved(QDesignerFormWindowInterface*)\0"
    "activeFormWindowChanged(QDesignerFormWindowInterface*)\0"
    "addFormWindow(QDesignerFormWindowInterface*)\0"
    "removeFormWindow(QDesignerFormWindowInterface*)\0"
    "setActiveFormWindow(QDesignerFormWindowInterface*)\0"
};

const QMetaObject QDesignerFormWindowManagerInterface::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDesignerFormWindowManagerInterface,
      qt_meta_data_QDesignerFormWindowManagerInterface, 0 }
};

const QMetaObject *QDesignerFormWindowManagerInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerFormWindowManagerInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerFormWindowManagerInterface))
        return static_cast<void*>(const_cast< QDesignerFormWindowManagerInterface*>(this));
    return QObject::qt_metacast(_clname);
}

int QDesignerFormWindowManagerInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: formWindowAdded((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 1: formWindowRemoved((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 2: activeFormWindowChanged((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 3: addFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 4: removeFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 5: setActiveFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QDesignerFormWindowManagerInterface::formWindowAdded(QDesignerFormWindowInterface * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDesignerFormWindowManagerInterface::formWindowRemoved(QDesignerFormWindowInterface * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QDesignerFormWindowManagerInterface::activeFormWindowChanged(QDesignerFormWindowInterface * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
