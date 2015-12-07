/****************************************************************************
** Meta object code from reading C++ file 'q3listbox.h'
**
** Created: Fri May 30 22:41:11 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/q3listbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3listbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3ListBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   10, // methods
      13,  165, // properties
       2,  204, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      34,   11,   10,   10, 0x05,
      48,   10,   10,   10, 0x05,
      69,   10,   10,   10, 0x05,
      87,   10,   10,   10, 0x05,
     115,   10,   10,   10, 0x05,
     140,   10,   10,   10, 0x05,
     159,   10,   10,   10, 0x05,
     192,   10,   10,   10, 0x05,
     223,   10,   10,   10, 0x05,
     249,  247,   10,   10, 0x05,
     280,   10,   10,   10, 0x05,
     304,  247,   10,   10, 0x05,
     335,   10,   10,   10, 0x05,
     365,   10,   10,   10, 0x05,
     395,  247,   10,   10, 0x05,
     437,  247,   10,   10, 0x05,
     482,  479,   10,   10, 0x05,
     528,  479,   10,   10, 0x05,
     574,  247,   10,   10, 0x05,
     623,  618,   10,   10, 0x05,
     646,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     659,   10,   10,   10, 0x0a,
     667,   10,   10,   10, 0x0a,
     690,   10,   10,   10, 0x0a,
     714,  707,   10,   10, 0x0a,
     730,   10,   10,   10, 0x0a,
     748,   10,   10,   10, 0x09,
     767,   10,   10,   10, 0x08,
     781,   10,   10,   10, 0x08,
     796,   10,   10,   10, 0x08,

 // properties: name, type, flags
     815,  810, 0x03095001,
     825,  821, 0x02095001,
     841,  821, 0x02195103,
     861,  853, 0x0a095001,
     873,  821, 0x02094103,
     895,  881, 0x0009510b,
     914,  909, 0x01094103,
     940,  929, 0x0009510b,
     951,  929, 0x0009510b,
     959,  821, 0x02095001,
     970,  821, 0x02095001,
     978,  909, 0x01095103,
     992,  909, 0x01095103,

 // enums: name, flags, count, data
     881, 0x0,    4,  212,
     929, 0x0,    4,  220,

 // enum data: key, value
    1007, uint(Q3ListBox::Single),
    1014, uint(Q3ListBox::Multi),
    1020, uint(Q3ListBox::Extended),
    1029, uint(Q3ListBox::NoSelection),
    1041, uint(Q3ListBox::FixedNumber),
    1053, uint(Q3ListBox::FitToWidth),
    1064, uint(Q3ListBox::FitToHeight),
    1076, uint(Q3ListBox::Variable),

       0        // eod
};

static const char qt_meta_stringdata_Q3ListBox[] = {
    "Q3ListBox\0\0index\0highlighted(int)\0"
    "selected(int)\0highlighted(QString)\0"
    "selected(QString)\0highlighted(Q3ListBoxItem*)\0"
    "selected(Q3ListBoxItem*)\0selectionChanged()\0"
    "selectionChanged(Q3ListBoxItem*)\0"
    "currentChanged(Q3ListBoxItem*)\0"
    "clicked(Q3ListBoxItem*)\0,\0"
    "clicked(Q3ListBoxItem*,QPoint)\0"
    "pressed(Q3ListBoxItem*)\0"
    "pressed(Q3ListBoxItem*,QPoint)\0"
    "doubleClicked(Q3ListBoxItem*)\0"
    "returnPressed(Q3ListBoxItem*)\0"
    "rightButtonClicked(Q3ListBoxItem*,QPoint)\0"
    "rightButtonPressed(Q3ListBoxItem*,QPoint)\0"
    ",,\0mouseButtonPressed(int,Q3ListBoxItem*,QPoint)\0"
    "mouseButtonClicked(int,Q3ListBoxItem*,QPoint)\0"
    "contextMenuRequested(Q3ListBoxItem*,QPoint)\0"
    "item\0onItem(Q3ListBoxItem*)\0onViewport()\0"
    "clear()\0ensureCurrentVisible()\0"
    "clearSelection()\0select\0selectAll(bool)\0"
    "invertSelection()\0clearInputString()\0"
    "refreshSlot()\0doAutoScroll()\0adjustItems()\0"
    "uint\0count\0int\0numItemsVisible\0"
    "currentItem\0QString\0currentText\0topItem\0"
    "SelectionMode\0selectionMode\0bool\0"
    "multiSelection\0LayoutMode\0columnMode\0"
    "rowMode\0numColumns\0numRows\0variableWidth\0"
    "variableHeight\0Single\0Multi\0Extended\0"
    "NoSelection\0FixedNumber\0FitToWidth\0"
    "FitToHeight\0Variable\0"
};

const QMetaObject Q3ListBox::staticMetaObject = {
    { &Q3ScrollView::staticMetaObject, qt_meta_stringdata_Q3ListBox,
      qt_meta_data_Q3ListBox, 0 }
};

const QMetaObject *Q3ListBox::metaObject() const
{
    return &staticMetaObject;
}

void *Q3ListBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3ListBox))
	return static_cast<void*>(const_cast< Q3ListBox*>(this));
    return Q3ScrollView::qt_metacast(_clname);
}

int Q3ListBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3ScrollView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: highlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: highlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: selected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: highlighted((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 5: selected((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 6: selectionChanged(); break;
        case 7: selectionChanged((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 8: currentChanged((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 9: clicked((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 10: clicked((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 11: pressed((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 12: pressed((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 13: doubleClicked((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 14: returnPressed((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 15: rightButtonClicked((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 16: rightButtonPressed((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 17: mouseButtonPressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Q3ListBoxItem*(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3]))); break;
        case 18: mouseButtonClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Q3ListBoxItem*(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3]))); break;
        case 19: contextMenuRequested((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 20: onItem((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 21: onViewport(); break;
        case 22: clear(); break;
        case 23: ensureCurrentVisible(); break;
        case 24: clearSelection(); break;
        case 25: selectAll((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: invertSelection(); break;
        case 27: clearInputString(); break;
        case 28: refreshSlot(); break;
        case 29: doAutoScroll(); break;
        case 30: adjustItems(); break;
        }
        _id -= 31;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< uint*>(_v) = count(); break;
        case 1: *reinterpret_cast< int*>(_v) = numItemsVisible(); break;
        case 2: *reinterpret_cast< int*>(_v) = currentItem(); break;
        case 3: *reinterpret_cast< QString*>(_v) = currentText(); break;
        case 4: *reinterpret_cast< int*>(_v) = topItem(); break;
        case 5: *reinterpret_cast< SelectionMode*>(_v) = selectionMode(); break;
        case 6: *reinterpret_cast< bool*>(_v) = isMultiSelection(); break;
        case 7: *reinterpret_cast< LayoutMode*>(_v) = columnMode(); break;
        case 8: *reinterpret_cast< LayoutMode*>(_v) = rowMode(); break;
        case 9: *reinterpret_cast< int*>(_v) = numColumns(); break;
        case 10: *reinterpret_cast< int*>(_v) = numRows(); break;
        case 11: *reinterpret_cast< bool*>(_v) = variableWidth(); break;
        case 12: *reinterpret_cast< bool*>(_v) = variableHeight(); break;
        }
        _id -= 13;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setCurrentItem(*reinterpret_cast< int*>(_v)); break;
        case 4: setTopItem(*reinterpret_cast< int*>(_v)); break;
        case 5: setSelectionMode(*reinterpret_cast< SelectionMode*>(_v)); break;
        case 6: setMultiSelection(*reinterpret_cast< bool*>(_v)); break;
        case 7: setColumnMode(*reinterpret_cast< LayoutMode*>(_v)); break;
        case 8: setRowMode(*reinterpret_cast< LayoutMode*>(_v)); break;
        case 11: setVariableWidth(*reinterpret_cast< bool*>(_v)); break;
        case 12: setVariableHeight(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 13;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 13;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3ListBox::highlighted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3ListBox::selected(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3ListBox::highlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3ListBox::selected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3ListBox::highlighted(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3ListBox::selected(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3ListBox::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void Q3ListBox::selectionChanged(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Q3ListBox::currentChanged(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Q3ListBox::clicked(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Q3ListBox::clicked(Q3ListBoxItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Q3ListBox::pressed(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Q3ListBox::pressed(Q3ListBoxItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Q3ListBox::doubleClicked(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Q3ListBox::returnPressed(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void Q3ListBox::rightButtonClicked(Q3ListBoxItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void Q3ListBox::rightButtonPressed(Q3ListBoxItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void Q3ListBox::mouseButtonPressed(int _t1, Q3ListBoxItem * _t2, const QPoint & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void Q3ListBox::mouseButtonClicked(int _t1, Q3ListBoxItem * _t2, const QPoint & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void Q3ListBox::contextMenuRequested(Q3ListBoxItem * _t1, const QPoint & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void Q3ListBox::onItem(Q3ListBoxItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void Q3ListBox::onViewport()
{
    QMetaObject::activate(this, &staticMetaObject, 21, 0);
}
QT_END_MOC_NAMESPACE
