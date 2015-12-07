/****************************************************************************
** Meta object code from reading C++ file 'q3iconview.h'
**
** Created: Thu Mar 5 19:05:49 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/q3iconview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3iconview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3IconDrag[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_Q3IconDrag[] = {
    "Q3IconDrag\0"
};

const QMetaObject Q3IconDrag::staticMetaObject = {
    { &Q3DragObject::staticMetaObject, qt_meta_stringdata_Q3IconDrag,
      qt_meta_data_Q3IconDrag, 0 }
};

const QMetaObject *Q3IconDrag::metaObject() const
{
    return &staticMetaObject;
}

void *Q3IconDrag::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3IconDrag))
        return static_cast<void*>(const_cast< Q3IconDrag*>(this));
    return Q3DragObject::qt_metacast(_clname);
}

int Q3IconDrag::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DragObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Q3IconView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   12, // methods
      17,  162, // properties
       4,  213, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      36,   31,   11,   11, 0x05,
      70,   31,   11,   11, 0x05,
     102,   11,   11,   11, 0x05,
     129,  127,   11,   11, 0x05,
     161,   11,   11,   11, 0x05,
     186,  127,   11,   11, 0x05,
     218,   31,   11,   11, 0x05,
     249,   31,   11,   11, 0x05,
     289,  280,   11,   11, 0x05,
     332,  280,   11,   11, 0x05,
     391,  375,   11,   11, 0x05,
     438,  375,   11,   11, 0x05,
     485,  280,   11,   11, 0x05,
     536,  530,   11,   11, 0x05,
     585,   11,   11,   11, 0x05,
     593,   31,   11,   11, 0x05,
     617,   11,   11,   11, 0x05,
     636,  630,   11,   11, 0x05,
     673,   31,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     714,  702,   11,   11, 0x0a,
     750,  745,   11,   11, 0x2a,
     783,  776,   11,   11, 0x0a,
     808,   11,   11,   11, 0x2a,
     833,  829,   11,   11, 0x0a,
     857,   11,   11,   11, 0x0a,
     874,   11,   11,   11, 0x09,
     889,   11,   11,   11, 0x09,
     903,   11,   11,   11, 0x09,
     922,  916,   11,   11, 0x08,

 // properties: name, type, flags
     950,  945, 0x01095001,
     958,  945, 0x01095001,
     986,  972, 0x0009510b,
    1004, 1000, 0x02095103,
    1010, 1000, 0x02095103,
    1016, 1000, 0x02095103,
    1036, 1024, 0x0009510b,
    1055, 1048, 0x42095103,
    1086, 1074, 0x0009510b,
    1109, 1098, 0x0009510b,
    1120, 1000, 0x02095103,
    1133, 1000, 0x02095103,
    1151,  945, 0x01095103,
    1163,  945, 0x01095103,
    1176,  945, 0x01095103,
    1193,  945, 0x01095103,
    1211, 1206, 0x03095001,

 // enums: name, flags, count, data
     972, 0x0,    4,  229,
    1074, 0x0,    2,  237,
    1098, 0x0,    2,  241,
    1024, 0x0,    2,  245,

 // enum data: key, value
    1217, uint(Q3IconView::Single),
    1224, uint(Q3IconView::Multi),
    1230, uint(Q3IconView::Extended),
    1239, uint(Q3IconView::NoSelection),
    1251, uint(Q3IconView::LeftToRight),
    1263, uint(Q3IconView::TopToBottom),
    1275, uint(Q3IconView::Fixed),
    1281, uint(Q3IconView::Adjust),
    1288, uint(Q3IconView::Bottom),
    1295, uint(Q3IconView::Right),

       0        // eod
};

static const char qt_meta_stringdata_Q3IconView[] = {
    "Q3IconView\0\0selectionChanged()\0item\0"
    "selectionChanged(Q3IconViewItem*)\0"
    "currentChanged(Q3IconViewItem*)\0"
    "clicked(Q3IconViewItem*)\0,\0"
    "clicked(Q3IconViewItem*,QPoint)\0"
    "pressed(Q3IconViewItem*)\0"
    "pressed(Q3IconViewItem*,QPoint)\0"
    "doubleClicked(Q3IconViewItem*)\0"
    "returnPressed(Q3IconViewItem*)\0item,pos\0"
    "rightButtonClicked(Q3IconViewItem*,QPoint)\0"
    "rightButtonPressed(Q3IconViewItem*,QPoint)\0"
    "button,item,pos\0"
    "mouseButtonPressed(int,Q3IconViewItem*,QPoint)\0"
    "mouseButtonClicked(int,Q3IconViewItem*,QPoint)\0"
    "contextMenuRequested(Q3IconViewItem*,QPoint)\0"
    "e,lst\0dropped(QDropEvent*,Q3ValueList<Q3IconDragItem>)\0"
    "moved()\0onItem(Q3IconViewItem*)\0"
    "onViewport()\0item,\0"
    "itemRenamed(Q3IconViewItem*,QString)\0"
    "itemRenamed(Q3IconViewItem*)\0grid,update\0"
    "arrangeItemsInGrid(QSize,bool)\0grid\0"
    "arrangeItemsInGrid(QSize)\0update\0"
    "arrangeItemsInGrid(bool)\0arrangeItemsInGrid()\0"
    "x,y\0setContentsPos(int,int)\0"
    "updateContents()\0doAutoScroll()\0"
    "adjustItems()\0slotUpdate()\0dx,dy\0"
    "movedContents(int,int)\0bool\0sorting\0"
    "sortDirection\0SelectionMode\0selectionMode\0"
    "int\0gridX\0gridY\0spacing\0ItemTextPos\0"
    "itemTextPos\0QBrush\0itemTextBackground\0"
    "Arrangement\0arrangement\0ResizeMode\0"
    "resizeMode\0maxItemWidth\0maxItemTextLength\0"
    "autoArrange\0itemsMovable\0wordWrapIconText\0"
    "showToolTips\0uint\0count\0Single\0Multi\0"
    "Extended\0NoSelection\0LeftToRight\0"
    "TopToBottom\0Fixed\0Adjust\0Bottom\0Right\0"
};

const QMetaObject Q3IconView::staticMetaObject = {
    { &Q3ScrollView::staticMetaObject, qt_meta_stringdata_Q3IconView,
      qt_meta_data_Q3IconView, 0 }
};

const QMetaObject *Q3IconView::metaObject() const
{
    return &staticMetaObject;
}

void *Q3IconView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3IconView))
        return static_cast<void*>(const_cast< Q3IconView*>(this));
    return Q3ScrollView::qt_metacast(_clname);
}

int Q3IconView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3ScrollView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectionChanged(); break;
        case 1: selectionChanged((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 2: currentChanged((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 3: clicked((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 4: clicked((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 5: pressed((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 6: pressed((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 7: doubleClicked((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 8: returnPressed((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 9: rightButtonClicked((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 10: rightButtonPressed((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 11: mouseButtonPressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Q3IconViewItem*(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3]))); break;
        case 12: mouseButtonClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Q3IconViewItem*(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3]))); break;
        case 13: contextMenuRequested((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 14: dropped((*reinterpret_cast< QDropEvent*(*)>(_a[1])),(*reinterpret_cast< const Q3ValueList<Q3IconDragItem>(*)>(_a[2]))); break;
        case 15: moved(); break;
        case 16: onItem((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 17: onViewport(); break;
        case 18: itemRenamed((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 19: itemRenamed((*reinterpret_cast< Q3IconViewItem*(*)>(_a[1]))); break;
        case 20: arrangeItemsInGrid((*reinterpret_cast< const QSize(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 21: arrangeItemsInGrid((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 22: arrangeItemsInGrid((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: arrangeItemsInGrid(); break;
        case 24: setContentsPos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 25: updateContents(); break;
        case 26: doAutoScroll(); break;
        case 27: adjustItems(); break;
        case 28: slotUpdate(); break;
        case 29: movedContents((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 30;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = sorting(); break;
        case 1: *reinterpret_cast< bool*>(_v) = sortDirection(); break;
        case 2: *reinterpret_cast< SelectionMode*>(_v) = selectionMode(); break;
        case 3: *reinterpret_cast< int*>(_v) = gridX(); break;
        case 4: *reinterpret_cast< int*>(_v) = gridY(); break;
        case 5: *reinterpret_cast< int*>(_v) = spacing(); break;
        case 6: *reinterpret_cast< ItemTextPos*>(_v) = itemTextPos(); break;
        case 7: *reinterpret_cast< QBrush*>(_v) = itemTextBackground(); break;
        case 8: *reinterpret_cast< Arrangement*>(_v) = arrangement(); break;
        case 9: *reinterpret_cast< ResizeMode*>(_v) = resizeMode(); break;
        case 10: *reinterpret_cast< int*>(_v) = maxItemWidth(); break;
        case 11: *reinterpret_cast< int*>(_v) = maxItemTextLength(); break;
        case 12: *reinterpret_cast< bool*>(_v) = autoArrange(); break;
        case 13: *reinterpret_cast< bool*>(_v) = itemsMovable(); break;
        case 14: *reinterpret_cast< bool*>(_v) = wordWrapIconText(); break;
        case 15: *reinterpret_cast< bool*>(_v) = showToolTips(); break;
        case 16: *reinterpret_cast< uint*>(_v) = count(); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setSelectionMode(*reinterpret_cast< SelectionMode*>(_v)); break;
        case 3: setGridX(*reinterpret_cast< int*>(_v)); break;
        case 4: setGridY(*reinterpret_cast< int*>(_v)); break;
        case 5: setSpacing(*reinterpret_cast< int*>(_v)); break;
        case 6: setItemTextPos(*reinterpret_cast< ItemTextPos*>(_v)); break;
        case 7: setItemTextBackground(*reinterpret_cast< QBrush*>(_v)); break;
        case 8: setArrangement(*reinterpret_cast< Arrangement*>(_v)); break;
        case 9: setResizeMode(*reinterpret_cast< ResizeMode*>(_v)); break;
        case 10: setMaxItemWidth(*reinterpret_cast< int*>(_v)); break;
        case 11: setMaxItemTextLength(*reinterpret_cast< int*>(_v)); break;
        case 12: setAutoArrange(*reinterpret_cast< bool*>(_v)); break;
        case 13: setItemsMovable(*reinterpret_cast< bool*>(_v)); break;
        case 14: setWordWrapIconText(*reinterpret_cast< bool*>(_v)); break;
        case 15: setShowToolTips(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 17;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3IconView::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Q3IconView::selectionChanged(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3IconView::currentChanged(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3IconView::clicked(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3IconView::clicked(Q3IconViewItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3IconView::pressed(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3IconView::pressed(Q3IconViewItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Q3IconView::doubleClicked(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Q3IconView::returnPressed(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Q3IconView::rightButtonClicked(Q3IconViewItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Q3IconView::rightButtonPressed(Q3IconViewItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Q3IconView::mouseButtonPressed(int _t1, Q3IconViewItem * _t2, const QPoint & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Q3IconView::mouseButtonClicked(int _t1, Q3IconViewItem * _t2, const QPoint & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Q3IconView::contextMenuRequested(Q3IconViewItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Q3IconView::dropped(QDropEvent * _t1, const Q3ValueList<Q3IconDragItem> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void Q3IconView::moved()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void Q3IconView::onItem(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void Q3IconView::onViewport()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void Q3IconView::itemRenamed(Q3IconViewItem * _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void Q3IconView::itemRenamed(Q3IconViewItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}
QT_END_MOC_NAMESPACE
