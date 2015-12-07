/****************************************************************************
** Meta object code from reading C++ file 'qsvgrenderer.h'
**
** Created: Thu Mar 5 19:21:02 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qsvgrenderer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsvgrenderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSvgRenderer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       3,   52, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   35,   30,   13, 0x0a,
      67,   58,   30,   13, 0x0a,
      84,   58,   30,   13, 0x0a,
     110,  108,   13,   13, 0x0a,
     137,  128,   13,   13, 0x0a,
     181,  162,   13,   13, 0x0a,
     226,  214,   13,   13, 0x2a,

 // properties: name, type, flags
     259,  252, 0x14095103,
     271,  267, 0x02095103,
     287,  267, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_QSvgRenderer[] = {
    "QSvgRenderer\0\0repaintNeeded()\0bool\0"
    "filename\0load(QString)\0contents\0"
    "load(QByteArray)\0load(QXmlStreamReader*)\0"
    "p\0render(QPainter*)\0p,bounds\0"
    "render(QPainter*,QRectF)\0p,elementId,bounds\0"
    "render(QPainter*,QString,QRectF)\0"
    "p,elementId\0render(QPainter*,QString)\0"
    "QRectF\0viewBox\0int\0framesPerSecond\0"
    "currentFrame\0"
};

const QMetaObject QSvgRenderer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QSvgRenderer,
      qt_meta_data_QSvgRenderer, 0 }
};

const QMetaObject *QSvgRenderer::metaObject() const
{
    return &staticMetaObject;
}

void *QSvgRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSvgRenderer))
        return static_cast<void*>(const_cast< QSvgRenderer*>(this));
    return QObject::qt_metacast(_clname);
}

int QSvgRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: repaintNeeded(); break;
        case 1: { bool _r = load((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = load((*reinterpret_cast< const QByteArray(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = load((*reinterpret_cast< QXmlStreamReader*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: render((*reinterpret_cast< QPainter*(*)>(_a[1]))); break;
        case 5: render((*reinterpret_cast< QPainter*(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2]))); break;
        case 6: render((*reinterpret_cast< QPainter*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QRectF(*)>(_a[3]))); break;
        case 7: render((*reinterpret_cast< QPainter*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QRectF*>(_v) = viewBoxF(); break;
        case 1: *reinterpret_cast< int*>(_v) = framesPerSecond(); break;
        case 2: *reinterpret_cast< int*>(_v) = currentFrame(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setViewBox(*reinterpret_cast< QRectF*>(_v)); break;
        case 1: setFramesPerSecond(*reinterpret_cast< int*>(_v)); break;
        case 2: setCurrentFrame(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QSvgRenderer::repaintNeeded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
