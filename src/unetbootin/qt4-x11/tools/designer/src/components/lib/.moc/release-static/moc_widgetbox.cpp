/****************************************************************************
** Meta object code from reading C++ file 'widgetbox.h'
**
** Created: Thu Mar 5 20:52:20 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgetbox/widgetbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__WidgetBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      57,   31,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__WidgetBox[] = {
    "qdesigner_internal::WidgetBox\0\0"
    "name,xml,global_mouse_pos\0"
    "handleMousePress(QString,QString,QPoint)\0"
};

const QMetaObject qdesigner_internal::WidgetBox::staticMetaObject = {
    { &QDesignerWidgetBox::staticMetaObject, qt_meta_stringdata_qdesigner_internal__WidgetBox,
      qt_meta_data_qdesigner_internal__WidgetBox, 0 }
};

const QMetaObject *qdesigner_internal::WidgetBox::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::WidgetBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__WidgetBox))
        return static_cast<void*>(const_cast< WidgetBox*>(this));
    return QDesignerWidgetBox::qt_metacast(_clname);
}

int qdesigner_internal::WidgetBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerWidgetBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: handleMousePress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
