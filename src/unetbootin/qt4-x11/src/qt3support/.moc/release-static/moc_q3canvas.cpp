/****************************************************************************
** Meta object code from reading C++ file 'q3canvas.h'
**
** Created: Thu Mar 5 19:06:11 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../canvas/q3canvas.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3canvas.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3Canvas[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      20,    9,    9,    9, 0x0a,
      30,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Q3Canvas[] = {
    "Q3Canvas\0\0resized()\0advance()\0update()\0"
};

const QMetaObject Q3Canvas::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3Canvas,
      qt_meta_data_Q3Canvas, 0 }
};

const QMetaObject *Q3Canvas::metaObject() const
{
    return &staticMetaObject;
}

void *Q3Canvas::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3Canvas))
        return static_cast<void*>(const_cast< Q3Canvas*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3Canvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resized(); break;
        case 1: advance(); break;
        case 2: update(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Q3Canvas::resized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_Q3CanvasView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Q3CanvasView[] = {
    "Q3CanvasView\0\0updateContentsSize()\0"
};

const QMetaObject Q3CanvasView::staticMetaObject = {
    { &Q3ScrollView::staticMetaObject, qt_meta_stringdata_Q3CanvasView,
      qt_meta_data_Q3CanvasView, 0 }
};

const QMetaObject *Q3CanvasView::metaObject() const
{
    return &staticMetaObject;
}

void *Q3CanvasView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3CanvasView))
        return static_cast<void*>(const_cast< Q3CanvasView*>(this));
    return Q3ScrollView::qt_metacast(_clname);
}

int Q3CanvasView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3ScrollView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateContentsSize(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
