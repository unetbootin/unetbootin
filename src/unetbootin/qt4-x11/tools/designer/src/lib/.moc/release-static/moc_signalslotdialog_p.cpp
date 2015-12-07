/****************************************************************************
** Meta object code from reading C++ file 'signalslotdialog_p.h'
**
** Created: Thu Mar 5 20:47:15 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/signalslotdialog_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'signalslotdialog_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__SignatureModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      49,   36,   35,   35, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__SignatureModel[] = {
    "qdesigner_internal::SignatureModel\0\0"
    "signature,ok\0checkSignature(QString,bool*)\0"
};

const QMetaObject qdesigner_internal::SignatureModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_qdesigner_internal__SignatureModel,
      qt_meta_data_qdesigner_internal__SignatureModel, 0 }
};

const QMetaObject *qdesigner_internal::SignatureModel::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::SignatureModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__SignatureModel))
        return static_cast<void*>(const_cast< SignatureModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int qdesigner_internal::SignatureModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: checkSignature((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::SignatureModel::checkSignature(const QString & _t1, bool * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__SignaturePanel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      49,   36,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
      79,   35,   35,   35, 0x08,
      89,   35,   35,   35, 0x08,
     104,  102,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__SignaturePanel[] = {
    "qdesigner_internal::SignaturePanel\0\0"
    "signature,ok\0checkSignature(QString,bool*)\0"
    "slotAdd()\0slotRemove()\0,\0"
    "slotSelectionChanged(QItemSelection,QItemSelection)\0"
};

const QMetaObject qdesigner_internal::SignaturePanel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__SignaturePanel,
      qt_meta_data_qdesigner_internal__SignaturePanel, 0 }
};

const QMetaObject *qdesigner_internal::SignaturePanel::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::SignaturePanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__SignaturePanel))
        return static_cast<void*>(const_cast< SignaturePanel*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::SignaturePanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: checkSignature((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool*(*)>(_a[2]))); break;
        case 1: slotAdd(); break;
        case 2: slotRemove(); break;
        case 3: slotSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::SignaturePanel::checkSignature(const QString & _t1, bool * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__SignalSlotDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      51,   38,   37,   37, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__SignalSlotDialog[] = {
    "qdesigner_internal::SignalSlotDialog\0"
    "\0signature,ok\0slotCheckSignature(QString,bool*)\0"
};

const QMetaObject qdesigner_internal::SignalSlotDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__SignalSlotDialog,
      qt_meta_data_qdesigner_internal__SignalSlotDialog, 0 }
};

const QMetaObject *qdesigner_internal::SignalSlotDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::SignalSlotDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__SignalSlotDialog))
        return static_cast<void*>(const_cast< SignalSlotDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::SignalSlotDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotCheckSignature((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
