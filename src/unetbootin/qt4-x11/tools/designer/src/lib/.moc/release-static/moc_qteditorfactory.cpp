/****************************************************************************
** Meta object code from reading C++ file 'qteditorfactory.h'
**
** Created: Thu Mar 5 20:41:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtpropertybrowser/qteditorfactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qteditorfactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtSpinBoxFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   18,   17,   17, 0x08,
      60,   57,   17,   17, 0x08,
      98,   18,   17,   17, 0x08,
     137,   17,   17,   17, 0x08,
     155,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtSpinBoxFactory[] = {
    "QtSpinBoxFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,int)\0"
    ",,\0slotRangeChanged(QtProperty*,int,int)\0"
    "slotSingleStepChanged(QtProperty*,int)\0"
    "slotSetValue(int)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtSpinBoxFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtIntPropertyManager>::staticMetaObject, qt_meta_stringdata_QtSpinBoxFactory,
      qt_meta_data_QtSpinBoxFactory, 0 }
};

const QMetaObject *QtSpinBoxFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtSpinBoxFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtSpinBoxFactory))
        return static_cast<void*>(const_cast< QtSpinBoxFactory*>(this));
    return QtAbstractEditorFactory<QtIntPropertyManager>::qt_metacast(_clname);
}

int QtSpinBoxFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtIntPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: d_func()->slotSingleStepChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: d_func()->slotSetValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_QtSliderFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   17,   16,   16, 0x08,
      59,   56,   16,   16, 0x08,
      97,   17,   16,   16, 0x08,
     136,   16,   16,   16, 0x08,
     154,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtSliderFactory[] = {
    "QtSliderFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,int)\0"
    ",,\0slotRangeChanged(QtProperty*,int,int)\0"
    "slotSingleStepChanged(QtProperty*,int)\0"
    "slotSetValue(int)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtSliderFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtIntPropertyManager>::staticMetaObject, qt_meta_stringdata_QtSliderFactory,
      qt_meta_data_QtSliderFactory, 0 }
};

const QMetaObject *QtSliderFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtSliderFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtSliderFactory))
        return static_cast<void*>(const_cast< QtSliderFactory*>(this));
    return QtAbstractEditorFactory<QtIntPropertyManager>::qt_metacast(_clname);
}

int QtSliderFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtIntPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: d_func()->slotSingleStepChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: d_func()->slotSetValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_QtScrollBarFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      22,   20,   19,   19, 0x08,
      62,   59,   19,   19, 0x08,
     100,   20,   19,   19, 0x08,
     139,   19,   19,   19, 0x08,
     157,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtScrollBarFactory[] = {
    "QtScrollBarFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,int)\0"
    ",,\0slotRangeChanged(QtProperty*,int,int)\0"
    "slotSingleStepChanged(QtProperty*,int)\0"
    "slotSetValue(int)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtScrollBarFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtIntPropertyManager>::staticMetaObject, qt_meta_stringdata_QtScrollBarFactory,
      qt_meta_data_QtScrollBarFactory, 0 }
};

const QMetaObject *QtScrollBarFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtScrollBarFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtScrollBarFactory))
        return static_cast<void*>(const_cast< QtScrollBarFactory*>(this));
    return QtAbstractEditorFactory<QtIntPropertyManager>::qt_metacast(_clname);
}

int QtScrollBarFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtIntPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: d_func()->slotSingleStepChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: d_func()->slotSetValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_QtCheckBoxFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   19,   18,   18, 0x08,
      59,   18,   18,   18, 0x08,
      78,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtCheckBoxFactory[] = {
    "QtCheckBoxFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,bool)\0"
    "slotSetValue(bool)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtCheckBoxFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtBoolPropertyManager>::staticMetaObject, qt_meta_stringdata_QtCheckBoxFactory,
      qt_meta_data_QtCheckBoxFactory, 0 }
};

const QMetaObject *QtCheckBoxFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtCheckBoxFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtCheckBoxFactory))
        return static_cast<void*>(const_cast< QtCheckBoxFactory*>(this));
    return QtAbstractEditorFactory<QtBoolPropertyManager>::qt_metacast(_clname);
}

int QtCheckBoxFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtBoolPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: d_func()->slotSetValue((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtDoubleSpinBoxFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   24,   23,   23, 0x08,
      69,   66,   23,   23, 0x08,
     113,   24,   23,   23, 0x08,
     155,   24,   23,   23, 0x08,
     192,   23,   23,   23, 0x08,
     213,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtDoubleSpinBoxFactory[] = {
    "QtDoubleSpinBoxFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,double)\0"
    ",,\0slotRangeChanged(QtProperty*,double,double)\0"
    "slotSingleStepChanged(QtProperty*,double)\0"
    "slotDecimalsChanged(QtProperty*,int)\0"
    "slotSetValue(double)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtDoubleSpinBoxFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtDoublePropertyManager>::staticMetaObject, qt_meta_stringdata_QtDoubleSpinBoxFactory,
      qt_meta_data_QtDoubleSpinBoxFactory, 0 }
};

const QMetaObject *QtDoubleSpinBoxFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtDoubleSpinBoxFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtDoubleSpinBoxFactory))
        return static_cast<void*>(const_cast< QtDoubleSpinBoxFactory*>(this));
    return QtAbstractEditorFactory<QtDoublePropertyManager>::qt_metacast(_clname);
}

int QtDoubleSpinBoxFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtDoublePropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 1: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 2: d_func()->slotSingleStepChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 3: d_func()->slotDecimalsChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: d_func()->slotSetValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
static const uint qt_meta_data_QtLineEditFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   19,   18,   18, 0x08,
      62,   19,   18,   18, 0x08,
     101,   18,   18,   18, 0x08,
     123,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtLineEditFactory[] = {
    "QtLineEditFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QString)\0"
    "slotRegExpChanged(QtProperty*,QRegExp)\0"
    "slotSetValue(QString)\0"
    "slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtLineEditFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtStringPropertyManager>::staticMetaObject, qt_meta_stringdata_QtLineEditFactory,
      qt_meta_data_QtLineEditFactory, 0 }
};

const QMetaObject *QtLineEditFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtLineEditFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtLineEditFactory))
        return static_cast<void*>(const_cast< QtLineEditFactory*>(this));
    return QtAbstractEditorFactory<QtStringPropertyManager>::qt_metacast(_clname);
}

int QtLineEditFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtStringPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: d_func()->slotRegExpChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QRegExp(*)>(_a[2]))); break;
        case 2: d_func()->slotSetValue((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_QtDateEditFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   19,   18,   18, 0x08,
      63,   60,   18,   18, 0x08,
     105,   18,   18,   18, 0x08,
     125,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtDateEditFactory[] = {
    "QtDateEditFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QDate)\0"
    ",,\0slotRangeChanged(QtProperty*,QDate,QDate)\0"
    "slotSetValue(QDate)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtDateEditFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtDatePropertyManager>::staticMetaObject, qt_meta_stringdata_QtDateEditFactory,
      qt_meta_data_QtDateEditFactory, 0 }
};

const QMetaObject *QtDateEditFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtDateEditFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtDateEditFactory))
        return static_cast<void*>(const_cast< QtDateEditFactory*>(this));
    return QtAbstractEditorFactory<QtDatePropertyManager>::qt_metacast(_clname);
}

int QtDateEditFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtDatePropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2]))); break;
        case 1: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2])),(*reinterpret_cast< const QDate(*)>(_a[3]))); break;
        case 2: d_func()->slotSetValue((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 3: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_QtTimeEditFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   19,   18,   18, 0x08,
      60,   18,   18,   18, 0x08,
      80,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtTimeEditFactory[] = {
    "QtTimeEditFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QTime)\0"
    "slotSetValue(QTime)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtTimeEditFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtTimePropertyManager>::staticMetaObject, qt_meta_stringdata_QtTimeEditFactory,
      qt_meta_data_QtTimeEditFactory, 0 }
};

const QMetaObject *QtTimeEditFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtTimeEditFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtTimeEditFactory))
        return static_cast<void*>(const_cast< QtTimeEditFactory*>(this));
    return QtAbstractEditorFactory<QtTimePropertyManager>::qt_metacast(_clname);
}

int QtTimeEditFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtTimePropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QTime(*)>(_a[2]))); break;
        case 1: d_func()->slotSetValue((*reinterpret_cast< const QTime(*)>(_a[1]))); break;
        case 2: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtDateTimeEditFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      25,   23,   22,   22, 0x08,
      68,   22,   22,   22, 0x08,
      92,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtDateTimeEditFactory[] = {
    "QtDateTimeEditFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QDateTime)\0"
    "slotSetValue(QDateTime)\0"
    "slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtDateTimeEditFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtDateTimePropertyManager>::staticMetaObject, qt_meta_stringdata_QtDateTimeEditFactory,
      qt_meta_data_QtDateTimeEditFactory, 0 }
};

const QMetaObject *QtDateTimeEditFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtDateTimeEditFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtDateTimeEditFactory))
        return static_cast<void*>(const_cast< QtDateTimeEditFactory*>(this));
    return QtAbstractEditorFactory<QtDateTimePropertyManager>::qt_metacast(_clname);
}

int QtDateTimeEditFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtDateTimePropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QDateTime(*)>(_a[2]))); break;
        case 1: d_func()->slotSetValue((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 2: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtKeySequenceEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      30,   28,   27,   27, 0x08,
      76,   27,   27,   27, 0x08,
     103,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtKeySequenceEditorFactory[] = {
    "QtKeySequenceEditorFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QKeySequence)\0"
    "slotSetValue(QKeySequence)\0"
    "slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtKeySequenceEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtKeySequencePropertyManager>::staticMetaObject, qt_meta_stringdata_QtKeySequenceEditorFactory,
      qt_meta_data_QtKeySequenceEditorFactory, 0 }
};

const QMetaObject *QtKeySequenceEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtKeySequenceEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtKeySequenceEditorFactory))
        return static_cast<void*>(const_cast< QtKeySequenceEditorFactory*>(this));
    return QtAbstractEditorFactory<QtKeySequencePropertyManager>::qt_metacast(_clname);
}

int QtKeySequenceEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtKeySequencePropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QKeySequence(*)>(_a[2]))); break;
        case 1: d_func()->slotSetValue((*reinterpret_cast< const QKeySequence(*)>(_a[1]))); break;
        case 2: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtCharEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   21,   20,   20, 0x08,
      62,   20,   20,   20, 0x08,
      82,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtCharEditorFactory[] = {
    "QtCharEditorFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QChar)\0"
    "slotSetValue(QChar)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtCharEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtCharPropertyManager>::staticMetaObject, qt_meta_stringdata_QtCharEditorFactory,
      qt_meta_data_QtCharEditorFactory, 0 }
};

const QMetaObject *QtCharEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtCharEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtCharEditorFactory))
        return static_cast<void*>(const_cast< QtCharEditorFactory*>(this));
    return QtAbstractEditorFactory<QtCharPropertyManager>::qt_metacast(_clname);
}

int QtCharEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtCharPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QChar(*)>(_a[2]))); break;
        case 1: d_func()->slotSetValue((*reinterpret_cast< const QChar(*)>(_a[1]))); break;
        case 2: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtEnumEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   21,   20,   20, 0x08,
      60,   21,   20,   20, 0x08,
     106,   21,   20,   20, 0x08,
     156,   20,   20,   20, 0x08,
     174,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtEnumEditorFactory[] = {
    "QtEnumEditorFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,int)\0"
    "slotEnumNamesChanged(QtProperty*,QStringList)\0"
    "slotEnumIconsChanged(QtProperty*,QMap<int,QIcon>)\0"
    "slotSetValue(int)\0slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtEnumEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtEnumPropertyManager>::staticMetaObject, qt_meta_stringdata_QtEnumEditorFactory,
      qt_meta_data_QtEnumEditorFactory, 0 }
};

const QMetaObject *QtEnumEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtEnumEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtEnumEditorFactory))
        return static_cast<void*>(const_cast< QtEnumEditorFactory*>(this));
    return QtAbstractEditorFactory<QtEnumPropertyManager>::qt_metacast(_clname);
}

int QtEnumEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtEnumPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: d_func()->slotEnumNamesChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 2: d_func()->slotEnumIconsChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QMap<int,QIcon>(*)>(_a[2]))); break;
        case 3: d_func()->slotSetValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_QtCursorEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      25,   23,   22,   22, 0x08,
      66,   23,   22,   22, 0x08,
      99,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtCursorEditorFactory[] = {
    "QtCursorEditorFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QCursor)\0"
    "slotEnumChanged(QtProperty*,int)\0"
    "slotEditorDestroyed(QObject*)\0"
};

const QMetaObject QtCursorEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtCursorPropertyManager>::staticMetaObject, qt_meta_stringdata_QtCursorEditorFactory,
      qt_meta_data_QtCursorEditorFactory, 0 }
};

const QMetaObject *QtCursorEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtCursorEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtCursorEditorFactory))
        return static_cast<void*>(const_cast< QtCursorEditorFactory*>(this));
    return QtAbstractEditorFactory<QtCursorPropertyManager>::qt_metacast(_clname);
}

int QtCursorEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtCursorPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QCursor(*)>(_a[2]))); break;
        case 1: d_func()->slotEnumChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtColorEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      24,   22,   21,   21, 0x08,
      64,   21,   21,   21, 0x08,
      94,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtColorEditorFactory[] = {
    "QtColorEditorFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QColor)\0"
    "slotEditorDestroyed(QObject*)\0"
    "slotSetValue(QColor)\0"
};

const QMetaObject QtColorEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtColorPropertyManager>::staticMetaObject, qt_meta_stringdata_QtColorEditorFactory,
      qt_meta_data_QtColorEditorFactory, 0 }
};

const QMetaObject *QtColorEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtColorEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtColorEditorFactory))
        return static_cast<void*>(const_cast< QtColorEditorFactory*>(this));
    return QtAbstractEditorFactory<QtColorPropertyManager>::qt_metacast(_clname);
}

int QtColorEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtColorPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 1: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: d_func()->slotSetValue((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QtFontEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   21,   20,   20, 0x08,
      62,   20,   20,   20, 0x08,
      92,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtFontEditorFactory[] = {
    "QtFontEditorFactory\0\0,\0"
    "slotPropertyChanged(QtProperty*,QFont)\0"
    "slotEditorDestroyed(QObject*)\0"
    "slotSetValue(QFont)\0"
};

const QMetaObject QtFontEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtFontPropertyManager>::staticMetaObject, qt_meta_stringdata_QtFontEditorFactory,
      qt_meta_data_QtFontEditorFactory, 0 }
};

const QMetaObject *QtFontEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtFontEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtFontEditorFactory))
        return static_cast<void*>(const_cast< QtFontEditorFactory*>(this));
    return QtAbstractEditorFactory<QtFontPropertyManager>::qt_metacast(_clname);
}

int QtFontEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtFontPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotPropertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QFont(*)>(_a[2]))); break;
        case 1: d_func()->slotEditorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: d_func()->slotSetValue((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
