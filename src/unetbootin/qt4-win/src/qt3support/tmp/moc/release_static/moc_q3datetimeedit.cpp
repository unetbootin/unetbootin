/****************************************************************************
** Meta object code from reading C++ file 'q3datetimeedit.h'
**
** Created: Fri May 30 22:41:16 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/q3datetimeedit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3datetimeedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3DateTimeEditBase[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,
      29,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Q3DateTimeEditBase[] = {
    "Q3DateTimeEditBase\0\0stepUp()\0stepDown()\0"
};

const QMetaObject Q3DateTimeEditBase::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Q3DateTimeEditBase,
      qt_meta_data_Q3DateTimeEditBase, 0 }
};

const QMetaObject *Q3DateTimeEditBase::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DateTimeEditBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DateTimeEditBase))
	return static_cast<void*>(const_cast< Q3DateTimeEditBase*>(this));
    return QWidget::qt_metacast(_clname);
}

int Q3DateTimeEditBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: stepUp(); break;
        case 1: stepDown(); break;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_Q3DateEdit[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       5,   25, // properties
       1,   40, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   12,   11,   11, 0x0a,
      52,   11,   11,   11, 0x09,

 // properties: name, type, flags
      74,   68, 0x0009510b,
      12,   80, 0x0e195103,
      91,   86, 0x01095103,
     103,   80, 0x0e095103,
     112,   80, 0x0e095103,

 // enums: name, flags, count, data
      68, 0x0,    4,   44,

 // enum data: key, value
     121, uint(Q3DateEdit::DMY),
     125, uint(Q3DateEdit::MDY),
     129, uint(Q3DateEdit::YMD),
     133, uint(Q3DateEdit::YDM),

       0        // eod
};

static const char qt_meta_stringdata_Q3DateEdit[] = {
    "Q3DateEdit\0\0date\0valueChanged(QDate)\0"
    "setDate(QDate)\0updateButtons()\0Order\0"
    "order\0QDate\0bool\0autoAdvance\0maxValue\0"
    "minValue\0DMY\0MDY\0YMD\0YDM\0"
};

const QMetaObject Q3DateEdit::staticMetaObject = {
    { &Q3DateTimeEditBase::staticMetaObject, qt_meta_stringdata_Q3DateEdit,
      qt_meta_data_Q3DateEdit, 0 }
};

const QMetaObject *Q3DateEdit::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DateEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DateEdit))
	return static_cast<void*>(const_cast< Q3DateEdit*>(this));
    return Q3DateTimeEditBase::qt_metacast(_clname);
}

int Q3DateEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DateTimeEditBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 1: setDate((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 2: updateButtons(); break;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Order*>(_v) = order(); break;
        case 1: *reinterpret_cast< QDate*>(_v) = date(); break;
        case 2: *reinterpret_cast< bool*>(_v) = autoAdvance(); break;
        case 3: *reinterpret_cast< QDate*>(_v) = maxValue(); break;
        case 4: *reinterpret_cast< QDate*>(_v) = minValue(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrder(*reinterpret_cast< Order*>(_v)); break;
        case 1: setDate(*reinterpret_cast< QDate*>(_v)); break;
        case 2: setAutoAdvance(*reinterpret_cast< bool*>(_v)); break;
        case 3: setMaxValue(*reinterpret_cast< QDate*>(_v)); break;
        case 4: setMinValue(*reinterpret_cast< QDate*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
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
void Q3DateEdit::valueChanged(const QDate & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_Q3TimeEdit[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       5,   25, // properties
       1,   40, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   12,   11,   11, 0x0a,
      52,   11,   11,   11, 0x09,

 // properties: name, type, flags
      12,   68, 0x0f195103,
      79,   74, 0x01095103,
      91,   68, 0x0f095103,
     100,   68, 0x0f095103,
     117,  109, 0x0009510b,

 // enums: name, flags, count, data
     109, 0x1,    4,   44,

 // enum data: key, value
     125, uint(Q3TimeEdit::Hours),
     131, uint(Q3TimeEdit::Minutes),
     139, uint(Q3TimeEdit::Seconds),
     147, uint(Q3TimeEdit::AMPM),

       0        // eod
};

static const char qt_meta_stringdata_Q3TimeEdit[] = {
    "Q3TimeEdit\0\0time\0valueChanged(QTime)\0"
    "setTime(QTime)\0updateButtons()\0QTime\0"
    "bool\0autoAdvance\0maxValue\0minValue\0"
    "Display\0display\0Hours\0Minutes\0Seconds\0"
    "AMPM\0"
};

const QMetaObject Q3TimeEdit::staticMetaObject = {
    { &Q3DateTimeEditBase::staticMetaObject, qt_meta_stringdata_Q3TimeEdit,
      qt_meta_data_Q3TimeEdit, 0 }
};

const QMetaObject *Q3TimeEdit::metaObject() const
{
    return &staticMetaObject;
}

void *Q3TimeEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3TimeEdit))
	return static_cast<void*>(const_cast< Q3TimeEdit*>(this));
    return Q3DateTimeEditBase::qt_metacast(_clname);
}

int Q3TimeEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DateTimeEditBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< const QTime(*)>(_a[1]))); break;
        case 1: setTime((*reinterpret_cast< const QTime(*)>(_a[1]))); break;
        case 2: updateButtons(); break;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QTime*>(_v) = time(); break;
        case 1: *reinterpret_cast< bool*>(_v) = autoAdvance(); break;
        case 2: *reinterpret_cast< QTime*>(_v) = maxValue(); break;
        case 3: *reinterpret_cast< QTime*>(_v) = minValue(); break;
        case 4: *reinterpret_cast<int*>(_v) = QFlag(display()); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTime(*reinterpret_cast< QTime*>(_v)); break;
        case 1: setAutoAdvance(*reinterpret_cast< bool*>(_v)); break;
        case 2: setMaxValue(*reinterpret_cast< QTime*>(_v)); break;
        case 3: setMinValue(*reinterpret_cast< QTime*>(_v)); break;
        case 4: setDisplay(QFlag(*reinterpret_cast<int*>(_v))); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
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
void Q3TimeEdit::valueChanged(const QTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_Q3DateTimeEdit[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       1,   30, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,   49,   15,   15, 0x0a,
      77,   75,   15,   15, 0x09,
      95,   93,   15,   15, 0x09,

 // properties: name, type, flags
     121,  111, 0x10195103,

       0        // eod
};

static const char qt_meta_stringdata_Q3DateTimeEdit[] = {
    "Q3DateTimeEdit\0\0datetime\0"
    "valueChanged(QDateTime)\0dt\0"
    "setDateTime(QDateTime)\0d\0newValue(QDate)\0"
    "t\0newValue(QTime)\0QDateTime\0dateTime\0"
};

const QMetaObject Q3DateTimeEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Q3DateTimeEdit,
      qt_meta_data_Q3DateTimeEdit, 0 }
};

const QMetaObject *Q3DateTimeEdit::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DateTimeEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DateTimeEdit))
	return static_cast<void*>(const_cast< Q3DateTimeEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int Q3DateTimeEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 1: setDateTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 2: newValue((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 3: newValue((*reinterpret_cast< const QTime(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QDateTime*>(_v) = dateTime(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setDateTime(*reinterpret_cast< QDateTime*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3DateTimeEdit::valueChanged(const QDateTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
