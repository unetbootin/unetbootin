/****************************************************************************
** Meta object code from reading C++ file 'qfontdialog.h'
**
** Created: Fri May 30 22:30:01 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qfontdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfontdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFontDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      37,   12,   12,   12, 0x08,
      63,   12,   12,   12, 0x08,
      96,   12,   12,   12, 0x08,
     121,   12,   12,   12, 0x08,
     145,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QFontDialog[] = {
    "QFontDialog\0\0_q_sizeChanged(QString)\0"
    "_q_familyHighlighted(int)\0"
    "_q_writingSystemHighlighted(int)\0"
    "_q_styleHighlighted(int)\0"
    "_q_sizeHighlighted(int)\0_q_updateSample()\0"
};

const QMetaObject QFontDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QFontDialog,
      qt_meta_data_QFontDialog, 0 }
};

const QMetaObject *QFontDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QFontDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFontDialog))
	return static_cast<void*>(const_cast< QFontDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QFontDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: d_func()->_q_sizeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: d_func()->_q_familyHighlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: d_func()->_q_writingSystemHighlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: d_func()->_q_styleHighlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: d_func()->_q_sizeHighlighted((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: d_func()->_q_updateSample(); break;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
