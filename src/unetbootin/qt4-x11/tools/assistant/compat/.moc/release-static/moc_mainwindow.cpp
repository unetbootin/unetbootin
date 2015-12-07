/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Mar 5 20:36:47 2009
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
      31,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      24,   11,   12,   11, 0x0a,
      36,   11,   11,   11, 0x0a,
      49,   44,   11,   11, 0x0a,
      73,   67,   11,   11, 0x0a,
      96,   11,   11,   11, 0x0a,
     111,   11,   11,   11, 0x0a,
     132,   11,   11,   11, 0x0a,
     151,   11,   11,   11, 0x08,
     182,   11,   11,   11, 0x08,
     210,   11,   11,   11, 0x08,
     241,   11,   11,   11, 0x08,
     268,   11,   11,   11, 0x08,
     303,   11,   11,   11, 0x08,
     338,   11,   11,   11, 0x08,
     376,   11,   11,   11, 0x08,
     412,   11,   11,   11, 0x08,
     440,   11,   11,   11, 0x08,
     469,   11,   11,   11, 0x08,
     477,   11,   11,   11, 0x08,
     504,  497,   11,   11, 0x08,
     527,   44,   11,   11, 0x08,
     555,   11,   11,   11, 0x08,
     579,  568,   11,   11, 0x08,
     615,   11,   11,   11, 0x08,
     634,   11,   11,   11, 0x08,
     658,   11,   11,   11, 0x08,
     682,   11,   11,   11, 0x08,
     705,   11,   11,   11, 0x08,
     729,  725,   11,   11, 0x08,
     755,  749,   11,   11, 0x08,
     777,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0MainWindow*\0newWindow()\0"
    "setup()\0link\0showLink(QString)\0links\0"
    "showLinks(QStringList)\0saveSettings()\0"
    "updateBookmarkMenu()\0printingFinished()\0"
    "on_actionNewWindow_triggered()\0"
    "on_actionGoHome_triggered()\0"
    "on_actionFilePrint_triggered()\0"
    "on_actionClose_triggered()\0"
    "on_actionHelpWhatsThis_triggered()\0"
    "on_actionHelpAssistant_triggered()\0"
    "on_actionAboutApplication_triggered()\0"
    "on_actionAboutAssistant_triggered()\0"
    "on_actionSaveAs_triggered()\0"
    "on_actionSyncToc_triggered()\0about()\0"
    "setupBookmarkMenu()\0action\0"
    "showBookmark(QAction*)\0"
    "showLinkFromClient(QString)\0showQtHelp()\0"
    "link,terms\0showSearchLink(QString,QStringList)\0"
    "showGoActionLink()\0updateProfileSettings()\0"
    "backwardAvailable(bool)\0forwardAvailable(bool)\0"
    "browserTabChanged()\0yes\0copyAvailable(bool)\0"
    "index\0updateTabActions(int)\0"
    "showFontSettingsDialog()\0"
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
        case 0: { MainWindow* _r = newWindow();
            if (_a[0]) *reinterpret_cast< MainWindow**>(_a[0]) = _r; }  break;
        case 1: setup(); break;
        case 2: showLink((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: showLinks((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 4: saveSettings(); break;
        case 5: updateBookmarkMenu(); break;
        case 6: printingFinished(); break;
        case 7: on_actionNewWindow_triggered(); break;
        case 8: on_actionGoHome_triggered(); break;
        case 9: on_actionFilePrint_triggered(); break;
        case 10: on_actionClose_triggered(); break;
        case 11: on_actionHelpWhatsThis_triggered(); break;
        case 12: on_actionHelpAssistant_triggered(); break;
        case 13: on_actionAboutApplication_triggered(); break;
        case 14: on_actionAboutAssistant_triggered(); break;
        case 15: on_actionSaveAs_triggered(); break;
        case 16: on_actionSyncToc_triggered(); break;
        case 17: about(); break;
        case 18: setupBookmarkMenu(); break;
        case 19: showBookmark((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 20: showLinkFromClient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: showQtHelp(); break;
        case 22: showSearchLink((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 23: showGoActionLink(); break;
        case 24: updateProfileSettings(); break;
        case 25: backwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 26: forwardAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: browserTabChanged(); break;
        case 28: copyAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: updateTabActions((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: showFontSettingsDialog(); break;
        default: ;
        }
        _id -= 31;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
