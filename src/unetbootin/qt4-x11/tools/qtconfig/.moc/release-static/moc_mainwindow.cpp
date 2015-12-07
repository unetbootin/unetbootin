/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Mar 5 20:59:34 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      27,   11,   11,   11, 0x0a,
      39,   11,   11,   11, 0x0a,
      53,   11,   11,   11, 0x0a,
      74,   11,   11,   11, 0x0a,
      97,   11,   11,   11, 0x0a,
     121,   11,   11,   11, 0x0a,
     149,   11,   11,   11, 0x0a,
     168,   11,   11,   11, 0x0a,
     184,   11,   11,   11, 0x0a,
     201,   11,   11,   11, 0x0a,
     216,   11,   11,   11, 0x0a,
     233,   11,   11,   11, 0x0a,
     247,   11,   11,   11, 0x0a,
     262,   11,   11,   11, 0x0a,
     275,   11,   11,   11, 0x0a,
     292,   11,   11,   11, 0x0a,
     303,   11,   11,   11, 0x0a,
     314,   11,   11,   11, 0x0a,
     334,   11,   11,   11, 0x0a,
     346,   11,   11,   11, 0x0a,
     360,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0buildPalette()\0buildFont()\0"
    "tunePalette()\0paletteSelected(int)\0"
    "styleSelected(QString)\0familySelected(QString)\0"
    "substituteSelected(QString)\0"
    "removeSubstitute()\0addSubstitute()\0"
    "downSubstitute()\0upSubstitute()\0"
    "removeFontpath()\0addFontpath()\0"
    "downFontpath()\0upFontpath()\0"
    "browseFontpath()\0fileSave()\0fileExit()\0"
    "somethingModified()\0helpAbout()\0"
    "helpAboutQt()\0pageChanged(QWidget*)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &MainWindowBase::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return MainWindowBase::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainWindowBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: buildPalette(); break;
        case 1: buildFont(); break;
        case 2: tunePalette(); break;
        case 3: paletteSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: styleSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: familySelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: substituteSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: removeSubstitute(); break;
        case 8: addSubstitute(); break;
        case 9: downSubstitute(); break;
        case 10: upSubstitute(); break;
        case 11: removeFontpath(); break;
        case 12: addFontpath(); break;
        case 13: downFontpath(); break;
        case 14: upFontpath(); break;
        case 15: browseFontpath(); break;
        case 16: fileSave(); break;
        case 17: fileExit(); break;
        case 18: somethingModified(); break;
        case 19: helpAbout(); break;
        case 20: helpAboutQt(); break;
        case 21: pageChanged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
