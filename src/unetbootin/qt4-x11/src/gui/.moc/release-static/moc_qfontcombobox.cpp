/****************************************************************************
** Meta object code from reading C++ file 'qfontcombobox.h'
**
** Created: Thu Mar 5 18:51:35 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qfontcombobox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfontcombobox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFontComboBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       3,   32, // properties
       1,   44, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,   15,   14,   14, 0x0a,
      65,   14,   14,   14, 0x08,
      92,   14,   14,   14, 0x08,

 // properties: name, type, flags
     138,  109, 0x0009510b,
     164,  152, 0x0009510b,
     182,  176, 0x40495103,

 // properties: notify_signal_id
       0,
       0,
       0,

 // enums: name, flags, count, data
     152, 0x1,    5,   48,

 // enum data: key, value
     194, uint(QFontComboBox::AllFonts),
     203, uint(QFontComboBox::ScalableFonts),
     217, uint(QFontComboBox::NonScalableFonts),
     234, uint(QFontComboBox::MonospacedFonts),
     250, uint(QFontComboBox::ProportionalFonts),

       0        // eod
};

static const char qt_meta_stringdata_QFontComboBox[] = {
    "QFontComboBox\0\0f\0currentFontChanged(QFont)\0"
    "setCurrentFont(QFont)\0_q_currentChanged(QString)\0"
    "_q_updateModel()\0QFontDatabase::WritingSystem\0"
    "writingSystem\0FontFilters\0fontFilters\0"
    "QFont\0currentFont\0AllFonts\0ScalableFonts\0"
    "NonScalableFonts\0MonospacedFonts\0"
    "ProportionalFonts\0"
};

static const QMetaObject *qt_meta_extradata_QFontComboBox[] = {
        &QFontDatabase::staticMetaObject,0
};

static const QMetaObjectExtraData qt_meta_extradata2_QFontComboBox = {
    qt_meta_extradata_QFontComboBox, 0 
};

const QMetaObject QFontComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_QFontComboBox,
      qt_meta_data_QFontComboBox, &qt_meta_extradata2_QFontComboBox }
};

const QMetaObject *QFontComboBox::metaObject() const
{
    return &staticMetaObject;
}

void *QFontComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFontComboBox))
        return static_cast<void*>(const_cast< QFontComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int QFontComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentFontChanged((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 1: setCurrentFont((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 2: d_func()->_q_currentChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: d_func()->_q_updateModel(); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QFontDatabase::WritingSystem*>(_v) = writingSystem(); break;
        case 1: *reinterpret_cast<int*>(_v) = QFlag(fontFilters()); break;
        case 2: *reinterpret_cast< QFont*>(_v) = currentFont(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setWritingSystem(*reinterpret_cast< QFontDatabase::WritingSystem*>(_v)); break;
        case 1: setFontFilters(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 2: setCurrentFont(*reinterpret_cast< QFont*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QFontComboBox::currentFontChanged(const QFont & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
