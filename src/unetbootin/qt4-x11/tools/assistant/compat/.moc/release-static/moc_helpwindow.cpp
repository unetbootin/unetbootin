/****************************************************************************
** Meta object code from reading C++ file 'helpwindow.h'
**
** Created: Thu Mar 5 20:36:39 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../helpwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'helpwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HelpWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      31,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   11,   11,   11, 0x09,
      71,   11,   11,   11, 0x08,
      93,   11,   11,   11, 0x08,
     118,   11,  113,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HelpWindow[] = {
    "HelpWindow\0\0chooseWebBrowser()\0"
    "choosePDFReader()\0ensureCursorVisible()\0"
    "openLinkInNewWindow()\0openLinkInNewPage()\0"
    "bool\0isKDERunning()\0"
};

const QMetaObject HelpWindow::staticMetaObject = {
    { &QTextBrowser::staticMetaObject, qt_meta_stringdata_HelpWindow,
      qt_meta_data_HelpWindow, 0 }
};

const QMetaObject *HelpWindow::metaObject() const
{
    return &staticMetaObject;
}

void *HelpWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HelpWindow))
        return static_cast<void*>(const_cast< HelpWindow*>(this));
    return QTextBrowser::qt_metacast(_clname);
}

int HelpWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextBrowser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: chooseWebBrowser(); break;
        case 1: choosePDFReader(); break;
        case 2: ensureCursorVisible(); break;
        case 3: openLinkInNewWindow(); break;
        case 4: openLinkInNewPage(); break;
        case 5: { bool _r = isKDERunning();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void HelpWindow::chooseWebBrowser()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void HelpWindow::choosePDFReader()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
