/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Mar 5 20:53:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindowBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_MainWindowBase[] = {
    "MainWindowBase\0\0e\0closeEventReceived(QCloseEvent*)\0"
};

const QMetaObject MainWindowBase::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindowBase,
      qt_meta_data_MainWindowBase, 0 }
};

const QMetaObject *MainWindowBase::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindowBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindowBase))
        return static_cast<void*>(const_cast< MainWindowBase*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindowBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: closeEventReceived((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void MainWindowBase::closeEventReceived(QCloseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_DockedMdiArea[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DockedMdiArea[] = {
    "DockedMdiArea\0\0fileDropped(QString)\0"
};

const QMetaObject DockedMdiArea::staticMetaObject = {
    { &QMdiArea::staticMetaObject, qt_meta_stringdata_DockedMdiArea,
      qt_meta_data_DockedMdiArea, 0 }
};

const QMetaObject *DockedMdiArea::metaObject() const
{
    return &staticMetaObject;
}

void *DockedMdiArea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DockedMdiArea))
        return static_cast<void*>(const_cast< DockedMdiArea*>(this));
    return QMdiArea::qt_metacast(_clname);
}

int DockedMdiArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMdiArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileDropped((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DockedMdiArea::fileDropped(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ToolBarManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      36,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ToolBarManager[] = {
    "ToolBarManager\0\0configureToolBars()\0"
    "updateToolBarMenu()\0"
};

const QMetaObject ToolBarManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ToolBarManager,
      qt_meta_data_ToolBarManager, 0 }
};

const QMetaObject *ToolBarManager::metaObject() const
{
    return &staticMetaObject;
}

void *ToolBarManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToolBarManager))
        return static_cast<void*>(const_cast< ToolBarManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ToolBarManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: configureToolBars(); break;
        case 1: updateToolBarMenu(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_DockedMainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      39,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      81,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DockedMainWindow[] = {
    "DockedMainWindow\0\0fileDropped(QString)\0"
    "formWindowActivated(QDesignerFormWindow*)\0"
    "slotSubWindowActivated(QMdiSubWindow*)\0"
};

const QMetaObject DockedMainWindow::staticMetaObject = {
    { &MainWindowBase::staticMetaObject, qt_meta_stringdata_DockedMainWindow,
      qt_meta_data_DockedMainWindow, 0 }
};

const QMetaObject *DockedMainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *DockedMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DockedMainWindow))
        return static_cast<void*>(const_cast< DockedMainWindow*>(this));
    return MainWindowBase::qt_metacast(_clname);
}

int DockedMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainWindowBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileDropped((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: formWindowActivated((*reinterpret_cast< QDesignerFormWindow*(*)>(_a[1]))); break;
        case 2: slotSubWindowActivated((*reinterpret_cast< QMdiSubWindow*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DockedMainWindow::fileDropped(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DockedMainWindow::formWindowActivated(QDesignerFormWindow * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
