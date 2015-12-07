/****************************************************************************
** Meta object code from reading C++ file 'q3tabdialog.h'
**
** Created: Fri May 30 22:41:34 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/q3tabdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3tabdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3TabDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      27,   12,   12,   12, 0x05,
      48,   12,   12,   12, 0x05,
      70,   12,   12,   12, 0x05,
      93,   12,   12,   12, 0x05,
     113,   12,   12,   12, 0x05,
     138,   12,   12,   12, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Q3TabDialog[] = {
    "Q3TabDialog\0\0aboutToShow()\0"
    "applyButtonPressed()\0cancelButtonPressed()\0"
    "defaultButtonPressed()\0helpButtonPressed()\0"
    "currentChanged(QWidget*)\0selected(QString)\0"
};

const QMetaObject Q3TabDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Q3TabDialog,
      qt_meta_data_Q3TabDialog, 0 }
};

const QMetaObject *Q3TabDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Q3TabDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3TabDialog))
	return static_cast<void*>(const_cast< Q3TabDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Q3TabDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aboutToShow(); break;
        case 1: applyButtonPressed(); break;
        case 2: cancelButtonPressed(); break;
        case 3: defaultButtonPressed(); break;
        case 4: helpButtonPressed(); break;
        case 5: currentChanged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 6: selected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Q3TabDialog::aboutToShow()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Q3TabDialog::applyButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Q3TabDialog::cancelButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Q3TabDialog::defaultButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Q3TabDialog::helpButtonPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void Q3TabDialog::currentChanged(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Q3TabDialog::selected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
