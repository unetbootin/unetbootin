/****************************************************************************
** Meta object code from reading C++ file 'fontpanel.h'
**
** Created: Thu Mar 5 20:33:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/fontpanel/fontpanel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fontpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FontPanel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      41,   10,   10,   10, 0x08,
      66,   10,   10,   10, 0x08,
      88,   10,   10,   10, 0x08,
     114,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FontPanel[] = {
    "FontPanel\0\0slotWritingSystemChanged(int)\0"
    "slotFamilyChanged(QFont)\0slotStyleChanged(int)\0"
    "slotPointSizeChanged(int)\0"
    "slotUpdatePreviewFont()\0"
};

const QMetaObject FontPanel::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_FontPanel,
      qt_meta_data_FontPanel, 0 }
};

const QMetaObject *FontPanel::metaObject() const
{
    return &staticMetaObject;
}

void *FontPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FontPanel))
        return static_cast<void*>(const_cast< FontPanel*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int FontPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotWritingSystemChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: slotFamilyChanged((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 2: slotStyleChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: slotPointSizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: slotUpdatePreviewFont(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
