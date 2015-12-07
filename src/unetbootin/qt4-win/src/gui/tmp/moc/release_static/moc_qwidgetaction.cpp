/****************************************************************************
** Meta object code from reading C++ file 'qwidgetaction.h'
**
** Created: Fri May 30 22:21:12 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qwidgetaction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwidgetaction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWidgetAction[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QWidgetAction[] = {
    "QWidgetAction\0\0_q_widgetDestroyed(QObject*)\0"
};

const QMetaObject QWidgetAction::staticMetaObject = {
    { &QAction::staticMetaObject, qt_meta_stringdata_QWidgetAction,
      qt_meta_data_QWidgetAction, 0 }
};

const QMetaObject *QWidgetAction::metaObject() const
{
    return &staticMetaObject;
}

void *QWidgetAction::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWidgetAction))
	return static_cast<void*>(const_cast< QWidgetAction*>(this));
    return QAction::qt_metacast(_clname);
}

int QWidgetAction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAction::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_widgetDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
