/****************************************************************************
** Meta object code from reading C++ file 'promotionmodel_p.h'
**
** Created: Thu Mar 5 20:47:03 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/promotionmodel_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'promotionmodel_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PromotionModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      49,   36,   35,   35, 0x05,
     124,  115,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
     193,  188,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PromotionModel[] = {
    "qdesigner_internal::PromotionModel\0\0"
    ",includeFile\0"
    "includeFileChanged(QDesignerWidgetDataBaseItemInterface*,QString)\0"
    ",newName\0"
    "classNameChanged(QDesignerWidgetDataBaseItemInterface*,QString)\0"
    "item\0slotItemChanged(QStandardItem*)\0"
};

const QMetaObject qdesigner_internal::PromotionModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PromotionModel,
      qt_meta_data_qdesigner_internal__PromotionModel, 0 }
};

const QMetaObject *qdesigner_internal::PromotionModel::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PromotionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PromotionModel))
        return static_cast<void*>(const_cast< PromotionModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int qdesigner_internal::PromotionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: includeFileChanged((*reinterpret_cast< QDesignerWidgetDataBaseItemInterface*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: classNameChanged((*reinterpret_cast< QDesignerWidgetDataBaseItemInterface*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: slotItemChanged((*reinterpret_cast< QStandardItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::PromotionModel::includeFileChanged(QDesignerWidgetDataBaseItemInterface * _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::PromotionModel::classNameChanged(QDesignerWidgetDataBaseItemInterface * _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
