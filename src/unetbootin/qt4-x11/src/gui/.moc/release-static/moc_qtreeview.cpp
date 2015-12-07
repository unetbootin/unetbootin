/****************************************************************************
** Meta object code from reading C++ file 'qtreeview.h'
**
** Created: Thu Mar 5 18:54:48 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qtreeview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtreeview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTreeView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   12, // methods
      11,  142, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      39,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      82,   62,   10,   10, 0x0a,
     126,  119,   10,   10, 0x0a,
     142,  119,   10,   10, 0x0a,
     158,   11,   10,   10, 0x0a,
     178,   11,   10,   10, 0x0a,
     200,  119,   10,   10, 0x0a,
     228,  119,   10,   10, 0x0a,
     246,   10,   10,   10, 0x0a,
     258,   10,   10,   10, 0x0a,
     270,   10,   10,   10, 0x0a,
     290,  284,   10,   10, 0x0a,
     332,  309,   10,   10, 0x09,
     377,  359,   10,   10, 0x09,
     405,   10,   10,   10, 0x09,
     419,   10,   10,   10, 0x09,
     448,  430,   10,   10, 0x09,
     481,   10,   10,   10, 0x08,
     507,   10,   10,   10, 0x08,
     522,  520,   10,   10, 0x08,
     568,  565,   10,   10, 0x08,
     616,  565,   10,   10, 0x08,
     655,   10,   10,   10, 0x08,
     693,  680,   10,   10, 0x08,
     736,   10,   10,   10, 0x08,

 // properties: name, type, flags
     760,  756, 0x02095103,
     776,  756, 0x02095103,
     793,  788, 0x01095103,
     809,  788, 0x01095103,
     827,  788, 0x01095103,
     843,  788, 0x01095103,
     858,  788, 0x01095103,
     867,  788, 0x01095103,
     887,  788, 0x01095103,
     896,  788, 0x01095103,
     909,  788, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QTreeView[] = {
    "QTreeView\0\0index\0expanded(QModelIndex)\0"
    "collapsed(QModelIndex)\0topLeft,bottomRight\0"
    "dataChanged(QModelIndex,QModelIndex)\0"
    "column\0hideColumn(int)\0showColumn(int)\0"
    "expand(QModelIndex)\0collapse(QModelIndex)\0"
    "resizeColumnToContents(int)\0"
    "sortByColumn(int)\0selectAll()\0expandAll()\0"
    "collapseAll()\0depth\0expandToDepth(int)\0"
    "column,oldSize,newSize\0"
    "columnResized(int,int,int)\0oldCount,newCount\0"
    "columnCountChanged(int,int)\0columnMoved()\0"
    "reexpand()\0parent,first,last\0"
    "rowsRemoved(QModelIndex,int,int)\0"
    "_q_endAnimatedOperation()\0_q_animate()\0"
    ",\0_q_currentChanged(QModelIndex,QModelIndex)\0"
    ",,\0_q_columnsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_columnsRemoved(QModelIndex,int,int)\0"
    "_q_modelAboutToBeReset()\0column,order\0"
    "_q_sortIndicatorChanged(int,Qt::SortOrder)\0"
    "_q_modelDestroyed()\0int\0autoExpandDelay\0"
    "indentation\0bool\0rootIsDecorated\0"
    "uniformRowHeights\0itemsExpandable\0"
    "sortingEnabled\0animated\0allColumnsShowFocus\0"
    "wordWrap\0headerHidden\0expandsOnDoubleClick\0"
};

const QMetaObject QTreeView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_QTreeView,
      qt_meta_data_QTreeView, 0 }
};

const QMetaObject *QTreeView::metaObject() const
{
    return &staticMetaObject;
}

void *QTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTreeView))
        return static_cast<void*>(const_cast< QTreeView*>(this));
    return QAbstractItemView::qt_metacast(_clname);
}

int QTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: expanded((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: collapsed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 3: hideColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: showColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: expand((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: collapse((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: resizeColumnToContents((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: sortByColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: selectAll(); break;
        case 10: expandAll(); break;
        case 11: collapseAll(); break;
        case 12: expandToDepth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: columnResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 14: columnCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: columnMoved(); break;
        case 16: reexpand(); break;
        case 17: rowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: d_func()->_q_endAnimatedOperation(); break;
        case 19: d_func()->_q_animate(); break;
        case 20: d_func()->_q_currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 21: d_func()->_q_columnsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 22: d_func()->_q_columnsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 23: d_func()->_q_modelAboutToBeReset(); break;
        case 24: d_func()->_q_sortIndicatorChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 25: d_func()->_q_modelDestroyed(); break;
        default: ;
        }
        _id -= 26;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = autoExpandDelay(); break;
        case 1: *reinterpret_cast< int*>(_v) = indentation(); break;
        case 2: *reinterpret_cast< bool*>(_v) = rootIsDecorated(); break;
        case 3: *reinterpret_cast< bool*>(_v) = uniformRowHeights(); break;
        case 4: *reinterpret_cast< bool*>(_v) = itemsExpandable(); break;
        case 5: *reinterpret_cast< bool*>(_v) = isSortingEnabled(); break;
        case 6: *reinterpret_cast< bool*>(_v) = isAnimated(); break;
        case 7: *reinterpret_cast< bool*>(_v) = allColumnsShowFocus(); break;
        case 8: *reinterpret_cast< bool*>(_v) = wordWrap(); break;
        case 9: *reinterpret_cast< bool*>(_v) = isHeaderHidden(); break;
        case 10: *reinterpret_cast< bool*>(_v) = expandsOnDoubleClick(); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAutoExpandDelay(*reinterpret_cast< int*>(_v)); break;
        case 1: setIndentation(*reinterpret_cast< int*>(_v)); break;
        case 2: setRootIsDecorated(*reinterpret_cast< bool*>(_v)); break;
        case 3: setUniformRowHeights(*reinterpret_cast< bool*>(_v)); break;
        case 4: setItemsExpandable(*reinterpret_cast< bool*>(_v)); break;
        case 5: setSortingEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 6: setAnimated(*reinterpret_cast< bool*>(_v)); break;
        case 7: setAllColumnsShowFocus(*reinterpret_cast< bool*>(_v)); break;
        case 8: setWordWrap(*reinterpret_cast< bool*>(_v)); break;
        case 9: setHeaderHidden(*reinterpret_cast< bool*>(_v)); break;
        case 10: setExpandsOnDoubleClick(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QTreeView::expanded(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTreeView::collapsed(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
