/****************************************************************************
** Meta object code from reading C++ file 'qabstractspinbox.h'
**
** Created: Thu Mar 5 18:51:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qabstractspinbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractspinbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractSpinBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
      11,   47, // properties
       2,   80, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   17,   17,   17, 0x0a,
      45,   17,   17,   17, 0x0a,
      56,   17,   17,   17, 0x0a,
      68,   17,   17,   17, 0x0a,
      76,   17,   17,   17, 0x08,
     108,  106,   17,   17, 0x08,

 // properties: name, type, flags
     153,  148, 0x01095103,
     162,  148, 0x01095103,
     182,  168, 0x0009510b,
     192,  148, 0x01095103,
     215,  201, 0x0009510b,
     237,  229, 0x0a095103,
     254,  229, 0x0a095001,
     259,  148, 0x01095103,
     286,  271, 0x0009510b,
     301,  148, 0x01095001,
     317,  148, 0x01095103,

 // enums: name, flags, count, data
     201, 0x0,    3,   88,
     271, 0x0,    2,   94,

 // enum data: key, value
     334, uint(QAbstractSpinBox::UpDownArrows),
     347, uint(QAbstractSpinBox::PlusMinus),
     357, uint(QAbstractSpinBox::NoButtons),
     367, uint(QAbstractSpinBox::CorrectToPreviousValue),
     390, uint(QAbstractSpinBox::CorrectToNearestValue),

       0        // eod
};

static const char qt_meta_stringdata_QAbstractSpinBox[] = {
    "QAbstractSpinBox\0\0editingFinished()\0"
    "stepUp()\0stepDown()\0selectAll()\0clear()\0"
    "_q_editorTextChanged(QString)\0,\0"
    "_q_editorCursorPositionChanged(int,int)\0"
    "bool\0wrapping\0frame\0Qt::Alignment\0"
    "alignment\0readOnly\0ButtonSymbols\0"
    "buttonSymbols\0QString\0specialValueText\0"
    "text\0accelerated\0CorrectionMode\0"
    "correctionMode\0acceptableInput\0"
    "keyboardTracking\0UpDownArrows\0PlusMinus\0"
    "NoButtons\0CorrectToPreviousValue\0"
    "CorrectToNearestValue\0"
};

const QMetaObject QAbstractSpinBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QAbstractSpinBox,
      qt_meta_data_QAbstractSpinBox, 0 }
};

const QMetaObject *QAbstractSpinBox::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractSpinBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractSpinBox))
        return static_cast<void*>(const_cast< QAbstractSpinBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int QAbstractSpinBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: editingFinished(); break;
        case 1: stepUp(); break;
        case 2: stepDown(); break;
        case 3: selectAll(); break;
        case 4: clear(); break;
        case 5: d_func()->_q_editorTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: d_func()->_q_editorCursorPositionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = wrapping(); break;
        case 1: *reinterpret_cast< bool*>(_v) = hasFrame(); break;
        case 2: *reinterpret_cast< Qt::Alignment*>(_v) = alignment(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isReadOnly(); break;
        case 4: *reinterpret_cast< ButtonSymbols*>(_v) = buttonSymbols(); break;
        case 5: *reinterpret_cast< QString*>(_v) = specialValueText(); break;
        case 6: *reinterpret_cast< QString*>(_v) = text(); break;
        case 7: *reinterpret_cast< bool*>(_v) = isAccelerated(); break;
        case 8: *reinterpret_cast< CorrectionMode*>(_v) = correctionMode(); break;
        case 9: *reinterpret_cast< bool*>(_v) = hasAcceptableInput(); break;
        case 10: *reinterpret_cast< bool*>(_v) = keyboardTracking(); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setWrapping(*reinterpret_cast< bool*>(_v)); break;
        case 1: setFrame(*reinterpret_cast< bool*>(_v)); break;
        case 2: setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 3: setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 4: setButtonSymbols(*reinterpret_cast< ButtonSymbols*>(_v)); break;
        case 5: setSpecialValueText(*reinterpret_cast< QString*>(_v)); break;
        case 7: setAccelerated(*reinterpret_cast< bool*>(_v)); break;
        case 8: setCorrectionMode(*reinterpret_cast< CorrectionMode*>(_v)); break;
        case 10: setKeyboardTracking(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QAbstractSpinBox::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
