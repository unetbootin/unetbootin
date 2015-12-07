/****************************************************************************
** Meta object code from reading C++ file 'abstractpropertyeditor.h'
**
** Created: Thu Mar 5 20:46:38 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../sdk/abstractpropertyeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstractpropertyeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerPropertyEditorInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      45,   34,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,   79,   33,   33, 0x0a,
     125,  106,   33,   33, 0x0a,
     165,   34,   33,   33, 0x2a,
     209,  200,   33,   33, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerPropertyEditorInterface[] = {
    "QDesignerPropertyEditorInterface\0\0"
    "name,value\0propertyChanged(QString,QVariant)\0"
    "object\0setObject(QObject*)\0"
    "name,value,changed\0"
    "setPropertyValue(QString,QVariant,bool)\0"
    "setPropertyValue(QString,QVariant)\0"
    "readOnly\0setReadOnly(bool)\0"
};

const QMetaObject QDesignerPropertyEditorInterface::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDesignerPropertyEditorInterface,
      qt_meta_data_QDesignerPropertyEditorInterface, 0 }
};

const QMetaObject *QDesignerPropertyEditorInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerPropertyEditorInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerPropertyEditorInterface))
        return static_cast<void*>(const_cast< QDesignerPropertyEditorInterface*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDesignerPropertyEditorInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: propertyChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 1: setObject((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: setPropertyValue((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: setPropertyValue((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 4: setReadOnly((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QDesignerPropertyEditorInterface::propertyChanged(const QString & _t1, const QVariant & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
