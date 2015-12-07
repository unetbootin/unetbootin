/****************************************************************************
** Meta object code from reading C++ file 'properties_p.h'
**
** Created: Thu Mar 5 20:40:07 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../lib/uilib/properties_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'properties_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFormInternal__QAbstractFormBuilderGadget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      16,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      56,   42, 0x00095009,
      81,   66, 0x00095009,
     106,   92, 0x00095009,
     136,  120, 0x00095009,
     168,  148, 0x00095009,
     197,  177, 0x00095009,
     228,  207, 0x00095009,
     260,  239, 0x00095009,
     290,  274, 0x00095009,
     317,  302, 0x00095009,
     344,  328, 0x00095009,
     372,  356, 0x00095009,
     403,  385, 0x00095009,
     444,  418, 0x00095009,
     481,  463, 0x00095009,
     507,  490, 0x00095009,

       0        // eod
};

static const char qt_meta_stringdata_QFormInternal__QAbstractFormBuilderGadget[] = {
    "QFormInternal::QAbstractFormBuilderGadget\0"
    "Qt::ItemFlags\0itemFlags\0Qt::CheckState\0"
    "checkState\0Qt::Alignment\0textAlignment\0"
    "Qt::Orientation\0orientation\0"
    "QSizePolicy::Policy\0sizeType\0"
    "QPalette::ColorRole\0colorRole\0"
    "QPalette::ColorGroup\0colorGroup\0"
    "QFont::StyleStrategy\0styleStrategy\0"
    "Qt::CursorShape\0cursorShape\0Qt::BrushStyle\0"
    "brushStyle\0Qt::ToolBarArea\0toolBarArea\0"
    "QGradient::Type\0gradientType\0"
    "QGradient::Spread\0gradientSpread\0"
    "QGradient::CoordinateMode\0gradientCoordinate\0"
    "QLocale::Language\0language\0QLocale::Country\0"
    "country\0"
};

static const QMetaObject *qt_meta_extradata_QFormInternal__QAbstractFormBuilderGadget[] = {
        &QSizePolicy::staticMetaObject,
        &QPalette::staticMetaObject,
        &QFont::staticMetaObject,
        &QGradient::staticMetaObject,
        &QLocale::staticMetaObject,0
};

static const QMetaObjectExtraData qt_meta_extradata2_QFormInternal__QAbstractFormBuilderGadget = {
    qt_meta_extradata_QFormInternal__QAbstractFormBuilderGadget, 0 
};

const QMetaObject QFormInternal::QAbstractFormBuilderGadget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QFormInternal__QAbstractFormBuilderGadget,
      qt_meta_data_QFormInternal__QAbstractFormBuilderGadget, &qt_meta_extradata2_QFormInternal__QAbstractFormBuilderGadget }
};

const QMetaObject *QFormInternal::QAbstractFormBuilderGadget::metaObject() const
{
    return &staticMetaObject;
}

void *QFormInternal::QAbstractFormBuilderGadget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFormInternal__QAbstractFormBuilderGadget))
        return static_cast<void*>(const_cast< QAbstractFormBuilderGadget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QFormInternal::QAbstractFormBuilderGadget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::ItemFlags*>(_v) = fakeItemFlags(); break;
        case 1: *reinterpret_cast< Qt::CheckState*>(_v) = fakeCheckState(); break;
        case 2: *reinterpret_cast< Qt::Alignment*>(_v) = fakeAlignment(); break;
        case 3: *reinterpret_cast< Qt::Orientation*>(_v) = fakeOrientation(); break;
        case 4: *reinterpret_cast< QSizePolicy::Policy*>(_v) = fakeSizeType(); break;
        case 5: *reinterpret_cast< QPalette::ColorRole*>(_v) = fakeColorRole(); break;
        case 6: *reinterpret_cast< QPalette::ColorGroup*>(_v) = fakeColorGroup(); break;
        case 7: *reinterpret_cast< QFont::StyleStrategy*>(_v) = fakeStyleStrategy(); break;
        case 8: *reinterpret_cast< Qt::CursorShape*>(_v) = fakeCursorShape(); break;
        case 9: *reinterpret_cast< Qt::BrushStyle*>(_v) = fakeBrushStyle(); break;
        case 10: *reinterpret_cast< Qt::ToolBarArea*>(_v) = fakeToolBarArea(); break;
        case 11: *reinterpret_cast< QGradient::Type*>(_v) = fakeGradientType(); break;
        case 12: *reinterpret_cast< QGradient::Spread*>(_v) = fakeGradientSpread(); break;
        case 13: *reinterpret_cast< QGradient::CoordinateMode*>(_v) = fakeGradientCoordinate(); break;
        case 14: *reinterpret_cast< QLocale::Language*>(_v) = fakeLanguage(); break;
        case 15: *reinterpret_cast< QLocale::Country*>(_v) = fakeCountry(); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 16;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 16;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
