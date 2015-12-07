/****************************************************************************
** Meta object code from reading C++ file 'qmessagebox.h'
**
** Created: Fri May 30 22:30:07 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qmessagebox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmessagebox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMessageBox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       7,   15, // properties
       2,   36, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,

 // properties: name, type, flags
      56,   48, 0x0a095103,
      66,   61, 0x0009510b,
      79,   71, 0x41095103,
     105,   90, 0x0009510b,
     132,  116, 0x0009510b,
     148,   48, 0x0a095103,
     161,   48, 0x0a095103,

 // enums: name, flags, count, data
      61, 0x0,    5,   44,
     116, 0x1,   27,   54,

 // enum data: key, value
     177, uint(QMessageBox::NoIcon),
     184, uint(QMessageBox::Information),
     196, uint(QMessageBox::Warning),
     204, uint(QMessageBox::Critical),
     213, uint(QMessageBox::Question),
     222, uint(QMessageBox::NoButton),
     231, uint(QMessageBox::Ok),
     234, uint(QMessageBox::Save),
     239, uint(QMessageBox::SaveAll),
     247, uint(QMessageBox::Open),
     252, uint(QMessageBox::Yes),
     256, uint(QMessageBox::YesToAll),
     265, uint(QMessageBox::No),
     268, uint(QMessageBox::NoToAll),
     276, uint(QMessageBox::Abort),
     282, uint(QMessageBox::Retry),
     288, uint(QMessageBox::Ignore),
     295, uint(QMessageBox::Close),
     301, uint(QMessageBox::Cancel),
     308, uint(QMessageBox::Discard),
     316, uint(QMessageBox::Help),
     321, uint(QMessageBox::Apply),
     327, uint(QMessageBox::Reset),
     333, uint(QMessageBox::RestoreDefaults),
     349, uint(QMessageBox::FirstButton),
     361, uint(QMessageBox::LastButton),
     372, uint(QMessageBox::YesAll),
     379, uint(QMessageBox::NoAll),
     385, uint(QMessageBox::Default),
     393, uint(QMessageBox::Escape),
     400, uint(QMessageBox::FlagMask),
     409, uint(QMessageBox::ButtonMask),

       0        // eod
};

static const char qt_meta_stringdata_QMessageBox[] = {
    "QMessageBox\0\0_q_buttonClicked(QAbstractButton*)\0"
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
        case 0: d_func()->_q_buttonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        }
        _id -= 1;
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
QT_END_MOC_NAMESPACE
