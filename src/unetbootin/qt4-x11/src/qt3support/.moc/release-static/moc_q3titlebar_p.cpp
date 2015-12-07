/****************************************************************************
** Meta object code from reading C++ file 'q3titlebar_p.h'
**
** Created: Thu Mar 5 19:06:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/q3titlebar_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3titlebar_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3TitleBar[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   12, // methods
       2,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      25,   11,   11,   11, 0x05,
      36,   11,   11,   11, 0x05,
      46,   11,   11,   11, 0x05,
      59,   11,   11,   11, 0x05,
      72,   11,   11,   11, 0x05,
      82,   11,   11,   11, 0x05,
     102,   11,   11,   11, 0x05,
     129,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     145,   11,   11,   11, 0x0a,

 // properties: name, type, flags
     166,  161, 0x01095103,
     176,  161, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_Q3TitleBar[] = {
    "Q3TitleBar\0\0doActivate()\0doNormal()\0"
    "doClose()\0doMaximize()\0doMinimize()\0"
    "doShade()\0showOperationMenu()\0"
    "popupOperationMenu(QPoint)\0doubleClicked()\0"
    "setActive(bool)\0bool\0autoRaise\0movable\0"
};

const QMetaObject Q3TitleBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Q3TitleBar,
      qt_meta_data_Q3TitleBar, 0 }
};

const QMetaObject *Q3TitleBar::metaObject() const
{
    return &staticMetaObject;
}

void *Q3TitleBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3TitleBar))
        return static_cast<void*>(const_cast< Q3TitleBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int Q3TitleBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doActivate(); break;
        case 1: doNormal(); break;
        case 2: doClose(); break;
        case 3: doMaximize(); break;
        case 4: doMinimize(); break;
        case 5: doShade(); break;
        case 6: showOperationMenu(); break;
        case 7: popupOperationMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 8: doubleClicked(); break;
        case 9: setActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = autoRaise(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isMovable(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAutoRaise(*reinterpret_cast< bool*>(_v)); break;
        case 1: setMovable(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3TitleBar::doActivate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Q3TitleBar::doNormal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Q3TitleBar::doClose()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Q3TitleBar::doMaximize()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Q3TitleBar::doMinimize()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void Q3TitleBar::doShade()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void Q3TitleBar::showOperationMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void Q3TitleBar::popupOperationMenu(const QPoint & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Q3TitleBar::doubleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}
QT_END_MOC_NAMESPACE
