/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_server.h'
**
** Created: Thu Mar 5 20:53:35 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdesigner_server.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_server.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerServer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      39,   16,   16,   16, 0x08,
      56,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerServer[] = {
    "QDesignerServer\0\0handleNewConnection()\0"
    "readFromClient()\0socketClosed()\0"
};

const QMetaObject QDesignerServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDesignerServer,
      qt_meta_data_QDesignerServer, 0 }
};

const QMetaObject *QDesignerServer::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerServer))
        return static_cast<void*>(const_cast< QDesignerServer*>(this));
    return QObject::qt_metacast(_clname);
}

int QDesignerServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: handleNewConnection(); break;
        case 1: readFromClient(); break;
        case 2: socketClosed(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QDesignerClient[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerClient[] = {
    "QDesignerClient\0\0readFromSocket()\0"
};

const QMetaObject QDesignerClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QDesignerClient,
      qt_meta_data_QDesignerClient, 0 }
};

const QMetaObject *QDesignerClient::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerClient))
        return static_cast<void*>(const_cast< QDesignerClient*>(this));
    return QObject::qt_metacast(_clname);
}

int QDesignerClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readFromSocket(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
