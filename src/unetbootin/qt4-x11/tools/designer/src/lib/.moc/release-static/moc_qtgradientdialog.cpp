/****************************************************************************
** Meta object code from reading C++ file 'qtgradientdialog.h'
**
** Created: Thu Mar 5 20:43:09 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradientdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradientdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       4,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      45,   18,   17,   17, 0x08,

 // properties: name, type, flags
      88,   78, 0x0009510b,
     102,   97, 0x01095103,
     122,   97, 0x01095103,
     137,   97, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientDialog[] = {
    "QtGradientDialog\0\0details,extensionWidthHint\0"
    "slotAboutToShowDetails(bool,int)\0"
    "QGradient\0gradient\0bool\0backgroundCheckered\0"
    "detailsVisible\0detailsButtonVisible\0"
};

const QMetaObject QtGradientDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QtGradientDialog,
      qt_meta_data_QtGradientDialog, 0 }
};

const QMetaObject *QtGradientDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientDialog))
        return static_cast<void*>(const_cast< QtGradientDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QtGradientDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotAboutToShowDetails((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QGradient*>(_v) = gradient(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isBackgroundCheckered(); break;
        case 2: *reinterpret_cast< bool*>(_v) = detailsVisible(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isDetailsButtonVisible(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setGradient(*reinterpret_cast< QGradient*>(_v)); break;
        case 1: setBackgroundCheckered(*reinterpret_cast< bool*>(_v)); break;
        case 2: setDetailsVisible(*reinterpret_cast< bool*>(_v)); break;
        case 3: setDetailsButtonVisible(*reinterpret_cast< bool*>(_v)); break;
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
QT_END_MOC_NAMESPACE
