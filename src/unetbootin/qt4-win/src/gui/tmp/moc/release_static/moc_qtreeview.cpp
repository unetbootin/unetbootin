/****************************************************************************
** Meta object code from reading C++ file 'qtreeview.h'
**
** Created: Fri May 30 22:31:04 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qtreeview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtreeview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTreeView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   10, // methods
      11,  125, // properties
       0,    0, // enums/sets

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
     509,  507,   10,   10, 0x08,
     555,  552,   10,   10, 0x08,
     603,  552,   10,   10, 0x08,
     642,   10,   10,   10, 0x08,

 // properties: name, type, flags
     671,  667, 0x02095103,
     687,  667, 0x02095103,
     704,  699, 0x01095103,
     720,  699, 0x01095103,
     738,  699, 0x01095103,
     754,  699, 0x01095103,
     769,  699, 0x01095103,
     778,  699, 0x01095103,
     798,  699, 0x01095103,
     807,  699, 0x01095103,
     820,  699, 0x01095103,

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
    "_q_endAnimatedOperation()\0,\0"
    "_q_currentChanged(QModelIndex,QModelIndex)\0"
    ",,\0_q_columnsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_columnsRemoved(QModelIndex,int,int)\0"
    "_q_modelAboutToBeReset()\0int\0"
    "autoExpandDelay\0indentation\0bool\0"
    "rootIsDecorated\0uniformRowHeights\0"
    "itemsExpandable\0sortingEnabled\0animated\0"
    "allColumnsShowFocus\0wordWrap\0headerHidden\0"
    "expandsOnDoubleClick\0"
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
        case 19: d_func()->_q_currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 20: d_func()->_q_columnsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 21: d_func()->_q_columnsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 22: d_func()->_q_modelAboutToBeReset(); break;
        }
        _id -= 23;
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
