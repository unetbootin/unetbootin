/****************************************************************************
** Meta object code from reading C++ file 'widgetboxcategorylistview.h'
**
** Created: Thu Mar 5 20:52:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgetbox/widgetboxcategorylistview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widgetboxcategorylistview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__WidgetBoxCategoryListView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      47,   46,   46,   46, 0x05,
      86,   67,   46,   46, 0x05,
     118,   46,   46,   46, 0x05,
     132,   46,   46,   46, 0x05,

 // slots: signature, parameters, type, tag, flags
     153,  150,   46,   46, 0x0a,
     175,  169,   46,   46, 0x08,
     200,   46,   46,   46, 0x08,
     220,   46,   46,   46, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__WidgetBoxCategoryListView[] = {
    "qdesigner_internal::WidgetBoxCategoryListView\0"
    "\0scratchPadChanged()\0name,xml,globalPos\0"
    "pressed(QString,QString,QPoint)\0"
    "itemRemoved()\0lastItemRemoved()\0re\0"
    "filter(QRegExp)\0index\0slotPressed(QModelIndex)\0"
    "removeCurrentItem()\0editCurrentItem()\0"
};

const QMetaObject qdesigner_internal::WidgetBoxCategoryListView::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_qdesigner_internal__WidgetBoxCategoryListView,
      qt_meta_data_qdesigner_internal__WidgetBoxCategoryListView, 0 }
};

const QMetaObject *qdesigner_internal::WidgetBoxCategoryListView::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::WidgetBoxCategoryListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__WidgetBoxCategoryListView))
        return static_cast<void*>(const_cast< WidgetBoxCategoryListView*>(this));
    return QListView::qt_metacast(_clname);
}

int qdesigner_internal::WidgetBoxCategoryListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: scratchPadChanged(); break;
        case 1: pressed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3]))); break;
        case 2: itemRemoved(); break;
        case 3: lastItemRemoved(); break;
        case 4: filter((*reinterpret_cast< const QRegExp(*)>(_a[1]))); break;
        case 5: slotPressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: removeCurrentItem(); break;
        case 7: editCurrentItem(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::WidgetBoxCategoryListView::scratchPadChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void qdesigner_internal::WidgetBoxCategoryListView::pressed(const QString & _t1, const QString & _t2, const QPoint & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::WidgetBoxCategoryListView::itemRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void qdesigner_internal::WidgetBoxCategoryListView::lastItemRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
