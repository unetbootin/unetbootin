/****************************************************************************
** Meta object code from reading C++ file 'qwidgetanimator_p.h'
**
** Created: Fri May 30 22:33:57 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qwidgetanimator_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwidgetanimator_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWidgetAnimator[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      24,   17,   16,   16, 0x05,
      43,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QWidgetAnimator[] = {
    "QWidgetAnimator\0\0widget\0finished(QWidget*)\0"
    "finishedAll()\0animationStep()\0"
};

const QMetaObject QWidgetAnimator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWidgetAnimator,
      qt_meta_data_QWidgetAnimator, 0 }
};

const QMetaObject *QWidgetAnimator::metaObject() const
{
    return &staticMetaObject;
}

void *QWidgetAnimator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWidgetAnimator))
	return static_cast<void*>(const_cast< QWidgetAnimator*>(this));
    return QObject::qt_metacast(_clname);
}

int QWidgetAnimator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: finishedAll(); break;
        case 2: animationStep(); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void QWidgetAnimator::finished(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QWidgetAnimator::finishedAll()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
