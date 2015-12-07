/****************************************************************************
** Meta object code from reading C++ file 'qassistantclient.h'
**
** Created: Thu Mar 5 20:37:00 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qassistantclient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qassistantclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAssistantClient[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       1,   72, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      36,   17,   17,   17, 0x05,
      58,   54,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      73,   17,   17,   17, 0x0a,
      89,   17,   17,   17, 0x0a,
     111,  106,   17,   17, 0x0a,
     129,   17,   17,   17, 0x08,
     147,   17,   17,   17, 0x08,
     172,   17,   17,   17, 0x08,
     187,  183,   17,   17, 0x08,
     221,   17,   17,   17, 0x08,
     235,   17,   17,   17, 0x08,

 // properties: name, type, flags
     255,  250, 0x01095001,

       0        // eod
};

static const char qt_meta_stringdata_QAssistantClient[] = {
    "QAssistantClient\0\0assistantOpened()\0"
    "assistantClosed()\0msg\0error(QString)\0"
    "openAssistant()\0closeAssistant()\0page\0"
    "showPage(QString)\0socketConnected()\0"
    "socketConnectionClosed()\0readPort()\0"
    "err\0procError(QProcess::ProcessError)\0"
    "socketError()\0readStdError()\0bool\0"
    "open\0"
};

const QMetaObject QAssistantClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAssistantClient,
      qt_meta_data_QAssistantClient, 0 }
};

const QMetaObject *QAssistantClient::metaObject() const
{
    return &staticMetaObject;
}

void *QAssistantClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAssistantClient))
        return static_cast<void*>(const_cast< QAssistantClient*>(this));
    return QObject::qt_metacast(_clname);
}

int QAssistantClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: assistantOpened(); break;
        case 1: assistantClosed(); break;
        case 2: error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: openAssistant(); break;
        case 4: closeAssistant(); break;
        case 5: showPage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: socketConnected(); break;
        case 7: socketConnectionClosed(); break;
        case 8: readPort(); break;
        case 9: procError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 10: socketError(); break;
        case 11: readStdError(); break;
        default: ;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isOpen(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QAssistantClient::assistantOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QAssistantClient::assistantClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QAssistantClient::error(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
