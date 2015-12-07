/****************************************************************************
** Meta object code from reading C++ file 'qdesigner_formwindow.h'
**
** Created: Thu Mar 5 20:53:34 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdesigner_formwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdesigner_formwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDesignerFormWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      42,   21,   20,   20, 0x05,
     103,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
     119,   20,   20,   20, 0x0a,
     144,  135,   20,   20, 0x08,
     171,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDesignerFormWindow[] = {
    "QDesignerFormWindow\0\0formWindow,minimized\0"
    "minimizationStateChanged(QDesignerFormWindowInterface*,bool)\0"
    "triggerAction()\0updateChanged()\0"
    "fileName\0updateWindowTitle(QString)\0"
    "geometryChanged()\0"
};

const QMetaObject QDesignerFormWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDesignerFormWindow,
      qt_meta_data_QDesignerFormWindow, 0 }
};

const QMetaObject *QDesignerFormWindow::metaObject() const
{
    return &staticMetaObject;
}

void *QDesignerFormWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDesignerFormWindow))
        return static_cast<void*>(const_cast< QDesignerFormWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDesignerFormWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: minimizationStateChanged((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: triggerAction(); break;
        case 2: updateChanged(); break;
        case 3: updateWindowTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: geometryChanged(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QDesignerFormWindow::minimizationStateChanged(QDesignerFormWindowInterface * _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDesignerFormWindow::triggerAction()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
