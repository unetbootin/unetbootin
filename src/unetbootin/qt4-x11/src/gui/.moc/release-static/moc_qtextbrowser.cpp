/****************************************************************************
** Meta object code from reading C++ file 'qtextbrowser.h'
**
** Created: Thu Mar 5 18:52:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qtextbrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtextbrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTextBrowser[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   12, // methods
       4,   87, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      38,   13,   13,   13, 0x05,
      61,   13,   13,   13, 0x05,
      78,   13,   13,   13, 0x05,
      98,   13,   13,   13, 0x05,
     116,   13,   13,   13, 0x05,
     137,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     162,  157,   13,   13, 0x0a,
     178,   13,   13,   13, 0x0a,
     189,   13,   13,   13, 0x0a,
     199,   13,   13,   13, 0x0a,
     206,   13,   13,   13, 0x0a,
     215,   13,   13,   13, 0x08,
     237,   13,   13,   13, 0x08,
     264,   13,   13,   13, 0x08,

 // properties: name, type, flags
     295,  290, 0x11095103,
     314,  302, 0x0b095103,
     331,  326, 0x01095103,
     349,  326, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QTextBrowser[] = {
    "QTextBrowser\0\0backwardAvailable(bool)\0"
    "forwardAvailable(bool)\0historyChanged()\0"
    "sourceChanged(QUrl)\0highlighted(QUrl)\0"
    "highlighted(QString)\0anchorClicked(QUrl)\0"
    "name\0setSource(QUrl)\0backward()\0"
    "forward()\0home()\0reload()\0"
    "_q_documentModified()\0_q_activateAnchor(QString)\0"
    "_q_highlightLink(QString)\0QUrl\0source\0"
    "QStringList\0searchPaths\0bool\0"
    "openExternalLinks\0openLinks\0"
};

const QMetaObject QTextBrowser::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_QTextBrowser,
      qt_meta_data_QTextBrowser, 0 }
};

const QMetaObject *QTextBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *QTextBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTextBrowser))
        return static_cast<void*>(const_cast< QTextBrowser*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int QTextBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: backwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: forwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: historyChanged(); break;
        case 3: sourceChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 4: highlighted((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 5: highlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: anchorClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 7: setSource((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 8: backward(); break;
        case 9: forward(); break;
        case 10: home(); break;
        case 11: reload(); break;
        case 12: d_func()->_q_documentModified(); break;
        case 13: d_func()->_q_activateAnchor((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: d_func()->_q_highlightLink((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 15;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QUrl*>(_v) = source(); break;
        case 1: *reinterpret_cast< QStringList*>(_v) = searchPaths(); break;
        case 2: *reinterpret_cast< bool*>(_v) = openExternalLinks(); break;
        case 3: *reinterpret_cast< bool*>(_v) = openLinks(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSource(*reinterpret_cast< QUrl*>(_v)); break;
        case 1: setSearchPaths(*reinterpret_cast< QStringList*>(_v)); break;
        case 2: setOpenExternalLinks(*reinterpret_cast< bool*>(_v)); break;
        case 3: setOpenLinks(*reinterpret_cast< bool*>(_v)); break;
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
void QTextBrowser::backwardAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTextBrowser::forwardAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTextBrowser::historyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QTextBrowser::sourceChanged(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QTextBrowser::highlighted(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QTextBrowser::highlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QTextBrowser::anchorClicked(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
