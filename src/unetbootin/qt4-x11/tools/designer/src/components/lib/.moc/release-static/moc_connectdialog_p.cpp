/****************************************************************************
** Meta object code from reading C++ file 'connectdialog_p.h'
**
** Created: Thu Mar 5 20:51:57 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../signalsloteditor/connectdialog_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connectdialog_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ConnectDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      35,   34,   34,   34, 0x08,
      56,   51,   34,   34, 0x08,
      87,   51,   34,   34, 0x08,
     116,   34,   34,   34, 0x08,
     144,  137,   34,   34, 0x08,
     170,   34,   34,   34, 0x28,
     189,   34,   34,   34, 0x08,
     203,   34,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ConnectDialog[] = {
    "qdesigner_internal::ConnectDialog\0\0"
    "populateLists()\0item\0"
    "selectSignal(QListWidgetItem*)\0"
    "selectSlot(QListWidgetItem*)\0"
    "populateSignalList()\0signal\0"
    "populateSlotList(QString)\0populateSlotList()\0"
    "editSignals()\0editSlots()\0"
};

const QMetaObject qdesigner_internal::ConnectDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ConnectDialog,
      qt_meta_data_qdesigner_internal__ConnectDialog, 0 }
};

const QMetaObject *qdesigner_internal::ConnectDialog::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ConnectDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ConnectDialog))
        return static_cast<void*>(const_cast< ConnectDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::ConnectDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: populateLists(); break;
        case 1: selectSignal((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 2: selectSlot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: populateSignalList(); break;
        case 4: populateSlotList((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: populateSlotList(); break;
        case 6: editSignals(); break;
        case 7: editSlots(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
