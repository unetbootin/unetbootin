/****************************************************************************
** Meta object code from reading C++ file 'qsystemtrayicon.h'
**
** Created: Thu Mar 5 18:57:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../util/qsystemtrayicon.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsystemtrayicon.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSystemTrayIcon[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       3,   37, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      24,   17,   16,   16, 0x05,
      69,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      94,   86,   16,   16, 0x0a,
     111,   16,   16,   16, 0x0a,
     118,   16,   16,   16, 0x0a,

 // properties: name, type, flags
     133,  125, 0x0a095103,
     147,  141, 0x45095103,
      86,  152, 0x01094103,

       0        // eod
};

static const char qt_meta_stringdata_QSystemTrayIcon[] = {
    "QSystemTrayIcon\0\0reason\0"
    "activated(QSystemTrayIcon::ActivationReason)\0"
    "messageClicked()\0visible\0setVisible(bool)\0"
    "show()\0hide()\0QString\0toolTip\0QIcon\0"
    "icon\0bool\0"
};

const QMetaObject QSystemTrayIcon::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QSystemTrayIcon,
      qt_meta_data_QSystemTrayIcon, 0 }
};

const QMetaObject *QSystemTrayIcon::metaObject() const
{
    return &staticMetaObject;
}

void *QSystemTrayIcon::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSystemTrayIcon))
        return static_cast<void*>(const_cast< QSystemTrayIcon*>(this));
    return QObject::qt_metacast(_clname);
}

int QSystemTrayIcon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 1: messageClicked(); break;
        case 2: setVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: show(); break;
        case 4: hide(); break;
        default: ;
        }
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = toolTip(); break;
        case 1: *reinterpret_cast< QIcon*>(_v) = icon(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isVisible(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setToolTip(*reinterpret_cast< QString*>(_v)); break;
        case 1: setIcon(*reinterpret_cast< QIcon*>(_v)); break;
        case 2: setVisible(*reinterpret_cast< bool*>(_v)); break;
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
void QSystemTrayIcon::activated(QSystemTrayIcon::ActivationReason _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QSystemTrayIcon::messageClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
