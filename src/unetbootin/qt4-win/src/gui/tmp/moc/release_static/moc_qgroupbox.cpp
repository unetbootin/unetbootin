/****************************************************************************
** Meta object code from reading C++ file 'qgroupbox.h'
**
** Created: Fri May 30 22:27:34 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qgroupbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgroupbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGroupBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       5,   35, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   11,   10,   10, 0x05,
      33,   10,   10,   10, 0x25,
      43,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   11,   10,   10, 0x0a,
      76,   74,   10,   10, 0x08,

 // properties: name, type, flags
     112,  104, 0x0a095103,
     132,  118, 0x0009510b,
     147,  142, 0x01095103,
     152,  142, 0x01095103,
      11,  142, 0x01195103,

       0        // eod
};

static const char qt_meta_stringdata_QGroupBox[] = {
    "QGroupBox\0\0checked\0clicked(bool)\0"
    "clicked()\0toggled(bool)\0setChecked(bool)\0"
    "b\0_q_setChildrenEnabled(bool)\0QString\0"
    "title\0Qt::Alignment\0alignment\0bool\0"
    "flat\0checkable\0"
};

const QMetaObject QGroupBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QGroupBox,
      qt_meta_data_QGroupBox, 0 }
};

const QMetaObject *QGroupBox::metaObject() const
{
    return &staticMetaObject;
}

void *QGroupBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGroupBox))
	return static_cast<void*>(const_cast< QGroupBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int QGroupBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: clicked(); break;
        case 2: toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: setChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: d_func()->_q_setChildrenEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = title(); break;
        case 1: *reinterpret_cast< Qt::Alignment*>(_v) = alignment(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isFlat(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isCheckable(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isChecked(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTitle(*reinterpret_cast< QString*>(_v)); break;
        case 1: setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 2: setFlat(*reinterpret_cast< bool*>(_v)); break;
        case 3: setCheckable(*reinterpret_cast< bool*>(_v)); break;
        case 4: setChecked(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        bool *_b = reinterpret_cast<bool*>(_a[0]);
        switch (_id) {
        case 4: *_b = isCheckable(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QGroupBox::clicked(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, 1, _a);
}

// SIGNAL 2
void QGroupBox::toggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
