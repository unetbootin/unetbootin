/****************************************************************************
** Meta object code from reading C++ file 'qgraphicsview.h'
**
** Created: Fri May 30 22:32:30 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../graphicsview/qgraphicsview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgraphicsview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGraphicsView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
      13,   50, // properties
       5,   89, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x0a,
      60,   48,   14,   14, 0x0a,
     117,  112,   14,   14, 0x2a,
     141,   14,   14,   14, 0x2a,
     159,  112,   14,   14, 0x0a,
     190,  183,   14,   14, 0x09,
     214,   14,   14,   14, 0x08,
     244,   14,   14,   14, 0x08,

 // properties: name, type, flags
     276,  269, 0x42095103,
     292,  269, 0x42095103,
     313,  308, 0x01095103,
     332,  325, 0x14095103,
     356,  342, 0x0009510b,
     388,  366, 0x0009510b,
     409,  400, 0x0009510b,
     428,  418, 0x0009510b,
     453,  438, 0x0009510b,
     474,  438, 0x0009510b,
     506,  487, 0x0009510b,
     547,  525, 0x0009510b,
     589,  571, 0x0009510b,

 // enums: name, flags, count, data
     438, 0x0,    3,  109,
     418, 0x1,    2,  115,
     400, 0x0,    3,  119,
     487, 0x0,    5,  125,
     571, 0x1,    3,  135,

 // enum data: key, value
     607, uint(QGraphicsView::NoAnchor),
     616, uint(QGraphicsView::AnchorViewCenter),
     633, uint(QGraphicsView::AnchorUnderMouse),
     650, uint(QGraphicsView::CacheNone),
     660, uint(QGraphicsView::CacheBackground),
     676, uint(QGraphicsView::NoDrag),
     683, uint(QGraphicsView::ScrollHandDrag),
     698, uint(QGraphicsView::RubberBandDrag),
     713, uint(QGraphicsView::FullViewportUpdate),
     732, uint(QGraphicsView::MinimalViewportUpdate),
     754, uint(QGraphicsView::SmartViewportUpdate),
     774, uint(QGraphicsView::NoViewportUpdate),
     791, uint(QGraphicsView::BoundingRectViewportUpdate),
     818, uint(QGraphicsView::DontClipPainter),
     834, uint(QGraphicsView::DontSavePainterState),
     855, uint(QGraphicsView::DontAdjustForAntialiasing),

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
    "_q_unsetViewportCursor()\0QBrush\0"
    "backgroundBrush\0foregroundBrush\0bool\0"
    "interactive\0QRectF\0sceneRect\0Qt::Alignment\0"
    "alignment\0QPainter::RenderHints\0"
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

const QMetaObject QGraphicsView::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QGraphicsView,
      qt_meta_data_QGraphicsView, qt_meta_extradata_QGraphicsView }
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
        }
        _id -= 8;
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
