/****************************************************************************
** Meta object code from reading C++ file 'qtresourceview_p.h'
**
** Created: Thu Mar 5 20:46:22 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qtresourceview_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtresourceview_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtResourceView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      25,   16,   15,   15, 0x05,
      51,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      78,   15,   15,   15, 0x08,
     119,   15,   15,   15, 0x08,
     160,   15,   15,   15, 0x08,
     205,   15,   15,   15, 0x08,
     245,   15,   15,   15, 0x08,
     265,   15,   15,   15, 0x08,
     287,   15,   15,   15, 0x08,
     314,  310,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtResourceView[] = {
    "QtResourceView\0\0resource\0"
    "resourceSelected(QString)\0"
    "resourceActivated(QString)\0"
    "slotResourceSetActivated(QtResourceSet*)\0"
    "slotCurrentPathChanged(QTreeWidgetItem*)\0"
    "slotCurrentResourceChanged(QListWidgetItem*)\0"
    "slotResourceActivated(QListWidgetItem*)\0"
    "slotEditResources()\0slotReloadResources()\0"
    "slotCopyResourcePath()\0pos\0"
    "slotListWidgetContextMenuRequested(QPoint)\0"
};

const QMetaObject QtResourceView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtResourceView,
      qt_meta_data_QtResourceView, 0 }
};

const QMetaObject *QtResourceView::metaObject() const
{
    return &staticMetaObject;
}

void *QtResourceView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtResourceView))
        return static_cast<void*>(const_cast< QtResourceView*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtResourceView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resourceSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: resourceActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: d_func()->slotResourceSetActivated((*reinterpret_cast< QtResourceSet*(*)>(_a[1]))); break;
        case 3: d_func()->slotCurrentPathChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 4: d_func()->slotCurrentResourceChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: d_func()->slotResourceActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: d_func()->slotEditResources(); break;
        case 7: d_func()->slotReloadResources(); break;
        case 8: d_func()->slotCopyResourcePath(); break;
        case 9: d_func()->slotListWidgetContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QtResourceView::resourceSelected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtResourceView::resourceActivated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_QtResourceViewDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtResourceViewDialog[] = {
    "QtResourceViewDialog\0\0"
    "slotResourceSelected(QString)\0"
};

const QMetaObject QtResourceViewDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QtResourceViewDialog,
      qt_meta_data_QtResourceViewDialog, 0 }
};

const QMetaObject *QtResourceViewDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QtResourceViewDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtResourceViewDialog))
        return static_cast<void*>(const_cast< QtResourceViewDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QtResourceViewDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotResourceSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
