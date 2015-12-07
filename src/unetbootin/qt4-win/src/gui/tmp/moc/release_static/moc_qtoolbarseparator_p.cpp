/****************************************************************************
** Meta object code from reading C++ file 'qtoolbarseparator_p.h'
**
** Created: Fri May 30 22:33:54 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qtoolbarseparator_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtoolbarseparator_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QToolBarSeparator[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      31,   19,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QToolBarSeparator[] = {
    "QToolBarSeparator\0\0orientation\0"
    "setOrientation(Qt::Orientation)\0"
};

const QMetaObject QToolBarSeparator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QToolBarSeparator,
      qt_meta_data_QToolBarSeparator, 0 }
};

const QMetaObject *QToolBarSeparator::metaObject() const
{
    return &staticMetaObject;
}

void *QToolBarSeparator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QToolBarSeparator))
	return static_cast<void*>(const_cast< QToolBarSeparator*>(this));
    return QWidget::qt_metacast(_clname);
}

int QToolBarSeparator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setOrientation((*reinterpret_cast< Qt::Orientation(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
