/****************************************************************************
** Meta object code from reading C++ file 'indexwindow.h'
**
** Created: Thu Mar 5 20:33:47 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../indexwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'indexwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_IndexWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x05,
      52,   38,   12,   12, 0x05,
      95,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     118,  111,   12,   12, 0x08,
     141,   12,   12,   12, 0x08,
     164,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_IndexWindow[] = {
    "IndexWindow\0\0link\0linkActivated(QUrl)\0"
    "links,keyword\0linksActivated(QMap<QString,QUrl>,QString)\0"
    "escapePressed()\0filter\0filterIndices(QString)\0"
    "enableSearchLineEdit()\0disableSearchLineEdit()\0"
};

const QMetaObject IndexWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_IndexWindow,
      qt_meta_data_IndexWindow, 0 }
};

const QMetaObject *IndexWindow::metaObject() const
{
    return &staticMetaObject;
}

void *IndexWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IndexWindow))
        return static_cast<void*>(const_cast< IndexWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int IndexWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: linkActivated((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: linksActivated((*reinterpret_cast< const QMap<QString,QUrl>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: escapePressed(); break;
        case 3: filterIndices((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: enableSearchLineEdit(); break;
        case 5: disableSearchLineEdit(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void IndexWindow::linkActivated(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void IndexWindow::linksActivated(const QMap<QString,QUrl> & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void IndexWindow::escapePressed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
