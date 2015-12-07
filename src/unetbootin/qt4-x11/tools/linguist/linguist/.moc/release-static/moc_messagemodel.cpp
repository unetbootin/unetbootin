/****************************************************************************
** Meta object code from reading C++ file 'messagemodel.h'
**
** Created: Thu Mar 5 20:55:55 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../messagemodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messagemodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      54,   11,   10,   10, 0x05,
     123,   92,   10,   10, 0x05,
     148,   10,   10,   10, 0x05,
     166,   10,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DataModel[] = {
    "DataModel\0\0words,characters,cs,words2,characters2,cs2\0"
    "statsChanged(int,int,int,int,int,int)\0"
    "finishedCount,oldFinishedCount\0"
    "progressChanged(int,int)\0languageChanged()\0"
    "modifiedChanged()\0"
};

const QMetaObject DataModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DataModel,
      qt_meta_data_DataModel, 0 }
};

const QMetaObject *DataModel::metaObject() const
{
    return &staticMetaObject;
}

void *DataModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataModel))
        return static_cast<void*>(const_cast< DataModel*>(this));
    return QObject::qt_metacast(_clname);
}

int DataModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: statsChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6]))); break;
        case 1: progressChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: languageChanged(); break;
        case 3: modifiedChanged(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DataModel::statsChanged(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DataModel::progressChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DataModel::languageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void DataModel::modifiedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
static const uint qt_meta_data_MultiDataModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      38,   32,   15,   15, 0x05,
      56,   15,   15,   15, 0x05,
      75,   32,   15,   15, 0x05,
     139,   96,   15,   15, 0x05,
     177,   15,   15,   15, 0x05,
     205,  199,   15,   15, 0x05,
     245,  199,   15,   15, 0x05,
     280,  199,   15,   15, 0x05,
     315,  199,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     350,   15,   15,   15, 0x08,
     370,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MultiDataModel[] = {
    "MultiDataModel\0\0modelAppended()\0model\0"
    "modelDeleted(int)\0allModelsDeleted()\0"
    "languageChanged(int)\0"
    "words,characters,cs,words2,characters2,cs2\0"
    "statsChanged(int,int,int,int,int,int)\0"
    "modifiedChanged(bool)\0index\0"
    "multiContextDataChanged(MultiDataIndex)\0"
    "contextDataChanged(MultiDataIndex)\0"
    "messageDataChanged(MultiDataIndex)\0"
    "translationChanged(MultiDataIndex)\0"
    "onModifiedChanged()\0onLanguageChanged()\0"
};

const QMetaObject MultiDataModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MultiDataModel,
      qt_meta_data_MultiDataModel, 0 }
};

const QMetaObject *MultiDataModel::metaObject() const
{
    return &staticMetaObject;
}

void *MultiDataModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MultiDataModel))
        return static_cast<void*>(const_cast< MultiDataModel*>(this));
    return QObject::qt_metacast(_clname);
}

int MultiDataModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: modelAppended(); break;
        case 1: modelDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: allModelsDeleted(); break;
        case 3: languageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: statsChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6]))); break;
        case 5: modifiedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: multiContextDataChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        case 7: contextDataChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        case 8: messageDataChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        case 9: translationChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        case 10: onModifiedChanged(); break;
        case 11: onLanguageChanged(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void MultiDataModel::modelAppended()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MultiDataModel::modelDeleted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MultiDataModel::allModelsDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void MultiDataModel::languageChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MultiDataModel::statsChanged(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MultiDataModel::modifiedChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MultiDataModel::multiContextDataChanged(const MultiDataIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MultiDataModel::contextDataChanged(const MultiDataIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void MultiDataModel::messageDataChanged(const MultiDataIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void MultiDataModel::translationChanged(const MultiDataIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
static const uint qt_meta_data_MessageModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      28,   22,   13,   13, 0x08,
      68,   22,   13,   13, 0x08,
     103,   22,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MessageModel[] = {
    "MessageModel\0\0reset()\0index\0"
    "multiContextItemChanged(MultiDataIndex)\0"
    "contextItemChanged(MultiDataIndex)\0"
    "messageItemChanged(MultiDataIndex)\0"
};

const QMetaObject MessageModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_MessageModel,
      qt_meta_data_MessageModel, 0 }
};

const QMetaObject *MessageModel::metaObject() const
{
    return &staticMetaObject;
}

void *MessageModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MessageModel))
        return static_cast<void*>(const_cast< MessageModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int MessageModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reset(); break;
        case 1: multiContextItemChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        case 2: contextItemChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        case 3: messageItemChanged((*reinterpret_cast< const MultiDataIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
