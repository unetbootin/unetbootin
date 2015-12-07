/****************************************************************************
** Meta object code from reading C++ file 'qlistview.h'
**
** Created: Thu Mar 5 18:57:34 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../itemviews/qlistview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qlistview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QListView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
      13,   17, // properties
       5,   56, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   11,   10,   10, 0x05,

 // properties: name, type, flags
      58,   49, 0x0009510b,
      72,   67, 0x0009510b,
      82,   77, 0x01095003,
     104,   93, 0x0009510b,
     126,  115, 0x0009510b,
     141,  137, 0x02095103,
     155,  149, 0x15095103,
     173,  164, 0x0009510b,
     182,  137, 0x02095103,
     194,   77, 0x01095103,
     211,  137, 0x02095103,
     221,   77, 0x01095103,
     230,   77, 0x01095103,

 // enums: name, flags, count, data
      49, 0x0,    3,   76,
      67, 0x0,    2,   82,
      93, 0x0,    2,   86,
     115, 0x0,    2,   90,
     164, 0x0,    2,   94,

 // enum data: key, value
     251, uint(QListView::Static),
     258, uint(QListView::Free),
     263, uint(QListView::Snap),
     268, uint(QListView::LeftToRight),
     280, uint(QListView::TopToBottom),
     292, uint(QListView::Fixed),
     298, uint(QListView::Adjust),
     305, uint(QListView::SinglePass),
     316, uint(QListView::Batched),
     324, uint(QListView::ListMode),
     333, uint(QListView::IconMode),

       0        // eod
};

static const char qt_meta_stringdata_QListView[] = {
    "QListView\0\0indexes\0indexesMoved(QModelIndexList)\0"
    "Movement\0movement\0Flow\0flow\0bool\0"
    "isWrapping\0ResizeMode\0resizeMode\0"
    "LayoutMode\0layoutMode\0int\0spacing\0"
    "QSize\0gridSize\0ViewMode\0viewMode\0"
    "modelColumn\0uniformItemSizes\0batchSize\0"
    "wordWrap\0selectionRectVisible\0Static\0"
    "Free\0Snap\0LeftToRight\0TopToBottom\0"
    "Fixed\0Adjust\0SinglePass\0Batched\0"
    "ListMode\0IconMode\0"
};

const QMetaObject QListView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_QListView,
      qt_meta_data_QListView, 0 }
};

const QMetaObject *QListView::metaObject() const
{
    return &staticMetaObject;
}

void *QListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QListView))
        return static_cast<void*>(const_cast< QListView*>(this));
    return QAbstractItemView::qt_metacast(_clname);
}

int QListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexesMoved((*reinterpret_cast< const QModelIndexList(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Movement*>(_v) = movement(); break;
        case 1: *reinterpret_cast< Flow*>(_v) = flow(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isWrapping(); break;
        case 3: *reinterpret_cast< ResizeMode*>(_v) = resizeMode(); break;
        case 4: *reinterpret_cast< LayoutMode*>(_v) = layoutMode(); break;
        case 5: *reinterpret_cast< int*>(_v) = spacing(); break;
        case 6: *reinterpret_cast< QSize*>(_v) = gridSize(); break;
        case 7: *reinterpret_cast< ViewMode*>(_v) = viewMode(); break;
        case 8: *reinterpret_cast< int*>(_v) = modelColumn(); break;
        case 9: *reinterpret_cast< bool*>(_v) = uniformItemSizes(); break;
        case 10: *reinterpret_cast< int*>(_v) = batchSize(); break;
        case 11: *reinterpret_cast< bool*>(_v) = wordWrap(); break;
        case 12: *reinterpret_cast< bool*>(_v) = isSelectionRectVisible(); break;
        }
        _id -= 13;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMovement(*reinterpret_cast< Movement*>(_v)); break;
        case 1: setFlow(*reinterpret_cast< Flow*>(_v)); break;
        case 2: setWrapping(*reinterpret_cast< bool*>(_v)); break;
        case 3: setResizeMode(*reinterpret_cast< ResizeMode*>(_v)); break;
        case 4: setLayoutMode(*reinterpret_cast< LayoutMode*>(_v)); break;
        case 5: setSpacing(*reinterpret_cast< int*>(_v)); break;
        case 6: setGridSize(*reinterpret_cast< QSize*>(_v)); break;
        case 7: setViewMode(*reinterpret_cast< ViewMode*>(_v)); break;
        case 8: setModelColumn(*reinterpret_cast< int*>(_v)); break;
        case 9: setUniformItemSizes(*reinterpret_cast< bool*>(_v)); break;
        case 10: setBatchSize(*reinterpret_cast< int*>(_v)); break;
        case 11: setWordWrap(*reinterpret_cast< bool*>(_v)); break;
        case 12: setSelectionRectVisible(*reinterpret_cast< bool*>(_v)); break;
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
void QListView::indexesMoved(const QModelIndexList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
