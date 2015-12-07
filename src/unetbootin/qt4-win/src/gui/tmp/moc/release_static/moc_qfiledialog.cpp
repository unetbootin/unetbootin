/****************************************************************************
** Meta object code from reading C++ file 'qfiledialog.h'
**
** Created: Fri May 30 22:29:53 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qfiledialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfiledialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFileDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   10, // methods
       8,  145, // properties
       3,  169, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   13,   12,   12, 0x05,
      51,   46,   12,   12, 0x05,
      85,   75,   12,   12, 0x05,
     118,  111,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     142,   12,   12,   12, 0x08,
     166,   12,   12,   12, 0x08,
     188,   12,   12,   12, 0x08,
     209,   12,   12,   12, 0x08,
     231,   12,   12,   12, 0x08,
     252,   12,   12,   12, 0x08,
     270,   12,   12,   12, 0x08,
     291,   12,   12,   12, 0x08,
     318,   12,   12,   12, 0x08,
     337,   12,   12,   12, 0x08,
     356,   12,   12,   12, 0x08,
     372,   12,   12,   12, 0x08,
     398,  392,   12,   12, 0x08,
     429,  392,   12,   12, 0x08,
     460,   46,   12,   12, 0x08,
     486,  392,   12,   12, 0x08,
     508,   12,   12,   12, 0x08,
     534,  530,   12,   12, 0x08,
     551,   12,   12,   12, 0x08,
     563,   12,   12,   12, 0x08,
     592,  587,   12,   12, 0x08,
     632,  625,   12,   12, 0x08,
     682,  661,   12,   12, 0x08,

 // properties: name, type, flags
     731,  722, 0x0009510b,
     749,  740, 0x0009510b,
     769,  758, 0x0009510b,
     785,  780, 0x01095103,
     794,  780, 0x01095103,
     810,  780, 0x01095103,
     835,  827, 0x0a095103,
     849,  780, 0x01095103,

 // enums: name, flags, count, data
     722, 0x0,    2,  181,
     740, 0x0,    5,  185,
     758, 0x0,    2,  195,

 // enum data: key, value
     874, uint(QFileDialog::Detail),
     881, uint(QFileDialog::List),
     886, uint(QFileDialog::AnyFile),
     894, uint(QFileDialog::ExistingFile),
     907, uint(QFileDialog::Directory),
     917, uint(QFileDialog::ExistingFiles),
     931, uint(QFileDialog::DirectoryOnly),
     945, uint(QFileDialog::AcceptOpen),
     956, uint(QFileDialog::AcceptSave),

       0        // eod
};

static const char qt_meta_stringdata_QFileDialog[] = {
    "QFileDialog\0\0files\0filesSelected(QStringList)\0"
    "path\0currentChanged(QString)\0directory\0"
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
    "Detail\0List\0AnyFile\0ExistingFile\0"
    "Directory\0ExistingFiles\0DirectoryOnly\0"
    "AcceptOpen\0AcceptSave\0"
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
        case 0: filesSelected((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: currentChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: directoryEntered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: filterSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: d_func()->_q_pathChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: d_func()->_q_navigateBackward(); break;
        case 6: d_func()->_q_navigateForward(); break;
        case 7: d_func()->_q_navigateToParent(); break;
        case 8: d_func()->_q_createDirectory(); break;
        case 9: d_func()->_q_showListView(); break;
        case 10: d_func()->_q_showDetailsView(); break;
        case 11: d_func()->_q_showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 12: d_func()->_q_renameCurrent(); break;
        case 13: d_func()->_q_deleteCurrent(); break;
        case 14: d_func()->_q_showHidden(); break;
        case 15: d_func()->_q_updateOkButton(); break;
        case 16: d_func()->_q_currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 17: d_func()->_q_enterDirectory((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 18: d_func()->_q_goToDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: d_func()->_q_useNameFilter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: d_func()->_q_selectionChanged(); break;
        case 21: d_func()->_q_goToUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 22: d_func()->_q_goHome(); break;
        case 23: d_func()->_q_showHeader((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 24: d_func()->_q_autoCompleteFileName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 25: d_func()->_q_rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 26: d_func()->_q_fileRenamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        }
        _id -= 27;
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
        }
        _id -= 8;
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
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QFileDialog::filesSelected(const QStringList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFileDialog::currentChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFileDialog::directoryEntered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QFileDialog::filterSelected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
