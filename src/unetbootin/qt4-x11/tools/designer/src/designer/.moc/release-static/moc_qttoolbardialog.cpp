/****************************************************************************
** Meta object code from reading C++ file 'qttoolbardialog.h'
**
** Created: Thu Mar 5 20:52:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qttoolbardialog/qttoolbardialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qttoolbardialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtToolBarManager[] = {

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

static const char qt_meta_stringdata_QtToolBarManager[] = {
    "QtToolBarManager\0"
};

const QMetaObject QtToolBarManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtToolBarManager,
      qt_meta_data_QtToolBarManager, 0 }
};

const QMetaObject *QtToolBarManager::metaObject() const
{
    return &staticMetaObject;
}

void *QtToolBarManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtToolBarManager))
        return static_cast<void*>(const_cast< QtToolBarManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QtToolBarManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QtToolBarDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      30,   16,   16,   16, 0x08,
      46,   16,   16,   16, 0x08,
      63,   16,   16,   16, 0x08,
      75,   16,   16,   16, 0x08,
      90,   16,   16,   16, 0x08,
     106,   16,   16,   16, 0x08,
     118,   16,   16,   16, 0x08,
     132,   16,   16,   16, 0x08,
     146,   16,   16,   16, 0x08,
     161,   16,   16,   16, 0x08,
     177,   16,   16,   16, 0x08,
     210,   16,   16,   16, 0x08,
     249,   16,   16,   16, 0x08,
     289,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtToolBarDialog[] = {
    "QtToolBarDialog\0\0newClicked()\0"
    "removeClicked()\0defaultClicked()\0"
    "okClicked()\0applyClicked()\0cancelClicked()\0"
    "upClicked()\0downClicked()\0leftClicked()\0"
    "rightClicked()\0renameClicked()\0"
    "toolBarRenamed(QListWidgetItem*)\0"
    "currentActionChanged(QTreeWidgetItem*)\0"
    "currentToolBarChanged(QListWidgetItem*)\0"
    "currentToolBarActionChanged(QListWidgetItem*)\0"
};

const QMetaObject QtToolBarDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QtToolBarDialog,
      qt_meta_data_QtToolBarDialog, 0 }
};

const QMetaObject *QtToolBarDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QtToolBarDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtToolBarDialog))
        return static_cast<void*>(const_cast< QtToolBarDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QtToolBarDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->newClicked(); break;
        case 1: d_func()->removeClicked(); break;
        case 2: d_func()->defaultClicked(); break;
        case 3: d_func()->okClicked(); break;
        case 4: d_func()->applyClicked(); break;
        case 5: d_func()->cancelClicked(); break;
        case 6: d_func()->upClicked(); break;
        case 7: d_func()->downClicked(); break;
        case 8: d_func()->leftClicked(); break;
        case 9: d_func()->rightClicked(); break;
        case 10: d_func()->renameClicked(); break;
        case 11: d_func()->toolBarRenamed((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 12: d_func()->currentActionChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 13: d_func()->currentToolBarChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 14: d_func()->currentToolBarActionChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
