/****************************************************************************
** Meta object code from reading C++ file 'FrameLoaderClientQt.h'
**
** Created: Fri May 30 23:17:49 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../WebKit/qt/WebCoreSupport/FrameLoaderClientQt.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FrameLoaderClientQt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WebCore__FrameLoaderClientQt[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x05,
      57,   29,   29,   29, 0x05,
      73,   71,   29,   29, 0x05,
      91,   29,   29,   29, 0x05,
     116,  110,   29,   29, 0x05,

 // slots: signature, parameters, type, tag, flags
     138,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WebCore__FrameLoaderClientQt[] = {
    "WebCore::FrameLoaderClientQt\0\0"
    "sigCallPolicyFunction(int)\0loadStarted()\0"
    "d\0loadProgress(int)\0loadFinished(bool)\0"
    "title\0titleChanged(QString)\0"
    "slotCallPolicyFunction(int)\0"
};

const QMetaObject WebCore::FrameLoaderClientQt::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WebCore__FrameLoaderClientQt,
      qt_meta_data_WebCore__FrameLoaderClientQt, 0 }
};

const QMetaObject *WebCore::FrameLoaderClientQt::metaObject() const
{
    return &staticMetaObject;
}

void *WebCore::FrameLoaderClientQt::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WebCore__FrameLoaderClientQt))
	return static_cast<void*>(const_cast< FrameLoaderClientQt*>(this));
    if (!strcmp(_clname, "FrameLoaderClient"))
	return static_cast< FrameLoaderClient*>(const_cast< FrameLoaderClientQt*>(this));
    return QObject::qt_metacast(_clname);
}

int WebCore::FrameLoaderClientQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sigCallPolicyFunction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: loadStarted(); break;
        case 2: loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: loadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: slotCallPolicyFunction((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void WebCore::FrameLoaderClientQt::sigCallPolicyFunction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WebCore::FrameLoaderClientQt::loadStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void WebCore::FrameLoaderClientQt::loadProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WebCore::FrameLoaderClientQt::loadFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WebCore::FrameLoaderClientQt::titleChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
