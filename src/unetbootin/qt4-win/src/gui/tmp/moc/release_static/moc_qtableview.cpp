/****************************************************************************
** Meta object code from reading C++ file 'qtableview.h'
**
** Created: Fri May 30 22:30:58 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qtableview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtableview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTableView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   10, // methods
       5,  110, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x0a,
      38,   31,   11,   11, 0x0a,
      56,   12,   11,   11, 0x0a,
      69,   31,   11,   11, 0x0a,
      85,   12,   11,   11, 0x0a,
      98,   31,   11,   11, 0x0a,
     114,   12,   11,   11, 0x0a,
     139,   11,   11,   11, 0x0a,
     162,   31,   11,   11, 0x0a,
     190,   11,   11,   11, 0x0a,
     216,   31,   11,   11, 0x0a,
     239,  234,   11,   11, 0x0a,
     279,  257,   11,   11, 0x09,
     326,  301,   11,   11, 0x09,
     375,  351,   11,   11, 0x09,
     424,  399,   11,   11, 0x09,
     469,  451,   11,   11, 0x09,
     494,  451,   11,   11, 0x09,
     522,   11,   11,   11, 0x08,
     540,   11,   11,   11, 0x08,

 // properties: name, type, flags
     566,  561, 0x01095103,
     588,  575, 0x0009510b,
     598,  561, 0x01095103,
     613,  561, 0x01095103,
     622,  561, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QTableView[] = {
    "QTableView\0\0row\0selectRow(int)\0column\0"
    "selectColumn(int)\0hideRow(int)\0"
    "hideColumn(int)\0showRow(int)\0"
    "showColumn(int)\0resizeRowToContents(int)\0"
    "resizeRowsToContents()\0"
    "resizeColumnToContents(int)\0"
    "resizeColumnsToContents()\0sortByColumn(int)\0"
    "show\0setShowGrid(bool)\0row,oldIndex,newIndex\0"
    "rowMoved(int,int,int)\0column,oldIndex,newIndex\0"
    "columnMoved(int,int,int)\0"
    "row,oldHeight,newHeight\0rowResized(int,int,int)\0"
    "column,oldWidth,newWidth\0"
    "columnResized(int,int,int)\0oldCount,newCount\0"
    "rowCountChanged(int,int)\0"
    "columnCountChanged(int,int)\0"
    "_q_selectRow(int)\0_q_selectColumn(int)\0"
    "bool\0showGrid\0Qt::PenStyle\0gridStyle\0"
    "sortingEnabled\0wordWrap\0cornerButtonEnabled\0"
};

const QMetaObject QTableView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_QTableView,
      qt_meta_data_QTableView, 0 }
};

const QMetaObject *QTableView::metaObject() const
{
    return &staticMetaObject;
}

void *QTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTableView))
	return static_cast<void*>(const_cast< QTableView*>(this));
    return QAbstractItemView::qt_metacast(_clname);
}

int QTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: selectColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: hideRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: hideColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: showRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: showColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: resizeRowToContents((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: resizeRowsToContents(); break;
        case 8: resizeColumnToContents((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: resizeColumnsToContents(); break;
        case 10: sortByColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: setShowGrid((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: rowMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 13: columnMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 14: rowResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: columnResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 16: rowCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: columnCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 18: d_func()->_q_selectRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: d_func()->_q_selectColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 20;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = showGrid(); break;
        case 1: *reinterpret_cast< Qt::PenStyle*>(_v) = gridStyle(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isSortingEnabled(); break;
        case 3: *reinterpret_cast< bool*>(_v) = wordWrap(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isCornerButtonEnabled(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setShowGrid(*reinterpret_cast< bool*>(_v)); break;
        case 1: setGridStyle(*reinterpret_cast< Qt::PenStyle*>(_v)); break;
        case 2: setSortingEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setWordWrap(*reinterpret_cast< bool*>(_v)); break;
        case 4: setCornerButtonEnabled(*reinterpret_cast< bool*>(_v)); break;
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
QT_END_MOC_NAMESPACE
