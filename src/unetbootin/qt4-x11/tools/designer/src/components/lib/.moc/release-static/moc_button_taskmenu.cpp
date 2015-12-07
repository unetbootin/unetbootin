/****************************************************************************
** Meta object code from reading C++ file 'button_taskmenu.h'
**
** Created: Thu Mar 5 20:52:22 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../taskmenu/button_taskmenu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'button_taskmenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ButtonGroupMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      37,   36,   36,   36, 0x08,
      51,   36,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ButtonGroupMenu[] = {
    "qdesigner_internal::ButtonGroupMenu\0"
    "\0selectGroup()\0breakGroup()\0"
};

const QMetaObject qdesigner_internal::ButtonGroupMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ButtonGroupMenu,
      qt_meta_data_qdesigner_internal__ButtonGroupMenu, 0 }
};

const QMetaObject *qdesigner_internal::ButtonGroupMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ButtonGroupMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ButtonGroupMenu))
        return static_cast<void*>(const_cast< ButtonGroupMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::ButtonGroupMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectGroup(); break;
        case 1: breakGroup(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__ButtonGroupTaskMenu[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__ButtonGroupTaskMenu[] = {
    "qdesigner_internal::ButtonGroupTaskMenu\0"
};

const QMetaObject qdesigner_internal::ButtonGroupTaskMenu::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ButtonGroupTaskMenu,
      qt_meta_data_qdesigner_internal__ButtonGroupTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::ButtonGroupTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ButtonGroupTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ButtonGroupTaskMenu))
        return static_cast<void*>(const_cast< ButtonGroupTaskMenu*>(this));
    if (!strcmp(_clname, "QDesignerTaskMenuExtension"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< ButtonGroupTaskMenu*>(this));
    if (!strcmp(_clname, "com.trolltech.Qt.Designer.TaskMenu"))
        return static_cast< QDesignerTaskMenuExtension*>(const_cast< ButtonGroupTaskMenu*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::ButtonGroupTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__ButtonTaskMenu[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      36,   35,   35,   35, 0x08,
      52,   50,   35,   35, 0x08,
      73,   35,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ButtonTaskMenu[] = {
    "qdesigner_internal::ButtonTaskMenu\0\0"
    "createGroup()\0a\0addToGroup(QAction*)\0"
    "removeFromGroup()\0"
};

const QMetaObject qdesigner_internal::ButtonTaskMenu::staticMetaObject = {
    { &QDesignerTaskMenu::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ButtonTaskMenu,
      qt_meta_data_qdesigner_internal__ButtonTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::ButtonTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ButtonTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ButtonTaskMenu))
        return static_cast<void*>(const_cast< ButtonTaskMenu*>(this));
    return QDesignerTaskMenu::qt_metacast(_clname);
}

int qdesigner_internal::ButtonTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerTaskMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: createGroup(); break;
        case 1: addToGroup((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: removeFromGroup(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__CommandLinkButtonTaskMenu[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__CommandLinkButtonTaskMenu[] = {
    "qdesigner_internal::CommandLinkButtonTaskMenu\0"
};

const QMetaObject qdesigner_internal::CommandLinkButtonTaskMenu::staticMetaObject = {
    { &ButtonTaskMenu::staticMetaObject, qt_meta_stringdata_qdesigner_internal__CommandLinkButtonTaskMenu,
      qt_meta_data_qdesigner_internal__CommandLinkButtonTaskMenu, 0 }
};

const QMetaObject *qdesigner_internal::CommandLinkButtonTaskMenu::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::CommandLinkButtonTaskMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__CommandLinkButtonTaskMenu))
        return static_cast<void*>(const_cast< CommandLinkButtonTaskMenu*>(this));
    return ButtonTaskMenu::qt_metacast(_clname);
}

int qdesigner_internal::CommandLinkButtonTaskMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ButtonTaskMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
