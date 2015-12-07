/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_propertysheet_p.h'
**
** Created: Thu Mar 5 20:46:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_propertysheet_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_propertysheet_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerPropertySheet[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QDesignerPropertySheet[] = {
    "QDesignerPropertySheet\0"
};

const QMetaObject QDesignerPropertySheet::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDesignerPropertySheet,
      qt_meta_data_QDesignerPropertySheet, 0 }
};

const QMetaObject *QDesignerPropertySheet::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerPropertySheet::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerPropertySheet))
        return static_cast<void*>(const_cast< QDesignerPropertySheet*>(this));
    if (!strcmp(_clname, "QDesignerPropertySheetExtension"))
        return static_cast< QDesignerPropertySheetExtension*>(const_cast< QDesignerPropertySheet*>(this));
    if (!strcmp(_clname, "QDesignerDynamicPropertySheetExtension"))
        return static_cast< QDesignerDynamicPropertySheetExtension*>(const_cast< QDesignerPropertySheet*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.PropertySheet"))
        return static_cast< QDesignerPropertySheetExtension*>(const_cast< QDesignerPropertySheet*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.DynamicPropertySheet"))
        return static_cast< QDesignerDynamicPropertySheetExtension*>(const_cast< QDesignerPropertySheet*>(this));
    return QObject::qt_metacast(_clname);
}

int QDesignerPropertySheet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QDesignerAbstractPropertySheetFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      46,   39,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerAbstractPropertySheetFactory[] = {
    "QDesignerAbstractPropertySheetFactory\0"
    "\0object\0objectDestroyed(QObject*)\0"
};

const QMetaObject QDesignerAbstractPropertySheetFactory::staticMetaObject = {
    { &QExtensionFactory::staticMetaObject, qt_meta_stringdata_QDesignerAbstractPropertySheetFactory,
      qt_meta_data_QDesignerAbstractPropertySheetFactory, 0 }
};

const QMetaObject *QDesignerAbstractPropertySheetFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerAbstractPropertySheetFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerAbstractPropertySheetFactory))
        return static_cast<void*>(const_cast< QDesignerAbstractPropertySheetFactory*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.QAbstractExtensionFactory"))
        return static_cast< QAbstractExtensionFactory*>(const_cast< QDesignerAbstractPropertySheetFactory*>(this));
    return QExtensionFactory::qt_metacast(_clname);
}

int QDesignerAbstractPropertySheetFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QExtensionFactory::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: objectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
