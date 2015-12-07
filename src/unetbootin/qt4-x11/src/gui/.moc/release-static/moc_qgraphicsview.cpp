/****************************************************************************
** Meta object code from reading C++ file 'qgraphicsview.h'
**
** Created: Thu Mar 5 18:56:18 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../graphicsview/qgraphicsview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgraphicsview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGraphicsView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
      13,   57, // properties
       5,   96, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x0a,
      60,   48,   14,   14, 0x0a,
     117,  112,   14,   14, 0x2a,
     141,   14,   14,   14, 0x2a,
     159,  112,   14,   14, 0x0a,
     190,  183,   14,   14, 0x09,
     214,   14,   14,   14, 0x08,
     244,   14,   14,   14, 0x08,
     269,   14,   14,   14, 0x08,

 // properties: name, type, flags
     297,  290, 0x42095103,
     313,  290, 0x42095103,
     334,  329, 0x01095103,
     353,  346, 0x14095103,
     377,  363, 0x0009510b,
     409,  387, 0x0009510b,
     430,  421, 0x0009510b,
     449,  439, 0x0009510b,
     474,  459, 0x0009510b,
     495,  459, 0x0009510b,
     527,  508, 0x0009510b,
     568,  546, 0x0009510b,
     610,  592, 0x0009510b,

 // enums: name, flags, count, data
     459, 0x0,    3,  116,
     439, 0x1,    2,  122,
     421, 0x0,    3,  126,
     508, 0x0,    5,  132,
     592, 0x1,    3,  142,

 // enum data: key, value
     628, uint(QGraphicsView::NoAnchor),
     637, uint(QGraphicsView::AnchorViewCenter),
     654, uint(QGraphicsView::AnchorUnderMouse),
     671, uint(QGraphicsView::CacheNone),
     681, uint(QGraphicsView::CacheBackground),
     697, uint(QGraphicsView::NoDrag),
     704, uint(QGraphicsView::ScrollHandDrag),
     719, uint(QGraphicsView::RubberBandDrag),
     734, uint(QGraphicsView::FullViewportUpdate),
     753, uint(QGraphicsView::MinimalViewportUpdate),
     775, uint(QGraphicsView::SmartViewportUpdate),
     795, uint(QGraphicsView::NoViewportUpdate),
     812, uint(QGraphicsView::BoundingRectViewportUpdate),
     839, uint(QGraphicsView::DontClipPainter),
     855, uint(QGraphicsView::DontSavePainterState),
     876, uint(QGraphicsView::DontAdjustForAntialiasing),

       0        // eod
};

static const char qt_meta_stringdata_QGraphicsView[] = {
    "QGraphicsView\0\0rects\0updateScene(QList<QRectF>)\0"
    "rect,layers\0"
    "invalidateScene(QRectF,QGraphicsScene::SceneLayers)\0"
    "rect\0invalidateScene(QRectF)\0"
    "invalidateScene()\0updateSceneRect(QRectF)\0"
    "widget\0setupViewport(QWidget*)\0"
    "_q_setViewportCursor(QCursor)\0"
    "_q_unsetViewportCursor()\0_q_updateLaterSlot()\0"
    "QBrush\0backgroundBrush\0foregroundBrush\0"
    "bool\0interactive\0QRectF\0sceneRect\0"
    "Qt::Alignment\0alignment\0QPainter::RenderHints\0"
    "renderHints\0DragMode\0dragMode\0CacheMode\0"
    "cacheMode\0ViewportAnchor\0transformationAnchor\0"
    "resizeAnchor\0ViewportUpdateMode\0"
    "viewportUpdateMode\0Qt::ItemSelectionMode\0"
    "rubberBandSelectionMode\0OptimizationFlags\0"
    "optimizationFlags\0NoAnchor\0AnchorViewCenter\0"
    "AnchorUnderMouse\0CacheNone\0CacheBackground\0"
    "NoDrag\0ScrollHandDrag\0RubberBandDrag\0"
    "FullViewportUpdate\0MinimalViewportUpdate\0"
    "SmartViewportUpdate\0NoViewportUpdate\0"
    "BoundingRectViewportUpdate\0DontClipPainter\0"
    "DontSavePainterState\0DontAdjustForAntialiasing\0"
};

static const QMetaObject *qt_meta_extradata_QGraphicsView[] = {
        &QPainter::staticMetaObject,0
};

static const QMetaObjectExtraData qt_meta_extradata2_QGraphicsView = {
    qt_meta_extradata_QGraphicsView, 0 
};

const QMetaObject QGraphicsView::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QGraphicsView,
      qt_meta_data_QGraphicsView, &qt_meta_extradata2_QGraphicsView }
};

const QMetaObject *QGraphicsView::metaObject() const
{
    return &staticMetaObject;
}

void *QGraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGraphicsView))
        return static_cast<void*>(const_cast< QGraphicsView*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QGraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateScene((*reinterpret_cast< const QList<QRectF>(*)>(_a[1]))); break;
        case 1: invalidateScene((*reinterpret_cast< const QRectF(*)>(_a[1])),(*reinterpret_cast< QGraphicsScene::SceneLayers(*)>(_a[2]))); break;
        case 2: invalidateScene((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 3: invalidateScene(); break;
        case 4: updateSceneRect((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 5: setupViewport((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 6: d_func()->_q_setViewportCursor((*reinterpret_cast< const QCursor(*)>(_a[1]))); break;
        case 7: d_func()->_q_unsetViewportCursor(); break;
        case 8: d_func()->_q_updateLaterSlot(); break;
        default: ;
        }
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QBrush*>(_v) = backgroundBrush(); break;
        case 1: *reinterpret_cast< QBrush*>(_v) = foregroundBrush(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isInteractive(); break;
        case 3: *reinterpret_cast< QRectF*>(_v) = sceneRect(); break;
        case 4: *reinterpret_cast< Qt::Alignment*>(_v) = alignment(); break;
        case 5: *reinterpret_cast<int*>(_v) = QFlag(renderHints()); break;
        case 6: *reinterpret_cast< DragMode*>(_v) = dragMode(); break;
        case 7: *reinterpret_cast<int*>(_v) = QFlag(cacheMode()); break;
        case 8: *reinterpret_cast< ViewportAnchor*>(_v) = transformationAnchor(); break;
        case 9: *reinterpret_cast< ViewportAnchor*>(_v) = resizeAnchor(); break;
        case 10: *reinterpret_cast< ViewportUpdateMode*>(_v) = viewportUpdateMode(); break;
        case 11: *reinterpret_cast< Qt::ItemSelectionMode*>(_v) = rubberBandSelectionMode(); break;
        case 12: *reinterpret_cast<int*>(_v) = QFlag(optimizationFlags()); break;
        }
        _id -= 13;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBackgroundBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 1: setForegroundBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 2: setInteractive(*reinterpret_cast< bool*>(_v)); break;
        case 3: setSceneRect(*reinterpret_cast< QRectF*>(_v)); break;
        case 4: setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 5: setRenderHints(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 6: setDragMode(*reinterpret_cast< DragMode*>(_v)); break;
        case 7: setCacheMode(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 8: setTransformationAnchor(*reinterpret_cast< ViewportAnchor*>(_v)); break;
        case 9: setResizeAnchor(*reinterpret_cast< ViewportAnchor*>(_v)); break;
        case 10: setViewportUpdateMode(*reinterpret_cast< ViewportUpdateMode*>(_v)); break;
        case 11: setRubberBandSelectionMode(*reinterpret_cast< Qt::ItemSelectionMode*>(_v)); break;
        case 12: setOptimizationFlags(QFlag(*reinterpret_cast<int*>(_v))); break;
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
QT_END_MOC_NAMESPACE
