/****************************************************************************
** Meta object code from reading C++ file 'mainwindowbase.h'
**
** Created: Thu Mar 5 20:59:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindowbase.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindowbase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindowBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      30,   15,   15,   15, 0x0a,
      46,   15,   15,   15, 0x0a,
      63,   15,   15,   15, 0x0a,
      75,   15,   15,   15, 0x0a,
      90,   15,   15,   15, 0x0a,
     105,   15,   15,   15, 0x0a,
     122,   15,   15,   15, 0x0a,
     146,   15,   15,   15, 0x0a,
     157,   15,   15,   15, 0x0a,
     168,   15,   15,   15, 0x0a,
     180,   15,   15,   15, 0x0a,
     194,   15,   15,   15, 0x0a,
     205,   15,   15,   15, 0x0a,
     227,   15,   15,   15, 0x0a,
     248,   15,   15,   15, 0x0a,
     265,   15,   15,   15, 0x0a,
     284,   15,   15,   15, 0x0a,
     304,   15,   15,   15, 0x0a,
     327,   15,   15,   15, 0x0a,
     355,   15,   15,   15, 0x0a,
     369,   15,   15,   15, 0x0a,
     382,   15,   15,   15, 0x0a,
     397,   15,   15,   15, 0x09,
     414,   15,   15,   15, 0x09,
     421,   15,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MainWindowBase[] = {
    "MainWindowBase\0\0addFontpath()\0"
    "addSubstitute()\0browseFontpath()\0"
    "buildFont()\0buildPalette()\0downFontpath()\0"
    "downSubstitute()\0familySelected(QString)\0"
    "fileExit()\0fileSave()\0helpAbout()\0"
    "helpAboutQt()\0new_slot()\0pageChanged(QWidget*)\0"
    "paletteSelected(int)\0removeFontpath()\0"
    "removeSubstitute()\0somethingModified()\0"
    "styleSelected(QString)\0"
    "substituteSelected(QString)\0tunePalette()\0"
    "upFontpath()\0upSubstitute()\0"
    "languageChange()\0init()\0destroy()\0"
};

const QMetaObject MainWindowBase::staticMetaObject = {
    { &Q3MainWindow::staticMetaObject, qt_meta_stringdata_MainWindowBase,
      qt_meta_data_MainWindowBase, 0 }
};

const QMetaObject *MainWindowBase::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindowBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindowBase))
        return static_cast<void*>(const_cast< MainWindowBase*>(this));
    if (!strcmp(_clname, "Ui::MainWindowBase"))
        return static_cast< Ui::MainWindowBase*>(const_cast< MainWindowBase*>(this));
    return Q3MainWindow::qt_metacast(_clname);
}

int MainWindowBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3MainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addFontpath(); break;
        case 1: addSubstitute(); break;
        case 2: browseFontpath(); break;
        case 3: buildFont(); break;
        case 4: buildPalette(); break;
        case 5: downFontpath(); break;
        case 6: downSubstitute(); break;
        case 7: familySelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: fileExit(); break;
        case 9: fileSave(); break;
        case 10: helpAbout(); break;
        case 11: helpAboutQt(); break;
        case 12: new_slot(); break;
        case 13: pageChanged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 14: paletteSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: removeFontpath(); break;
        case 16: removeSubstitute(); break;
        case 17: somethingModified(); break;
        case 18: styleSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: substituteSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: tunePalette(); break;
        case 21: upFontpath(); break;
        case 22: upSubstitute(); break;
        case 23: languageChange(); break;
        case 24: init(); break;
        case 25: destroy(); break;
        default: ;
        }
        _id -= 26;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
