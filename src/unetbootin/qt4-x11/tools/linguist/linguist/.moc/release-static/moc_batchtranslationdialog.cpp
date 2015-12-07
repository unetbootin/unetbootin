/****************************************************************************
** Meta object code from reading C++ file 'batchtranslationdialog.h'
**
** Created: Thu Mar 5 20:55:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../batchtranslationdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'batchtranslationdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BatchTranslationDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   23,   23,   23, 0x08,
      54,   23,   23,   23, 0x08,
      73,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BatchTranslationDialog[] = {
    "BatchTranslationDialog\0\0finished()\0"
    "startTranslation()\0movePhraseBookUp()\0"
    "movePhraseBookDown()\0"
};

const QMetaObject BatchTranslationDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BatchTranslationDialog,
      qt_meta_data_BatchTranslationDialog, 0 }
};

const QMetaObject *BatchTranslationDialog::metaObject() const
{
    return &staticMetaObject;
}

void *BatchTranslationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BatchTranslationDialog))
        return static_cast<void*>(const_cast< BatchTranslationDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int BatchTranslationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished(); break;
        case 1: startTranslation(); break;
        case 2: movePhraseBookUp(); break;
        case 3: movePhraseBookDown(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void BatchTranslationDialog::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
