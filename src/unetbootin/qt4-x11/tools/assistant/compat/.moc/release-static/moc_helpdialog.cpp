/****************************************************************************
** Meta object code from reading C++ file 'helpdialog.h'
**
** Created: Thu Mar 5 20:36:44 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../helpdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'helpdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SearchValidator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_SearchValidator[] = {
    "SearchValidator\0"
};

const QMetaObject SearchValidator::staticMetaObject = {
    { &QValidator::staticMetaObject, qt_meta_stringdata_SearchValidator,
      qt_meta_data_SearchValidator, 0 }
};

const QMetaObject *SearchValidator::metaObject() const
{
    return &staticMetaObject;
}

void *SearchValidator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SearchValidator))
        return static_cast<void*>(const_cast< SearchValidator*>(this));
    return QValidator::qt_metacast(_clname);
}

int SearchValidator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QValidator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_HelpDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,   12,   11,   11, 0x05,
      40,   32,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      76,   11,   11,   11, 0x0a,
      89,   11,   11,   11, 0x0a,
     103,   11,   11,   11, 0x0a,
     123,  117,   11,   11, 0x0a,
     151,  146,   11,   11, 0x0a,
     175,   11,   11,   11, 0x08,
     198,   11,   11,   11, 0x08,
     224,   11,   11,   11, 0x08,
     253,   11,   11,   11, 0x08,
     277,   11,   11,   11, 0x08,
     303,   11,   11,   11, 0x08,
     352,  348,   11,   11, 0x08,
     380,   11,   11,   11, 0x08,
     413,   11,   11,   11, 0x08,
     441,   11,   11,   11, 0x08,
     457,   11,   11,   11, 0x08,
     474,   11,   11,   11, 0x08,
     495,   11,   11,   11, 0x08,
     507,   12,   11,   11, 0x08,
     530,   11,   11,   11, 0x08,
     547,   11,   11,   11, 0x08,
     561,   11,   11,   11, 0x08,
     579,   11,   11,   11, 0x08,
     594,   11,   11,   11, 0x08,
     615,  610,   11,   11, 0x08,
     644,  640,   11,   11, 0x08,
     669,  640,   11,   11, 0x08,
     695,  640,   11,   11, 0x08,
     720,   11,   11,   11, 0x08,
     738,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HelpDialog[] = {
    "HelpDialog\0\0s\0showLink(QString)\0s,terms\0"
    "showSearchLink(QString,QStringList)\0"
    "initialize()\0startSearch()\0addBookmark()\0"
    "index\0currentTabChanged(int)\0link\0"
    "locateContents(QString)\0on_buttonAdd_clicked()\0"
    "on_buttonRemove_clicked()\0"
    "on_termsEdit_returnPressed()\0"
    "on_helpButton_clicked()\0"
    "on_searchButton_clicked()\0"
    "on_resultBox_itemActivated(QListWidgetItem*)\0"
    "txt\0updateSearchButton(QString)\0"
    "showResultPage(QListWidgetItem*)\0"
    "showTopic(QTreeWidgetItem*)\0loadIndexFile()\0"
    "insertContents()\0setupFullTextIndex()\0"
    "showTopic()\0searchInIndex(QString)\0"
    "toggleContents()\0toggleIndex()\0"
    "toggleBookmarks()\0toggleSearch()\0"
    "lastWinClosed()\0prog\0setIndexingProgress(int)\0"
    "pos\0showListItemMenu(QPoint)\0"
    "showIndexItemMenu(QPoint)\0"
    "showTreeItemMenu(QPoint)\0insertBookmarks()\0"
    "processEvents()\0"
};

const QMetaObject HelpDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_HelpDialog,
      qt_meta_data_HelpDialog, 0 }
};

const QMetaObject *HelpDialog::metaObject() const
{
    return &staticMetaObject;
}

void *HelpDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HelpDialog))
        return static_cast<void*>(const_cast< HelpDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int HelpDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showLink((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: showSearchLink((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 2: initialize(); break;
        case 3: startSearch(); break;
        case 4: addBookmark(); break;
        case 5: currentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: locateContents((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: on_buttonAdd_clicked(); break;
        case 8: on_buttonRemove_clicked(); break;
        case 9: on_termsEdit_returnPressed(); break;
        case 10: on_helpButton_clicked(); break;
        case 11: on_searchButton_clicked(); break;
        case 12: on_resultBox_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 13: updateSearchButton((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: showResultPage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 15: showTopic((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 16: loadIndexFile(); break;
        case 17: insertContents(); break;
        case 18: setupFullTextIndex(); break;
        case 19: showTopic(); break;
        case 20: searchInIndex((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: toggleContents(); break;
        case 22: toggleIndex(); break;
        case 23: toggleBookmarks(); break;
        case 24: toggleSearch(); break;
        case 25: lastWinClosed(); break;
        case 26: setIndexingProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: showListItemMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 28: showIndexItemMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 29: showTreeItemMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 30: insertBookmarks(); break;
        case 31: processEvents(); break;
        default: ;
        }
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void HelpDialog::showLink(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HelpDialog::showSearchLink(const QString & _t1, const QStringList & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
