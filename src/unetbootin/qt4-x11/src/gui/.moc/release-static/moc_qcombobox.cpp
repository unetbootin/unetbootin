/****************************************************************************
** Meta object code from reading C++ file 'qcombobox.h'
**
** Created: Thu Mar 5 18:51:11 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qcombobox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcombobox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QComboBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   12, // methods
      15,  132, // properties
       2,  192, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      42,   36,   10,   10, 0x05,
      57,   10,   10,   10, 0x05,
      76,   36,   10,   10, 0x05,
      93,   10,   10,   10, 0x05,
     114,   36,   10,   10, 0x05,
     139,   10,   10,   10, 0x05,
     168,   10,   10,   10, 0x15,

 // slots: signature, parameters, type, tag, flags
     189,   10,   10,   10, 0x0a,
     197,   10,   10,   10, 0x0a,
     218,  213,   10,   10, 0x0a,
     239,   36,   10,   10, 0x0a,
     265,  260,   10,   10, 0x08,
     294,   10,   10,   10, 0x08,
     326,   36,   10,   10, 0x08,
     366,   10,   10,   10, 0x08,
     385,   10,   10,   10, 0x08,
     404,  402,   10,   10, 0x08,
     461,  444,   10,   10, 0x08,
     507,  444,   10,   10, 0x08,
     544,  444,   10,   10, 0x08,
     589,  444,   10,   10, 0x08,
     625,   10,   10,   10, 0x08,
     645,   10,   10,   10, 0x08,

 // properties: name, type, flags
     666,  661, 0x01095103,
     679,  675, 0x02095001,
     693,  685, 0x0a095001,
     705,  675, 0x02495103,
     718,  675, 0x02095103,
     734,  675, 0x02095103,
     756,  743, 0x0009510b,
     786,  769, 0x0009510b,
     803,  675, 0x02095103,
     831,  825, 0x15095103,
     840,  661, 0x01094103,
     875,  855, 0x0009410b,
     905,  661, 0x01095103,
     923,  661, 0x01095103,
     929,  675, 0x02095103,

 // properties: notify_signal_id
       0,
       0,
       0,
       5,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

 // enums: name, flags, count, data
     743, 0x0,    7,  200,
     769, 0x0,    4,  214,

 // enum data: key, value
     941, uint(QComboBox::NoInsert),
     950, uint(QComboBox::InsertAtTop),
     962, uint(QComboBox::InsertAtCurrent),
     978, uint(QComboBox::InsertAtBottom),
     993, uint(QComboBox::InsertAfterCurrent),
    1012, uint(QComboBox::InsertBeforeCurrent),
    1032, uint(QComboBox::InsertAlphabetically),
    1053, uint(QComboBox::AdjustToContents),
    1070, uint(QComboBox::AdjustToContentsOnFirstShow),
    1098, uint(QComboBox::AdjustToMinimumContentsLength),
    1128, uint(QComboBox::AdjustToMinimumContentsLengthWithIcon),

       0        // eod
};

static const char qt_meta_stringdata_QComboBox[] = {
    "QComboBox\0\0editTextChanged(QString)\0"
    "index\0activated(int)\0activated(QString)\0"
    "highlighted(int)\0highlighted(QString)\0"
    "currentIndexChanged(int)\0"
    "currentIndexChanged(QString)\0"
    "textChanged(QString)\0clear()\0"
    "clearEditText()\0text\0setEditText(QString)\0"
    "setCurrentIndex(int)\0item\0"
    "_q_itemSelected(QModelIndex)\0"
    "_q_emitHighlighted(QModelIndex)\0"
    "_q_emitCurrentIndexChanged(QModelIndex)\0"
    "_q_returnPressed()\0_q_resetButton()\0"
    ",\0_q_dataChanged(QModelIndex,QModelIndex)\0"
    "parent,start,end\0"
    "_q_rowsAboutToBeInserted(QModelIndex,int,int)\0"
    "_q_rowsInserted(QModelIndex,int,int)\0"
    "_q_rowsAboutToBeRemoved(QModelIndex,int,int)\0"
    "_q_rowsRemoved(QModelIndex,int,int)\0"
    "_q_modelDestroyed()\0_q_modelReset()\0"
    "bool\0editable\0int\0count\0QString\0"
    "currentText\0currentIndex\0maxVisibleItems\0"
    "maxCount\0InsertPolicy\0insertPolicy\0"
    "SizeAdjustPolicy\0sizeAdjustPolicy\0"
    "minimumContentsLength\0QSize\0iconSize\0"
    "autoCompletion\0Qt::CaseSensitivity\0"
    "autoCompletionCaseSensitivity\0"
    "duplicatesEnabled\0frame\0modelColumn\0"
    "NoInsert\0InsertAtTop\0InsertAtCurrent\0"
    "InsertAtBottom\0InsertAfterCurrent\0"
    "InsertBeforeCurrent\0InsertAlphabetically\0"
    "AdjustToContents\0AdjustToContentsOnFirstShow\0"
    "AdjustToMinimumContentsLength\0"
    "AdjustToMinimumContentsLengthWithIcon\0"
};

const QMetaObject QComboBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QComboBox,
      qt_meta_data_QComboBox, 0 }
};

const QMetaObject *QComboBox::metaObject() const
{
    return &staticMetaObject;
}

void *QComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QComboBox))
        return static_cast<void*>(const_cast< QComboBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int QComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: editTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: highlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: highlighted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: clear(); break;
        case 9: clearEditText(); break;
        case 10: setEditText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: d_func()->_q_itemSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 13: d_func()->_q_emitHighlighted((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 14: d_func()->_q_emitCurrentIndexChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 15: d_func()->_q_returnPressed(); break;
        case 16: d_func()->_q_resetButton(); break;
        case 17: d_func()->_q_dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 18: d_func()->_q_rowsAboutToBeInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: d_func()->_q_rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 20: d_func()->_q_rowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 21: d_func()->_q_rowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 22: d_func()->_q_modelDestroyed(); break;
        case 23: d_func()->_q_modelReset(); break;
        default: ;
        }
        _id -= 24;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isEditable(); break;
        case 1: *reinterpret_cast< int*>(_v) = count(); break;
        case 2: *reinterpret_cast< QString*>(_v) = currentText(); break;
        case 3: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        case 4: *reinterpret_cast< int*>(_v) = maxVisibleItems(); break;
        case 5: *reinterpret_cast< int*>(_v) = maxCount(); break;
        case 6: *reinterpret_cast< InsertPolicy*>(_v) = insertPolicy(); break;
        case 7: *reinterpret_cast< SizeAdjustPolicy*>(_v) = sizeAdjustPolicy(); break;
        case 8: *reinterpret_cast< int*>(_v) = minimumContentsLength(); break;
        case 9: *reinterpret_cast< QSize*>(_v) = iconSize(); break;
        case 10: *reinterpret_cast< bool*>(_v) = autoCompletion(); break;
        case 11: *reinterpret_cast< Qt::CaseSensitivity*>(_v) = autoCompletionCaseSensitivity(); break;
        case 12: *reinterpret_cast< bool*>(_v) = duplicatesEnabled(); break;
        case 13: *reinterpret_cast< bool*>(_v) = hasFrame(); break;
        case 14: *reinterpret_cast< int*>(_v) = modelColumn(); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setEditable(*reinterpret_cast< bool*>(_v)); break;
        case 3: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        case 4: setMaxVisibleItems(*reinterpret_cast< int*>(_v)); break;
        case 5: setMaxCount(*reinterpret_cast< int*>(_v)); break;
        case 6: setInsertPolicy(*reinterpret_cast< InsertPolicy*>(_v)); break;
        case 7: setSizeAdjustPolicy(*reinterpret_cast< SizeAdjustPolicy*>(_v)); break;
        case 8: setMinimumContentsLength(*reinterpret_cast< int*>(_v)); break;
        case 9: setIconSize(*reinterpret_cast< QSize*>(_v)); break;
        case 10: setAutoCompletion(*reinterpret_cast< bool*>(_v)); break;
        case 11: setAutoCompletionCaseSensitivity(*reinterpret_cast< Qt::CaseSensitivity*>(_v)); break;
        case 12: setDuplicatesEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 13: setFrame(*reinterpret_cast< bool*>(_v)); break;
        case 14: setModelColumn(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 15;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QComboBox::editTextChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QComboBox::activated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QComboBox::activated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QComboBox::highlighted(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QComboBox::highlighted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QComboBox::currentIndexChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QComboBox::currentIndexChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QComboBox::textChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
