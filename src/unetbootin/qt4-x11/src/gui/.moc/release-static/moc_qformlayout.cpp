/****************************************************************************
** Meta object code from reading C++ file 'qformlayout.h'
**
** Created: Thu Mar 5 18:56:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qformlayout.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qformlayout.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFormLayout[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       6,   12, // properties
       3,   30, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      30,   12, 0x0009510f,
      62,   48, 0x0009510f,
      90,   76, 0x0009510f,
     105,   76, 0x0009510f,
     123,  119, 0x02095103,
     141,  119, 0x02095103,

 // enums: name, flags, count, data
      12, 0x0,    3,   42,
      48, 0x0,    3,   48,
     157, 0x0,    3,   54,

 // enum data: key, value
     166, uint(QFormLayout::FieldsStayAtSizeHint),
     187, uint(QFormLayout::ExpandingFieldsGrow),
     207, uint(QFormLayout::AllNonFixedFieldsGrow),
     229, uint(QFormLayout::DontWrapRows),
     242, uint(QFormLayout::WrapLongRows),
     255, uint(QFormLayout::WrapAllRows),
     267, uint(QFormLayout::LabelRole),
     277, uint(QFormLayout::FieldRole),
     287, uint(QFormLayout::SpanningRole),

       0        // eod
};

static const char qt_meta_stringdata_QFormLayout[] = {
    "QFormLayout\0FieldGrowthPolicy\0"
    "fieldGrowthPolicy\0RowWrapPolicy\0"
    "rowWrapPolicy\0Qt::Alignment\0labelAlignment\0"
    "formAlignment\0int\0horizontalSpacing\0"
    "verticalSpacing\0ItemRole\0FieldsStayAtSizeHint\0"
    "ExpandingFieldsGrow\0AllNonFixedFieldsGrow\0"
    "DontWrapRows\0WrapLongRows\0WrapAllRows\0"
    "LabelRole\0FieldRole\0SpanningRole\0"
};

const QMetaObject QFormLayout::staticMetaObject = {
    { &QLayout::staticMetaObject, qt_meta_stringdata_QFormLayout,
      qt_meta_data_QFormLayout, 0 }
};

const QMetaObject *QFormLayout::metaObject() const
{
    return &staticMetaObject;
}

void *QFormLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFormLayout))
        return static_cast<void*>(const_cast< QFormLayout*>(this));
    return QLayout::qt_metacast(_clname);
}

int QFormLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< FieldGrowthPolicy*>(_v) = fieldGrowthPolicy(); break;
        case 1: *reinterpret_cast< RowWrapPolicy*>(_v) = rowWrapPolicy(); break;
        case 2: *reinterpret_cast< Qt::Alignment*>(_v) = labelAlignment(); break;
        case 3: *reinterpret_cast< Qt::Alignment*>(_v) = formAlignment(); break;
        case 4: *reinterpret_cast< int*>(_v) = horizontalSpacing(); break;
        case 5: *reinterpret_cast< int*>(_v) = verticalSpacing(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFieldGrowthPolicy(*reinterpret_cast< FieldGrowthPolicy*>(_v)); break;
        case 1: setRowWrapPolicy(*reinterpret_cast< RowWrapPolicy*>(_v)); break;
        case 2: setLabelAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 3: setFormAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 4: setHorizontalSpacing(*reinterpret_cast< int*>(_v)); break;
        case 5: setVerticalSpacing(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        switch (_id) {
        case 0: resetFieldGrowthPolicy(); break;
        case 1: resetRowWrapPolicy(); break;
        case 2: resetLabelAlignment(); break;
        case 3: resetFormAlignment(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
