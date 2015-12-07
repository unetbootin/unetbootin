/****************************************************************************
** Meta object code from reading C++ file 'q3textbrowser.h'
**
** Created: Fri May 30 22:41:38 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../text/q3textbrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3textbrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3TextBrowser[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       1,   75, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      39,   14,   14,   14, 0x05,
      62,   14,   14,   14, 0x05,
      85,   14,   14,   14, 0x05,
     106,   14,   14,   14, 0x05,
     129,  127,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     165,  160,   14,   14, 0x0a,
     184,   14,   14,   14, 0x0a,
     195,   14,   14,   14, 0x0a,
     205,   14,   14,   14, 0x0a,
     212,   14,   14,   14, 0x0a,
     225,  221,   14,   14, 0x0a,
     254,  242,   14,   14, 0x0a,

 // properties: name, type, flags
     287,  279, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_Q3TextBrowser[] = {
    "Q3TextBrowser\0\0backwardAvailable(bool)\0"
    "forwardAvailable(bool)\0sourceChanged(QString)\0"
    "highlighted(QString)\0linkClicked(QString)\0"
    ",\0anchorClicked(QString,QString)\0name\0"
    "setSource(QString)\0backward()\0forward()\0"
    "home()\0reload()\0txt\0setText(QString)\0"
    "txt,context\0setText(QString,QString)\0"
    "QString\0source\0"
};

const QMetaObject Q3TextBrowser::staticMetaObject = {
    { &Q3TextEdit::staticMetaObject, qt_meta_stringdata_Q3TextBrowser,
      qt_meta_data_Q3TextBrowser, 0 }
};

const QMetaObject *Q3TextBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *Q3TextBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3TextBrowser))
	return static_cast<void*>(const_cast< Q3TextBrowser*>(this));
    return Q3TextEdit::qt_metacast(_clname);
}

int Q3TextBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3TextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: backwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: forwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: sourceChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: highlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: linkClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: anchorClicked((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: setSource((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: backward(); break;
        case 8: forward(); break;
        case 9: home(); break;
        case 10: reload(); break;
        case 11: setText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: setText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        }
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = source(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSource(*reinterpret_cast< QString*>(_v)); break;
        }
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
void Q3TextBrowser::backwardAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3TextBrowser::forwardAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3TextBrowser::sourceChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3TextBrowser::highlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3TextBrowser::linkClicked(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Q3TextBrowser::anchorClicked(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
