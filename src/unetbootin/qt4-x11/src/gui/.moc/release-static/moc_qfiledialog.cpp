/****************************************************************************
** Meta object code from reading C++ file 'qfiledialog.h'
**
** Created: Thu Mar 5 18:53:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dialogs/qfiledialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfiledialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFileDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   12, // methods
       9,  152, // properties
       4,  179, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x05,
      46,   40,   12,   12, 0x05,
      78,   73,   12,   12, 0x05,
     112,  102,   12,   12, 0x05,
     145,  138,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     169,   12,   12,   12, 0x08,
     193,   12,   12,   12, 0x08,
     215,   12,   12,   12, 0x08,
     236,   12,   12,   12, 0x08,
     258,   12,   12,   12, 0x08,
     279,   12,   12,   12, 0x08,
     297,   12,   12,   12, 0x08,
     318,   12,   12,   12, 0x08,
     345,   12,   12,   12, 0x08,
     364,   12,   12,   12, 0x08,
     383,   12,   12,   12, 0x08,
     399,   12,   12,   12, 0x08,
     425,  419,   12,   12, 0x08,
     456,  419,   12,   12, 0x08,
     487,   73,   12,   12, 0x08,
     513,  419,   12,   12, 0x08,
     535,   12,   12,   12, 0x08,
     561,  557,   12,   12, 0x08,
     578,   12,   12,   12, 0x08,
     590,   12,   12,   12, 0x08,
     619,  614,   12,   12, 0x08,
     659,  652,   12,   12, 0x08,
     709,  688,   12,   12, 0x08,

 // properties: name, type, flags
     758,  749, 0x0009510b,
     776,  767, 0x0009510b,
     796,  785, 0x0009510b,
     812,  807, 0x01094103,
     821,  807, 0x01094103,
     837,  807, 0x01094103,
     862,  854, 0x0a095103,
     876,  807, 0x01094103,
     909,  901, 0x0009510b,

 // enums: name, flags, count, data
     749, 0x0,    2,  195,
     767, 0x0,    5,  199,
     785, 0x0,    2,  209,
     917, 0x0,    7,  213,

 // enum data: key, value
     924, uint(QFileDialog::Detail),
     931, uint(QFileDialog::List),
     936, uint(QFileDialog::AnyFile),
     944, uint(QFileDialog::ExistingFile),
     957, uint(QFileDialog::Directory),
     967, uint(QFileDialog::ExistingFiles),
     981, uint(QFileDialog::DirectoryOnly),
     995, uint(QFileDialog::AcceptOpen),
    1006, uint(QFileDialog::AcceptSave),
    1017, uint(QFileDialog::ShowDirsOnly),
    1030, uint(QFileDialog::DontResolveSymlinks),
    1050, uint(QFileDialog::DontConfirmOverwrite),
    1071, uint(QFileDialog::DontUseSheet),
    1084, uint(QFileDialog::DontUseNativeDialog),
    1104, uint(QFileDialog::ReadOnly),
    1113, uint(QFileDialog::HideNameFilterDetails),

       0        // eod
};

static const char qt_meta_stringdata_QFileDialog[] = {
    "QFileDialog\0\0file\0fileSelected(QString)\0"
    "files\0filesSelected(QStringList)\0path\0"
    "currentChanged(QString)\0directory\0"
    "directoryEntered(QString)\0filter\0"
    "filterSelected(QString)\0_q_pathChanged(QString)\0"
    "_q_navigateBackward()\0_q_navigateForward()\0"
    "_q_navigateToParent()\0_q_createDirectory()\0"
    "_q_showListView()\0_q_showDetailsView()\0"
    "_q_showContextMenu(QPoint)\0"
    "_q_renameCurrent()\0_q_deleteCurrent()\0"
    "_q_showHidden()\0_q_updateOkButton()\0"
    "index\0_q_currentChanged(QModelIndex)\0"
    "_q_enterDirectory(QModelIndex)\0"
    "_q_goToDirectory(QString)\0"
    "_q_useNameFilter(int)\0_q_selectionChanged()\0"
    "url\0_q_goToUrl(QUrl)\0_q_goHome()\0"
    "_q_showHeader(QAction*)\0text\0"
    "_q_autoCompleteFileName(QString)\0"
    "parent\0_q_rowsInserted(QModelIndex)\0"
    "path,oldName,newName\0"
    "_q_fileRenamed(QString,QString,QString)\0"
    "ViewMode\0viewMode\0FileMode\0fileMode\0"
    "AcceptMode\0acceptMode\0bool\0readOnly\0"
    "resolveSymlinks\0confirmOverwrite\0"
    "QString\0defaultSuffix\0nameFilterDetailsVisible\0"
    "Options\0options\0Option\0Detail\0List\0"
    "AnyFile\0ExistingFile\0Directory\0"
    "ExistingFiles\0DirectoryOnly\0AcceptOpen\0"
    "AcceptSave\0ShowDirsOnly\0DontResolveSymlinks\0"
    "DontConfirmOverwrite\0DontUseSheet\0"
    "DontUseNativeDialog\0ReadOnly\0"
    "HideNameFilterDetails\0"
};

const QMetaObject QFileDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QFileDialog,
      qt_meta_data_QFileDialog, 0 }
};

const QMetaObject *QFileDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QFileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFileDialog))
        return static_cast<void*>(const_cast< QFileDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QFileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: filesSelected((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 2: currentChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: directoryEntered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: filterSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: d_func()->_q_pathChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: d_func()->_q_navigateBackward(); break;
        case 7: d_func()->_q_navigateForward(); break;
        case 8: d_func()->_q_navigateToParent(); break;
        case 9: d_func()->_q_createDirectory(); break;
        case 10: d_func()->_q_showListView(); break;
        case 11: d_func()->_q_showDetailsView(); break;
        case 12: d_func()->_q_showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 13: d_func()->_q_renameCurrent(); break;
        case 14: d_func()->_q_deleteCurrent(); break;
        case 15: d_func()->_q_showHidden(); break;
        case 16: d_func()->_q_updateOkButton(); break;
        case 17: d_func()->_q_currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 18: d_func()->_q_enterDirectory((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 19: d_func()->_q_goToDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: d_func()->_q_useNameFilter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: d_func()->_q_selectionChanged(); break;
        case 22: d_func()->_q_goToUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 23: d_func()->_q_goHome(); break;
        case 24: d_func()->_q_showHeader((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 25: d_func()->_q_autoCompleteFileName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: d_func()->_q_rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 27: d_func()->_q_fileRenamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 28;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ViewMode*>(_v) = viewMode(); break;
        case 1: *reinterpret_cast< FileMode*>(_v) = fileMode(); break;
        case 2: *reinterpret_cast< AcceptMode*>(_v) = acceptMode(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isReadOnly(); break;
        case 4: *reinterpret_cast< bool*>(_v) = resolveSymlinks(); break;
        case 5: *reinterpret_cast< bool*>(_v) = confirmOverwrite(); break;
        case 6: *reinterpret_cast< QString*>(_v) = defaultSuffix(); break;
        case 7: *reinterpret_cast< bool*>(_v) = isNameFilterDetailsVisible(); break;
        case 8: *reinterpret_cast< Options*>(_v) = options(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setViewMode(*reinterpret_cast< ViewMode*>(_v)); break;
        case 1: setFileMode(*reinterpret_cast< FileMode*>(_v)); break;
        case 2: setAcceptMode(*reinterpret_cast< AcceptMode*>(_v)); break;
        case 3: setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 4: setResolveSymlinks(*reinterpret_cast< bool*>(_v)); break;
        case 5: setConfirmOverwrite(*reinterpret_cast< bool*>(_v)); break;
        case 6: setDefaultSuffix(*reinterpret_cast< QString*>(_v)); break;
        case 7: setNameFilterDetailsVisible(*reinterpret_cast< bool*>(_v)); break;
        case 8: setOptions(*reinterpret_cast< Options*>(_v)); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QFileDialog::fileSelected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFileDialog::filesSelected(const QStringList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFileDialog::currentChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QFileDialog::directoryEntered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QFileDialog::filterSelected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
