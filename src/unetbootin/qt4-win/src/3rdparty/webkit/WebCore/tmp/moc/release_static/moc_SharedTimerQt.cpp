/****************************************************************************
** Meta object code from reading C++ file 'SharedTimerQt.h'
**
** Created: Fri May 30 23:34:14 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../platform/qt/SharedTimerQt.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SharedTimerQt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WebCore__SharedTimerQt[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WebCore__SharedTimerQt[] = {
    "WebCore::SharedTimerQt\0\0fire()\0"
};

const QMetaObject WebCore::SharedTimerQt::staticMetaObject = {
    { &QTimer::staticMetaObject, qt_meta_stringdata_WebCore__SharedTimerQt,
      qt_meta_data_WebCore__SharedTimerQt, 0 }
};

const QMetaObject *WebCore::SharedTimerQt::metaObject() const
{
    return &staticMetaObject;
}

void *WebCore::SharedTimerQt::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WebCore__SharedTimerQt))
	return static_cast<void*>(const_cast< SharedTimerQt*>(this));
    return QTimer::qt_metacast(_clname);
}

int WebCore::SharedTimerQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTimer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fire(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
