/****************************************************************************
** Meta object code from reading C++ file 'messageeditorwidgets.h'
**
** Created: Thu Mar 5 20:55:52 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../messageeditorwidgets.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messageeditorwidgets.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExpandingTextEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      32,   19,   18,   18, 0x08,
      53,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ExpandingTextEdit[] = {
    "ExpandingTextEdit\0\0documentSize\0"
    "updateHeight(QSizeF)\0reallyEnsureCursorVisible()\0"
};

const QMetaObject ExpandingTextEdit::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_ExpandingTextEdit,
      qt_meta_data_ExpandingTextEdit, 0 }
};

const QMetaObject *ExpandingTextEdit::metaObject() const
{
    return &staticMetaObject;
}

void *ExpandingTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExpandingTextEdit))
        return static_cast<void*>(const_cast< ExpandingTextEdit*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int ExpandingTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateHeight((*reinterpret_cast< const QSizeF(*)>(_a[1]))); break;
        case 1: reallyEnsureCursorVisible(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_FormatTextEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      32,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FormatTextEdit[] = {
    "FormatTextEdit\0\0text,userAction\0"
    "setPlainText(QString,bool)\0"
};

const QMetaObject FormatTextEdit::staticMetaObject = {
    { &ExpandingTextEdit::staticMetaObject, qt_meta_stringdata_FormatTextEdit,
      qt_meta_data_FormatTextEdit, 0 }
};

const QMetaObject *FormatTextEdit::metaObject() const
{
    return &staticMetaObject;
}

void *FormatTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FormatTextEdit))
        return static_cast<void*>(const_cast< FormatTextEdit*>(this));
    return ExpandingTextEdit::qt_metacast(_clname);
}

int FormatTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ExpandingTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setPlainText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FormWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      26,   11,   11,   11, 0x05,
      45,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_FormWidget[] = {
    "FormWidget\0\0textChanged()\0selectionChanged()\0"
    "cursorPositionChanged()\0"
};

const QMetaObject FormWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FormWidget,
      qt_meta_data_FormWidget, 0 }
};

const QMetaObject *FormWidget::metaObject() const
{
    return &staticMetaObject;
}

void *FormWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FormWidget))
        return static_cast<void*>(const_cast< FormWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int FormWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: textChanged(); break;
        case 1: selectionChanged(); break;
        case 2: cursorPositionChanged(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FormWidget::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void FormWidget::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void FormWidget::cursorPositionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
