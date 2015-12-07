/****************************************************************************
** Meta object code from reading C++ file 'translatedialog.h'
**
** Created: Thu Mar 5 20:56:08 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../translatedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'translatedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TranslateDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      21,   17,   16,   16, 0x05,
      52,   47,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      67,   16,   16,   16, 0x08,
      82,   16,   16,   16, 0x08,
     109,   16,   16,   16, 0x08,
     128,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TranslateDialog[] = {
    "TranslateDialog\0\0hit\0requestMatchUpdate(bool&)\0"
    "mode\0activated(int)\0emitFindNext()\0"
    "emitTranslateAndFindNext()\0"
    "emitTranslateAll()\0verifyText()\0"
};

const QMetaObject TranslateDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TranslateDialog,
      qt_meta_data_TranslateDialog, 0 }
};

const QMetaObject *TranslateDialog::metaObject() const
{
    return &staticMetaObject;
}

void *TranslateDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TranslateDialog))
        return static_cast<void*>(const_cast< TranslateDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int TranslateDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: requestMatchUpdate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: emitFindNext(); break;
        case 3: emitTranslateAndFindNext(); break;
        case 4: emitTranslateAll(); break;
        case 5: verifyText(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TranslateDialog::requestMatchUpdate(bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TranslateDialog::activated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
