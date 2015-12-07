/****************************************************************************
** Meta object code from reading C++ file 'appfontdialog.h'
**
** Created: Thu Mar 5 20:53:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../appfontdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'appfontdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AppFontWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      26,   14,   14,   14, 0x08,
      44,   14,   14,   14, 0x08,
      80,   60,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AppFontWidget[] = {
    "AppFontWidget\0\0addFiles()\0slotRemoveFiles()\0"
    "slotRemoveAll()\0selected,deselected\0"
    "selectionChanged(QItemSelection,QItemSelection)\0"
};

const QMetaObject AppFontWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_AppFontWidget,
      qt_meta_data_AppFontWidget, 0 }
};

const QMetaObject *AppFontWidget::metaObject() const
{
    return &staticMetaObject;
}

void *AppFontWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppFontWidget))
        return static_cast<void*>(const_cast< AppFontWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int AppFontWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addFiles(); break;
        case 1: slotRemoveFiles(); break;
        case 2: slotRemoveAll(); break;
        case 3: selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_AppFontDialog[] = {

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

static const char qt_meta_stringdata_AppFontDialog[] = {
    "AppFontDialog\0"
};

const QMetaObject AppFontDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AppFontDialog,
      qt_meta_data_AppFontDialog, 0 }
};

const QMetaObject *AppFontDialog::metaObject() const
{
    return &staticMetaObject;
}

void *AppFontDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AppFontDialog))
        return static_cast<void*>(const_cast< AppFontDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int AppFontDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
