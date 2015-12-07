/****************************************************************************
** Meta object code from reading C++ file 'qprogressdialog.h'
**
** Created: Fri May 30 22:34:01 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qprogressdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qprogressdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QProgressDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   10, // methods
       8,   65, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   16,   16,   16, 0x0a,
      37,   16,   16,   16, 0x0a,
      53,   45,   16,   16, 0x0a,
      77,   69,   16,   16, 0x0a,
     109,   93,   16,   16, 0x0a,
     136,  127,   16,   16, 0x0a,
     150,   16,   16,   16, 0x0a,
     172,   16,   16,   16, 0x0a,
     204,  201,   16,   16, 0x0a,
     228,   16,   16,   16, 0x09,

 // properties: name, type, flags
     245,  240, 0x01095001,
      69,  257, 0x02095103,
      45,  257, 0x02095103,
     261,  257, 0x02095103,
     267,  240, 0x01095103,
     277,  240, 0x01095103,
     287,  257, 0x02095103,
     311,  303, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QProgressDialog[] = {
    "QProgressDialog\0\0canceled()\0cancel()\0"
    "reset()\0maximum\0setMaximum(int)\0minimum\0"
    "setMinimum(int)\0minimum,maximum\0"
    "setRange(int,int)\0progress\0setValue(int)\0"
    "setLabelText(QString)\0"
    "setCancelButtonText(QString)\0ms\0"
    "setMinimumDuration(int)\0forceShow()\0"
    "bool\0wasCanceled\0int\0value\0autoReset\0"
    "autoClose\0minimumDuration\0QString\0"
    "labelText\0"
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
        }
        _id -= 11;
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
