/****************************************************************************
** Meta object code from reading C++ file 'qspinbox.h'
**
** Created: Fri May 30 22:33:49 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qspinbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qspinbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSpinBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       7,   25, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      28,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      54,   50,    9,    9, 0x0a,

 // properties: name, type, flags
      76,   68, 0x0a095103,
      83,   68, 0x0a095103,
      90,   68, 0x0a095001,
     104,  100, 0x02095103,
     112,  100, 0x02095103,
     120,  100, 0x02095103,
     131,  100, 0x02195103,

       0        // eod
};

static const char qt_meta_stringdata_QSpinBox[] = {
    "QSpinBox\0\0valueChanged(int)\0"
    "valueChanged(QString)\0val\0setValue(int)\0"
    "QString\0suffix\0prefix\0cleanText\0int\0"
    "minimum\0maximum\0singleStep\0value\0"
};

const QMetaObject QSpinBox::staticMetaObject = {
    { &QAbstractSpinBox::staticMetaObject, qt_meta_stringdata_QSpinBox,
      qt_meta_data_QSpinBox, 0 }
};

const QMetaObject *QSpinBox::metaObject() const
{
    return &staticMetaObject;
}

void *QSpinBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSpinBox))
	return static_cast<void*>(const_cast< QSpinBox*>(this));
    return QAbstractSpinBox::qt_metacast(_clname);
}

int QSpinBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: valueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = suffix(); break;
        case 1: *reinterpret_cast< QString*>(_v) = prefix(); break;
        case 2: *reinterpret_cast< QString*>(_v) = cleanText(); break;
        case 3: *reinterpret_cast< int*>(_v) = minimum(); break;
        case 4: *reinterpret_cast< int*>(_v) = maximum(); break;
        case 5: *reinterpret_cast< int*>(_v) = singleStep(); break;
        case 6: *reinterpret_cast< int*>(_v) = value(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSuffix(*reinterpret_cast< QString*>(_v)); break;
        case 1: setPrefix(*reinterpret_cast< QString*>(_v)); break;
        case 3: setMinimum(*reinterpret_cast< int*>(_v)); break;
        case 4: setMaximum(*reinterpret_cast< int*>(_v)); break;
        case 5: setSingleStep(*reinterpret_cast< int*>(_v)); break;
        case 6: setValue(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QSpinBox::valueChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QSpinBox::valueChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_QDoubleSpinBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       8,   25, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      37,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   59,   15,   15, 0x0a,

 // properties: name, type, flags
      88,   80, 0x0a095103,
      95,   80, 0x0a095103,
     102,   80, 0x0a095001,
     116,  112, 0x02095103,
     132,  125, 0x06095103,
     140,  125, 0x06095103,
     148,  125, 0x06095103,
     159,  125, 0x06195103,

       0        // eod
};

static const char qt_meta_stringdata_QDoubleSpinBox[] = {
    "QDoubleSpinBox\0\0valueChanged(double)\0"
    "valueChanged(QString)\0val\0setValue(double)\0"
    "QString\0prefix\0suffix\0cleanText\0int\0"
    "decimals\0double\0minimum\0maximum\0"
    "singleStep\0value\0"
};

const QMetaObject QDoubleSpinBox::staticMetaObject = {
    { &QAbstractSpinBox::staticMetaObject, qt_meta_stringdata_QDoubleSpinBox,
      qt_meta_data_QDoubleSpinBox, 0 }
};

const QMetaObject *QDoubleSpinBox::metaObject() const
{
    return &staticMetaObject;
}

void *QDoubleSpinBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDoubleSpinBox))
	return static_cast<void*>(const_cast< QDoubleSpinBox*>(this));
    return QAbstractSpinBox::qt_metacast(_clname);
}

int QDoubleSpinBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: valueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = prefix(); break;
        case 1: *reinterpret_cast< QString*>(_v) = suffix(); break;
        case 2: *reinterpret_cast< QString*>(_v) = cleanText(); break;
        case 3: *reinterpret_cast< int*>(_v) = decimals(); break;
        case 4: *reinterpret_cast< double*>(_v) = minimum(); break;
        case 5: *reinterpret_cast< double*>(_v) = maximum(); break;
        case 6: *reinterpret_cast< double*>(_v) = singleStep(); break;
        case 7: *reinterpret_cast< double*>(_v) = value(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPrefix(*reinterpret_cast< QString*>(_v)); break;
        case 1: setSuffix(*reinterpret_cast< QString*>(_v)); break;
        case 3: setDecimals(*reinterpret_cast< int*>(_v)); break;
        case 4: setMinimum(*reinterpret_cast< double*>(_v)); break;
        case 5: setMaximum(*reinterpret_cast< double*>(_v)); break;
        case 6: setSingleStep(*reinterpret_cast< double*>(_v)); break;
        case 7: setValue(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QDoubleSpinBox::valueChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDoubleSpinBox::valueChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
