/****************************************************************************
** Meta object code from reading C++ file 'qhelpengine_p.h'
**
** Created: Thu Mar 5 20:32:30 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpengine_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpengine_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHelpEngineCorePrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      28,   24,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHelpEngineCorePrivate[] = {
    "QHelpEngineCorePrivate\0\0msg\0"
    "errorReceived(QString)\0"
};

const QMetaObject QHelpEngineCorePrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHelpEngineCorePrivate,
      qt_meta_data_QHelpEngineCorePrivate, 0 }
};

const QMetaObject *QHelpEngineCorePrivate::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpEngineCorePrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpEngineCorePrivate))
        return static_cast<void*>(const_cast< QHelpEngineCorePrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int QHelpEngineCorePrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: errorReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_QHelpEnginePrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,
      44,   19,   19,   19, 0x0a,
      70,   19,   19,   19, 0x0a,
      91,   19,   19,   19, 0x0a,
     114,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHelpEnginePrivate[] = {
    "QHelpEnginePrivate\0\0setContentsWidgetBusy()\0"
    "unsetContentsWidgetBusy()\0"
    "setIndexWidgetBusy()\0unsetIndexWidgetBusy()\0"
    "applyCurrentFilter()\0"
};

const QMetaObject QHelpEnginePrivate::staticMetaObject = {
    { &QHelpEngineCorePrivate::staticMetaObject, qt_meta_stringdata_QHelpEnginePrivate,
      qt_meta_data_QHelpEnginePrivate, 0 }
};

const QMetaObject *QHelpEnginePrivate::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpEnginePrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpEnginePrivate))
        return static_cast<void*>(const_cast< QHelpEnginePrivate*>(this));
    return QHelpEngineCorePrivate::qt_metacast(_clname);
}

int QHelpEnginePrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QHelpEngineCorePrivate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setContentsWidgetBusy(); break;
        case 1: unsetContentsWidgetBusy(); break;
        case 2: setIndexWidgetBusy(); break;
        case 3: unsetIndexWidgetBusy(); break;
        case 4: applyCurrentFilter(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
