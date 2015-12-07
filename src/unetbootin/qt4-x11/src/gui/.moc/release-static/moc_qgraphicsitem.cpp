/****************************************************************************
** Meta object code from reading C++ file 'qgraphicsitem.h'
**
** Created: Thu Mar 5 18:55:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../graphicsview/qgraphicsitem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgraphicsitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGraphicsTextItem[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      42,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   18,   18,   18, 0x08,
      93,   18,   18,   18, 0x08,
     111,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QGraphicsTextItem[] = {
    "QGraphicsTextItem\0\0linkActivated(QString)\0"
    "linkHovered(QString)\0_q_updateBoundingRect(QSizeF)\0"
    "_q_update(QRectF)\0_q_ensureVisible(QRectF)\0"
};

const QMetaObject QGraphicsTextItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QGraphicsTextItem,
      qt_meta_data_QGraphicsTextItem, 0 }
};

const QMetaObject *QGraphicsTextItem::metaObject() const
{
    return &staticMetaObject;
}

void *QGraphicsTextItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGraphicsTextItem))
        return static_cast<void*>(const_cast< QGraphicsTextItem*>(this));
    if (!strcmp(_clname, "QGraphicsItem"))
        return static_cast< QGraphicsItem*>(const_cast< QGraphicsTextItem*>(this));
    return QObject::qt_metacast(_clname);
}

int QGraphicsTextItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: linkActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: linkHovered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: dd->_q_updateBoundingRect((*reinterpret_cast< const QSizeF(*)>(_a[1]))); break;
        case 3: dd->_q_update((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 4: dd->_q_ensureVisible((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QGraphicsTextItem::linkActivated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QGraphicsTextItem::linkHovered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
