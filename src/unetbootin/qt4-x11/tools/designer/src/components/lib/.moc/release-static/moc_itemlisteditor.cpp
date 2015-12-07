/****************************************************************************
** Meta object code from reading C++ file 'itemlisteditor.h'
**
** Created: Thu Mar 5 20:52:32 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../taskmenu/itemlisteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'itemlisteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__AbstractItemEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      49,   40,   39,   39, 0x08,
      78,   40,   39,   39, 0x08,
     105,   39,   39,   39, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__AbstractItemEditor[] = {
    "qdesigner_internal::AbstractItemEditor\0"
    "\0property\0propertyChanged(QtProperty*)\0"
    "resetProperty(QtProperty*)\0cacheReloaded()\0"
};

const QMetaObject qdesigner_internal::AbstractItemEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__AbstractItemEditor,
      qt_meta_data_qdesigner_internal__AbstractItemEditor, 0 }
};

const QMetaObject *qdesigner_internal::AbstractItemEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::AbstractItemEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__AbstractItemEditor))
        return static_cast<void*>(const_cast< AbstractItemEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::AbstractItemEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: propertyChanged((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 1: resetProperty((*reinterpret_cast< QtProperty*(*)>(_a[1]))); break;
        case 2: cacheReloaded(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__ItemListEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      40,   36,   35,   35, 0x05,
      69,   58,   35,   35, 0x05,
      99,   36,   35,   35, 0x05,
     117,   36,   35,   35, 0x05,
     134,   36,   35,   35, 0x05,
     151,   36,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
     170,   35,   35,   35, 0x08,
     201,   35,   35,   35, 0x08,
     235,   35,   35,   35, 0x08,
     269,   35,   35,   35, 0x08,
     305,   35,   35,   35, 0x08,
     344,  339,   35,   35, 0x08,
     388,   35,   35,   35, 0x08,
     412,   35,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ItemListEditor[] = {
    "qdesigner_internal::ItemListEditor\0\0"
    "idx\0indexChanged(int)\0idx,role,v\0"
    "itemChanged(int,int,QVariant)\0"
    "itemInserted(int)\0itemDeleted(int)\0"
    "itemMovedUp(int)\0itemMovedDown(int)\0"
    "on_newListItemButton_clicked()\0"
    "on_deleteListItemButton_clicked()\0"
    "on_moveListItemUpButton_clicked()\0"
    "on_moveListItemDownButton_clicked()\0"
    "on_listWidget_currentRowChanged()\0"
    "item\0on_listWidget_itemChanged(QListWidgetItem*)\0"
    "togglePropertyBrowser()\0cacheReloaded()\0"
};

const QMetaObject qdesigner_internal::ItemListEditor::staticMetaObject = {
    { &AbstractItemEditor::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ItemListEditor,
      qt_meta_data_qdesigner_internal__ItemListEditor, 0 }
};

const QMetaObject *qdesigner_internal::ItemListEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ItemListEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ItemListEditor))
        return static_cast<void*>(const_cast< ItemListEditor*>(this));
    return AbstractItemEditor::qt_metacast(_clname);
}

int qdesigner_internal::ItemListEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractItemEditor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: itemChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3]))); break;
        case 2: itemInserted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: itemDeleted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: itemMovedUp((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: itemMovedDown((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: on_newListItemButton_clicked(); break;
        case 7: on_deleteListItemButton_clicked(); break;
        case 8: on_moveListItemUpButton_clicked(); break;
        case 9: on_moveListItemDownButton_clicked(); break;
        case 10: on_listWidget_currentRowChanged(); break;
        case 11: on_listWidget_itemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 12: togglePropertyBrowser(); break;
        case 13: cacheReloaded(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ItemListEditor::indexChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::ItemListEditor::itemChanged(int _t1, int _t2, const QVariant & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::ItemListEditor::itemInserted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qdesigner_internal::ItemListEditor::itemDeleted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void qdesigner_internal::ItemListEditor::itemMovedUp(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void qdesigner_internal::ItemListEditor::itemMovedDown(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
