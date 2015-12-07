/****************************************************************************
** Meta object code from reading C++ file 'phrasebookbox.h'
**
** Created: Thu Mar 5 20:55:57 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../phrasebookbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'phrasebookbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PhraseBookBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      27,   14,   14,   14, 0x08,
      42,   14,   14,   14, 0x08,
      53,   14,   14,   14, 0x08,
      67,   60,   14,   14, 0x08,
      97,   90,   14,   14, 0x08,
     131,  120,   14,   14, 0x08,
     158,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PhraseBookBox[] = {
    "PhraseBookBox\0\0newPhrase()\0removePhrase()\0"
    "settings()\0save()\0source\0"
    "sourceChanged(QString)\0target\0"
    "targetChanged(QString)\0definition\0"
    "definitionChanged(QString)\0"
    "selectionChanged()\0"
};

const QMetaObject PhraseBookBox::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PhraseBookBox,
      qt_meta_data_PhraseBookBox, 0 }
};

const QMetaObject *PhraseBookBox::metaObject() const
{
    return &staticMetaObject;
}

void *PhraseBookBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PhraseBookBox))
        return static_cast<void*>(const_cast< PhraseBookBox*>(this));
    if (!strcmp(_clname, "Ui::PhraseBookBox"))
        return static_cast< Ui::PhraseBookBox*>(const_cast< PhraseBookBox*>(this));
    return QDialog::qt_metacast(_clname);
}

int PhraseBookBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newPhrase(); break;
        case 1: removePhrase(); break;
        case 2: settings(); break;
        case 3: save(); break;
        case 4: sourceChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: targetChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: definitionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: selectionChanged(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
