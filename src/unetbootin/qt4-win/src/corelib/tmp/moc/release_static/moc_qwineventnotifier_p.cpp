/****************************************************************************
** Meta object code from reading C++ file 'qwineventnotifier_p.h'
**
** Created: Fri May 30 22:18:35 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qwineventnotifier_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwineventnotifier_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWinEventNotifier[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      26,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   44,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QWinEventNotifier[] = {
    "QWinEventNotifier\0\0hEvent\0activated(HANDLE)\0"
    "enable\0setEnabled(bool)\0"
};

const QMetaObject QWinEventNotifier::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWinEventNotifier,
      qt_meta_data_QWinEventNotifier, 0 }
};

const QMetaObject *QWinEventNotifier::metaObject() const
{
    return &staticMetaObject;
}

void *QWinEventNotifier::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWinEventNotifier))
	return static_cast<void*>(const_cast< QWinEventNotifier*>(this));
    return QObject::qt_metacast(_clname);
}

int QWinEventNotifier::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< HANDLE(*)>(_a[1]))); break;
        case 1: setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QWinEventNotifier::activated(HANDLE _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
