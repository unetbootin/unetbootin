/****************************************************************************
** Meta object code from reading C++ file 'widgetfactory_p.h'
**
** Created: Thu Mar 5 20:47:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/widgetfactory_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetfactory_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__WidgetFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      35,   34,   34,   34, 0x0a,
      60,   49,   34,   34, 0x08,
     115,   49,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__WidgetFactory[] = {
    "qdesigner_internal::WidgetFactory\0\0"
    "loadPlugins()\0formWindow\0"
    "activeFormWindowChanged(QDesignerFormWindowInterface*)\0"
    "formWindowAdded(QDesignerFormWindowInterface*)\0"
};

const QMetaObject qdesigner_internal::WidgetFactory::staticMetaObject = {
    { &QDesignerWidgetFactoryInterface::staticMetaObject, qt_meta_stringdata_qdesigner_internal__WidgetFactory,
      qt_meta_data_qdesigner_internal__WidgetFactory, 0 }
};

const QMetaObject *qdesigner_internal::WidgetFactory::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::WidgetFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__WidgetFactory))
        return static_cast<void*>(const_cast< WidgetFactory*>(this));
    return QDesignerWidgetFactoryInterface::qt_metacast(_clname);
}

int qdesigner_internal::WidgetFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerWidgetFactoryInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadPlugins(); break;
        case 1: activeFormWindowChanged((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 2: formWindowAdded((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
