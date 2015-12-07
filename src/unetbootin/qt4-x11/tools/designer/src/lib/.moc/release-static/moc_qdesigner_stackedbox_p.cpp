/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_stackedbox_p.h'
**
** Created: Thu Mar 5 20:46:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qdesigner_stackedbox_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_stackedbox_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QStackedWidgetPreviewEventFilter[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      34,   33,   33,   33, 0x0a,
      50,   33,   33,   33, 0x0a,
      61,   33,   33,   33, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QStackedWidgetPreviewEventFilter[] = {
    "QStackedWidgetPreviewEventFilter\0\0"
    "updateButtons()\0prevPage()\0nextPage()\0"
};

const QMetaObject QStackedWidgetPreviewEventFilter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QStackedWidgetPreviewEventFilter,
      qt_meta_data_QStackedWidgetPreviewEventFilter, 0 }
};

const QMetaObject *QStackedWidgetPreviewEventFilter::metaObject() const
{
    return &staticMetaObject;
}

void *QStackedWidgetPreviewEventFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStackedWidgetPreviewEventFilter))
        return static_cast<void*>(const_cast< QStackedWidgetPreviewEventFilter*>(this));
    return QObject::qt_metacast(_clname);
}

int QStackedWidgetPreviewEventFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateButtons(); break;
        case 1: prevPage(); break;
        case 2: nextPage(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_QStackedWidgetEventFilter[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x08,
      47,   26,   26,   26, 0x08,
      57,   26,   26,   26, 0x08,
      72,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QStackedWidgetEventFilter[] = {
    "QStackedWidgetEventFilter\0\0"
    "removeCurrentPage()\0addPage()\0"
    "addPageAfter()\0changeOrder()\0"
};

const QMetaObject QStackedWidgetEventFilter::staticMetaObject = {
    { &QStackedWidgetPreviewEventFilter::staticMetaObject, qt_meta_stringdata_QStackedWidgetEventFilter,
      qt_meta_data_QStackedWidgetEventFilter, 0 }
};

const QMetaObject *QStackedWidgetEventFilter::metaObject() const
{
    return &staticMetaObject;
}

void *QStackedWidgetEventFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStackedWidgetEventFilter))
        return static_cast<void*>(const_cast< QStackedWidgetEventFilter*>(this));
    return QStackedWidgetPreviewEventFilter::qt_metacast(_clname);
}

int QStackedWidgetEventFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStackedWidgetPreviewEventFilter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: removeCurrentPage(); break;
        case 1: addPage(); break;
        case 2: addPageAfter(); break;
        case 3: changeOrder(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
