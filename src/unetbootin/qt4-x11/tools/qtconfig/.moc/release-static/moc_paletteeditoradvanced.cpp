/****************************************************************************
** Meta object code from reading C++ file 'paletteeditoradvanced.h'
**
** Created: Thu Mar 5 20:59:36 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../paletteeditoradvanced.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paletteeditoradvanced.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PaletteEditorAdvanced[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x09,
      44,   22,   22,   22, 0x09,
      59,   22,   22,   22, 0x09,
      73,   22,   22,   22, 0x09,
      96,   22,   22,   22, 0x09,
     118,   22,   22,   22, 0x09,
     145,   22,   22,   22, 0x09,
     173,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PaletteEditorAdvanced[] = {
    "PaletteEditorAdvanced\0\0paletteSelected(int)\0"
    "onCentral(int)\0onEffect(int)\0"
    "onChooseCentralColor()\0onChooseEffectColor()\0"
    "onToggleBuildEffects(bool)\0"
    "onToggleBuildInactive(bool)\0"
    "onToggleBuildDisabled(bool)\0"
};

const QMetaObject PaletteEditorAdvanced::staticMetaObject = {
    { &PaletteEditorAdvancedBase::staticMetaObject, qt_meta_stringdata_PaletteEditorAdvanced,
      qt_meta_data_PaletteEditorAdvanced, 0 }
};

const QMetaObject *PaletteEditorAdvanced::metaObject() const
{
    return &staticMetaObject;
}

void *PaletteEditorAdvanced::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PaletteEditorAdvanced))
        return static_cast<void*>(const_cast< PaletteEditorAdvanced*>(this));
    return PaletteEditorAdvancedBase::qt_metacast(_clname);
}

int PaletteEditorAdvanced::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PaletteEditorAdvancedBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: paletteSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: onCentral((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: onEffect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: onChooseCentralColor(); break;
        case 4: onChooseEffectColor(); break;
        case 5: onToggleBuildEffects((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: onToggleBuildInactive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: onToggleBuildDisabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
