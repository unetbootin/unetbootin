/****************************************************************************
** Meta object code from reading C++ file 'q3filedialog.h'
**
** Created: Fri May 30 22:39:24 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/q3filedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3filedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3FileIconProvider[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_Q3FileIconProvider[] = {
    "Q3FileIconProvider\0"
};

const QMetaObject Q3FileIconProvider::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Q3FileIconProvider,
      qt_meta_data_Q3FileIconProvider, 0 }
};

const QMetaObject *Q3FileIconProvider::metaObject() const
{
    return &staticMetaObject;
}

void *Q3FileIconProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3FileIconProvider))
	return static_cast<void*>(const_cast< Q3FileIconProvider*>(this));
    return QObject::qt_metacast(_clname);
}

int Q3FileIconProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Q3FileDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   10, // methods
      10,  240, // properties
       3,  270, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      39,   13,   13,   13, 0x05,
      61,   13,   13,   13, 0x05,
      88,   13,   13,   13, 0x05,
     108,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     132,   13,   13,   13, 0x0a,
     142,   13,   13,   13, 0x0a,
     162,  158,   13,   13, 0x0a,
     184,   13,   13,   13, 0x0a,
     203,   13,   13,   13, 0x0a,
     223,   13,   13,   13, 0x0a,
     248,   13,   13,   13, 0x0a,
     272,   13,   13,   13, 0x08,
     301,   13,   13,   13, 0x08,
     327,   13,   13,   13, 0x08,
     343,   13,   13,   13, 0x08,
     371,   13,   13,   13, 0x08,
     382,   13,   13,   13, 0x08,
     402,   13,   13,   13, 0x08,
     420,   13,   13,   13, 0x08,
     441,   13,   13,   13, 0x08,
     458,   13,   13,   13, 0x08,
     476,   13,   13,   13, 0x08,
     512,   13,   13,   13, 0x08,
     554,  551,   13,   13, 0x08,
     601,  599,   13,   13, 0x08,
     641,   13,   13,   13, 0x08,
     676,   13,   13,   13, 0x08,
     714,   13,   13,   13, 0x08,
     733,   13,   13,   13, 0x08,
     745,   13,   13,   13, 0x08,
     761,   13,   13,   13, 0x08,
     777,   13,   13,   13, 0x08,
     791,   13,   13,   13, 0x08,
     810,   13,   13,   13, 0x08,
     826,   13,   13,   13, 0x08,
     845,   13,   13,   13, 0x08,
     864,   13,   13,   13, 0x08,
     890,  887,   13,   13, 0x08,
     920,  887,   13,   13, 0x08,
     975,  953,   13,   13, 0x08,
    1031, 1025,   13,   13, 0x08,
    1086,   13,   13,   13, 0x08,
    1125, 1119,   13,   13, 0x08,
    1172,   13,   13,   13, 0x08,
    1205,   13,   13,   13, 0x08,

 // properties: name, type, flags
    1222, 1214, 0x0a095001,
    1235, 1214, 0x0a095001,
    1262, 1250, 0x0b095001,
    1276, 1214, 0x0a095001,
    1289, 1284, 0x01095103,
    1310, 1305, 0x0009510b,
    1324, 1315, 0x0009510b,
    1345, 1333, 0x0009510b,
    1357, 1284, 0x01095003,
    1369, 1284, 0x01095003,

 // enums: name, flags, count, data
    1305, 0x0,    5,  282,
    1315, 0x0,    2,  292,
    1333, 0x0,    3,  296,

 // enum data: key, value
    1385, uint(Q3FileDialog::AnyFile),
    1393, uint(Q3FileDialog::ExistingFile),
    1406, uint(Q3FileDialog::Directory),
    1416, uint(Q3FileDialog::ExistingFiles),
    1430, uint(Q3FileDialog::DirectoryOnly),
    1444, uint(Q3FileDialog::Detail),
    1451, uint(Q3FileDialog::List),
    1456, uint(Q3FileDialog::NoPreview),
    1466, uint(Q3FileDialog::Contents),
    1475, uint(Q3FileDialog::Info),

       0        // eod
};

static const char qt_meta_stringdata_Q3FileDialog[] = {
    "Q3FileDialog\0\0fileHighlighted(QString)\0"
    "fileSelected(QString)\0filesSelected(QStringList)\0"
    "dirEntered(QString)\0filterSelected(QString)\0"
    "done(int)\0setDir(QString)\0url\0"
    "setUrl(Q3UrlOperator)\0setFilter(QString)\0"
    "setFilters(QString)\0setFilters(const char**)\0"
    "setFilters(QStringList)\0"
    "detailViewSelectionChanged()\0"
    "listBoxSelectionChanged()\0changeMode(int)\0"
    "fileNameEditReturnPressed()\0stopCopy()\0"
    "removeProgressDia()\0fileSelected(int)\0"
    "fileHighlighted(int)\0dirSelected(int)\0"
    "pathSelected(int)\0updateFileNameEdit(Q3ListViewItem*)\0"
    "selectDirectoryOrFile(Q3ListViewItem*)\0"
    ",,\0popupContextMenu(Q3ListViewItem*,QPoint,int)\0"
    ",\0popupContextMenu(Q3ListBoxItem*,QPoint)\0"
    "updateFileNameEdit(Q3ListBoxItem*)\0"
    "selectDirectoryOrFile(Q3ListBoxItem*)\0"
    "fileNameEditDone()\0okClicked()\0"
    "filterClicked()\0cancelClicked()\0"
    "cdUpClicked()\0newFolderClicked()\0"
    "fixupNameEdit()\0doMimeTypeLookup()\0"
    "updateGeometries()\0modeButtonsDestroyed()\0"
    "op\0urlStart(Q3NetworkOperation*)\0"
    "urlFinished(Q3NetworkOperation*)\0"
    "bytesDone,bytesTotal,\0"
    "dataTransferProgress(int,int,Q3NetworkOperation*)\0"
    "fi,op\0insertEntry(Q3ValueList<QUrlInfo>,Q3NetworkOperation*)\0"
    "removeEntry(Q3NetworkOperation*)\0info,\0"
    "createdDirectory(QUrlInfo,Q3NetworkOperation*)\0"
    "itemChanged(Q3NetworkOperation*)\0"
    "goBack()\0QString\0selectedFile\0"
    "selectedFilter\0QStringList\0selectedFiles\0"
    "dirPath\0bool\0showHiddenFiles\0Mode\0"
    "mode\0ViewMode\0viewMode\0PreviewMode\0"
    "previewMode\0infoPreview\0contentsPreview\0"
    "AnyFile\0ExistingFile\0Directory\0"
    "ExistingFiles\0DirectoryOnly\0Detail\0"
    "List\0NoPreview\0Contents\0Info\0"
};

const QMetaObject Q3FileDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Q3FileDialog,
      qt_meta_data_Q3FileDialog, 0 }
};

const QMetaObject *Q3FileDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Q3FileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3FileDialog))
	return static_cast<void*>(const_cast< Q3FileDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Q3FileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileHighlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: fileSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: filesSelected((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 3: dirEntered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: filterSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: done((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: setDir((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: setUrl((*reinterpret_cast< const Q3UrlOperator(*)>(_a[1]))); break;
        case 8: setFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: setFilters((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: setFilters((*reinterpret_cast< const char**(*)>(_a[1]))); break;
        case 11: setFilters((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 12: detailViewSelectionChanged(); break;
        case 13: listBoxSelectionChanged(); break;
        case 14: changeMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: fileNameEditReturnPressed(); break;
        case 16: stopCopy(); break;
        case 17: removeProgressDia(); break;
        case 18: fileSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: fileHighlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: dirSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: pathSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: updateFileNameEdit((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 23: selectDirectoryOrFile((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1]))); break;
        case 24: popupContextMenu((*reinterpret_cast< Q3ListViewItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 25: popupContextMenu((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 26: updateFileNameEdit((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 27: selectDirectoryOrFile((*reinterpret_cast< Q3ListBoxItem*(*)>(_a[1]))); break;
        case 28: fileNameEditDone(); break;
        case 29: okClicked(); break;
        case 30: filterClicked(); break;
        case 31: cancelClicked(); break;
        case 32: cdUpClicked(); break;
        case 33: newFolderClicked(); break;
        case 34: fixupNameEdit(); break;
        case 35: doMimeTypeLookup(); break;
        case 36: updateGeometries(); break;
        case 37: modeButtonsDestroyed(); break;
        case 38: urlStart((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 39: urlFinished((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 40: dataTransferProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[3]))); break;
        case 41: insertEntry((*reinterpret_cast< const Q3ValueList<QUrlInfo>(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 42: removeEntry((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 43: createdDirectory((*reinterpret_cast< const QUrlInfo(*)>(_a[1])),(*reinterpret_cast< Q3NetworkOperation*(*)>(_a[2]))); break;
        case 44: itemChanged((*reinterpret_cast< Q3NetworkOperation*(*)>(_a[1]))); break;
        case 45: goBack(); break;
        }
        _id -= 46;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = selectedFile(); break;
        case 1: *reinterpret_cast< QString*>(_v) = selectedFilter(); break;
        case 2: *reinterpret_cast< QStringList*>(_v) = selectedFiles(); break;
        case 3: *reinterpret_cast< QString*>(_v) = dirPath(); break;
        case 4: *reinterpret_cast< bool*>(_v) = showHiddenFiles(); break;
        case 5: *reinterpret_cast< Mode*>(_v) = mode(); break;
        case 6: *reinterpret_cast< ViewMode*>(_v) = viewMode(); break;
        case 7: *reinterpret_cast< PreviewMode*>(_v) = previewMode(); break;
        case 8: *reinterpret_cast< bool*>(_v) = isInfoPreviewEnabled(); break;
        case 9: *reinterpret_cast< bool*>(_v) = isContentsPreviewEnabled(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 4: setShowHiddenFiles(*reinterpret_cast< bool*>(_v)); break;
        case 5: setMode(*reinterpret_cast< Mode*>(_v)); break;
        case 6: setViewMode(*reinterpret_cast< ViewMode*>(_v)); break;
        case 7: setPreviewMode(*reinterpret_cast< PreviewMode*>(_v)); break;
        case 8: setInfoPreviewEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 9: setContentsPreviewEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3FileDialog::fileHighlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3FileDialog::fileSelected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3FileDialog::filesSelected(const QStringList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3FileDialog::dirEntered(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3FileDialog::filterSelected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
