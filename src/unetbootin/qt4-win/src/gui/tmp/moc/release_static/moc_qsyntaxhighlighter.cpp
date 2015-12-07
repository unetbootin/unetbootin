/****************************************************************************
** Meta object code from reading C++ file 'qsyntaxhighlighter.h'
**
** Created: Fri May 30 22:25:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../text/qsyntaxhighlighter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsyntaxhighlighter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSyntaxHighlighter[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,
      63,   34,   19,   19, 0x08,
      94,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QSyntaxHighlighter[] = {
    "QSyntaxHighlighter\0\0rehighlight()\0"
    "from,charsRemoved,charsAdded\0"
    "_q_reformatBlocks(int,int,int)\0"
    "_q_delayedRehighlight()\0"
};

const QMetaObject QSyntaxHighlighter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QSyntaxHighlighter,
      qt_meta_data_QSyntaxHighlighter, 0 }
};

const QMetaObject *QSyntaxHighlighter::metaObject() const
{
    return &staticMetaObject;
}

void *QSyntaxHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSyntaxHighlighter))
	return static_cast<void*>(const_cast< QSyntaxHighlighter*>(this));
    return QObject::qt_metacast(_clname);
}

int QSyntaxHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rehighlight(); break;
        case 1: d_func()->_q_reformatBlocks((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: d_func()->_q_delayedRehighlight(); break;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
