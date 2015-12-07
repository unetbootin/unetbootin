/****************************************************************************
** Meta object code from reading C++ file 'qapplication.h'
**
** Created: Thu Mar 5 18:43:18 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qapplication.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QApplication[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
      11,   57, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      41,   33,   13,   13, 0x05,
      73,   13,   13,   13, 0x05,
     110,   95,   13,   13, 0x05,
     146,   95,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     187,  181,   13,   13, 0x0a,
     210,   13,   13,   13, 0x0a,
     228,   13,   13,   13, 0x0a,
     238,   13,   13,   13, 0x08,

 // properties: name, type, flags
     276,  256, 0x0009510b,
     298,  292, 0x45095103,
     313,  309, 0x02095103,
     329,  309, 0x02095103,
     349,  309, 0x02095103,
     371,  309, 0x02095103,
     394,  388, 0x15095103,
     406,  309, 0x02095103,
     420,  309, 0x02095103,
     443,  438, 0x01095103,
     474,  466, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QApplication[] = {
    "QApplication\0\0lastWindowClosed()\0"
    "old,now\0focusChanged(QWidget*,QWidget*)\0"
    "fontDatabaseChanged()\0sessionManager\0"
    "commitDataRequest(QSessionManager&)\0"
    "saveStateRequest(QSessionManager&)\0"
    "sheet\0setStyleSheet(QString)\0"
    "closeAllWindows()\0aboutQt()\0"
    "_q_alertTimeOut()\0Qt::LayoutDirection\0"
    "layoutDirection\0QIcon\0windowIcon\0int\0"
    "cursorFlashTime\0doubleClickInterval\0"
    "keyboardInputInterval\0wheelScrollLines\0"
    "QSize\0globalStrut\0startDragTime\0"
    "startDragDistance\0bool\0quitOnLastWindowClosed\0"
    "QString\0styleSheet\0"
};

const QMetaObject QApplication::staticMetaObject = {
    { &QCoreApplication::staticMetaObject, qt_meta_stringdata_QApplication,
      qt_meta_data_QApplication, 0 }
};

const QMetaObject *QApplication::metaObject() const
{
    return &staticMetaObject;
}

void *QApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QApplication))
        return static_cast<void*>(const_cast< QApplication*>(this));
    return QCoreApplication::qt_metacast(_clname);
}

int QApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCoreApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: lastWindowClosed(); break;
        case 1: focusChanged((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2]))); break;
        case 2: fontDatabaseChanged(); break;
        case 3: commitDataRequest((*reinterpret_cast< QSessionManager(*)>(_a[1]))); break;
        case 4: saveStateRequest((*reinterpret_cast< QSessionManager(*)>(_a[1]))); break;
        case 5: setStyleSheet((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: closeAllWindows(); break;
        case 7: aboutQt(); break;
        case 8: d_func()->_q_alertTimeOut(); break;
        default: ;
        }
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::LayoutDirection*>(_v) = layoutDirection(); break;
        case 1: *reinterpret_cast< QIcon*>(_v) = windowIcon(); break;
        case 2: *reinterpret_cast< int*>(_v) = cursorFlashTime(); break;
        case 3: *reinterpret_cast< int*>(_v) = doubleClickInterval(); break;
        case 4: *reinterpret_cast< int*>(_v) = keyboardInputInterval(); break;
        case 5: *reinterpret_cast< int*>(_v) = wheelScrollLines(); break;
        case 6: *reinterpret_cast< QSize*>(_v) = globalStrut(); break;
        case 7: *reinterpret_cast< int*>(_v) = startDragTime(); break;
        case 8: *reinterpret_cast< int*>(_v) = startDragDistance(); break;
        case 9: *reinterpret_cast< bool*>(_v) = quitOnLastWindowClosed(); break;
        case 10: *reinterpret_cast< QString*>(_v) = styleSheet(); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setLayoutDirection(*reinterpret_cast< Qt::LayoutDirection*>(_v)); break;
        case 1: setWindowIcon(*reinterpret_cast< QIcon*>(_v)); break;
        case 2: setCursorFlashTime(*reinterpret_cast< int*>(_v)); break;
        case 3: setDoubleClickInterval(*reinterpret_cast< int*>(_v)); break;
        case 4: setKeyboardInputInterval(*reinterpret_cast< int*>(_v)); break;
        case 5: setWheelScrollLines(*reinterpret_cast< int*>(_v)); break;
        case 6: setGlobalStrut(*reinterpret_cast< QSize*>(_v)); break;
        case 7: setStartDragTime(*reinterpret_cast< int*>(_v)); break;
        case 8: setStartDragDistance(*reinterpret_cast< int*>(_v)); break;
        case 9: setQuitOnLastWindowClosed(*reinterpret_cast< bool*>(_v)); break;
        case 10: setStyleSheet(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QApplication::lastWindowClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QApplication::focusChanged(QWidget * _t1, QWidget * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QApplication::fontDatabaseChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QApplication::commitDataRequest(QSessionManager & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QApplication::saveStateRequest(QSessionManager & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
