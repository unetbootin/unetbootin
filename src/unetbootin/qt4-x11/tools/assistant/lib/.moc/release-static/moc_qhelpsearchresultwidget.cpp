/****************************************************************************
** Meta object code from reading C++ file 'qhelpsearchresultwidget.h'
**
** Created: Thu Mar 5 20:32:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpsearchresultwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpsearchresultwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHelpSearchResultWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      29,   25,   24,   24, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QHelpSearchResultWidget[] = {
    "QHelpSearchResultWidget\0\0url\0"
    "requestShowLink(QUrl)\0"
};

const QMetaObject QHelpSearchResultWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QHelpSearchResultWidget,
      qt_meta_data_QHelpSearchResultWidget, 0 }
};

const QMetaObject *QHelpSearchResultWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpSearchResultWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpSearchResultWidget))
        return static_cast<void*>(const_cast< QHelpSearchResultWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QHelpSearchResultWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: requestShowLink((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QHelpSearchResultWidget::requestShowLink(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
