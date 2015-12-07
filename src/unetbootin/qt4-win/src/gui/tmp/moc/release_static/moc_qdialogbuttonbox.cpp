/****************************************************************************
** Meta object code from reading C++ file 'qdialogbuttonbox.h'
**
** Created: Fri May 30 22:27:16 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qdialogbuttonbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdialogbuttonbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDialogButtonBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       3,   40, // properties
       1,   49, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   18,   17,   17, 0x05,
      51,   17,   17,   17, 0x05,
      62,   17,   17,   17, 0x05,
      78,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      89,   17,   17,   17, 0x08,
     114,   17,   17,   17, 0x08,

 // properties: name, type, flags
     157,  141, 0x0009510b,
     185,  169, 0x0009510b,
     206,  201, 0x01095103,

 // enums: name, flags, count, data
     169, 0x1,   19,   53,

 // enum data: key, value
     220, uint(QDialogButtonBox::NoButton),
     229, uint(QDialogButtonBox::Ok),
     232, uint(QDialogButtonBox::Save),
     237, uint(QDialogButtonBox::SaveAll),
     245, uint(QDialogButtonBox::Open),
     250, uint(QDialogButtonBox::Yes),
     254, uint(QDialogButtonBox::YesToAll),
     263, uint(QDialogButtonBox::No),
     266, uint(QDialogButtonBox::NoToAll),
     274, uint(QDialogButtonBox::Abort),
     280, uint(QDialogButtonBox::Retry),
     286, uint(QDialogButtonBox::Ignore),
     293, uint(QDialogButtonBox::Close),
     299, uint(QDialogButtonBox::Cancel),
     306, uint(QDialogButtonBox::Discard),
     314, uint(QDialogButtonBox::Help),
     319, uint(QDialogButtonBox::Apply),
     325, uint(QDialogButtonBox::Reset),
     331, uint(QDialogButtonBox::RestoreDefaults),

       0        // eod
};

static const char qt_meta_stringdata_QDialogButtonBox[] = {
    "QDialogButtonBox\0\0button\0"
    "clicked(QAbstractButton*)\0accepted()\0"
    "helpRequested()\0rejected()\0"
    "_q_handleButtonClicked()\0"
    "_q_handleButtonDestroyed()\0Qt::Orientation\0"
    "orientation\0StandardButtons\0standardButtons\0"
    "bool\0centerButtons\0NoButton\0Ok\0Save\0"
    "SaveAll\0Open\0Yes\0YesToAll\0No\0NoToAll\0"
    "Abort\0Retry\0Ignore\0Close\0Cancel\0Discard\0"
    "Help\0Apply\0Reset\0RestoreDefaults\0"
};

const QMetaObject QDialogButtonBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDialogButtonBox,
      qt_meta_data_QDialogButtonBox, 0 }
};

const QMetaObject *QDialogButtonBox::metaObject() const
{
    return &staticMetaObject;
}

void *QDialogButtonBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDialogButtonBox))
	return static_cast<void*>(const_cast< QDialogButtonBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDialogButtonBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 1: accepted(); break;
        case 2: helpRequested(); break;
        case 3: rejected(); break;
        case 4: d_func()->_q_handleButtonClicked(); break;
        case 5: d_func()->_q_handleButtonDestroyed(); break;
        }
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 1: *reinterpret_cast<int*>(_v) = QFlag(standardButtons()); break;
        case 2: *reinterpret_cast< bool*>(_v) = centerButtons(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 1: setStandardButtons(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 2: setCenterButtons(*reinterpret_cast< bool*>(_v)); break;
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
void QDialogButtonBox::clicked(QAbstractButton * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDialogButtonBox::accepted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QDialogButtonBox::helpRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QDialogButtonBox::rejected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
