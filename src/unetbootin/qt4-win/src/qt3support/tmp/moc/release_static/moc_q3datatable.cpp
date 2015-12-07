/****************************************************************************
** Meta object code from reading C++ file 'q3datatable.h'
**
** Created: Fri May 30 22:41:04 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../sql/q3datatable.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3datatable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3DataTable[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   10, // methods
      14,  130, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      20,   13,   12,   12, 0x05,
      52,   48,   12,   12, 0x05,
      77,   48,   12,   12, 0x05,
     102,   48,   12,   12, 0x05,
     127,   48,   12,   12, 0x05,
     153,   48,   12,   12, 0x05,
     179,   48,   12,   12, 0x05,
     210,  205,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     262,  234,   12,   12, 0x0a,
     290,  286,   12,   12, 0x0a,
     309,  286,   12,   12, 0x0a,
     329,   12,   12,   12, 0x0a,
     345,  339,   12,   12, 0x0a,
     369,  286,   12,   12, 0x0a,
     399,  387,   12,   12, 0x0a,
     452,  430,   12,   12, 0x0a,
     488,  478,   12,   12, 0x2a,
     509,   12,   12,   12, 0x08,
     532,  524,   12,   12, 0x08,
     561,   12,   12,   12, 0x08,
     586,   12,   12,   12, 0x08,
     602,   12,   12,   12, 0x08,
     619,   12,   12,   12, 0x08,
     637,   12,   12,   12, 0x08,

 // properties: name, type, flags
     663,  655, 0x0a095103,
     672,  655, 0x0a095103,
     681,  655, 0x0a095103,
     706,  691, 0x0009510b,
     722,  717, 0x01095103,
     735,  717, 0x01095103,
     749,  717, 0x01095103,
     763,  717, 0x01095103,
     777,  717, 0x01095103,
     792,  717, 0x01095103,
     801,  655, 0x0a095103,
     820,  808, 0x0b095103,
     829,  825, 0x02095001,
     837,  825, 0x02095001,

       0        // eod
};

static const char qt_meta_stringdata_Q3DataTable[] = {
    "Q3DataTable\0\0record\0currentChanged(QSqlRecord*)\0"
    "buf\0primeInsert(QSqlRecord*)\0"
    "primeUpdate(QSqlRecord*)\0"
    "primeDelete(QSqlRecord*)\0"
    "beforeInsert(QSqlRecord*)\0"
    "beforeUpdate(QSqlRecord*)\0"
    "beforeDelete(QSqlRecord*)\0mode\0"
    "cursorChanged(QSql::Op)\0"
    "str,caseSensitive,backwards\0"
    "find(QString,bool,bool)\0col\0"
    "sortAscending(int)\0sortDescending(int)\0"
    "refresh()\0col,w\0setColumnWidth(int,int)\0"
    "adjustColumn(int)\0col,stretch\0"
    "setColumnStretchable(int,bool)\0"
    "col1,col2,swapHeaders\0swapColumns(int,int,bool)\0"
    "col1,col2\0swapColumns(int,int)\0"
    "loadNextPage()\0row,col\0"
    "setCurrentSelection(int,int)\0"
    "updateCurrentSelection()\0sliderPressed()\0"
    "sliderReleased()\0doInsertCurrent()\0"
    "doUpdateCurrent()\0QString\0nullText\0"
    "trueText\0falseText\0Qt::DateFormat\0"
    "dateFormat\0bool\0confirmEdits\0confirmInsert\0"
    "confirmUpdate\0confirmDelete\0confirmCancels\0"
    "autoEdit\0filter\0QStringList\0sort\0int\0"
    "numCols\0numRows\0"
};

const QMetaObject Q3DataTable::staticMetaObject = {
    { &Q3Table::staticMetaObject, qt_meta_stringdata_Q3DataTable,
      qt_meta_data_Q3DataTable, 0 }
};

const QMetaObject *Q3DataTable::metaObject() const
{
    return &staticMetaObject;
}

void *Q3DataTable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3DataTable))
	return static_cast<void*>(const_cast< Q3DataTable*>(this));
    return Q3Table::qt_metacast(_clname);
}

int Q3DataTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3Table::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentChanged((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 1: primeInsert((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 2: primeUpdate((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 3: primeDelete((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 4: beforeInsert((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 5: beforeUpdate((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 6: beforeDelete((*reinterpret_cast< QSqlRecord*(*)>(_a[1]))); break;
        case 7: cursorChanged((*reinterpret_cast< QSql::Op(*)>(_a[1]))); break;
        case 8: find((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 9: sortAscending((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: sortDescending((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: refresh(); break;
        case 12: setColumnWidth((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: adjustColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: setColumnStretchable((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 15: swapColumns((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 16: swapColumns((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: loadNextPage(); break;
        case 18: setCurrentSelection((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 19: updateCurrentSelection(); break;
        case 20: sliderPressed(); break;
        case 21: sliderReleased(); break;
        case 22: doInsertCurrent(); break;
        case 23: doUpdateCurrent(); break;
        }
        _id -= 24;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = nullText(); break;
        case 1: *reinterpret_cast< QString*>(_v) = trueText(); break;
        case 2: *reinterpret_cast< QString*>(_v) = falseText(); break;
        case 3: *reinterpret_cast< Qt::DateFormat*>(_v) = dateFormat(); break;
        case 4: *reinterpret_cast< bool*>(_v) = confirmEdits(); break;
        case 5: *reinterpret_cast< bool*>(_v) = confirmInsert(); break;
        case 6: *reinterpret_cast< bool*>(_v) = confirmUpdate(); break;
        case 7: *reinterpret_cast< bool*>(_v) = confirmDelete(); break;
        case 8: *reinterpret_cast< bool*>(_v) = confirmCancels(); break;
        case 9: *reinterpret_cast< bool*>(_v) = autoEdit(); break;
        case 10: *reinterpret_cast< QString*>(_v) = filter(); break;
        case 11: *reinterpret_cast< QStringList*>(_v) = sort(); break;
        case 12: *reinterpret_cast< int*>(_v) = numCols(); break;
        case 13: *reinterpret_cast< int*>(_v) = numRows(); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setNullText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setTrueText(*reinterpret_cast< QString*>(_v)); break;
        case 2: setFalseText(*reinterpret_cast< QString*>(_v)); break;
        case 3: setDateFormat(*reinterpret_cast< Qt::DateFormat*>(_v)); break;
        case 4: setConfirmEdits(*reinterpret_cast< bool*>(_v)); break;
        case 5: setConfirmInsert(*reinterpret_cast< bool*>(_v)); break;
        case 6: setConfirmUpdate(*reinterpret_cast< bool*>(_v)); break;
        case 7: setConfirmDelete(*reinterpret_cast< bool*>(_v)); break;
        case 8: setConfirmCancels(*reinterpret_cast< bool*>(_v)); break;
        case 9: setAutoEdit(*reinterpret_cast< bool*>(_v)); break;
        case 10: setFilter(*reinterpret_cast< QString*>(_v)); break;
        case 11: setSort(*reinterpret_cast< QStringList*>(_v)); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 14;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3DataTable::currentChanged(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3DataTable::primeInsert(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3DataTable::primeUpdate(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3DataTable::primeDelete(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3DataTable::beforeInsert(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3DataTable::beforeUpdate(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3DataTable::beforeDelete(QSqlRecord * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Q3DataTable::cursorChanged(QSql::Op _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
