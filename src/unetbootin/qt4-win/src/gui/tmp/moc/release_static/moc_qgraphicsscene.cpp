/****************************************************************************
** Meta object code from reading C++ file 'qgraphicsscene.h'
**
** Created: Fri May 30 22:32:21 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../graphicsview/qgraphicsscene.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgraphicsscene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGraphicsScene[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   10, // methods
       7,   95, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      23,   16,   15,   15, 0x05,
      51,   46,   15,   15, 0x05,
      76,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      95,   46,   15,   15, 0x0a,
     110,   15,   15,   15, 0x2a,
     131,  119,   15,   15, 0x0a,
     162,   46,   15,   15, 0x2a,
     181,   15,   15,   15, 0x2a,
     194,   15,   15,   15, 0x0a,
     204,   15,   15,   15, 0x0a,
     221,   15,   15,   15, 0x0a,
     239,  234,  229,   15, 0x09,
     264,   15,   15,   15, 0x08,
     281,   15,   15,   15, 0x08,
     303,  298,   15,   15, 0x08,
     338,   15,   15,   15, 0x08,
     355,   15,   15,   15, 0x08,

 // properties: name, type, flags
     379,  372, 0x42095103,
     395,  372, 0x42095103,
     427,  411, 0x0009510b,
     450,  443, 0x14095103,
     464,  460, 0x02095103,
     486,  477, 0x44095103,
     500,  494, 0x40095103,

       0        // eod
};

static const char qt_meta_stringdata_QGraphicsScene[] = {
    "QGraphicsScene\0\0region\0changed(QList<QRectF>)\0"
    "rect\0sceneRectChanged(QRectF)\0"
    "selectionChanged()\0update(QRectF)\0"
    "update()\0rect,layers\0"
    "invalidate(QRectF,SceneLayers)\0"
    "invalidate(QRectF)\0invalidate()\0"
    "advance()\0clearSelection()\0clear()\0"
    "bool\0next\0focusNextPrevChild(bool)\0"
    "_q_updateIndex()\0_q_emitUpdated()\0"
    "item\0_q_removeItemLater(QGraphicsItem*)\0"
    "_q_updateLater()\0_q_polishItems()\0"
    "QBrush\0backgroundBrush\0foregroundBrush\0"
    "ItemIndexMethod\0itemIndexMethod\0QRectF\0"
    "sceneRect\0int\0bspTreeDepth\0QPalette\0"
    "palette\0QFont\0font\0"
};

const QMetaObject QGraphicsScene::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QGraphicsScene,
      qt_meta_data_QGraphicsScene, 0 }
};

const QMetaObject *QGraphicsScene::metaObject() const
{
    return &staticMetaObject;
}

void *QGraphicsScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGraphicsScene))
	return static_cast<void*>(const_cast< QGraphicsScene*>(this));
    return QObject::qt_metacast(_clname);
}

int QGraphicsScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changed((*reinterpret_cast< const QList<QRectF>(*)>(_a[1]))); break;
        case 1: sceneRectChanged((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 2: selectionChanged(); break;
        case 3: update((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 4: update(); break;
        case 5: invalidate((*reinterpret_cast< const QRectF(*)>(_a[1])),(*reinterpret_cast< SceneLayers(*)>(_a[2]))); break;
        case 6: invalidate((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 7: invalidate(); break;
        case 8: advance(); break;
        case 9: clearSelection(); break;
        case 10: clear(); break;
        case 11: { bool _r = focusNextPrevChild((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: d_func()->_q_updateIndex(); break;
        case 13: d_func()->_q_emitUpdated(); break;
        case 14: d_func()->_q_removeItemLater((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 15: d_func()->_q_updateLater(); break;
        case 16: d_func()->_q_polishItems(); break;
        }
        _id -= 17;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QBrush*>(_v) = backgroundBrush(); break;
        case 1: *reinterpret_cast< QBrush*>(_v) = foregroundBrush(); break;
        case 2: *reinterpret_cast< ItemIndexMethod*>(_v) = itemIndexMethod(); break;
        case 3: *reinterpret_cast< QRectF*>(_v) = sceneRect(); break;
        case 4: *reinterpret_cast< int*>(_v) = bspTreeDepth(); break;
        case 5: *reinterpret_cast< QPalette*>(_v) = palette(); break;
        case 6: *reinterpret_cast< QFont*>(_v) = font(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBackgroundBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 1: setForegroundBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 2: setItemIndexMethod(*reinterpret_cast< ItemIndexMethod*>(_v)); break;
        case 3: setSceneRect(*reinterpret_cast< QRectF*>(_v)); break;
        case 4: setBspTreeDepth(*reinterpret_cast< int*>(_v)); break;
        case 5: setPalette(*reinterpret_cast< QPalette*>(_v)); break;
        case 6: setFont(*reinterpret_cast< QFont*>(_v)); break;
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
void QGraphicsScene::changed(const QList<QRectF> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QGraphicsScene::sceneRectChanged(const QRectF & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QGraphicsScene::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
