/****************************************************************************
** Meta object code from reading C++ file 'qgraphicsproxywidget.h'
**
** Created: Fri May 30 22:32:45 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../graphicsview/qgraphicsproxywidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgraphicsproxywidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGraphicsProxyWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QGraphicsProxyWidget[] = {
    "QGraphicsProxyWidget\0\0_q_removeWidgetSlot()\0"
};

const QMetaObject QGraphicsProxyWidget::staticMetaObject = {
    { &QGraphicsWidget::staticMetaObject, qt_meta_stringdata_QGraphicsProxyWidget,
      qt_meta_data_QGraphicsProxyWidget, 0 }
};

const QMetaObject *QGraphicsProxyWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QGraphicsProxyWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGraphicsProxyWidget))
	return static_cast<void*>(const_cast< QGraphicsProxyWidget*>(this));
    return QGraphicsWidget::qt_metacast(_clname);
}

int QGraphicsProxyWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_removeWidgetSlot(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
