/****************************************************************************
** Meta object code from reading C++ file 'q3scrollview.h'
**
** Created: Fri May 30 22:41:27 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/q3scrollview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3scrollview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3ScrollView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   10, // methods
      10,  125, // properties
       2,  155, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   14,   13,   13, 0x05,
      42,   13,   13,   13, 0x05,
      68,   13,   13,   13, 0x05,
      95,   13,   13,   13, 0x05,
     119,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     148,  144,   13,   13, 0x0a,
     178,  172,   13,   13, 0x0a,
     196,   14,   13,   13, 0x0a,
     220,   14,   13,   13, 0x0a,
     263,  243,   13,   13, 0x0a,
     294,   14,   13,   13, 0x0a,
     310,  243,   13,   13, 0x0a,
     338,   13,   13,   13, 0x0a,
     364,  357,   13,   13, 0x0a,
     381,   13,   13,   13, 0x08,
     393,   13,   13,   13, 0x08,
     405,   13,   13,   13, 0x08,
     421,   13,   13,   13, 0x08,
     438,   13,   13,   13, 0x08,
     454,   13,   13,   13, 0x08,
     471,   13,   13,   13, 0x08,
     490,   13,   13,   13, 0x08,
     512,   13,   13,   13, 0x08,

 // properties: name, type, flags
     546,  533, 0x0009510b,
     573,  559, 0x0009510b,
     588,  559, 0x0009510b,
     607,  603, 0x02095001,
     620,  603, 0x02095001,
     634,  603, 0x02095001,
     648,  603, 0x02095001,
     663,  603, 0x02095001,
     673,  603, 0x02095001,
     688,  683, 0x01095103,

 // enums: name, flags, count, data
     533, 0x0,    4,  163,
     559, 0x0,    3,  171,

 // enum data: key, value
     703, uint(Q3ScrollView::Default),
     711, uint(Q3ScrollView::Manual),
     718, uint(Q3ScrollView::AutoOne),
     726, uint(Q3ScrollView::AutoOneFit),
     737, uint(Q3ScrollView::Auto),
     742, uint(Q3ScrollView::AlwaysOff),
     752, uint(Q3ScrollView::AlwaysOn),

       0        // eod
};

static const char qt_meta_stringdata_Q3ScrollView[] = {
    "Q3ScrollView\0\0x,y\0contentsMoving(int,int)\0"
    "horizontalSliderPressed()\0"
    "horizontalSliderReleased()\0"
    "verticalSliderPressed()\0"
    "verticalSliderReleased()\0w,h\0"
    "resizeContents(int,int)\0dx,dy\0"
    "scrollBy(int,int)\0setContentsPos(int,int)\0"
    "ensureVisible(int,int)\0x,y,xmargin,ymargin\0"
    "ensureVisible(int,int,int,int)\0"
    "center(int,int)\0center(int,int,float,float)\0"
    "updateScrollBars()\0enable\0setEnabled(bool)\0"
    "hslide(int)\0vslide(int)\0hbarIsPressed()\0"
    "hbarIsReleased()\0vbarIsPressed()\0"
    "vbarIsReleased()\0doDragAutoScroll()\0"
    "startDragAutoScroll()\0stopDragAutoScroll()\0"
    "ResizePolicy\0resizePolicy\0ScrollBarMode\0"
    "vScrollBarMode\0hScrollBarMode\0int\0"
    "visibleWidth\0visibleHeight\0contentsWidth\0"
    "contentsHeight\0contentsX\0contentsY\0"
    "bool\0dragAutoScroll\0Default\0Manual\0"
    "AutoOne\0AutoOneFit\0Auto\0AlwaysOff\0"
    "AlwaysOn\0"
};

const QMetaObject Q3ScrollView::staticMetaObject = {
    { &Q3Frame::staticMetaObject, qt_meta_stringdata_Q3ScrollView,
      qt_meta_data_Q3ScrollView, 0 }
};

const QMetaObject *Q3ScrollView::metaObject() const
{
    return &staticMetaObject;
}

void *Q3ScrollView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3ScrollView))
	return static_cast<void*>(const_cast< Q3ScrollView*>(this));
    return Q3Frame::qt_metacast(_clname);
}

int Q3ScrollView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3Frame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contentsMoving((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: horizontalSliderPressed(); break;
        case 2: horizontalSliderReleased(); break;
        case 3: verticalSliderPressed(); break;
        case 4: verticalSliderReleased(); break;
        case 5: resizeContents((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: scrollBy((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: setContentsPos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: ensureVisible((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: ensureVisible((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 10: center((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: center((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        case 12: updateScrollBars(); break;
        case 13: setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: hslide((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: vslide((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: hbarIsPressed(); break;
        case 17: hbarIsReleased(); break;
        case 18: vbarIsPressed(); break;
        case 19: vbarIsReleased(); break;
        case 20: doDragAutoScroll(); break;
        case 21: startDragAutoScroll(); break;
        case 22: stopDragAutoScroll(); break;
        }
        _id -= 23;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ResizePolicy*>(_v) = resizePolicy(); break;
        case 1: *reinterpret_cast< ScrollBarMode*>(_v) = vScrollBarMode(); break;
        case 2: *reinterpret_cast< ScrollBarMode*>(_v) = hScrollBarMode(); break;
        case 3: *reinterpret_cast< int*>(_v) = visibleWidth(); break;
        case 4: *reinterpret_cast< int*>(_v) = visibleHeight(); break;
        case 5: *reinterpret_cast< int*>(_v) = contentsWidth(); break;
        case 6: *reinterpret_cast< int*>(_v) = contentsHeight(); break;
        case 7: *reinterpret_cast< int*>(_v) = contentsX(); break;
        case 8: *reinterpret_cast< int*>(_v) = contentsY(); break;
        case 9: *reinterpret_cast< bool*>(_v) = dragAutoScroll(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setResizePolicy(*reinterpret_cast< ResizePolicy*>(_v)); break;
        case 1: setVScrollBarMode(*reinterpret_cast< ScrollBarMode*>(_v)); break;
        case 2: setHScrollBarMode(*reinterpret_cast< ScrollBarMode*>(_v)); break;
        case 9: setDragAutoScroll(*reinterpret_cast< bool*>(_v)); break;
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
void Q3ScrollView::contentsMoving(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3ScrollView::horizontalSliderPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Q3ScrollView::horizontalSliderReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Q3ScrollView::verticalSliderPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Q3ScrollView::verticalSliderReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
