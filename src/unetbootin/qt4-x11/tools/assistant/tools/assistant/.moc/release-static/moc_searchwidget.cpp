/****************************************************************************
** Meta object code from reading C++ file 'searchwidget.h'
**
** Created: Thu Mar 5 20:33:52 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../searchwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'searchwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SearchWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   14,   13,   13, 0x05,
      40,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      70,   13,   13,   13, 0x08,
      79,   13,   13,   13, 0x08,
     103,   98,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SearchWidget[] = {
    "SearchWidget\0\0url\0requestShowLink(QUrl)\0"
    "requestShowLinkInNewTab(QUrl)\0search()\0"
    "searchingStarted()\0hits\0searchingFinished(int)\0"
};

const QMetaObject SearchWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SearchWidget,
      qt_meta_data_SearchWidget, 0 }
};

const QMetaObject *SearchWidget::metaObject() const
{
    return &staticMetaObject;
}

void *SearchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SearchWidget))
        return static_cast<void*>(const_cast< SearchWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SearchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: requestShowLink((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: requestShowLinkInNewTab((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 2: search(); break;
        case 3: searchingStarted(); break;
        case 4: searchingFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SearchWidget::requestShowLink(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SearchWidget::requestShowLinkInNewTab(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
