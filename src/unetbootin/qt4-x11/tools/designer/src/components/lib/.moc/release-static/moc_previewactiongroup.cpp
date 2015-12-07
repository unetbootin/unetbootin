/****************************************************************************
** Meta object code from reading C++ file 'previewactiongroup.h'
**
** Created: Thu Mar 5 20:52:16 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../formeditor/previewactiongroup.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'previewactiongroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PreviewActionGroup[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      65,   40,   39,   39, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,   39,   39,   39, 0x0a,
     109,   39,   39,   39, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PreviewActionGroup[] = {
    "qdesigner_internal::PreviewActionGroup\0"
    "\0style,deviceProfileIndex\0"
    "preview(QString,int)\0updateDeviceProfiles()\0"
    "slotTriggered(QAction*)\0"
};

const QMetaObject qdesigner_internal::PreviewActionGroup::staticMetaObject = {
    { &QActionGroup::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PreviewActionGroup,
      qt_meta_data_qdesigner_internal__PreviewActionGroup, 0 }
};

const QMetaObject *qdesigner_internal::PreviewActionGroup::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PreviewActionGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PreviewActionGroup))
        return static_cast<void*>(const_cast< PreviewActionGroup*>(this));
    return QActionGroup::qt_metacast(_clname);
}

int qdesigner_internal::PreviewActionGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QActionGroup::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: preview((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: updateDeviceProfiles(); break;
        case 2: slotTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::PreviewActionGroup::preview(const QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
