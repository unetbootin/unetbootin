/****************************************************************************
** Meta object code from reading C++ file 'qtbuttonpropertybrowser.h'
**
** Created: Thu Mar 5 20:48:48 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../shared/qtpropertybrowser/qtbuttonpropertybrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtbuttonpropertybrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtButtonPropertyBrowser[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      30,   25,   24,   24, 0x05,
      56,   25,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
      81,   24,   24,   24, 0x08,
      94,   24,   24,   24, 0x08,
     116,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtButtonPropertyBrowser[] = {
    "QtButtonPropertyBrowser\0\0item\0"
    "collapsed(QtBrowserItem*)\0"
    "expanded(QtBrowserItem*)\0slotUpdate()\0"
    "slotEditorDestroyed()\0slotToggled(bool)\0"
};

const QMetaObject QtButtonPropertyBrowser::staticMetaObject = {
    { &QtAbstractPropertyBrowser::staticMetaObject, qt_meta_stringdata_QtButtonPropertyBrowser,
      qt_meta_data_QtButtonPropertyBrowser, 0 }
};

const QMetaObject *QtButtonPropertyBrowser::metaObject() const
{
    return &staticMetaObject;
}

void *QtButtonPropertyBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtButtonPropertyBrowser))
        return static_cast<void*>(const_cast< QtButtonPropertyBrowser*>(this));
    return QtAbstractPropertyBrowser::qt_metacast(_clname);
}

int QtButtonPropertyBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtAbstractPropertyBrowser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: collapsed((*reinterpret_cast< QtBrowserItem*(*)>(_a[1]))); break;
        case 1: expanded((*reinterpret_cast< QtBrowserItem*(*)>(_a[1]))); break;
        case 2: d_func()->slotUpdate(); break;
        case 3: d_func()->slotEditorDestroyed(); break;
        case 4: d_func()->slotToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QtButtonPropertyBrowser::collapsed(QtBrowserItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtButtonPropertyBrowser::expanded(QtBrowserItem * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
