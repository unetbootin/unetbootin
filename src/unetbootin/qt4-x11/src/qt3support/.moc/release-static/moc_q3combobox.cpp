/****************************************************************************
** Meta object code from reading C++ file 'q3combobox.h'
**
** Created: Thu Mar 5 19:06:04 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/q3combobox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'q3combobox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Q3ComboBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   12, // methods
      10,   77, // properties
       1,  107, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,
      33,   12,   11,   11, 0x05,
      50,   11,   11,   11, 0x05,
      69,   11,   11,   11, 0x05,
      90,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     111,   11,   11,   11, 0x0a,
     119,   11,   11,   11, 0x0a,
     136,   11,   11,   11, 0x0a,
     148,   11,   11,   11, 0x0a,
     169,   11,   11,   11, 0x08,
     191,   11,   11,   11, 0x08,
     214,   11,   11,   11, 0x08,
     237,   11,   11,   11, 0x08,

 // properties: name, type, flags
     258,  253, 0x01095103,
     271,  267, 0x02095001,
     285,  277, 0x0a094103,
     297,  267, 0x02095103,
     309,  253, 0x01094103,
     320,  267, 0x02095103,
     330,  267, 0x02095103,
     346,  339, 0x0009510b,
     362,  253, 0x01095103,
     377,  253, 0x01095103,

 // enums: name, flags, count, data
     339, 0x0,   12,  111,

 // enum data: key, value
     395, uint(Q3ComboBox::NoInsertion),
     407, uint(Q3ComboBox::AtTop),
     413, uint(Q3ComboBox::AtCurrent),
     423, uint(Q3ComboBox::AtBottom),
     432, uint(Q3ComboBox::AfterCurrent),
     445, uint(Q3ComboBox::BeforeCurrent),
     459, uint(Q3ComboBox::NoInsert),
     468, uint(Q3ComboBox::InsertAtTop),
     480, uint(Q3ComboBox::InsertAtCurrent),
     496, uint(Q3ComboBox::InsertAtBottom),
     511, uint(Q3ComboBox::InsertAfterCurrent),
     530, uint(Q3ComboBox::InsertBeforeCurrent),

       0        // eod
};

static const char qt_meta_stringdata_Q3ComboBox[] = {
    "Q3ComboBox\0\0index\0activated(int)\0"
    "highlighted(int)\0activated(QString)\0"
    "highlighted(QString)\0textChanged(QString)\0"
    "clear()\0clearValidator()\0clearEdit()\0"
    "setEditText(QString)\0internalActivate(int)\0"
    "internalHighlight(int)\0internalClickTimeout()\0"
    "returnPressed()\0bool\0editable\0int\0"
    "count\0QString\0currentText\0currentItem\0"
    "autoResize\0sizeLimit\0maxCount\0Policy\0"
    "insertionPolicy\0autoCompletion\0"
    "duplicatesEnabled\0NoInsertion\0AtTop\0"
    "AtCurrent\0AtBottom\0AfterCurrent\0"
    "BeforeCurrent\0NoInsert\0InsertAtTop\0"
    "InsertAtCurrent\0InsertAtBottom\0"
    "InsertAfterCurrent\0InsertBeforeCurrent\0"
};

const QMetaObject Q3ComboBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Q3ComboBox,
      qt_meta_data_Q3ComboBox, 0 }
};

const QMetaObject *Q3ComboBox::metaObject() const
{
    return &staticMetaObject;
}

void *Q3ComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Q3ComboBox))
        return static_cast<void*>(const_cast< Q3ComboBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int Q3ComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: highlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: highlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: clear(); break;
        case 6: clearValidator(); break;
        case 7: clearEdit(); break;
        case 8: setEditText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: internalActivate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: internalHighlight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: internalClickTimeout(); break;
        case 12: returnPressed(); break;
        default: ;
        }
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = editable(); break;
        case 1: *reinterpret_cast< int*>(_v) = count(); break;
        case 2: *reinterpret_cast< QString*>(_v) = currentText(); break;
        case 3: *reinterpret_cast< int*>(_v) = currentItem(); break;
        case 4: *reinterpret_cast< bool*>(_v) = autoResize(); break;
        case 5: *reinterpret_cast< int*>(_v) = sizeLimit(); break;
        case 6: *reinterpret_cast< int*>(_v) = maxCount(); break;
        case 7: *reinterpret_cast< Policy*>(_v) = insertionPolicy(); break;
        case 8: *reinterpret_cast< bool*>(_v) = autoCompletion(); break;
        case 9: *reinterpret_cast< bool*>(_v) = duplicatesEnabled(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setEditable(*reinterpret_cast< bool*>(_v)); break;
        case 2: setCurrentText(*reinterpret_cast< QString*>(_v)); break;
        case 3: setCurrentItem(*reinterpret_cast< int*>(_v)); break;
        case 4: setAutoResize(*reinterpret_cast< bool*>(_v)); break;
        case 5: setSizeLimit(*reinterpret_cast< int*>(_v)); break;
        case 6: setMaxCount(*reinterpret_cast< int*>(_v)); break;
        case 7: setInsertionPolicy(*reinterpret_cast< Policy*>(_v)); break;
        case 8: setAutoCompletion(*reinterpret_cast< bool*>(_v)); break;
        case 9: setDuplicatesEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Q3ComboBox::activated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Q3ComboBox::highlighted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Q3ComboBox::activated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Q3ComboBox::highlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Q3ComboBox::textChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
