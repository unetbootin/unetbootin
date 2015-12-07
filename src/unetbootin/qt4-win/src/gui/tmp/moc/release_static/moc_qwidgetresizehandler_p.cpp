/****************************************************************************
** Meta object code from reading C++ file 'qwidgetresizehandler_p.h'
**
** Created: Fri May 30 22:33:55 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qwidgetresizehandler_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwidgetresizehandler_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWidgetResizeHandler[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QWidgetResizeHandler[] = {
    "QWidgetResizeHandler\0\0activate()\0"
};

const QMetaObject QWidgetResizeHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWidgetResizeHandler,
      qt_meta_data_QWidgetResizeHandler, 0 }
};

const QMetaObject *QWidgetResizeHandler::metaObject() const
{
    return &staticMetaObject;
}

void *QWidgetResizeHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWidgetResizeHandler))
	return static_cast<void*>(const_cast< QWidgetResizeHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int QWidgetResizeHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activate(); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QWidgetResizeHandler::activate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
