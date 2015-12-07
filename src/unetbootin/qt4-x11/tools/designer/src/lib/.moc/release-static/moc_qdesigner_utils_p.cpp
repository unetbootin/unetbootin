/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_utils_p.h'
**
** Created: Thu Mar 5 20:47:14 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_utils_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_utils_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__DesignerPixmapCache[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      41,   40,   40,   40, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__DesignerPixmapCache[] = {
    "qdesigner_internal::DesignerPixmapCache\0"
    "\0reloaded()\0"
};

const QMetaObject qdesigner_internal::DesignerPixmapCache::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__DesignerPixmapCache,
      qt_meta_data_qdesigner_internal__DesignerPixmapCache, 0 }
};

const QMetaObject *qdesigner_internal::DesignerPixmapCache::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::DesignerPixmapCache::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__DesignerPixmapCache))
        return static_cast<void*>(const_cast< DesignerPixmapCache*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::DesignerPixmapCache::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reloaded(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::DesignerPixmapCache::reloaded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_qdesigner_internal__DesignerIconCache[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      39,   38,   38,   38, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__DesignerIconCache[] = {
    "qdesigner_internal::DesignerIconCache\0"
    "\0reloaded()\0"
};

const QMetaObject qdesigner_internal::DesignerIconCache::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__DesignerIconCache,
      qt_meta_data_qdesigner_internal__DesignerIconCache, 0 }
};

const QMetaObject *qdesigner_internal::DesignerIconCache::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::DesignerIconCache::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__DesignerIconCache))
        return static_cast<void*>(const_cast< DesignerIconCache*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::DesignerIconCache::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reloaded(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::DesignerIconCache::reloaded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
