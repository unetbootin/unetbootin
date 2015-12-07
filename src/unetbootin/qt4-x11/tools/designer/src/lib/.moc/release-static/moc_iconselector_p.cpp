/****************************************************************************
** Meta object code from reading C++ file 'iconselector_p.h'
**
** Created: Thu Mar 5 20:46:26 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/iconselector_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iconselector_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__LanguageResourceDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      44,   43,   43,   43, 0x08,
      59,   43,   43,   43, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__LanguageResourceDialog[] = {
    "qdesigner_internal::LanguageResourceDialog\0"
    "\0slotAccepted()\0slotPathChanged(QString)\0"
};

const QMetaObject qdesigner_internal::LanguageResourceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__LanguageResourceDialog,
      qt_meta_data_qdesigner_internal__LanguageResourceDialog, 0 }
};

const QMetaObject *qdesigner_internal::LanguageResourceDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::LanguageResourceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__LanguageResourceDialog))
        return static_cast<void*>(const_cast< LanguageResourceDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::LanguageResourceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->slotAccepted(); break;
        case 1: d_func()->slotPathChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__IconSelector[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      39,   34,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
      75,   33,   33,   33, 0x08,
      96,   33,   33,   33, 0x08,
     115,   33,   33,   33, 0x08,
     142,   33,   33,   33, 0x08,
     165,   33,   33,   33, 0x08,
     186,   33,   33,   33, 0x08,
     210,   33,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__IconSelector[] = {
    "qdesigner_internal::IconSelector\0\0"
    "icon\0iconChanged(PropertySheetIconValue)\0"
    "slotStateActivated()\0slotSetActivated()\0"
    "slotSetResourceActivated()\0"
    "slotSetFileActivated()\0slotResetActivated()\0"
    "slotResetAllActivated()\0slotUpdate()\0"
};

const QMetaObject qdesigner_internal::IconSelector::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__IconSelector,
      qt_meta_data_qdesigner_internal__IconSelector, 0 }
};

const QMetaObject *qdesigner_internal::IconSelector::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::IconSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__IconSelector))
        return static_cast<void*>(const_cast< IconSelector*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::IconSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: iconChanged((*reinterpret_cast< const PropertySheetIconValue(*)>(_a[1]))); break;
        case 1: d_func()->slotStateActivated(); break;
        case 2: d_func()->slotSetActivated(); break;
        case 3: d_func()->slotSetResourceActivated(); break;
        case 4: d_func()->slotSetFileActivated(); break;
        case 5: d_func()->slotResetActivated(); break;
        case 6: d_func()->slotResetAllActivated(); break;
        case 7: d_func()->slotUpdate(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::IconSelector::iconChanged(const PropertySheetIconValue & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
