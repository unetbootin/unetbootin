/****************************************************************************
** Meta object code from reading C++ file 'qprogressdialog.h'
**
** Created: Thu Mar 5 18:53:56 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dialogs/qprogressdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qprogressdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QProgressDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       8,   72, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   16,   16,   16, 0x0a,
      37,   16,   16,   16, 0x0a,
      53,   45,   16,   16, 0x0a,
      77,   69,   16,   16, 0x0a,
     109,   93,   16,   16, 0x0a,
     136,  127,   16,   16, 0x0a,
     155,  150,   16,   16, 0x0a,
     177,  150,   16,   16, 0x0a,
     209,  206,   16,   16, 0x0a,
     233,   16,   16,   16, 0x09,
     245,   16,   16,   16, 0x08,

 // properties: name, type, flags
     273,  268, 0x01095001,
      69,  285, 0x02095103,
      45,  285, 0x02095103,
     289,  285, 0x02095103,
     295,  268, 0x01095103,
     305,  268, 0x01095103,
     315,  285, 0x02095103,
     339,  331, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QProgressDialog[] = {
    "QProgressDialog\0\0canceled()\0cancel()\0"
    "reset()\0maximum\0setMaximum(int)\0minimum\0"
    "setMinimum(int)\0minimum,maximum\0"
    "setRange(int,int)\0progress\0setValue(int)\0"
    "text\0setLabelText(QString)\0"
    "setCancelButtonText(QString)\0ms\0"
    "setMinimumDuration(int)\0forceShow()\0"
    "_q_disconnectOnClose()\0bool\0wasCanceled\0"
    "int\0value\0autoReset\0autoClose\0"
    "minimumDuration\0QString\0labelText\0"
};

const QMetaObject QProgressDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QProgressDialog,
      qt_meta_data_QProgressDialog, 0 }
};

const QMetaObject *QProgressDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QProgressDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QProgressDialog))
        return static_cast<void*>(const_cast< QProgressDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QProgressDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: canceled(); break;
        case 1: cancel(); break;
        case 2: reset(); break;
        case 3: setMaximum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setMinimum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setRange((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setLabelText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: setCancelButtonText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: setMinimumDuration((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: forceShow(); break;
        case 11: d_func()->_q_disconnectOnClose(); break;
        default: ;
        }
        _id -= 12;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = wasCanceled(); break;
        case 1: *reinterpret_cast< int*>(_v) = minimum(); break;
        case 2: *reinterpret_cast< int*>(_v) = maximum(); break;
        case 3: *reinterpret_cast< int*>(_v) = value(); break;
        case 4: *reinterpret_cast< bool*>(_v) = autoReset(); break;
        case 5: *reinterpret_cast< bool*>(_v) = autoClose(); break;
        case 6: *reinterpret_cast< int*>(_v) = minimumDuration(); break;
        case 7: *reinterpret_cast< QString*>(_v) = labelText(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setMinimum(*reinterpret_cast< int*>(_v)); break;
        case 2: setMaximum(*reinterpret_cast< int*>(_v)); break;
        case 3: setValue(*reinterpret_cast< int*>(_v)); break;
        case 4: setAutoReset(*reinterpret_cast< bool*>(_v)); break;
        case 5: setAutoClose(*reinterpret_cast< bool*>(_v)); break;
        case 6: setMinimumDuration(*reinterpret_cast< int*>(_v)); break;
        case 7: setLabelText(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QProgressDialog::canceled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
