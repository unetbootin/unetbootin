/****************************************************************************
** Meta object code from reading C++ file 'qstatusbar.h'
**
** Created: Fri May 30 22:33:52 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qstatusbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qstatusbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QStatusBar[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       1,   45, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      54,   41,   11,   11, 0x0a,
      79,   12,   11,   11, 0x2a,
     100,   11,   11,   11, 0x0a,
     115,   41,   11,   11, 0x1a,
     136,   12,   11,   11, 0x3a,
     153,   11,   11,   11, 0x1a,

 // properties: name, type, flags
     166,  161, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QStatusBar[] = {
    "QStatusBar\0\0text\0messageChanged(QString)\0"
    "text,timeout\0showMessage(QString,int)\0"
    "showMessage(QString)\0clearMessage()\0"
    "message(QString,int)\0message(QString)\0"
    "clear()\0bool\0sizeGripEnabled\0"
};

const QMetaObject QStatusBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QStatusBar,
      qt_meta_data_QStatusBar, 0 }
};

const QMetaObject *QStatusBar::metaObject() const
{
    return &staticMetaObject;
}

void *QStatusBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStatusBar))
	return static_cast<void*>(const_cast< QStatusBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int QStatusBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: messageChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: showMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: showMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: clearMessage(); break;
        case 4: message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: clear(); break;
        }
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isSizeGripEnabled(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSizeGripEnabled(*reinterpret_cast< bool*>(_v)); break;
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
void QStatusBar::messageChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
