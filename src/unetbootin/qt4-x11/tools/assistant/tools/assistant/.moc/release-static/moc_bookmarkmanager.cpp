/****************************************************************************
** Meta object code from reading C++ file 'bookmarkmanager.h'
**
** Created: Thu Mar 5 20:34:02 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../bookmarkmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bookmarkmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BookmarkDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      30,   15,   15,   15, 0x08,
      45,   15,   15,   15, 0x08,
      70,   65,   15,   15, 0x08,
     105,   98,   15,   15, 0x08,
     137,  126,   15,   15, 0x08,
     173,  167,   15,   15, 0x08,
     225,  208,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BookmarkDialog[] = {
    "BookmarkDialog\0\0addAccepted()\0"
    "addNewFolder()\0toolButtonClicked()\0"
    "item\0itemChanged(QStandardItem*)\0"
    "string\0textChanged(QString)\0folderName\0"
    "selectBookmarkFolder(QString)\0point\0"
    "customContextMenuRequested(QPoint)\0"
    "current,previous\0"
    "currentChanged(QModelIndex,QModelIndex)\0"
};

const QMetaObject BookmarkDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BookmarkDialog,
      qt_meta_data_BookmarkDialog, 0 }
};

const QMetaObject *BookmarkDialog::metaObject() const
{
    return &staticMetaObject;
}

void *BookmarkDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BookmarkDialog))
        return static_cast<void*>(const_cast< BookmarkDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int BookmarkDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addAccepted(); break;
        case 1: addNewFolder(); break;
        case 2: toolButtonClicked(); break;
        case 3: itemChanged((*reinterpret_cast< QStandardItem*(*)>(_a[1]))); break;
        case 4: textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: selectBookmarkFolder((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 7: currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
static const uint qt_meta_data_TreeView[] = {

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

static const char qt_meta_stringdata_TreeView[] = {
    "TreeView\0"
};

const QMetaObject TreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_TreeView,
      qt_meta_data_TreeView, 0 }
};

const QMetaObject *TreeView::metaObject() const
{
    return &staticMetaObject;
}

void *TreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TreeView))
        return static_cast<void*>(const_cast< TreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int TreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_BookmarkWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      34,   30,   15,   15, 0x05,
      56,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      72,   15,   15,   15, 0x08,
      88,   15,   15,   15, 0x08,
     110,  104,   15,   15, 0x08,
     130,  104,   15,   15, 0x08,
     159,  153,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BookmarkWidget[] = {
    "BookmarkWidget\0\0addBookmark()\0url\0"
    "requestShowLink(QUrl)\0escapePressed()\0"
    "removeClicked()\0filterChanged()\0index\0"
    "expand(QModelIndex)\0activated(QModelIndex)\0"
    "point\0customContextMenuRequested(QPoint)\0"
};

const QMetaObject BookmarkWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BookmarkWidget,
      qt_meta_data_BookmarkWidget, 0 }
};

const QMetaObject *BookmarkWidget::metaObject() const
{
    return &staticMetaObject;
}

void *BookmarkWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BookmarkWidget))
        return static_cast<void*>(const_cast< BookmarkWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int BookmarkWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addBookmark(); break;
        case 1: requestShowLink((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 2: escapePressed(); break;
        case 3: removeClicked(); break;
        case 4: filterChanged(); break;
        case 5: expand((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void BookmarkWidget::addBookmark()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void BookmarkWidget::requestShowLink(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BookmarkWidget::escapePressed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
static const uint qt_meta_data_BookmarkModel[] = {

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

static const char qt_meta_stringdata_BookmarkModel[] = {
    "BookmarkModel\0"
};

const QMetaObject BookmarkModel::staticMetaObject = {
    { &QStandardItemModel::staticMetaObject, qt_meta_stringdata_BookmarkModel,
      qt_meta_data_BookmarkModel, 0 }
};

const QMetaObject *BookmarkModel::metaObject() const
{
    return &staticMetaObject;
}

void *BookmarkModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BookmarkModel))
        return static_cast<void*>(const_cast< BookmarkModel*>(this));
    return QStandardItemModel::qt_metacast(_clname);
}

int BookmarkModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStandardItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_BookmarkManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BookmarkManager[] = {
    "BookmarkManager\0\0item\0itemChanged(QStandardItem*)\0"
};

const QMetaObject BookmarkManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BookmarkManager,
      qt_meta_data_BookmarkManager, 0 }
};

const QMetaObject *BookmarkManager::metaObject() const
{
    return &staticMetaObject;
}

void *BookmarkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BookmarkManager))
        return static_cast<void*>(const_cast< BookmarkManager*>(this));
    return QObject::qt_metacast(_clname);
}

int BookmarkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemChanged((*reinterpret_cast< QStandardItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
