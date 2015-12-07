/****************************************************************************
** Meta object code from reading C++ file 'designerpropertymanager.h'
**
** Created: Thu Mar 5 20:51:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../propertyeditor/designerpropertymanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'designerpropertymanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ResetDecorator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      45,   36,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
      72,   36,   35,   35, 0x08,
     112,  105,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ResetDecorator[] = {
    "qdesigner_internal::ResetDecorator\0\0"
    "property\0resetProperty(QtProperty*)\0"
    "slotPropertyChanged(QtProperty*)\0"
    "object\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject qdesigner_internal::ResetDecorator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ResetDecorator,
      qt_meta_data_qdesigner_internal__ResetDecorator, 0 }
};

const QMetaObject *qdesigner_internal::ResetDecorator::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ResetDecorator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ResetDecorator))
        return static_cast<void*>(const_cast< ResetDecorator*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::ResetDecorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resetProperty((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 1: slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 2: slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ResetDecorator::resetProperty(QtProperty * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__DesignerPropertyManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      86,   45,   44,   44, 0x05,

 // slots: signature, parameters, type, tag, flags
     151,  126,   44,   44, 0x0a,
     209,  194,   44,   44, 0x0a,
     240,  194,   44,   44, 0x08,
     288,  279,   44,   44, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__DesignerPropertyManager[] = {
    "qdesigner_internal::DesignerPropertyManager\0"
    "\0property,value,enableSubPropertyHandling\0"
    "valueChanged(QtProperty*,QVariant,bool)\0"
    "property,attribute,value\0"
    "setAttribute(QtProperty*,QString,QVariant)\0"
    "property,value\0setValue(QtProperty*,QVariant)\0"
    "slotValueChanged(QtProperty*,QVariant)\0"
    "property\0slotPropertyDestroyed(QtProperty*)\0"
};

const QMetaObject qdesigner_internal::DesignerPropertyManager::staticMetaObject = {
    { &QtVariantPropertyManager::staticMetaObject, qt_meta_stringdata_qdesigner_internal__DesignerPropertyManager,
      qt_meta_data_qdesigner_internal__DesignerPropertyManager, 0 }
};

const QMetaObject *qdesigner_internal::DesignerPropertyManager::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::DesignerPropertyManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__DesignerPropertyManager))
        return static_cast<void*>(const_cast< DesignerPropertyManager*>(this));
    return QtVariantPropertyManager::qt_metacast(_clname);
}

int qdesigner_internal::DesignerPropertyManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtVariantPropertyManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: setAttribute((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 2: setValue((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 3: slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 4: slotPropertyDestroyed((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::DesignerPropertyManager::valueChanged(QtProperty * _t1, const QVariant & _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__DesignerEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      52,   43,   42,   42, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,   79,   42,   42, 0x08,
     141,  116,   42,   42, 0x08,
     192,   43,   42,   42, 0x08,
     240,  225,   42,   42, 0x08,
     285,  279,   42,   42, 0x08,
     316,  279,   42,   42, 0x08,
     353,  279,   42,   42, 0x08,
     382,  279,   42,   42, 0x08,
     409,  279,   42,   42, 0x08,
     434,  279,   42,   42, 0x08,
     459,  279,   42,   42, 0x08,
     488,  279,   42,   42, 0x08,
     518,  279,   42,   42, 0x08,
     542,  279,   42,   42, 0x08,
     572,  279,   42,   42, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__DesignerEditorFactory[] = {
    "qdesigner_internal::DesignerEditorFactory\0"
    "\0property\0resetProperty(QtProperty*)\0"
    "object\0slotEditorDestroyed(QObject*)\0"
    "property,attribute,value\0"
    "slotAttributeChanged(QtProperty*,QString,QVariant)\0"
    "slotPropertyChanged(QtProperty*)\0"
    "property,value\0slotValueChanged(QtProperty*,QVariant)\0"
    "value\0slotStringTextChanged(QString)\0"
    "slotKeySequenceChanged(QKeySequence)\0"
    "slotPaletteChanged(QPalette)\0"
    "slotPixmapChanged(QString)\0"
    "slotIconChanged(QString)\0"
    "slotUintChanged(QString)\0"
    "slotLongLongChanged(QString)\0"
    "slotULongLongChanged(QString)\0"
    "slotUrlChanged(QString)\0"
    "slotByteArrayChanged(QString)\0"
    "slotStringListChanged(QStringList)\0"
};

const QMetaObject qdesigner_internal::DesignerEditorFactory::staticMetaObject = {
    { &QtVariantEditorFactory::staticMetaObject, qt_meta_stringdata_qdesigner_internal__DesignerEditorFactory,
      qt_meta_data_qdesigner_internal__DesignerEditorFactory, 0 }
};

const QMetaObject *qdesigner_internal::DesignerEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::DesignerEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__DesignerEditorFactory))
        return static_cast<void*>(const_cast< DesignerEditorFactory*>(this));
    return QtVariantEditorFactory::qt_metacast(_clname);
}

int qdesigner_internal::DesignerEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtVariantEditorFactory::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resetProperty((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 1: slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: slotAttributeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 3: slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 4: slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 5: slotStringTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: slotKeySequenceChanged((*reinterpret_cast< const QKeySequence(*)>(_a[1]))); break;
        case 7: slotPaletteChanged((*reinterpret_cast< const QPalette(*)>(_a[1]))); break;
        case 8: slotPixmapChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: slotIconChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: slotUintChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: slotLongLongChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: slotULongLongChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: slotUrlChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: slotByteArrayChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: slotStringListChanged((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::DesignerEditorFactory::resetProperty(QtProperty * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
