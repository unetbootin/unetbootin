/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Mar 5 20:33:46 2009
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
      27,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      23,   11,   11,   11, 0x0a,
      38,   11,   11,   11, 0x0a,
      50,   11,   11,   11, 0x0a,
      66,   11,   11,   11, 0x0a,
      79,   11,   11,   11, 0x0a,
      94,   11,   11,   11, 0x0a,
     128,   11,   11,   11, 0x08,
     146,   11,   11,   11, 0x08,
     160,   11,   11,   11, 0x08,
     174,   11,   11,   11, 0x08,
     192,   11,   11,   11, 0x08,
     209,   11,   11,   11, 0x08,
     231,  227,   11,   11, 0x08,
     251,   11,   11,   11, 0x08,
     279,  275,   11,   11, 0x08,
     310,  300,   11,   11, 0x08,
     356,  342,   11,   11, 0x08,
     401,   11,   11,   11, 0x08,
     438,  425,   11,   11, 0x08,
     467,   11,   11,   11, 0x08,
     486,   11,   11,   11, 0x08,
     514,   11,   11,   11, 0x08,
     532,   11,   11,   11, 0x08,
     564,  551,   11,   11, 0x08,
     615,  598,   11,   11, 0x08,
     646,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0initDone()\0showContents()\0"
    "showIndex()\0showBookmarks()\0showSearch()\0"
    "syncContents()\0activateCurrentCentralWidgetTab()\0"
    "insertLastPages()\0addBookmark()\0"
    "gotoAddress()\0showPreferences()\0"
    "showNewAddress()\0showAboutDialog()\0"
    "yes\0copyAvailable(bool)\0updateNavigationItems()\0"
    "url\0showNewAddress(QUrl)\0title,url\0"
    "addNewBookmark(QString,QString)\0"
    "links,keyword\0"
    "showTopicChooser(QMap<QString,QUrl>,QString)\0"
    "updateApplicationFont()\0customFilter\0"
    "filterDocumentation(QString)\0"
    "setupFilterCombo()\0lookForNewQtDocumentation()\0"
    "indexingStarted()\0indexingFinished()\0"
    "errorMessage\0displayInstallationError(QString)\0"
    "newDocsInstalled\0qtDocumentationInstalled(bool)\0"
    "checkInitState()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
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
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initDone(); break;
        case 1: showContents(); break;
        case 2: showIndex(); break;
        case 3: showBookmarks(); break;
        case 4: showSearch(); break;
        case 5: syncContents(); break;
        case 6: activateCurrentCentralWidgetTab(); break;
        case 7: insertLastPages(); break;
        case 8: addBookmark(); break;
        case 9: gotoAddress(); break;
        case 10: showPreferences(); break;
        case 11: showNewAddress(); break;
        case 12: showAboutDialog(); break;
        case 13: copyAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: updateNavigationItems(); break;
        case 15: showNewAddress((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 16: addNewBookmark((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 17: showTopicChooser((*reinterpret_cast< const QMap<QString,QUrl>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 18: updateApplicationFont(); break;
        case 19: filterDocumentation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: setupFilterCombo(); break;
        case 21: lookForNewQtDocumentation(); break;
        case 22: indexingStarted(); break;
        case 23: indexingFinished(); break;
        case 24: displayInstallationError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 25: qtDocumentationInstalled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: checkInitState(); break;
        default: ;
        }
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::initDone()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
