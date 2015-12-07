/****************************************************************************
** Meta object code from reading C++ file 'embeddedoptionspage.h'
**
** Created: Thu Mar 5 20:52:14 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../formeditor/embeddedoptionspage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'embeddedoptionspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__EmbeddedOptionsControl[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      44,   43,   43,   43, 0x0a,
      59,   43,   43,   43, 0x0a,
      74,   43,   43,   43, 0x08,
      84,   43,   43,   43, 0x08,
      95,   43,   43,   43, 0x08,
     108,   43,   43,   43, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__EmbeddedOptionsControl[] = {
    "qdesigner_internal::EmbeddedOptionsControl\0"
    "\0loadSettings()\0saveSettings()\0slotAdd()\0"
    "slotEdit()\0slotDelete()\0"
    "slotProfileIndexChanged(int)\0"
};

const QMetaObject qdesigner_internal::EmbeddedOptionsControl::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__EmbeddedOptionsControl,
      qt_meta_data_qdesigner_internal__EmbeddedOptionsControl, 0 }
};

const QMetaObject *qdesigner_internal::EmbeddedOptionsControl::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::EmbeddedOptionsControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__EmbeddedOptionsControl))
        return static_cast<void*>(const_cast< EmbeddedOptionsControl*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::EmbeddedOptionsControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadSettings(); break;
        case 1: saveSettings(); break;
        case 2: slotAdd(); break;
        case 3: slotEdit(); break;
        case 4: slotDelete(); break;
        case 5: slotProfileIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
