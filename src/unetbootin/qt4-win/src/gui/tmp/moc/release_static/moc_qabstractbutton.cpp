/****************************************************************************
** Meta object code from reading C++ file 'qabstractbutton.h'
**
** Created: Fri May 30 22:33:32 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qabstractbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractButton[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
      11,   70, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      27,   16,   16,   16, 0x05,
      46,   38,   16,   16, 0x05,
      60,   16,   16,   16, 0x25,
      70,   38,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      89,   84,   16,   16, 0x0a,
     113,  108,   16,   16, 0x0a,
     131,   16,   16,   16, 0x2a,
     146,   16,   16,   16, 0x0a,
     154,   16,   16,   16, 0x0a,
     163,   16,   16,   16, 0x0a,
     182,  180,   16,   16, 0x1a,

 // properties: name, type, flags
     202,  194, 0x0a095103,
     213,  207, 0x45095103,
     224,  218, 0x15095103,
     246,  233, 0x4c095103,
     260,  255, 0x01095103,
      38,  255, 0x01195103,
     270,  255, 0x01095103,
     281,  255, 0x01095103,
     299,  295, 0x02095103,
     315,  295, 0x02095103,
     334,  255, 0x01094103,

       0        // eod
};

static const char qt_meta_stringdata_QAbstractButton[] = {
    "QAbstractButton\0\0pressed()\0released()\0"
    "checked\0clicked(bool)\0clicked()\0"
    "toggled(bool)\0size\0setIconSize(QSize)\0"
    "msec\0animateClick(int)\0animateClick()\0"
    "click()\0toggle()\0setChecked(bool)\0b\0"
    "setOn(bool)\0QString\0text\0QIcon\0icon\0"
    "QSize\0iconSize\0QKeySequence\0shortcut\0"
    "bool\0checkable\0autoRepeat\0autoExclusive\0"
    "int\0autoRepeatDelay\0autoRepeatInterval\0"
    "down\0"
};

const QMetaObject QAbstractButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QAbstractButton,
      qt_meta_data_QAbstractButton, 0 }
};

const QMetaObject *QAbstractButton::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractButton))
	return static_cast<void*>(const_cast< QAbstractButton*>(this));
    return QWidget::qt_metacast(_clname);
}

int QAbstractButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: pressed(); break;
        case 1: released(); break;
        case 2: clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: clicked(); break;
        case 4: toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: setIconSize((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 6: animateClick((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: animateClick(); break;
        case 8: click(); break;
        case 9: toggle(); break;
        case 10: setChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: setOn((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< QIcon*>(_v) = icon(); break;
        case 2: *reinterpret_cast< QSize*>(_v) = iconSize(); break;
        case 3: *reinterpret_cast< QKeySequence*>(_v) = shortcut(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isCheckable(); break;
        case 5: *reinterpret_cast< bool*>(_v) = isChecked(); break;
        case 6: *reinterpret_cast< bool*>(_v) = autoRepeat(); break;
        case 7: *reinterpret_cast< bool*>(_v) = autoExclusive(); break;
        case 8: *reinterpret_cast< int*>(_v) = autoRepeatDelay(); break;
        case 9: *reinterpret_cast< int*>(_v) = autoRepeatInterval(); break;
        case 10: *reinterpret_cast< bool*>(_v) = isDown(); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setIcon(*reinterpret_cast< QIcon*>(_v)); break;
        case 2: setIconSize(*reinterpret_cast< QSize*>(_v)); break;
        case 3: setShortcut(*reinterpret_cast< QKeySequence*>(_v)); break;
        case 4: setCheckable(*reinterpret_cast< bool*>(_v)); break;
        case 5: setChecked(*reinterpret_cast< bool*>(_v)); break;
        case 6: setAutoRepeat(*reinterpret_cast< bool*>(_v)); break;
        case 7: setAutoExclusive(*reinterpret_cast< bool*>(_v)); break;
        case 8: setAutoRepeatDelay(*reinterpret_cast< int*>(_v)); break;
        case 9: setAutoRepeatInterval(*reinterpret_cast< int*>(_v)); break;
        case 10: setDown(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        bool *_b = reinterpret_cast<bool*>(_a[0]);
        switch (_id) {
        case 5: *_b = isCheckable(); break;
        }
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
void QAbstractButton::pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QAbstractButton::released()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QAbstractButton::clicked(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, 3, _a);
}

// SIGNAL 4
void QAbstractButton::toggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
