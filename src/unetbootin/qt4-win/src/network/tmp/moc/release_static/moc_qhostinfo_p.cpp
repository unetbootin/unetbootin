/****************************************************************************
** Meta object code from reading C++ file 'qhostinfo_p.h'
**
** Created: Fri May 30 22:20:06 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qhostinfo_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhostinfo_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHostInfoResult[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QHostInfoResult[] = {
    "QHostInfoResult\0\0info\0resultsReady(QHostInfo)\0"
};

const QMetaObject QHostInfoResult::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QHostInfoResult,
      qt_meta_data_QHostInfoResult, 0 }
};

const QMetaObject *QHostInfoResult::metaObject() const
{
    return &staticMetaObject;
}

void *QHostInfoResult::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHostInfoResult))
	return static_cast<void*>(const_cast< QHostInfoResult*>(this));
    return QObject::qt_metacast(_clname);
}

int QHostInfoResult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resultsReady((*reinterpret_cast< const QHostInfo(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QHostInfoResult::resultsReady(const QHostInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_QHostInfoAgent[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QHostInfoAgent[] = {
    "QHostInfoAgent\0\0cleanup()\0"
};

const QMetaObject QHostInfoAgent::staticMetaObject = {
    { &QHostInfoAgentBase::staticMetaObject, qt_meta_stringdata_QHostInfoAgent,
      qt_meta_data_QHostInfoAgent, 0 }
};

const QMetaObject *QHostInfoAgent::metaObject() const
{
    return &staticMetaObject;
}

void *QHostInfoAgent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHostInfoAgent))
	return static_cast<void*>(const_cast< QHostInfoAgent*>(this));
    return QHostInfoAgentBase::qt_metacast(_clname);
}

int QHostInfoAgent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QHostInfoAgentBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cleanup(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
