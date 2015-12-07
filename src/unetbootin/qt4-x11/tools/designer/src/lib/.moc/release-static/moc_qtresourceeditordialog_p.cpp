/****************************************************************************
** Meta object code from reading C++ file 'qtresourceeditordialog_p.h'
**
** Created: Thu Mar 5 20:46:07 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/qtresourceeditordialog_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtresourceeditordialog_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtResourceEditorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,
      33,   23,   23,   23, 0x08,
      65,   23,   23,   23, 0x08,
      94,   23,   23,   23, 0x08,
     125,   23,   23,   23, 0x08,
     171,   23,   23,   23, 0x08,
     214,   23,   23,   23, 0x08,
     259,   23,   23,   23, 0x08,
     306,   23,   23,   23, 0x08,
     351,   23,   23,   23, 0x08,
     393,   23,   23,   23, 0x08,
     432,   23,   23,   23, 0x08,
     474,   23,   23,   23, 0x08,
     515,   23,   23,   23, 0x08,
     559,   23,   23,   23, 0x08,
     603,   23,   23,   23, 0x08,
     646,   23,   23,   23, 0x08,
     687,   23,   23,   23, 0x08,
     727,   23,   23,   23, 0x08,
     744,   23,   23,   23, 0x08,
     764,   23,   23,   23, 0x08,
     784,   23,   23,   23, 0x08,
     804,   23,   23,   23, 0x08,
     826,   23,   23,   23, 0x08,
     842,   23,   23,   23, 0x08,
     857,   23,   23,   23, 0x08,
     876,   23,   23,   23, 0x08,
     897,   23,   23,   23, 0x08,
     915,   23,   23,   23, 0x08,
     933,   23,   23,   23, 0x08,
     946,   23,   23,   23, 0x08,
     959,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtResourceEditorDialog[] = {
    "QtResourceEditorDialog\0\0accept()\0"
    "slotQrcFileInserted(QtQrcFile*)\0"
    "slotQrcFileMoved(QtQrcFile*)\0"
    "slotQrcFileRemoved(QtQrcFile*)\0"
    "slotResourcePrefixInserted(QtResourcePrefix*)\0"
    "slotResourcePrefixMoved(QtResourcePrefix*)\0"
    "slotResourcePrefixChanged(QtResourcePrefix*)\0"
    "slotResourceLanguageChanged(QtResourcePrefix*)\0"
    "slotResourcePrefixRemoved(QtResourcePrefix*)\0"
    "slotResourceFileInserted(QtResourceFile*)\0"
    "slotResourceFileMoved(QtResourceFile*)\0"
    "slotResourceAliasChanged(QtResourceFile*)\0"
    "slotResourceFileRemoved(QtResourceFile*)\0"
    "slotCurrentQrcFileChanged(QListWidgetItem*)\0"
    "slotCurrentTreeViewItemChanged(QModelIndex)\0"
    "slotListWidgetContextMenuRequested(QPoint)\0"
    "slotTreeViewContextMenuRequested(QPoint)\0"
    "slotTreeViewItemChanged(QStandardItem*)\0"
    "slotNewQrcFile()\0slotImportQrcFile()\0"
    "slotRemoveQrcFile()\0slotMoveUpQrcFile()\0"
    "slotMoveDownQrcFile()\0slotNewPrefix()\0"
    "slotAddFiles()\0slotChangePrefix()\0"
    "slotChangeLanguage()\0slotChangeAlias()\0"
    "slotClonePrefix()\0slotRemove()\0"
    "slotMoveUp()\0slotMoveDown()\0"
};

const QMetaObject QtResourceEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QtResourceEditorDialog,
      qt_meta_data_QtResourceEditorDialog, 0 }
};

const QMetaObject *QtResourceEditorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QtResourceEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtResourceEditorDialog))
        return static_cast<void*>(const_cast< QtResourceEditorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QtResourceEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: d_func()->slotQrcFileInserted((*reinterpret_cast< QtQrcFile*(*)>(_a[1]))); break;
        case 2: d_func()->slotQrcFileMoved((*reinterpret_cast< QtQrcFile*(*)>(_a[1]))); break;
        case 3: d_func()->slotQrcFileRemoved((*reinterpret_cast< QtQrcFile*(*)>(_a[1]))); break;
        case 4: d_func()->slotResourcePrefixInserted((*reinterpret_cast< QtResourcePrefix*(*)>(_a[1]))); break;
        case 5: d_func()->slotResourcePrefixMoved((*reinterpret_cast< QtResourcePrefix*(*)>(_a[1]))); break;
        case 6: d_func()->slotResourcePrefixChanged((*reinterpret_cast< QtResourcePrefix*(*)>(_a[1]))); break;
        case 7: d_func()->slotResourceLanguageChanged((*reinterpret_cast< QtResourcePrefix*(*)>(_a[1]))); break;
        case 8: d_func()->slotResourcePrefixRemoved((*reinterpret_cast< QtResourcePrefix*(*)>(_a[1]))); break;
        case 9: d_func()->slotResourceFileInserted((*reinterpret_cast< QtResourceFile*(*)>(_a[1]))); break;
        case 10: d_func()->slotResourceFileMoved((*reinterpret_cast< QtResourceFile*(*)>(_a[1]))); break;
        case 11: d_func()->slotResourceAliasChanged((*reinterpret_cast< QtResourceFile*(*)>(_a[1]))); break;
        case 12: d_func()->slotResourceFileRemoved((*reinterpret_cast< QtResourceFile*(*)>(_a[1]))); break;
        case 13: d_func()->slotCurrentQrcFileChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 14: d_func()->slotCurrentTreeViewItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 15: d_func()->slotListWidgetContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 16: d_func()->slotTreeViewContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 17: d_func()->slotTreeViewItemChanged((*reinterpret_cast< QStandardItem*(*)>(_a[1]))); break;
        case 18: d_func()->slotNewQrcFile(); break;
        case 19: d_func()->slotImportQrcFile(); break;
        case 20: d_func()->slotRemoveQrcFile(); break;
        case 21: d_func()->slotMoveUpQrcFile(); break;
        case 22: d_func()->slotMoveDownQrcFile(); break;
        case 23: d_func()->slotNewPrefix(); break;
        case 24: d_func()->slotAddFiles(); break;
        case 25: d_func()->slotChangePrefix(); break;
        case 26: d_func()->slotChangeLanguage(); break;
        case 27: d_func()->slotChangeAlias(); break;
        case 28: d_func()->slotClonePrefix(); break;
        case 29: d_func()->slotRemove(); break;
        case 30: d_func()->slotMoveUp(); break;
        case 31: d_func()->slotMoveDown(); break;
        default: ;
        }
        _id -= 32;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
