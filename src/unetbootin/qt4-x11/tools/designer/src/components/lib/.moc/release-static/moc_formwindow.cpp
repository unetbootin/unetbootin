/****************************************************************************
** Meta object code from reading C++ file 'formwindow.h'
**
** Created: Thu Mar 5 20:52:02 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../formeditor/formwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'formwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__FormWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      44,   32,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
      82,   31,   31,   31, 0x0a,
      98,   31,   31,   31, 0x0a,
     113,   31,   31,   31, 0x0a,
     128,   31,   31,   31, 0x0a,
     135,   31,   31,   31, 0x0a,
     141,   31,   31,   31, 0x0a,
     149,   31,   31,   31, 0x0a,
     176,  161,   31,   31, 0x0a,
     208,  203,   31,   31, 0x2a,
     244,  226,   31,   31, 0x0a,
     272,  270,   31,   31, 0x0a,
     294,   31,   31,   31, 0x0a,
     309,   31,   31,   31, 0x08,
     337,   31,   31,   31, 0x08,
     351,   31,   31,   31, 0x08,
     368,   31,   31,   31, 0x08,
     388,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__FormWindow[] = {
    "qdesigner_internal::FormWindow\0\0"
    "menu,widget\0contextMenuRequested(QMenu*,QWidget*)\0"
    "deleteWidgets()\0raiseWidgets()\0"
    "lowerWidgets()\0copy()\0cut()\0paste()\0"
    "selectAll()\0type,container\0"
    "createLayout(int,QWidget*)\0type\0"
    "createLayout(int)\0container,newType\0"
    "morphLayout(QWidget*,int)\0w\0"
    "breakLayout(QWidget*)\0editContents()\0"
    "selectionChangedTimerDone()\0updateDirty()\0"
    "checkSelection()\0checkSelectionNow()\0"
    "slotSelectWidget(QAction*)\0"
};

const QMetaObject qdesigner_internal::FormWindow::staticMetaObject = {
    { &FormWindowBase::staticMetaObject, qt_meta_stringdata_qdesigner_internal__FormWindow,
      qt_meta_data_qdesigner_internal__FormWindow, 0 }
};

const QMetaObject *qdesigner_internal::FormWindow::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::FormWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__FormWindow))
        return static_cast<void*>(const_cast< FormWindow*>(this));
    return FormWindowBase::qt_metacast(_clname);
}

int qdesigner_internal::FormWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FormWindowBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contextMenuRequested((*reinterpret_cast< QMenu*(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2]))); break;
        case 1: deleteWidgets(); break;
        case 2: raiseWidgets(); break;
        case 3: lowerWidgets(); break;
        case 4: copy(); break;
        case 5: cut(); break;
        case 6: paste(); break;
        case 7: selectAll(); break;
        case 8: createLayout((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2]))); break;
        case 9: createLayout((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: morphLayout((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: breakLayout((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 12: editContents(); break;
        case 13: selectionChangedTimerDone(); break;
        case 14: updateDirty(); break;
        case 15: checkSelection(); break;
        case 16: checkSelectionNow(); break;
        case 17: slotSelectWidget((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::FormWindow::contextMenuRequested(QMenu * _t1, QWidget * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
