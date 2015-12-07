/****************************************************************************
** Meta object code from reading C++ file 'abstractnewformwidget_p.h'
**
** Created: Thu Mar 5 20:46:43 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../sdk/abstractnewformwidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstractnewformwidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerNewFormWidgetInterface[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,
      70,   53,   32,   32, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerNewFormWidgetInterface[] = {
    "QDesignerNewFormWidgetInterface\0\0"
    "templateActivated()\0templateSelected\0"
    "currentTemplateChanged(bool)\0"
};

const QMetaObject QDesignerNewFormWidgetInterface::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDesignerNewFormWidgetInterface,
      qt_meta_data_QDesignerNewFormWidgetInterface, 0 }
};

const QMetaObject *QDesignerNewFormWidgetInterface::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerNewFormWidgetInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerNewFormWidgetInterface))
        return static_cast<void*>(const_cast< QDesignerNewFormWidgetInterface*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDesignerNewFormWidgetInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: templateActivated(); break;
        case 1: currentTemplateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QDesignerNewFormWidgetInterface::templateActivated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QDesignerNewFormWidgetInterface::currentTemplateChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
