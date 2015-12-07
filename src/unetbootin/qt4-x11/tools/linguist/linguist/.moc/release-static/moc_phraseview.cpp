/****************************************************************************
** Meta object code from reading C++ file 'phraseview.h'
**
** Created: Thu Mar 5 20:56:01 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../phraseview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'phraseview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuessShortcut[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      20,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GuessShortcut[] = {
    "GuessShortcut\0\0nkey\0activated(int)\0"
    "keyActivated()\0"
};

const QMetaObject GuessShortcut::staticMetaObject = {
    { &QShortcut::staticMetaObject, qt_meta_stringdata_GuessShortcut,
      qt_meta_data_GuessShortcut, 0 }
};

const QMetaObject *GuessShortcut::metaObject() const
{
    return &staticMetaObject;
}

void *GuessShortcut::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuessShortcut))
        return static_cast<void*>(const_cast< GuessShortcut*>(this));
    return QShortcut::qt_metacast(_clname);
}

int GuessShortcut::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QShortcut::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: keyActivated(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GuessShortcut::activated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_PhraseView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      31,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      59,   11,   11,   11, 0x0a,
      76,   11,   11,   11, 0x0a,
      90,   85,   11,   11, 0x08,
     115,  109,   11,   11, 0x08,
     141,   11,   11,   11, 0x08,
     156,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PhraseView[] = {
    "PhraseView\0\0latestModel,phrase\0"
    "phraseSelected(int,QString)\0"
    "toggleGuessing()\0update()\0nkey\0"
    "guessShortcut(int)\0index\0"
    "selectPhrase(QModelIndex)\0selectPhrase()\0"
    "editPhrase()\0"
};

const QMetaObject PhraseView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_PhraseView,
      qt_meta_data_PhraseView, 0 }
};

const QMetaObject *PhraseView::metaObject() const
{
    return &staticMetaObject;
}

void *PhraseView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PhraseView))
        return static_cast<void*>(const_cast< PhraseView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int PhraseView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: phraseSelected((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: toggleGuessing(); break;
        case 2: update(); break;
        case 3: guessShortcut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: selectPhrase((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: selectPhrase(); break;
        case 6: editPhrase(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void PhraseView::phraseSelected(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
