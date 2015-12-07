/****************************************************************************
** Meta object code from reading C++ file 'qdesktopwidget.h'
**
** Created: Fri May 30 22:33:05 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qdesktopwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesktopwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesktopWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      29,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QDesktopWidget[] = {
    "QDesktopWidget\0\0resized(int)\0"
    "workAreaResized(int)\0"
};

const QMetaObject QDesktopWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDesktopWidget,
      qt_meta_data_QDesktopWidget, 0 }
};

const QMetaObject *QDesktopWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QDesktopWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesktopWidget))
	return static_cast<void*>(const_cast< QDesktopWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDesktopWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resized((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: workAreaResized((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QDesktopWidget::resized(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDesktopWidget::workAreaResized(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
