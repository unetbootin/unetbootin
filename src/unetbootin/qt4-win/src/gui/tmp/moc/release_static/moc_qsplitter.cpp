/****************************************************************************
** Meta object code from reading C++ file 'qsplitter.h'
**
** Created: Fri May 30 22:33:50 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qsplitter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsplitter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSplitter[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       4,   15, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      21,   11,   10,   10, 0x05,

 // properties: name, type, flags
      60,   44, 0x0009510b,
      77,   72, 0x01095103,
      94,   90, 0x02095103,
     106,   72, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QSplitter[] = {
    "QSplitter\0\0pos,index\0splitterMoved(int,int)\0"
    "Qt::Orientation\0orientation\0bool\0"
    "opaqueResize\0int\0handleWidth\0"
    "childrenCollapsible\0"
};

const QMetaObject QSplitter::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_QSplitter,
      qt_meta_data_QSplitter, 0 }
};

const QMetaObject *QSplitter::metaObject() const
{
    return &staticMetaObject;
}

void *QSplitter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSplitter))
	return static_cast<void*>(const_cast< QSplitter*>(this));
    return QFrame::qt_metacast(_clname);
}

int QSplitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: splitterMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 1: *reinterpret_cast< bool*>(_v) = opaqueResize(); break;
        case 2: *reinterpret_cast< int*>(_v) = handleWidth(); break;
        case 3: *reinterpret_cast< bool*>(_v) = childrenCollapsible(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 1: setOpaqueResize(*reinterpret_cast< bool*>(_v)); break;
        case 2: setHandleWidth(*reinterpret_cast< int*>(_v)); break;
        case 3: setChildrenCollapsible(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QSplitter::splitterMoved(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_QSplitterHandle[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QSplitterHandle[] = {
    "QSplitterHandle\0"
};

const QMetaObject QSplitterHandle::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QSplitterHandle,
      qt_meta_data_QSplitterHandle, 0 }
};

const QMetaObject *QSplitterHandle::metaObject() const
{
    return &staticMetaObject;
}

void *QSplitterHandle::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSplitterHandle))
	return static_cast<void*>(const_cast< QSplitterHandle*>(this));
    return QWidget::qt_metacast(_clname);
}

int QSplitterHandle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
