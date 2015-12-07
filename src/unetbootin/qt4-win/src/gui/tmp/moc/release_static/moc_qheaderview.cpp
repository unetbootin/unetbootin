/****************************************************************************
** Meta object code from reading C++ file 'qheaderview.h'
**
** Created: Fri May 30 22:30:46 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../itemviews/qheaderview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qheaderview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHeaderView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   10, // methods
       7,  120, // properties
       1,  141, // enums/sets

 // signals: signature, parameters, type, tag, flags
      56,   13,   12,   12, 0x05,
     111,   82,   12,   12, 0x05,
     152,  139,   12,   12, 0x05,
     172,  139,   12,   12, 0x05,
     192,  139,   12,   12, 0x05,
     212,  139,   12,   12, 0x05,
     256,  238,   12,   12, 0x05,
     285,  139,   12,   12, 0x05,
     335,  317,   12,   12, 0x05,
     382,   12,   12,   12, 0x05,
     421,  402,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     468,  461,   12,   12, 0x0a,
     495,  483,   12,   12, 0x0a,
     527,   12,   12,   12, 0x0a,
     589,  552,   12,   12, 0x0a,
     632,  139,   12,   12, 0x09,
     651,   12,   12,   12, 0x09,
     700,  668,   12,   12, 0x09,
     738,  668,   12,   12, 0x09,
     784,  668,   12,   12, 0x08,
     824,   12,   12,   12, 0x08,
     852,   12,   12,   12, 0x08,

 // properties: name, type, flags
     876,  871, 0x01095003,
     894,  871, 0x01095103,
     912,  871, 0x01095103,
     931,  871, 0x01095103,
     959,  955, 0x02095103,
     978,  955, 0x02095103,
    1011,  997, 0x0009510b,

 // enums: name, flags, count, data
    1028, 0x0,    5,  145,

 // enum data: key, value
    1039, uint(QHeaderView::Interactive),
    1051, uint(QHeaderView::Stretch),
    1059, uint(QHeaderView::Fixed),
    1065, uint(QHeaderView::ResizeToContents),
    1082, uint(QHeaderView::Custom),

       0        // eod
};

static const char qt_meta_stringdata_QHeaderView[] = {
    "QHeaderView\0\0logicalIndex,oldVisualIndex,newVisualIndex\0"
    "sectionMoved(int,int,int)\0"
    "logicalIndex,oldSize,newSize\0"
    "sectionResized(int,int,int)\0logicalIndex\0"
    "sectionPressed(int)\0sectionClicked(int)\0"
    "sectionEntered(int)\0sectionDoubleClicked(int)\0"
    "oldCount,newCount\0sectionCountChanged(int,int)\0"
    "sectionHandleDoubleClicked(int)\0"
    "logicalIndex,mode\0"
    "sectionAutoResize(int,QHeaderView::ResizeMode)\0"
    "geometriesChanged()\0logicalIndex,order\0"
    "sortIndicatorChanged(int,Qt::SortOrder)\0"
    "offset\0setOffset(int)\0visualIndex\0"
    "setOffsetToSectionPosition(int)\0"
    "setOffsetToLastSection()\0"
    "orientation,logicalFirst,logicalLast\0"
    "headerDataChanged(Qt::Orientation,int,int)\0"
    "updateSection(int)\0resizeSections()\0"
    "parent,logicalFirst,logicalLast\0"
    "sectionsInserted(QModelIndex,int,int)\0"
    "sectionsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_sectionsRemoved(QModelIndex,int,int)\0"
    "_q_layoutAboutToBeChanged()\0"
    "_q_layoutChanged()\0bool\0showSortIndicator\0"
    "highlightSections\0stretchLastSection\0"
    "cascadingSectionResizes\0int\0"
    "defaultSectionSize\0minimumSectionSize\0"
    "Qt::Alignment\0defaultAlignment\0"
    "ResizeMode\0Interactive\0Stretch\0Fixed\0"
    "ResizeToContents\0Custom\0"
};

const QMetaObject QHeaderView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_QHeaderView,
      qt_meta_data_QHeaderView, 0 }
};

const QMetaObject *QHeaderView::metaObject() const
{
    return &staticMetaObject;
}

void *QHeaderView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHeaderView))
	return static_cast<void*>(const_cast< QHeaderView*>(this));
    return QAbstractItemView::qt_metacast(_clname);
}

int QHeaderView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sectionMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: sectionResized((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: sectionPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: sectionClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: sectionEntered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: sectionDoubleClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: sectionCountChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: sectionHandleDoubleClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: sectionAutoResize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QHeaderView::ResizeMode(*)>(_a[2]))); break;
        case 9: geometriesChanged(); break;
        case 10: sortIndicatorChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        case 11: setOffset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: setOffsetToSectionPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: setOffsetToLastSection(); break;
        case 14: headerDataChanged((*reinterpret_cast< Qt::Orientation(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: updateSection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: resizeSections(); break;
        case 17: sectionsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: sectionsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: d_func()->_q_sectionsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 20: d_func()->_q_layoutAboutToBeChanged(); break;
        case 21: d_func()->_q_layoutChanged(); break;
        }
        _id -= 22;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isSortIndicatorShown(); break;
        case 1: *reinterpret_cast< bool*>(_v) = highlightSections(); break;
        case 2: *reinterpret_cast< bool*>(_v) = stretchLastSection(); break;
        case 3: *reinterpret_cast< bool*>(_v) = cascadingSectionResizes(); break;
        case 4: *reinterpret_cast< int*>(_v) = defaultSectionSize(); break;
        case 5: *reinterpret_cast< int*>(_v) = minimumSectionSize(); break;
        case 6: *reinterpret_cast< Qt::Alignment*>(_v) = defaultAlignment(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSortIndicatorShown(*reinterpret_cast< bool*>(_v)); break;
        case 1: setHighlightSections(*reinterpret_cast< bool*>(_v)); break;
        case 2: setStretchLastSection(*reinterpret_cast< bool*>(_v)); break;
        case 3: setCascadingSectionResizes(*reinterpret_cast< bool*>(_v)); break;
        case 4: setDefaultSectionSize(*reinterpret_cast< int*>(_v)); break;
        case 5: setMinimumSectionSize(*reinterpret_cast< int*>(_v)); break;
        case 6: setDefaultAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QHeaderView::sectionMoved(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QHeaderView::sectionResized(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QHeaderView::sectionPressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QHeaderView::sectionClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QHeaderView::sectionEntered(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QHeaderView::sectionDoubleClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QHeaderView::sectionCountChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QHeaderView::sectionHandleDoubleClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QHeaderView::sectionAutoResize(int _t1, QHeaderView::ResizeMode _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QHeaderView::geometriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void QHeaderView::sortIndicatorChanged(int _t1, Qt::SortOrder _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
