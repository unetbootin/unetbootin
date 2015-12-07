/****************************************************************************
** Meta object code from reading C++ file 'templateoptionspage.h'
**
** Created: Thu Mar 5 20:52:18 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../formeditor/templateoptionspage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'templateoptionspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__TemplateOptionsWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      43,   42,   42,   42, 0x08,
      61,   42,   42,   42, 0x08,
      82,   42,   42,   42, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__TemplateOptionsWidget[] = {
    "qdesigner_internal::TemplateOptionsWidget\0"
    "\0addTemplatePath()\0removeTemplatePath()\0"
    "templatePathSelectionChanged()\0"
};

const QMetaObject qdesigner_internal::TemplateOptionsWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__TemplateOptionsWidget,
      qt_meta_data_qdesigner_internal__TemplateOptionsWidget, 0 }
};

const QMetaObject *qdesigner_internal::TemplateOptionsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::TemplateOptionsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__TemplateOptionsWidget))
        return static_cast<void*>(const_cast< TemplateOptionsWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::TemplateOptionsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addTemplatePath(); break;
        case 1: removeTemplatePath(); break;
        case 2: templatePathSelectionChanged(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
