/****************************************************************************
** Meta object code from reading C++ file 'qcolordialog.h'
**
** Created: Fri May 30 22:29:45 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qcolordialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcolordialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QColorDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      35,   29,   13,   13, 0x08,
      62,   58,   13,   13, 0x08,
      89,   87,   13,   13, 0x08,
     111,   87,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QColorDialog[] = {
    "QColorDialog\0\0_q_addCustom()\0h,s,v\0"
    "_q_newHsv(int,int,int)\0rgb\0"
    "_q_newColorTypedIn(QRgb)\0,\0"
    "_q_newCustom(int,int)\0_q_newStandard(int,int)\0"
};

const QMetaObject QColorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QColorDialog,
      qt_meta_data_QColorDialog, 0 }
};

const QMetaObject *QColorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QColorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QColorDialog))
	return static_cast<void*>(const_cast< QColorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QColorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_addCustom(); break;
        case 1: d_func()->_q_newHsv((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: d_func()->_q_newColorTypedIn((*reinterpret_cast< QRgb(*)>(_a[1]))); break;
        case 3: d_func()->_q_newCustom((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: d_func()->_q_newStandard((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
