/****************************************************************************
** Meta object code from reading C++ file 'qshortcut.h'
**
** Created: Fri May 30 22:33:10 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qshortcut.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qshortcut.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QShortcut[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       5,   20, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      23,   10,   10,   10, 0x05,

 // properties: name, type, flags
      59,   46, 0x4c095103,
      71,   63, 0x0a095103,
      86,   81, 0x01095103,
      94,   81, 0x01095103,
     125,  105, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_QShortcut[] = {
    "QShortcut\0\0activated()\0activatedAmbiguously()\0"
    "QKeySequence\0key\0QString\0whatsThis\0"
    "bool\0enabled\0autoRepeat\0Qt::ShortcutContext\0"
    "context\0"
};

const QMetaObject QShortcut::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QShortcut,
      qt_meta_data_QShortcut, 0 }
};

const QMetaObject *QShortcut::metaObject() const
{
    return &staticMetaObject;
}

void *QShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QShortcut))
	return static_cast<void*>(const_cast< QShortcut*>(this));
    return QObject::qt_metacast(_clname);
}

int QShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated(); break;
        case 1: activatedAmbiguously(); break;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QKeySequence*>(_v) = key(); break;
        case 1: *reinterpret_cast< QString*>(_v) = whatsThis(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 3: *reinterpret_cast< bool*>(_v) = autoRepeat(); break;
        case 4: *reinterpret_cast< Qt::ShortcutContext*>(_v) = context(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setKey(*reinterpret_cast< QKeySequence*>(_v)); break;
        case 1: setWhatsThis(*reinterpret_cast< QString*>(_v)); break;
        case 2: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setAutoRepeat(*reinterpret_cast< bool*>(_v)); break;
        case 4: setContext(*reinterpret_cast< Qt::ShortcutContext*>(_v)); break;
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
void QShortcut::activated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QShortcut::activatedAmbiguously()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
