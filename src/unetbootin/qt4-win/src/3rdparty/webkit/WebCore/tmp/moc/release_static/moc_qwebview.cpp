/****************************************************************************
** Meta object code from reading C++ file 'qwebview.h'
**
** Created: Fri May 30 23:34:24 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../WebKit/qt/Api/qwebview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwebview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWebView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   10, // methods
       6,   80, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      33,   24,    9,    9, 0x05,
      51,    9,    9,    9, 0x05,
      76,   70,    9,    9, 0x05,
     103,   98,    9,    9, 0x05,
     133,  129,    9,    9, 0x05,
     151,    9,    9,    9, 0x05,
     170,    9,    9,    9, 0x05,
     184,  129,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     201,    9,    9,    9, 0x0a,
     208,    9,    9,    9, 0x0a,
     215,    9,    9,    9, 0x0a,
     225,    9,    9,    9, 0x0a,
     242,  234,    9,    9, 0x0a,

 // properties: name, type, flags
      70,  259, 0x0a095001,
     129,  267, 0x11095103,
     278,  272, 0x45095001,
     283,  259, 0x0a095001,
     301,  296, 0x01095001,
     316,  310, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_QWebView[] = {
    "QWebView\0\0loadStarted()\0progress\0"
    "loadProgress(int)\0loadFinished(bool)\0"
    "title\0titleChanged(QString)\0text\0"
    "statusBarMessage(QString)\0url\0"
    "linkClicked(QUrl)\0selectionChanged()\0"
    "iconChanged()\0urlChanged(QUrl)\0stop()\0"
    "back()\0forward()\0reload()\0printer\0"
    "print(QPrinter*)\0QString\0QUrl\0QIcon\0"
    "icon\0selectedText\0bool\0modified\0qreal\0"
    "textSizeMultiplier\0"
};

const QMetaObject QWebView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QWebView,
      qt_meta_data_QWebView, 0 }
};

const QMetaObject *QWebView::metaObject() const
{
    return &staticMetaObject;
}

void *QWebView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWebView))
	return static_cast<void*>(const_cast< QWebView*>(this));
    return QWidget::qt_metacast(_clname);
}

int QWebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadStarted(); break;
        case 1: loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: loadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: statusBarMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: linkClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 6: selectionChanged(); break;
        case 7: iconChanged(); break;
        case 8: urlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 9: stop(); break;
        case 10: back(); break;
        case 11: forward(); break;
        case 12: reload(); break;
        case 13: print((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        }
        _id -= 14;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = title(); break;
        case 1: *reinterpret_cast< QUrl*>(_v) = url(); break;
        case 2: *reinterpret_cast< QIcon*>(_v) = icon(); break;
        case 3: *reinterpret_cast< QString*>(_v) = selectedText(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isModified(); break;
        case 5: *reinterpret_cast< qreal*>(_v) = textSizeMultiplier(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setUrl(*reinterpret_cast< QUrl*>(_v)); break;
        case 5: setTextSizeMultiplier(*reinterpret_cast< qreal*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QWebView::loadStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QWebView::loadProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QWebView::loadFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QWebView::titleChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QWebView::statusBarMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QWebView::linkClicked(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QWebView::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void QWebView::iconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void QWebView::urlChanged(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
