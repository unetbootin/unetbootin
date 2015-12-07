/****************************************************************************
** Meta object code from reading C++ file 'previewconfigurationwidget_p.h'
**
** Created: Thu Mar 5 20:47:17 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/previewconfigurationwidget_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'previewconfigurationwidget_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PreviewConfigurationWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      48,   47,   47,   47, 0x08,
      72,   47,   47,   47, 0x08,
      94,   47,   47,   47, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PreviewConfigurationWidget[] = {
    "qdesigner_internal::PreviewConfigurationWidget\0"
    "\0slotEditAppStyleSheet()\0slotDeleteSkinEntry()\0"
    "slotSkinChanged(int)\0"
};

const QMetaObject qdesigner_internal::PreviewConfigurationWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PreviewConfigurationWidget,
      qt_meta_data_qdesigner_internal__PreviewConfigurationWidget, 0 }
};

const QMetaObject *qdesigner_internal::PreviewConfigurationWidget::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PreviewConfigurationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PreviewConfigurationWidget))
        return static_cast<void*>(const_cast< PreviewConfigurationWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int qdesigner_internal::PreviewConfigurationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotEditAppStyleSheet(); break;
        case 1: slotDeleteSkinEntry(); break;
        case 2: slotSkinChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
