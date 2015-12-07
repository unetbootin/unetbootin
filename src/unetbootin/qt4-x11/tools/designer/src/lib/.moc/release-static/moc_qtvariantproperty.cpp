/****************************************************************************
** Meta object code from reading C++ file 'qtvariantproperty.h'
**
** Created: Thu Mar 5 20:42:16 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtpropertybrowser/qtvariantproperty.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtvariantproperty.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtVariantPropertyManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      39,   26,   25,   25, 0x05,
      97,   74,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     144,   26,   25,   25, 0x0a,
     200,  175,   25,   25, 0x0a,
     245,  243,   25,   25, 0x08,
     282,  279,   25,   25, 0x08,
     320,  243,   25,   25, 0x08,
     359,  243,   25,   25, 0x08,
     396,  279,   25,   25, 0x08,
     440,  243,   25,   25, 0x08,
     482,  243,   25,   25, 0x08,
     519,  243,   25,   25, 0x08,
     554,  243,   25,   25, 0x08,
     592,  243,   25,   25, 0x08,
     631,  243,   25,   25, 0x08,
     667,  279,   25,   25, 0x08,
     709,  243,   25,   25, 0x08,
     745,  243,   25,   25, 0x08,
     785,  243,   25,   25, 0x08,
     828,  243,   25,   25, 0x08,
     864,  243,   25,   25, 0x08,
     902,  243,   25,   25, 0x08,
     939,  243,   25,   25, 0x08,
     977,  243,   25,   25, 0x08,
    1013,  279,   25,   25, 0x08,
    1055,  243,   25,   25, 0x08,
    1092,  279,   25,   25, 0x08,
    1136,  243,   25,   25, 0x08,
    1172,  243,   25,   25, 0x08,
    1213,  243,   25,   25, 0x08,
    1250,  243,   25,   25, 0x08,
    1292,  243,   25,   25, 0x08,
    1329,  243,   25,   25, 0x08,
    1375,  243,   25,   25, 0x08,
    1425,  243,   25,   25, 0x08,
    1467,  243,   25,   25, 0x08,
    1503,  243,   25,   25, 0x08,
    1541,  243,   25,   25, 0x08,
    1587,  279,   25,   25, 0x08,
    1645,  243,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtVariantPropertyManager[] = {
    "QtVariantPropertyManager\0\0property,val\0"
    "valueChanged(QtProperty*,QVariant)\0"
    "property,attribute,val\0"
    "attributeChanged(QtProperty*,QString,QVariant)\0"
    "setValue(QtProperty*,QVariant)\0"
    "property,attribute,value\0"
    "setAttribute(QtProperty*,QString,QVariant)\0"
    ",\0slotValueChanged(QtProperty*,int)\0"
    ",,\0slotRangeChanged(QtProperty*,int,int)\0"
    "slotSingleStepChanged(QtProperty*,int)\0"
    "slotValueChanged(QtProperty*,double)\0"
    "slotRangeChanged(QtProperty*,double,double)\0"
    "slotSingleStepChanged(QtProperty*,double)\0"
    "slotDecimalsChanged(QtProperty*,int)\0"
    "slotValueChanged(QtProperty*,bool)\0"
    "slotValueChanged(QtProperty*,QString)\0"
    "slotRegExpChanged(QtProperty*,QRegExp)\0"
    "slotValueChanged(QtProperty*,QDate)\0"
    "slotRangeChanged(QtProperty*,QDate,QDate)\0"
    "slotValueChanged(QtProperty*,QTime)\0"
    "slotValueChanged(QtProperty*,QDateTime)\0"
    "slotValueChanged(QtProperty*,QKeySequence)\0"
    "slotValueChanged(QtProperty*,QChar)\0"
    "slotValueChanged(QtProperty*,QLocale)\0"
    "slotValueChanged(QtProperty*,QPoint)\0"
    "slotValueChanged(QtProperty*,QPointF)\0"
    "slotValueChanged(QtProperty*,QSize)\0"
    "slotRangeChanged(QtProperty*,QSize,QSize)\0"
    "slotValueChanged(QtProperty*,QSizeF)\0"
    "slotRangeChanged(QtProperty*,QSizeF,QSizeF)\0"
    "slotValueChanged(QtProperty*,QRect)\0"
    "slotConstraintChanged(QtProperty*,QRect)\0"
    "slotValueChanged(QtProperty*,QRectF)\0"
    "slotConstraintChanged(QtProperty*,QRectF)\0"
    "slotValueChanged(QtProperty*,QColor)\0"
    "slotEnumNamesChanged(QtProperty*,QStringList)\0"
    "slotEnumIconsChanged(QtProperty*,QMap<int,QIcon>)\0"
    "slotValueChanged(QtProperty*,QSizePolicy)\0"
    "slotValueChanged(QtProperty*,QFont)\0"
    "slotValueChanged(QtProperty*,QCursor)\0"
    "slotFlagNamesChanged(QtProperty*,QStringList)\0"
    "slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)\0"
    "slotPropertyRemoved(QtProperty*,QtProperty*)\0"
};

const QMetaObject QtVariantPropertyManager::staticMetaObject = {
    { &QtAbstractPropertyManager::staticMetaObject, qt_meta_stringdata_QtVariantPropertyManager,
      qt_meta_data_QtVariantPropertyManager, 0 }
};

const QMetaObject *QtVariantPropertyManager::metaObject() const
{
    return &staticMetaObject;
}

void *QtVariantPropertyManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtVariantPropertyManager))
        return static_cast<void*>(const_cast< QtVariantPropertyManager*>(this));
    return QtAbstractPropertyManager::qt_metacast(_clname);
}

int QtVariantPropertyManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractPropertyManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 1: attributeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 2: setValue((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 3: setAttribute((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 4: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: d_func()->slotSingleStepChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 8: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 9: d_func()->slotSingleStepChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 10: d_func()->slotDecimalsChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 12: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 13: d_func()->slotRegExpChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QRegExp(*)>(_a[2]))); break;
        case 14: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2]))); break;
        case 15: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QDate(*)>(_a[2])),(*reinterpret_cast< const QDate(*)>(_a[3]))); break;
        case 16: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QTime(*)>(_a[2]))); break;
        case 17: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QDateTime(*)>(_a[2]))); break;
        case 18: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QKeySequence(*)>(_a[2]))); break;
        case 19: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QChar(*)>(_a[2]))); break;
        case 20: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QLocale(*)>(_a[2]))); break;
        case 21: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 22: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QPointF(*)>(_a[2]))); break;
        case 23: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QSize(*)>(_a[2]))); break;
        case 24: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QSize(*)>(_a[2])),(*reinterpret_cast< const QSize(*)>(_a[3]))); break;
        case 25: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QSizeF(*)>(_a[2]))); break;
        case 26: d_func()->slotRangeChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QSizeF(*)>(_a[2])),(*reinterpret_cast< const QSizeF(*)>(_a[3]))); break;
        case 27: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QRect(*)>(_a[2]))); break;
        case 28: d_func()->slotConstraintChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QRect(*)>(_a[2]))); break;
        case 29: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2]))); break;
        case 30: d_func()->slotConstraintChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2]))); break;
        case 31: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 32: d_func()->slotEnumNamesChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 33: d_func()->slotEnumIconsChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QMap<int,QIcon>(*)>(_a[2]))); break;
        case 34: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QSizePolicy(*)>(_a[2]))); break;
        case 35: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QFont(*)>(_a[2]))); break;
        case 36: d_func()->slotValueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QCursor(*)>(_a[2]))); break;
        case 37: d_func()->slotFlagNamesChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 38: d_func()->slotPropertyInserted((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2])),(*reinterpret_cast< QtProperty*(*)>(_a[3]))); break;
        case 39: d_func()->slotPropertyRemoved((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< QtProperty*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 40;
    }
    return _id;
}

// SIGNAL 0
void QtVariantPropertyManager::valueChanged(QtProperty * _t1, const QVariant & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtVariantPropertyManager::attributeChanged(QtProperty * _t1, const QString & _t2, const QVariant & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_QtVariantEditorFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QtVariantEditorFactory[] = {
    "QtVariantEditorFactory\0"
};

const QMetaObject QtVariantEditorFactory::staticMetaObject = {
    { &QtAbstractEditorFactory<QtVariantPropertyManager>::staticMetaObject, qt_meta_stringdata_QtVariantEditorFactory,
      qt_meta_data_QtVariantEditorFactory, 0 }
};

const QMetaObject *QtVariantEditorFactory::metaObject() const
{
    return &staticMetaObject;
}

void *QtVariantEditorFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtVariantEditorFactory))
        return static_cast<void*>(const_cast< QtVariantEditorFactory*>(this));
    return QtAbstractEditorFactory<QtVariantPropertyManager>::qt_metacast(_clname);
}

int QtVariantEditorFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractEditorFactory<QtVariantPropertyManager>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
