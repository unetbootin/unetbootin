/****************************************************************************
** Meta object code from reading C++ file 'qmessagebox.h'
**
** Created: Thu Mar 5 18:53:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dialogs/qmessagebox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmessagebox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMessageBox[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       7,   22, // properties
       2,   43, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      20,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,   12,   12,   12, 0x08,

 // properties: name, type, flags
      95,   87, 0x0a095103,
     105,  100, 0x0009510b,
     118,  110, 0x41095103,
     144,  129, 0x0009510b,
     171,  155, 0x0009510b,
     187,   87, 0x0a095103,
     200,   87, 0x0a095103,

 // enums: name, flags, count, data
     100, 0x0,    5,   51,
     155, 0x1,   27,   61,

 // enum data: key, value
     216, uint(QMessageBox::NoIcon),
     223, uint(QMessageBox::Information),
     235, uint(QMessageBox::Warning),
     243, uint(QMessageBox::Critical),
     252, uint(QMessageBox::Question),
     261, uint(QMessageBox::NoButton),
     270, uint(QMessageBox::Ok),
     273, uint(QMessageBox::Save),
     278, uint(QMessageBox::SaveAll),
     286, uint(QMessageBox::Open),
     291, uint(QMessageBox::Yes),
     295, uint(QMessageBox::YesToAll),
     304, uint(QMessageBox::No),
     307, uint(QMessageBox::NoToAll),
     315, uint(QMessageBox::Abort),
     321, uint(QMessageBox::Retry),
     327, uint(QMessageBox::Ignore),
     334, uint(QMessageBox::Close),
     340, uint(QMessageBox::Cancel),
     347, uint(QMessageBox::Discard),
     355, uint(QMessageBox::Help),
     360, uint(QMessageBox::Apply),
     366, uint(QMessageBox::Reset),
     372, uint(QMessageBox::RestoreDefaults),
     388, uint(QMessageBox::FirstButton),
     400, uint(QMessageBox::LastButton),
     411, uint(QMessageBox::YesAll),
     418, uint(QMessageBox::NoAll),
     424, uint(QMessageBox::Default),
     432, uint(QMessageBox::Escape),
     439, uint(QMessageBox::FlagMask),
     448, uint(QMessageBox::ButtonMask),

       0        // eod
};

static const char qt_meta_stringdata_QMessageBox[] = {
    "QMessageBox\0\0button\0buttonClicked(QAbstractButton*)\0"
    "_q_buttonClicked(QAbstractButton*)\0"
    "QString\0text\0Icon\0icon\0QPixmap\0"
    "iconPixmap\0Qt::TextFormat\0textFormat\0"
    "StandardButtons\0standardButtons\0"
    "detailedText\0informativeText\0NoIcon\0"
    "Information\0Warning\0Critical\0Question\0"
    "NoButton\0Ok\0Save\0SaveAll\0Open\0Yes\0"
    "YesToAll\0No\0NoToAll\0Abort\0Retry\0Ignore\0"
    "Close\0Cancel\0Discard\0Help\0Apply\0Reset\0"
    "RestoreDefaults\0FirstButton\0LastButton\0"
    "YesAll\0NoAll\0Default\0Escape\0FlagMask\0"
    "ButtonMask\0"
};

const QMetaObject QMessageBox::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QMessageBox,
      qt_meta_data_QMessageBox, 0 }
};

const QMetaObject *QMessageBox::metaObject() const
{
    return &staticMetaObject;
}

void *QMessageBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMessageBox))
        return static_cast<void*>(const_cast< QMessageBox*>(this));
    return QDialog::qt_metacast(_clname);
}

int QMessageBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: buttonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 1: d_func()->_q_buttonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< Icon*>(_v) = icon(); break;
        case 2: *reinterpret_cast< QPixmap*>(_v) = iconPixmap(); break;
        case 3: *reinterpret_cast< Qt::TextFormat*>(_v) = textFormat(); break;
        case 4: *reinterpret_cast<int*>(_v) = QFlag(standardButtons()); break;
        case 5: *reinterpret_cast< QString*>(_v) = detailedText(); break;
        case 6: *reinterpret_cast< QString*>(_v) = informativeText(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setIcon(*reinterpret_cast< Icon*>(_v)); break;
        case 2: setIconPixmap(*reinterpret_cast< QPixmap*>(_v)); break;
        case 3: setTextFormat(*reinterpret_cast< Qt::TextFormat*>(_v)); break;
        case 4: setStandardButtons(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 5: setDetailedText(*reinterpret_cast< QString*>(_v)); break;
        case 6: setInformativeText(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QMessageBox::buttonClicked(QAbstractButton * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
