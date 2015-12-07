/****************************************************************************
** Meta object code from reading C++ file 'qwebframe.h'
**
** Created: Thu Mar 5 20:23:48 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../WebKit/qt/Api/qwebframe.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwebframe.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWebFrame[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       7,   52, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      43,   10,   10,   10, 0x05,
      67,   61,   10,   10, 0x05,
      93,   89,   10,   10, 0x05,
     110,   10,   10,   10, 0x05,
     135,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     171,  158,  149,   10, 0x0a,
     207,  199,   10,   10, 0x0a,

 // properties: name, type, flags
     230,  224, 0x06094103,
     249,  224, 0x06095103,
      61,  260, 0x0a095001,
      89,  268, 0x11095103,
     279,  273, 0x45095001,
     290,  284, 0x15095001,
     310,  303, 0x19095103,

       0        // eod
};

static const char qt_meta_stringdata_QWebFrame[] = {
    "QWebFrame\0\0javaScriptWindowObjectCleared()\0"
    "provisionalLoad()\0title\0titleChanged(QString)\0"
    "url\0urlChanged(QUrl)\0initialLayoutCompleted()\0"
    "iconChanged()\0QVariant\0scriptSource\0"
    "evaluateJavaScript(QString)\0printer\0"
    "print(QPrinter*)\0qreal\0textSizeMultiplier\0"
    "zoomFactor\0QString\0QUrl\0QIcon\0icon\0"
    "QSize\0contentsSize\0QPoint\0scrollPosition\0"
};

const QMetaObject QWebFrame::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWebFrame,
      qt_meta_data_QWebFrame, 0 }
};

const QMetaObject *QWebFrame::metaObject() const
{
    return &staticMetaObject;
}

void *QWebFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWebFrame))
        return static_cast<void*>(const_cast< QWebFrame*>(this));
    return QObject::qt_metacast(_clname);
}

int QWebFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: javaScriptWindowObjectCleared(); break;
        case 1: provisionalLoad(); break;
        case 2: titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: urlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 4: initialLayoutCompleted(); break;
        case 5: iconChanged(); break;
        case 6: { QVariant _r = evaluateJavaScript((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 7: print((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qreal*>(_v) = textSizeMultiplier(); break;
        case 1: *reinterpret_cast< qreal*>(_v) = zoomFactor(); break;
        case 2: *reinterpret_cast< QString*>(_v) = title(); break;
        case 3: *reinterpret_cast< QUrl*>(_v) = url(); break;
        case 4: *reinterpret_cast< QIcon*>(_v) = icon(); break;
        case 5: *reinterpret_cast< QSize*>(_v) = contentsSize(); break;
        case 6: *reinterpret_cast< QPoint*>(_v) = scrollPosition(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTextSizeMultiplier(*reinterpret_cast< qreal*>(_v)); break;
        case 1: setZoomFactor(*reinterpret_cast< qreal*>(_v)); break;
        case 3: setUrl(*reinterpret_cast< QUrl*>(_v)); break;
        case 6: setScrollPosition(*reinterpret_cast< QPoint*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QWebFrame::javaScriptWindowObjectCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QWebFrame::provisionalLoad()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QWebFrame::titleChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QWebFrame::urlChanged(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QWebFrame::initialLayoutCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QWebFrame::iconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
