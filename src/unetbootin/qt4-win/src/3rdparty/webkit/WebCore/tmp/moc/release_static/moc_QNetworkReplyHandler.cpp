/****************************************************************************
** Meta object code from reading C++ file 'QNetworkReplyHandler.h'
**
** Created: Fri May 30 23:15:47 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../platform/network/qt/QNetworkReplyHandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QNetworkReplyHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WebCore__QNetworkReplyHandler[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x08,
      40,   30,   30,   30, 0x08,
      63,   30,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WebCore__QNetworkReplyHandler[] = {
    "WebCore::QNetworkReplyHandler\0\0finish()\0"
    "sendResponseIfNeeded()\0forwardData()\0"
};

const QMetaObject WebCore::QNetworkReplyHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WebCore__QNetworkReplyHandler,
      qt_meta_data_WebCore__QNetworkReplyHandler, 0 }
};

const QMetaObject *WebCore::QNetworkReplyHandler::metaObject() const
{
    return &staticMetaObject;
}

void *WebCore::QNetworkReplyHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WebCore__QNetworkReplyHandler))
	return static_cast<void*>(const_cast< QNetworkReplyHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int WebCore::QNetworkReplyHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finish(); break;
        case 1: sendResponseIfNeeded(); break;
        case 2: forwardData(); break;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_WebCore__FormDataIODevice[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WebCore__FormDataIODevice[] = {
    "WebCore::FormDataIODevice\0\0slotFinished()\0"
};

const QMetaObject WebCore::FormDataIODevice::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_WebCore__FormDataIODevice,
      qt_meta_data_WebCore__FormDataIODevice, 0 }
};

const QMetaObject *WebCore::FormDataIODevice::metaObject() const
{
    return &staticMetaObject;
}

void *WebCore::FormDataIODevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WebCore__FormDataIODevice))
	return static_cast<void*>(const_cast< FormDataIODevice*>(this));
    return QIODevice::qt_metacast(_clname);
}

int WebCore::FormDataIODevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotFinished(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
