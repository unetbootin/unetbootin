/****************************************************************************
** Meta object code from reading C++ file 'qscrollarea.h'
**
** Created: Fri May 30 22:33:56 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qscrollarea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscrollarea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScrollArea[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   10, // properties
       0,    0, // enums/sets

 // properties: name, type, flags
      17,   12, 0x01095103,
      47,   33, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_QScrollArea[] = {
    "QScrollArea\0bool\0widgetResizable\0"
    "Qt::Alignment\0alignment\0"
};

const QMetaObject QScrollArea::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QScrollArea,
      qt_meta_data_QScrollArea, 0 }
};

const QMetaObject *QScrollArea::metaObject() const
{
    return &staticMetaObject;
}

void *QScrollArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScrollArea))
	return static_cast<void*>(const_cast< QScrollArea*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QScrollArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = widgetResizable(); break;
        case 1: *reinterpret_cast< Qt::Alignment*>(_v) = alignment(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setWidgetResizable(*reinterpret_cast< bool*>(_v)); break;
        case 1: setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
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
QT_END_MOC_NAMESPACE
