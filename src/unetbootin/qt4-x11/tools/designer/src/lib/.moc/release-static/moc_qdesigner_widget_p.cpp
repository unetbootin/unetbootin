/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_widget_p.h'
**
** Created: Thu Mar 5 20:46:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_widget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_widget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QDesignerWidget[] = {
    "QDesignerWidget\0"
};

const QMetaObject QDesignerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDesignerWidget,
      qt_meta_data_QDesignerWidget, 0 }
};

const QMetaObject *QDesignerWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerWidget))
        return static_cast<void*>(const_cast< QDesignerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDesignerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QDesignerDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_QDesignerDialog[] = {
    "QDesignerDialog\0"
};

const QMetaObject QDesignerDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QDesignerDialog,
      qt_meta_data_QDesignerDialog, 0 }
};

const QMetaObject *QDesignerDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerDialog))
        return static_cast<void*>(const_cast< QDesignerDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QDesignerDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Line[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      21,    5, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_Line[] = {
    "Line\0Qt::Orientation\0orientation\0"
};

const QMetaObject Line::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Line,
      qt_meta_data_Line, 0 }
};

const QMetaObject *Line::metaObject() const
{
    return &staticMetaObject;
}

void *Line::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Line))
        return static_cast<void*>(const_cast< Line*>(this));
    return QFrame::qt_metacast(_clname);
}

int Line::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
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
QT_END_MOC_NAMESPACE
