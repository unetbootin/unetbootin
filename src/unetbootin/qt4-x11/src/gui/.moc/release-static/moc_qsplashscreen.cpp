/****************************************************************************
** Meta object code from reading C++ file 'qsplashscreen.h'
**
** Created: Thu Mar 5 18:57:23 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qsplashscreen.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsplashscreen.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSplashScreen[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      23,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      71,   47,   14,   14, 0x0a,
     121,  103,   14,   14, 0x2a,
     146,   15,   14,   14, 0x2a,
     167,   14,   14,   14, 0x0a,
     202,  182,   14,   14, 0x1a,
     244,  230,   14,   14, 0x3a,
     269,  265,   14,   14, 0x3a,
     286,   14,   14,   14, 0x1a,

       0        // eod
};

static const char qt_meta_stringdata_QSplashScreen[] = {
    "QSplashScreen\0\0message\0messageChanged(QString)\0"
    "message,alignment,color\0"
    "showMessage(QString,int,QColor)\0"
    "message,alignment\0showMessage(QString,int)\0"
    "showMessage(QString)\0clearMessage()\0"
    "str,alignment,color\0message(QString,int,QColor)\0"
    "str,alignment\0message(QString,int)\0"
    "str\0message(QString)\0clear()\0"
};

const QMetaObject QSplashScreen::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QSplashScreen,
      qt_meta_data_QSplashScreen, 0 }
};

const QMetaObject *QSplashScreen::metaObject() const
{
    return &staticMetaObject;
}

void *QSplashScreen::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSplashScreen))
        return static_cast<void*>(const_cast< QSplashScreen*>(this));
    return QWidget::qt_metacast(_clname);
}

int QSplashScreen::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: messageChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: showMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QColor(*)>(_a[3]))); break;
        case 2: showMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: showMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: clearMessage(); break;
        case 5: message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QColor(*)>(_a[3]))); break;
        case 6: message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: clear(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QSplashScreen::messageChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
