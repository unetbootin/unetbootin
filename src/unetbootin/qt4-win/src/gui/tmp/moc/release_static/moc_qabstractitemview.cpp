/****************************************************************************
** Meta object code from reading C++ file 'qabstractitemview.h'
**
** Created: Fri May 30 22:30:39 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qabstractitemview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qabstractitemview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAbstractItemView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      37,   10, // methods
      15,  195, // properties
       6,  240, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   19,   18,   18, 0x05,
      46,   19,   18,   18, 0x05,
      67,   19,   18,   18, 0x05,
      94,   19,   18,   18, 0x05,
     117,   19,   18,   18, 0x05,
     138,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     156,   18,   18,   18, 0x0a,
     164,   19,   18,   18, 0x0a,
     190,   18,   18,   18, 0x0a,
     206,   18,   18,   18, 0x0a,
     218,   19,   18,   18, 0x0a,
     236,   18,   18,   18, 0x0a,
     253,   19,   18,   18, 0x0a,
     282,   18,   18,   18, 0x0a,
     296,   18,   18,   18, 0x0a,
     313,   19,   18,   18, 0x0a,
     353,  333,   18,   18, 0x09,
     407,  390,   18,   18, 0x09,
     441,  390,   18,   18, 0x09,
     503,  483,   18,   18, 0x09,
     568,  551,   18,   18, 0x09,
     608,   18,   18,   18, 0x09,
     627,   18,   18,   18, 0x09,
     652,   18,   18,   18, 0x09,
     678,  671,   18,   18, 0x09,
     707,  671,   18,   18, 0x09,
     744,  738,   18,   18, 0x09,
     779,  738,   18,   18, 0x09,
     828,  816,   18,   18, 0x09,
     892,  885,   18,   18, 0x09,
     913,  885,   18,   18, 0x09,
     942,  939,   18,   18, 0x08,
     990,  939,   18,   18, 0x08,
    1029,  939,   18,   18, 0x08,
    1065,   18,   18,   18, 0x08,
    1085,   18,   18,   18, 0x08,
    1104,   18,   18,   18, 0x08,

 // properties: name, type, flags
    1124, 1119, 0x01095103,
    1139, 1135, 0x02095103,
    1169, 1156, 0x0009510b,
    1182, 1119, 0x01095103,
    1199, 1119, 0x01095003,
    1217, 1119, 0x01095103,
    1229, 1119, 0x01095103,
    1264, 1251, 0x0009510b,
    1277, 1119, 0x01095103,
    1312, 1298, 0x0009510b,
    1344, 1326, 0x0009510b,
    1368, 1362, 0x15095103,
    1395, 1377, 0x0009510b,
    1420, 1409, 0x0009510b,
    1439, 1409, 0x0009510b,

 // enums: name, flags, count, data
    1298, 0x0,    5,  264,
    1326, 0x0,    3,  274,
    1460, 0x0,    4,  280,
    1156, 0x1,    7,  288,
    1409, 0x0,    2,  302,
    1251, 0x0,    5,  306,

 // enum data: key, value
    1471, uint(QAbstractItemView::NoSelection),
    1483, uint(QAbstractItemView::SingleSelection),
    1499, uint(QAbstractItemView::MultiSelection),
    1514, uint(QAbstractItemView::ExtendedSelection),
    1532, uint(QAbstractItemView::ContiguousSelection),
    1552, uint(QAbstractItemView::SelectItems),
    1564, uint(QAbstractItemView::SelectRows),
    1575, uint(QAbstractItemView::SelectColumns),
    1589, uint(QAbstractItemView::EnsureVisible),
    1603, uint(QAbstractItemView::PositionAtTop),
    1617, uint(QAbstractItemView::PositionAtBottom),
    1634, uint(QAbstractItemView::PositionAtCenter),
    1651, uint(QAbstractItemView::NoEditTriggers),
    1666, uint(QAbstractItemView::CurrentChanged),
    1681, uint(QAbstractItemView::DoubleClicked),
    1695, uint(QAbstractItemView::SelectedClicked),
    1711, uint(QAbstractItemView::EditKeyPressed),
    1726, uint(QAbstractItemView::AnyKeyPressed),
    1740, uint(QAbstractItemView::AllEditTriggers),
    1756, uint(QAbstractItemView::ScrollPerItem),
    1770, uint(QAbstractItemView::ScrollPerPixel),
    1785, uint(QAbstractItemView::NoDragDrop),
    1796, uint(QAbstractItemView::DragOnly),
    1805, uint(QAbstractItemView::DropOnly),
    1814, uint(QAbstractItemView::DragDrop),
    1823, uint(QAbstractItemView::InternalMove),

       0        // eod
};

static const char qt_meta_stringdata_QAbstractItemView[] = {
    "QAbstractItemView\0\0index\0pressed(QModelIndex)\0"
    "clicked(QModelIndex)\0doubleClicked(QModelIndex)\0"
    "activated(QModelIndex)\0entered(QModelIndex)\0"
    "viewportEntered()\0reset()\0"
    "setRootIndex(QModelIndex)\0doItemsLayout()\0"
    "selectAll()\0edit(QModelIndex)\0"
    "clearSelection()\0setCurrentIndex(QModelIndex)\0"
    "scrollToTop()\0scrollToBottom()\0"
    "update(QModelIndex)\0topLeft,bottomRight\0"
    "dataChanged(QModelIndex,QModelIndex)\0"
    "parent,start,end\0rowsInserted(QModelIndex,int,int)\0"
    "rowsAboutToBeRemoved(QModelIndex,int,int)\0"
    "selected,deselected\0"
    "selectionChanged(QItemSelection,QItemSelection)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
    "updateEditorData()\0updateEditorGeometries()\0"
    "updateGeometries()\0action\0"
    "verticalScrollbarAction(int)\0"
    "horizontalScrollbarAction(int)\0value\0"
    "verticalScrollbarValueChanged(int)\0"
    "horizontalScrollbarValueChanged(int)\0"
    "editor,hint\0"
    "closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)\0"
    "editor\0commitData(QWidget*)\0"
    "editorDestroyed(QObject*)\0,,\0"
    "_q_columnsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_columnsRemoved(QModelIndex,int,int)\0"
    "_q_rowsRemoved(QModelIndex,int,int)\0"
    "_q_modelDestroyed()\0_q_layoutChanged()\0"
    "_q_fetchMore()\0bool\0autoScroll\0int\0"
    "autoScrollMargin\0EditTriggers\0"
    "editTriggers\0tabKeyNavigation\0"
    "showDropIndicator\0dragEnabled\0"
    "dragDropOverwriteMode\0DragDropMode\0"
    "dragDropMode\0alternatingRowColors\0"
    "SelectionMode\0selectionMode\0"
    "SelectionBehavior\0selectionBehavior\0"
    "QSize\0iconSize\0Qt::TextElideMode\0"
    "textElideMode\0ScrollMode\0verticalScrollMode\0"
    "horizontalScrollMode\0ScrollHint\0"
    "NoSelection\0SingleSelection\0MultiSelection\0"
    "ExtendedSelection\0ContiguousSelection\0"
    "SelectItems\0SelectRows\0SelectColumns\0"
    "EnsureVisible\0PositionAtTop\0"
    "PositionAtBottom\0PositionAtCenter\0"
    "NoEditTriggers\0CurrentChanged\0"
    "DoubleClicked\0SelectedClicked\0"
    "EditKeyPressed\0AnyKeyPressed\0"
    "AllEditTriggers\0ScrollPerItem\0"
    "ScrollPerPixel\0NoDragDrop\0DragOnly\0"
    "DropOnly\0DragDrop\0InternalMove\0"
};

const QMetaObject QAbstractItemView::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QAbstractItemView,
      qt_meta_data_QAbstractItemView, 0 }
};

const QMetaObject *QAbstractItemView::metaObject() const
{
    return &staticMetaObject;
}

void *QAbstractItemView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAbstractItemView))
	return static_cast<void*>(const_cast< QAbstractItemView*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QAbstractItemView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: pressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: entered((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: viewportEntered(); break;
        case 6: reset(); break;
        case 7: setRootIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: doItemsLayout(); break;
        case 9: selectAll(); break;
        case 10: edit((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 11: clearSelection(); break;
        case 12: setCurrentIndex((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 13: scrollToTop(); break;
        case 14: scrollToBottom(); break;
        case 15: update((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 16: dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 17: rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: rowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 20: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 21: updateEditorData(); break;
        case 22: updateEditorGeometries(); break;
        case 23: updateGeometries(); break;
        case 24: verticalScrollbarAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: horizontalScrollbarAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: verticalScrollbarValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: horizontalScrollbarValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: closeEditor((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QAbstractItemDelegate::EndEditHint(*)>(_a[2]))); break;
        case 29: commitData((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 30: editorDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 31: d_func()->_q_columnsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 32: d_func()->_q_columnsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 33: d_func()->_q_rowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 34: d_func()->_q_modelDestroyed(); break;
        case 35: d_func()->_q_layoutChanged(); break;
        case 36: d_func()->_q_fetchMore(); break;
        }
        _id -= 37;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = hasAutoScroll(); break;
        case 1: *reinterpret_cast< int*>(_v) = autoScrollMargin(); break;
        case 2: *reinterpret_cast<int*>(_v) = QFlag(editTriggers()); break;
        case 3: *reinterpret_cast< bool*>(_v) = tabKeyNavigation(); break;
        case 4: *reinterpret_cast< bool*>(_v) = showDropIndicator(); break;
        case 5: *reinterpret_cast< bool*>(_v) = dragEnabled(); break;
        case 6: *reinterpret_cast< bool*>(_v) = dragDropOverwriteMode(); break;
        case 7: *reinterpret_cast< DragDropMode*>(_v) = dragDropMode(); break;
        case 8: *reinterpret_cast< bool*>(_v) = alternatingRowColors(); break;
        case 9: *reinterpret_cast< SelectionMode*>(_v) = selectionMode(); break;
        case 10: *reinterpret_cast< SelectionBehavior*>(_v) = selectionBehavior(); break;
        case 11: *reinterpret_cast< QSize*>(_v) = iconSize(); break;
        case 12: *reinterpret_cast< Qt::TextElideMode*>(_v) = textElideMode(); break;
        case 13: *reinterpret_cast< ScrollMode*>(_v) = verticalScrollMode(); break;
        case 14: *reinterpret_cast< ScrollMode*>(_v) = horizontalScrollMode(); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAutoScroll(*reinterpret_cast< bool*>(_v)); break;
        case 1: setAutoScrollMargin(*reinterpret_cast< int*>(_v)); break;
        case 2: setEditTriggers(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 3: setTabKeyNavigation(*reinterpret_cast< bool*>(_v)); break;
        case 4: setDropIndicatorShown(*reinterpret_cast< bool*>(_v)); break;
        case 5: setDragEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 6: setDragDropOverwriteMode(*reinterpret_cast< bool*>(_v)); break;
        case 7: setDragDropMode(*reinterpret_cast< DragDropMode*>(_v)); break;
        case 8: setAlternatingRowColors(*reinterpret_cast< bool*>(_v)); break;
        case 9: setSelectionMode(*reinterpret_cast< SelectionMode*>(_v)); break;
        case 10: setSelectionBehavior(*reinterpret_cast< SelectionBehavior*>(_v)); break;
        case 11: setIconSize(*reinterpret_cast< QSize*>(_v)); break;
        case 12: setTextElideMode(*reinterpret_cast< Qt::TextElideMode*>(_v)); break;
        case 13: setVerticalScrollMode(*reinterpret_cast< ScrollMode*>(_v)); break;
        case 14: setHorizontalScrollMode(*reinterpret_cast< ScrollMode*>(_v)); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 15;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QAbstractItemView::pressed(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QAbstractItemView::clicked(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QAbstractItemView::doubleClicked(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QAbstractItemView::activated(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QAbstractItemView::entered(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QAbstractItemView::viewportEntered()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
