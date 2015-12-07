/****************************************************************************
** Meta object code from reading C++ file 'preferencesdialog.h'
**
** Created: Thu Mar 5 20:33:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../preferencesdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'preferencesdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PreferencesDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      39,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      68,   63,   18,   18, 0x08,
     103,   18,   18,   18, 0x08,
     121,   18,   18,   18, 0x08,
     133,   18,   18,   18, 0x08,
     148,   18,   18,   18, 0x08,
     172,   18,   18,   18, 0x08,
     194,   18,   18,   18, 0x08,
     212,  209,   18,   18, 0x08,
     246,  240,   18,   18, 0x08,
     273,  209,   18,   18, 0x08,
     305,  240,   18,   18, 0x08,
     336,   18,   18,   18, 0x08,
     361,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PreferencesDialog[] = {
    "PreferencesDialog\0\0updateBrowserFont()\0"
    "updateApplicationFont()\0item\0"
    "updateAttributes(QListWidgetItem*)\0"
    "updateFilterMap()\0addFilter()\0"
    "removeFilter()\0addDocumentationLocal()\0"
    "removeDocumentation()\0applyChanges()\0"
    "on\0appFontSettingToggled(bool)\0index\0"
    "appFontSettingChanged(int)\0"
    "browserFontSettingToggled(bool)\0"
    "browserFontSettingChanged(int)\0"
    "restoreDefaultHomepage()\0"
    "currentHomepageChanged()\0"
};

const QMetaObject PreferencesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PreferencesDialog,
      qt_meta_data_PreferencesDialog, 0 }
};

const QMetaObject *PreferencesDialog::metaObject() const
{
    return &staticMetaObject;
}

void *PreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PreferencesDialog))
        return static_cast<void*>(const_cast< PreferencesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateBrowserFont(); break;
        case 1: updateApplicationFont(); break;
        case 2: updateAttributes((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: updateFilterMap(); break;
        case 4: addFilter(); break;
        case 5: removeFilter(); break;
        case 6: addDocumentationLocal(); break;
        case 7: removeDocumentation(); break;
        case 8: applyChanges(); break;
        case 9: appFontSettingToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: appFontSettingChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: browserFontSettingToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: browserFontSettingChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: restoreDefaultHomepage(); break;
        case 14: currentHomepageChanged(); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void PreferencesDialog::updateBrowserFont()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void PreferencesDialog::updateApplicationFont()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
