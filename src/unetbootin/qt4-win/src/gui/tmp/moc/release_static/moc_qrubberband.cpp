/****************************************************************************
** Meta object code from reading C++ file 'qrubberband.h'
**
** Created: Fri May 30 22:33:47 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qrubberband.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qrubberband.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QRubberBand[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QRubberBand[] = {
    "QRubberBand\0"
};

const QMetaObject QRubberBand::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QRubberBand,
      qt_meta_data_QRubberBand, 0 }
};

const QMetaObject *QRubberBand::metaObject() const
{
    return &staticMetaObject;
}

void *QRubberBand::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QRubberBand))
	return static_cast<void*>(const_cast< QRubberBand*>(this));
    return QWidget::qt_metacast(_clname);
}

int QRubberBand::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
