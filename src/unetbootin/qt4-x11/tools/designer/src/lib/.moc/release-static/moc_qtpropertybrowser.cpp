/****************************************************************************
** Meta object code from reading C++ file 'qtpropertybrowser.h'
**
** Created: Thu Mar 5 20:41:31 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtpropertybrowser/qtpropertybrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtpropertybrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtAbstractPropertyManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      49,   27,   26,   26, 0x05,
     112,  103,   26,   26, 0x05,
     157,  141,   26,   26, 0x05,
     198,  103,   26,   26, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QtAbstractPropertyManager[] = {
    "QtAbstractPropertyManager\0\0"
    "property,parent,after\0"
    "propertyInserted(QtProperty*,QtProperty*,QtProperty*)\0"
    "property\0propertyChanged(QtProperty*)\0"
    "property,parent\0"
    "propertyRemoved(QtProperty*,QtProperty*)\0"
    "propertyDestroyed(QtProperty*)\0"
};

const QMetaObject QtAbstractPropertyManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtAbstractPropertyManager,
      qt_meta_data_QtAbstractPropertyManager, 0 }
};

const QMetaObject *QtAbstractPropertyManager::metaObject() const
{
    return &staticMetaObject;
}

void *QtAbstractPropertyManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtAbstractPropertyManager))
        return static_cast<void*>(const_cast< QtAbstractPropertyManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QtAbstractPropertyManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: propertyInserted((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2])),(*reinterpret_cast< QtProperty*(*)>(_a[3]))); break;
        case 1: propertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 2: propertyRemoved((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2]))); break;
        case 3: propertyDestroyed((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QtAbstractPropertyManager::propertyInserted(QtProperty * _t1, QtProperty * _t2, QtProperty * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtAbstractPropertyManager::propertyChanged(QtProperty * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtAbstractPropertyManager::propertyRemoved(QtProperty * _t1, QtProperty * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtAbstractPropertyManager::propertyDestroyed(QtProperty * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
static const uint qt_meta_data_QtAbstractEditorFactoryBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      37,   29,   28,   28, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_QtAbstractEditorFactoryBase[] = {
    "QtAbstractEditorFactoryBase\0\0manager\0"
    "managerDestroyed(QObject*)\0"
};

const QMetaObject QtAbstractEditorFactoryBase::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtAbstractEditorFactoryBase,
      qt_meta_data_QtAbstractEditorFactoryBase, 0 }
};

const QMetaObject *QtAbstractEditorFactoryBase::metaObject() const
{
    return &staticMetaObject;
}

void *QtAbstractEditorFactoryBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtAbstractEditorFactoryBase))
        return static_cast<void*>(const_cast< QtAbstractEditorFactoryBase*>(this));
    return QObject::qt_metacast(_clname);
}

int QtAbstractEditorFactoryBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: managerDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_QtAbstractPropertyBrowser[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,   77,   62,   26, 0x0a,
     134,  111,   62,   26, 0x0a,
     174,   77,   26,   26, 0x0a,
     205,  202,   26,   26, 0x08,
     265,  263,   26,   26, 0x08,
     310,   26,   26,   26, 0x08,
     345,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtAbstractPropertyBrowser[] = {
    "QtAbstractPropertyBrowser\0\0"
    "currentItemChanged(QtBrowserItem*)\0"
    "QtBrowserItem*\0property\0"
    "addProperty(QtProperty*)\0"
    "property,afterProperty\0"
    "insertProperty(QtProperty*,QtProperty*)\0"
    "removeProperty(QtProperty*)\0,,\0"
    "slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)\0"
    ",\0slotPropertyRemoved(QtProperty*,QtProperty*)\0"
    "slotPropertyDestroyed(QtProperty*)\0"
    "slotPropertyDataChanged(QtProperty*)\0"
};

const QMetaObject QtAbstractPropertyBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtAbstractPropertyBrowser,
      qt_meta_data_QtAbstractPropertyBrowser, 0 }
};

const QMetaObject *QtAbstractPropertyBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *QtAbstractPropertyBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtAbstractPropertyBrowser))
        return static_cast<void*>(const_cast< QtAbstractPropertyBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtAbstractPropertyBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentItemChanged((*reinterpret_cast< QtBrowserItem*(*)>(_a[1]))); break;
        case 1: { QtBrowserItem* _r = addProperty((*reinterpret_cast< QtProperty*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QtBrowserItem**>(_a[0]) = _r; }  break;
        case 2: { QtBrowserItem* _r = insertProperty((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QtBrowserItem**>(_a[0]) = _r; }  break;
        case 3: removeProperty((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 4: d_func()->slotPropertyInserted((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2])),(*reinterpret_cast< QtProperty*(*)>(_a[3]))); break;
        case 5: d_func()->slotPropertyRemoved((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2]))); break;
        case 6: d_func()->slotPropertyDestroyed((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 7: d_func()->slotPropertyDataChanged((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void QtAbstractPropertyBrowser::currentItemChanged(QtBrowserItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
