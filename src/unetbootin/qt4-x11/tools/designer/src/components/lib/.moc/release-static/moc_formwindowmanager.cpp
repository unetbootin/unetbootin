/****************************************************************************
** Meta object code from reading C++ file 'formwindowmanager.h'
**
** Created: Thu Mar 5 20:52:05 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../formeditor/formwindowmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'formwindowmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__FormWindowManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      50,   39,   38,   38, 0x0a,
      95,   39,   38,   38, 0x0a,
     143,   39,   38,   38, 0x0a,
     194,   38,   38,   38, 0x0a,
     213,   38,   38,   38, 0x0a,
     237,   38,   38,   38, 0x08,
     262,   38,   38,   38, 0x08,
     288,   38,   38,   38, 0x08,
     315,   38,   38,   38, 0x08,
     343,   38,   38,   38, 0x08,
     374,   38,   38,   38, 0x08,
     401,   38,   38,   38, 0x08,
     428,   38,   38,   38, 0x08,
     443,   38,   38,   38, 0x08,
     476,   38,   38,   38, 0x08,
     508,   38,   38,   38, 0x08,
     544,   38,   38,   38, 0x08,
     605,  580,   38,   38, 0x08,
     648,   38,   38,   38, 0x08,
     689,   38,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__FormWindowManager[] = {
    "qdesigner_internal::FormWindowManager\0"
    "\0formWindow\0addFormWindow(QDesignerFormWindowInterface*)\0"
    "removeFormWindow(QDesignerFormWindowInterface*)\0"
    "setActiveFormWindow(QDesignerFormWindowInterface*)\0"
    "closeAllPreviews()\0deviceProfilesChanged()\0"
    "slotActionCutActivated()\0"
    "slotActionCopyActivated()\0"
    "slotActionPasteActivated()\0"
    "slotActionDeleteActivated()\0"
    "slotActionSelectAllActivated()\0"
    "slotActionLowerActivated()\0"
    "slotActionRaiseActivated()\0createLayout()\0"
    "slotActionBreakLayoutActivated()\0"
    "slotActionAdjustSizeActivated()\0"
    "slotActionSimplifyLayoutActivated()\0"
    "slotActionDefaultPreviewActivated()\0"
    "style,deviceProfileIndex\0"
    "slotActionGroupPreviewInStyle(QString,int)\0"
    "slotActionShowFormWindowSettingsDialog()\0"
    "slotUpdateActions()\0"
};

const QMetaObject qdesigner_internal::FormWindowManager::staticMetaObject = {
    { &QDesignerFormWindowManager::staticMetaObject, qt_meta_stringdata_qdesigner_internal__FormWindowManager,
      qt_meta_data_qdesigner_internal__FormWindowManager, 0 }
};

const QMetaObject *qdesigner_internal::FormWindowManager::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::FormWindowManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__FormWindowManager))
        return static_cast<void*>(const_cast< FormWindowManager*>(this));
    return QDesignerFormWindowManager::qt_metacast(_clname);
}

int qdesigner_internal::FormWindowManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDesignerFormWindowManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 1: removeFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 2: setActiveFormWindow((*reinterpret_cast< QDesignerFormWindowInterface*(*)>(_a[1]))); break;
        case 3: closeAllPreviews(); break;
        case 4: deviceProfilesChanged(); break;
        case 5: slotActionCutActivated(); break;
        case 6: slotActionCopyActivated(); break;
        case 7: slotActionPasteActivated(); break;
        case 8: slotActionDeleteActivated(); break;
        case 9: slotActionSelectAllActivated(); break;
        case 10: slotActionLowerActivated(); break;
        case 11: slotActionRaiseActivated(); break;
        case 12: createLayout(); break;
        case 13: slotActionBreakLayoutActivated(); break;
        case 14: slotActionAdjustSizeActivated(); break;
        case 15: slotActionSimplifyLayoutActivated(); break;
        case 16: slotActionDefaultPreviewActivated(); break;
        case 17: slotActionGroupPreviewInStyle((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 18: slotActionShowFormWindowSettingsDialog(); break;
        case 19: slotUpdateActions(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
