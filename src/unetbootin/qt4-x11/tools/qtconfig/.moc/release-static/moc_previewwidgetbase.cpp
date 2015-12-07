/****************************************************************************
** Meta object code from reading C++ file 'previewwidgetbase.h'
**
** Created: Thu Mar 5 20:59:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../previewwidgetbase.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'previewwidgetbase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PreviewWidgetBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      36,   18,   18,   18, 0x09,
      43,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PreviewWidgetBase[] = {
    "PreviewWidgetBase\0\0languageChange()\0"
    "init()\0destroy()\0"
};

const QMetaObject PreviewWidgetBase::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PreviewWidgetBase,
      qt_meta_data_PreviewWidgetBase, 0 }
};

const QMetaObject *PreviewWidgetBase::metaObject() const
{
    return &staticMetaObject;
}

void *PreviewWidgetBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PreviewWidgetBase))
        return static_cast<void*>(const_cast< PreviewWidgetBase*>(this));
    if (!strcmp(_clname, "Ui::PreviewWidgetBase"))
        return static_cast< Ui::PreviewWidgetBase*>(const_cast< PreviewWidgetBase*>(this));
    return QWidget::qt_metacast(_clname);
}

int PreviewWidgetBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: languageChange(); break;
        case 1: init(); break;
        case 2: destroy(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
