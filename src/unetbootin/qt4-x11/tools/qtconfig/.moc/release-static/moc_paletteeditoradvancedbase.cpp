/****************************************************************************
** Meta object code from reading C++ file 'paletteeditoradvancedbase.h'
**
** Created: Thu Mar 5 20:59:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../paletteeditoradvancedbase.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paletteeditoradvancedbase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PaletteEditorAdvancedBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x09,
      44,   26,   26,   26, 0x09,
      51,   26,   26,   26, 0x09,
      61,   26,   26,   26, 0x09,
      76,   26,   26,   26, 0x09,
      99,   26,   26,   26, 0x09,
     121,   26,   26,   26, 0x09,
     135,   26,   26,   26, 0x09,
     163,   26,   26,   26, 0x09,
     190,   26,   26,   26, 0x09,
     218,   26,   26,   26, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PaletteEditorAdvancedBase[] = {
    "PaletteEditorAdvancedBase\0\0languageChange()\0"
    "init()\0destroy()\0onCentral(int)\0"
    "onChooseCentralColor()\0onChooseEffectColor()\0"
    "onEffect(int)\0onToggleBuildDisabled(bool)\0"
    "onToggleBuildEffects(bool)\0"
    "onToggleBuildInactive(bool)\0"
    "paletteSelected(int)\0"
};

const QMetaObject PaletteEditorAdvancedBase::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PaletteEditorAdvancedBase,
      qt_meta_data_PaletteEditorAdvancedBase, 0 }
};

const QMetaObject *PaletteEditorAdvancedBase::metaObject() const
{
    return &staticMetaObject;
}

void *PaletteEditorAdvancedBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PaletteEditorAdvancedBase))
        return static_cast<void*>(const_cast< PaletteEditorAdvancedBase*>(this));
    if (!strcmp(_clname, "Ui::PaletteEditorAdvancedBase"))
        return static_cast< Ui::PaletteEditorAdvancedBase*>(const_cast< PaletteEditorAdvancedBase*>(this));
    return QDialog::qt_metacast(_clname);
}

int PaletteEditorAdvancedBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: languageChange(); break;
        case 1: init(); break;
        case 2: destroy(); break;
        case 3: onCentral((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: onChooseCentralColor(); break;
        case 5: onChooseEffectColor(); break;
        case 6: onEffect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: onToggleBuildDisabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: onToggleBuildEffects((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: onToggleBuildInactive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: paletteSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
