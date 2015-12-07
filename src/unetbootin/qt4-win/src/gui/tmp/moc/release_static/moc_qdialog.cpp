/****************************************************************************
** Meta object code from reading C++ file 'qdialog.h'
**
** Created: Fri May 30 22:29:49 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       2,   50, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,    9,    8,    8, 0x05,
      30,    8,    8,    8, 0x05,
      41,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,    8,   52,    8, 0x0a,
      63,    8,    8,    8, 0x0a,
      73,    8,    8,    8, 0x0a,
      82,    8,    8,    8, 0x0a,
      91,    8,    8,    8, 0x0a,

 // properties: name, type, flags
     116,  111, 0x01095103,
     132,  111, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QDialog[] = {
    "QDialog\0\0result\0finished(int)\0accepted()\0"
    "rejected()\0int\0exec()\0done(int)\0"
    "accept()\0reject()\0showExtension(bool)\0"
    "bool\0sizeGripEnabled\0modal\0"
};

const QMetaObject QDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDialog,
      qt_meta_data_QDialog, 0 }
};

const QMetaObject *QDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDialog))
	return static_cast<void*>(const_cast< QDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: accepted(); break;
        case 2: rejected(); break;
        case 3: { int _r = exec();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: done((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: accept(); break;
        case 6: reject(); break;
        case 7: showExtension((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isSizeGripEnabled(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isModal(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSizeGripEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 1: setModal(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QDialog::finished(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDialog::accepted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QDialog::rejected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
