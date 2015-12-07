/****************************************************************************
** Meta object code from reading C++ file 'q3databrowser.h'
**
** Created: Fri May 30 22:41:07 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../sql/q3databrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3databrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3DataBrowser[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   10, // methods
      10,  130, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   15,   14,   14, 0x05,
      52,   15,   14,   14, 0x05,
      78,   15,   14,   14, 0x05,
     104,   15,   14,   14, 0x05,
     137,  130,   14,   14, 0x05,
     175,  171,   14,   14, 0x05,
     200,  171,   14,   14, 0x05,
     225,  171,   14,   14, 0x05,
     250,  171,   14,   14, 0x05,
     276,  171,   14,   14, 0x05,
     302,  171,   14,   14, 0x05,
     333,  328,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     366,   14,   14,   14, 0x0a,
     376,   14,   14,   14, 0x0a,
     385,   14,   14,   14, 0x0a,
     394,   14,   14,   14, 0x0a,
     400,   14,   14,   14, 0x0a,
     408,   14,   14,   14, 0x0a,
     415,   14,   14,   14, 0x0a,
     422,   14,   14,   14, 0x0a,
     429,   14,   14,   14, 0x0a,
     442,   14,   14,   14, 0x0a,
     456,   14,   14,   14, 0x0a,
     470,   14,   14,   14, 0x0a,

 // properties: name, type, flags
     492,  487, 0x01095103,
     517,  509, 0x0a095103,
     536,  524, 0x0b095103,
     541,  487, 0x01095103,
     554,  487, 0x01095103,
     568,  487, 0x01095103,
     582,  487, 0x01095103,
     596,  487, 0x01095103,
     611,  487, 0x01095103,
     620,  487, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_Q3DataBrowser[] = {
    "Q3DataBrowser\0\0available\0"
    "firstRecordAvailable(bool)\0"
    "lastRecordAvailable(bool)\0"
    "nextRecordAvailable(bool)\0"
    "prevRecordAvailable(bool)\0record\0"
    "currentChanged(const QSqlRecord*)\0buf\0"
    "primeInsert(QSqlRecord*)\0"
    "primeUpdate(QSqlRecord*)\0"
    "primeDelete(QSqlRecord*)\0"
    "beforeInsert(QSqlRecord*)\0"
    "beforeUpdate(QSqlRecord*)\0"
    "beforeDelete(QSqlRecord*)\0mode\0"
    "cursorChanged(Q3SqlCursor::Mode)\0"
    "refresh()\0insert()\0update()\0del()\0"
    "first()\0last()\0next()\0prev()\0readFields()\0"
    "writeFields()\0clearValues()\0"
    "updateBoundary()\0bool\0boundaryChecking\0"
    "QString\0filter\0QStringList\0sort\0"
    "confirmEdits\0confirmInsert\0confirmUpdate\0"
    "confirmDelete\0confirmCancels\0readOnly\0"
    "autoEdit\0"
};

const QMetaObject Q3DataBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Q3DataBrowser,
      qt_meta_data_Q3DataBrowser, 0 }
};

const QMetaObject *Q3DataBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DataBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DataBrowser))
	return static_cast<void*>(const_cast< Q3DataBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int Q3DataBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: firstRecordAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: lastRecordAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: nextRecordAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: prevRecordAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: currentChanged((*reinterpret_cast< const QSqlRecord*(*)>(_a[1]))); break;
        case 5: primeInsert((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 6: primeUpdate((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 7: primeDelete((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 8: beforeInsert((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 9: beforeUpdate((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 10: beforeDelete((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 11: cursorChanged((*reinterpret_cast< Q3SqlCursor::Mode(*)>(_a[1]))); break;
        case 12: refresh(); break;
        case 13: insert(); break;
        case 14: update(); break;
        case 15: del(); break;
        case 16: first(); break;
        case 17: last(); break;
        case 18: next(); break;
        case 19: prev(); break;
        case 20: readFields(); break;
        case 21: writeFields(); break;
        case 22: clearValues(); break;
        case 23: updateBoundary(); break;
        }
        _id -= 24;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = boundaryChecking(); break;
        case 1: *reinterpret_cast< QString*>(_v) = filter(); break;
        case 2: *reinterpret_cast< QStringList*>(_v) = sort(); break;
        case 3: *reinterpret_cast< bool*>(_v) = confirmEdits(); break;
        case 4: *reinterpret_cast< bool*>(_v) = confirmInsert(); break;
        case 5: *reinterpret_cast< bool*>(_v) = confirmUpdate(); break;
        case 6: *reinterpret_cast< bool*>(_v) = confirmDelete(); break;
        case 7: *reinterpret_cast< bool*>(_v) = confirmCancels(); break;
        case 8: *reinterpret_cast< bool*>(_v) = isReadOnly(); break;
        case 9: *reinterpret_cast< bool*>(_v) = autoEdit(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBoundaryChecking(*reinterpret_cast< bool*>(_v)); break;
        case 1: setFilter(*reinterpret_cast< QString*>(_v)); break;
        case 2: setSort(*reinterpret_cast< QStringList*>(_v)); break;
        case 3: setConfirmEdits(*reinterpret_cast< bool*>(_v)); break;
        case 4: setConfirmInsert(*reinterpret_cast< bool*>(_v)); break;
        case 5: setConfirmUpdate(*reinterpret_cast< bool*>(_v)); break;
        case 6: setConfirmDelete(*reinterpret_cast< bool*>(_v)); break;
        case 7: setConfirmCancels(*reinterpret_cast< bool*>(_v)); break;
        case 8: setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 9: setAutoEdit(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3DataBrowser::firstRecordAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3DataBrowser::lastRecordAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3DataBrowser::nextRecordAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3DataBrowser::prevRecordAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3DataBrowser::currentChanged(const QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3DataBrowser::primeInsert(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3DataBrowser::primeUpdate(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Q3DataBrowser::primeDelete(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Q3DataBrowser::beforeInsert(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Q3DataBrowser::beforeUpdate(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Q3DataBrowser::beforeDelete(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Q3DataBrowser::cursorChanged(Q3SqlCursor::Mode _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_END_MOC_NAMESPACE
