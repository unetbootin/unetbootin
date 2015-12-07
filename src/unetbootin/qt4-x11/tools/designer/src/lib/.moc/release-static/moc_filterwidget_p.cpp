/****************************************************************************
** Meta object code from reading C++ file 'filterwidget_p.h'
**
** Created: Thu Mar 5 20:47:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/filterwidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filterwidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__HintLineEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      37,   34,   33,   33, 0x0a,
      64,   58,   33,   33, 0x0a,
      83,   33,   33,   33, 0x2a,
      98,   33,   33,   33, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__HintLineEdit[] = {
    "qdesigner_internal::HintLineEdit\0\0ht\0"
    "setHintText(QString)\0force\0"
    "showHintText(bool)\0showHintText()\0"
    "hideHintText()\0"
};

const QMetaObject qdesigner_internal::HintLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_qdesigner_internal__HintLineEdit,
      qt_meta_data_qdesigner_internal__HintLineEdit, 0 }
};

const QMetaObject *qdesigner_internal::HintLineEdit::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::HintLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__HintLineEdit))
        return static_cast<void*>(const_cast< HintLineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int qdesigner_internal::HintLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setHintText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: showHintText((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: showHintText(); break;
        case 3: hideHintText(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__FilterWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      34,   33,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   33,   33,   33, 0x0a,
      70,   65,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__FilterWidget[] = {
    "qdesigner_internal::FilterWidget\0\0"
    "filterChanged(QString)\0reset()\0text\0"
    "checkButton(QString)\0"
};

const QMetaObject qdesigner_internal::FilterWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__FilterWidget,
      qt_meta_data_qdesigner_internal__FilterWidget, 0 }
};

const QMetaObject *qdesigner_internal::FilterWidget::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::FilterWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__FilterWidget))
        return static_cast<void*>(const_cast< FilterWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::FilterWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: filterChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: reset(); break;
        case 2: checkButton((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::FilterWidget::filterChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
