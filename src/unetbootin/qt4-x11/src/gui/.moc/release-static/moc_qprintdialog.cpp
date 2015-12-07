/****************************************************************************
** Meta object code from reading C++ file 'qprintdialog.h'
**
** Created: Thu Mar 5 18:53:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dialogs/qprintdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qprintdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QUnixPrintWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      41,   17,   17,   17, 0x08,
      63,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QUnixPrintWidget[] = {
    "QUnixPrintWidget\0\0_q_printerChanged(int)\0"
    "_q_btnBrowseClicked()\0_q_btnPropertiesClicked()\0"
};

const QMetaObject QUnixPrintWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QUnixPrintWidget,
      qt_meta_data_QUnixPrintWidget, 0 }
};

const QMetaObject *QUnixPrintWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QUnixPrintWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QUnixPrintWidget))
        return static_cast<void*>(const_cast< QUnixPrintWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QUnixPrintWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d->_q_printerChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: d->_q_btnBrowseClicked(); break;
        case 2: d->_q_btnPropertiesClicked(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QPrintDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       1,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      22,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   13,   13,   13, 0x08,
      76,   13,   13,   13, 0x08,
     104,   13,   13,   13, 0x08,

 // properties: name, type, flags
     140,  121, 0x0009510b,

 // enums: name, flags, count, data

 // enum data: key, value

       0        // eod
};

static const char qt_meta_stringdata_QPrintDialog[] = {
    "QPrintDialog\0\0printer\0accepted(QPrinter*)\0"
    "_q_chbPrintLastFirstToggled(bool)\0"
    "_q_collapseOrExpandDialog()\0"
    "_q_checkFields()\0PrintDialogOptions\0"
    "options\0"
};

const QMetaObject QPrintDialog::staticMetaObject = {
    { &QAbstractPrintDialog::staticMetaObject, qt_meta_stringdata_QPrintDialog,
      qt_meta_data_QPrintDialog, 0 }
};

const QMetaObject *QPrintDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QPrintDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPrintDialog))
        return static_cast<void*>(const_cast< QPrintDialog*>(this));
    return QAbstractPrintDialog::qt_metacast(_clname);
}

int QPrintDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractPrintDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accepted((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        case 1: d_func()->_q_chbPrintLastFirstToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: d_func()->_q_collapseOrExpandDialog(); break;
        case 3: d_func()->_q_checkFields(); break;
        default: ;
        }
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< PrintDialogOptions*>(_v) = options(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOptions(*reinterpret_cast< PrintDialogOptions*>(_v)); break;
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
void QPrintDialog::accepted(QPrinter * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
