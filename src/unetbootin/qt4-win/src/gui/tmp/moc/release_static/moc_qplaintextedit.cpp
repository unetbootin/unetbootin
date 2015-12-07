/****************************************************************************
** Meta object code from reading C++ file 'qplaintextedit.h'
**
** Created: Fri May 30 22:29:26 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qplaintextedit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qplaintextedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPlainTextEdit[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   10, // methods
      14,  140, // properties
       1,  182, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      32,   30,   15,   15, 0x05,
      52,   30,   15,   15, 0x05,
      72,   30,   15,   15, 0x05,
      92,   15,   15,   15, 0x05,
     111,   15,   15,   15, 0x05,
     143,  135,   15,   15, 0x05,
     182,  168,   15,   15, 0x05,
     205,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     236,  231,   15,   15, 0x0a,
     258,   15,   15,   15, 0x0a,
     264,   15,   15,   15, 0x0a,
     271,   15,   15,   15, 0x0a,
     279,   15,   15,   15, 0x0a,
     286,   15,   15,   15, 0x0a,
     293,   15,   15,   15, 0x0a,
     301,   15,   15,   15, 0x0a,
     313,  231,   15,   15, 0x0a,
     338,  231,   15,   15, 0x0a,
     368,  363,   15,   15, 0x0a,
     388,   15,   15,   15, 0x0a,
     405,  403,   15,   15, 0x08,
     432,   15,   15,   15, 0x08,
     457,  454,   15,   15, 0x08,
     502,   15,   15,   15, 0x28,
     543,   15,   15,   15, 0x08,

 // properties: name, type, flags
     575,  570, 0x01095103,
     599,  591, 0x0a095103,
     613,  570, 0x01095103,
     642,  629, 0x0009510b,
     655,  570, 0x01095103,
     664,  591, 0x0a195103,
     674,  570, 0x01095103,
     692,  688, 0x02095103,
     705,  688, 0x02095103,
     742,  717, 0x0009510b,
     763,  688, 0x02095001,
     774,  688, 0x02095103,
     792,  570, 0x01095103,
     810,  570, 0x01095103,

 // enums: name, flags, count, data
     629, 0x0,    2,  186,

 // enum data: key, value
     825, uint(QPlainTextEdit::NoWrap),
     832, uint(QPlainTextEdit::WidgetWidth),

       0        // eod
};

static const char qt_meta_stringdata_QPlainTextEdit[] = {
    "QPlainTextEdit\0\0textChanged()\0b\0"
    "undoAvailable(bool)\0redoAvailable(bool)\0"
    "copyAvailable(bool)\0selectionChanged()\0"
    "cursorPositionChanged()\0rect,dy\0"
    "updateRequest(QRect,int)\0newBlockCount\0"
    "blockCountChanged(int)\0modificationChanged(bool)\0"
    "text\0setPlainText(QString)\0cut()\0"
    "copy()\0paste()\0undo()\0redo()\0clear()\0"
    "selectAll()\0insertPlainText(QString)\0"
    "appendPlainText(QString)\0html\0"
    "appendHtml(QString)\0centerCursor()\0r\0"
    "_q_repaintContents(QRectF)\0"
    "_q_adjustScrollbars()\0,n\0"
    "_q_verticalScrollbarActionTriggered(int,int)\0"
    "_q_verticalScrollbarActionTriggered(int)\0"
    "_q_cursorPositionChanged()\0bool\0"
    "tabChangesFocus\0QString\0documentTitle\0"
    "undoRedoEnabled\0LineWrapMode\0lineWrapMode\0"
    "readOnly\0plainText\0overwriteMode\0int\0"
    "tabStopWidth\0cursorWidth\0"
    "Qt::TextInteractionFlags\0textInteractionFlags\0"
    "blockCount\0maximumBlockCount\0"
    "backgroundVisible\0centerOnScroll\0"
    "NoWrap\0WidgetWidth\0"
};

const QMetaObject QPlainTextEdit::staticMetaObject = {
    { &QAbstractScrollArea::staticMetaObject, qt_meta_stringdata_QPlainTextEdit,
      qt_meta_data_QPlainTextEdit, 0 }
};

const QMetaObject *QPlainTextEdit::metaObject() const
{
    return &staticMetaObject;
}

void *QPlainTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPlainTextEdit))
	return static_cast<void*>(const_cast< QPlainTextEdit*>(this));
    return QAbstractScrollArea::qt_metacast(_clname);
}

int QPlainTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: textChanged(); break;
        case 1: undoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: redoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: copyAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: selectionChanged(); break;
        case 5: cursorPositionChanged(); break;
        case 6: updateRequest((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: blockCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: modificationChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: setPlainText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: cut(); break;
        case 11: copy(); break;
        case 12: paste(); break;
        case 13: undo(); break;
        case 14: redo(); break;
        case 15: clear(); break;
        case 16: selectAll(); break;
        case 17: insertPlainText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: appendPlainText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: appendHtml((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 20: centerCursor(); break;
        case 21: d_func()->_q_repaintContents((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 22: d_func()->_q_adjustScrollbars(); break;
        case 23: d_func()->_q_verticalScrollbarActionTriggered((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 24: d_func()->_q_verticalScrollbarActionTriggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: d_func()->_q_cursorPositionChanged(); break;
        }
        _id -= 26;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = tabChangesFocus(); break;
        case 1: *reinterpret_cast< QString*>(_v) = documentTitle(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isUndoRedoEnabled(); break;
        case 3: *reinterpret_cast< LineWrapMode*>(_v) = lineWrapMode(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isReadOnly(); break;
        case 5: *reinterpret_cast< QString*>(_v) = toPlainText(); break;
        case 6: *reinterpret_cast< bool*>(_v) = overwriteMode(); break;
        case 7: *reinterpret_cast< int*>(_v) = tabStopWidth(); break;
        case 8: *reinterpret_cast< int*>(_v) = cursorWidth(); break;
        case 9: *reinterpret_cast< Qt::TextInteractionFlags*>(_v) = textInteractionFlags(); break;
        case 10: *reinterpret_cast< int*>(_v) = blockCount(); break;
        case 11: *reinterpret_cast< int*>(_v) = maximumBlockCount(); break;
        case 12: *reinterpret_cast< bool*>(_v) = backgroundVisible(); break;
        case 13: *reinterpret_cast< bool*>(_v) = centerOnScroll(); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTabChangesFocus(*reinterpret_cast< bool*>(_v)); break;
        case 1: setDocumentTitle(*reinterpret_cast< QString*>(_v)); break;
        case 2: setUndoRedoEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setLineWrapMode(*reinterpret_cast< LineWrapMode*>(_v)); break;
        case 4: setReadOnly(*reinterpret_cast< bool*>(_v)); break;
        case 5: setPlainText(*reinterpret_cast< QString*>(_v)); break;
        case 6: setOverwriteMode(*reinterpret_cast< bool*>(_v)); break;
        case 7: setTabStopWidth(*reinterpret_cast< int*>(_v)); break;
        case 8: setCursorWidth(*reinterpret_cast< int*>(_v)); break;
        case 9: setTextInteractionFlags(*reinterpret_cast< Qt::TextInteractionFlags*>(_v)); break;
        case 11: setMaximumBlockCount(*reinterpret_cast< int*>(_v)); break;
        case 12: setBackgroundVisible(*reinterpret_cast< bool*>(_v)); break;
        case 13: setCenterOnScroll(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 14;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QPlainTextEdit::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QPlainTextEdit::undoAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QPlainTextEdit::redoAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QPlainTextEdit::copyAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QPlainTextEdit::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QPlainTextEdit::cursorPositionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QPlainTextEdit::updateRequest(const QRect & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QPlainTextEdit::blockCountChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QPlainTextEdit::modificationChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
static const uint qt_meta_data_QPlainTextDocumentLayout[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   10, // properties
       0,    0, // enums/sets

 // properties: name, type, flags
      29,   25, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_QPlainTextDocumentLayout[] = {
    "QPlainTextDocumentLayout\0int\0cursorWidth\0"
};

const QMetaObject QPlainTextDocumentLayout::staticMetaObject = {
    { &QAbstractTextDocumentLayout::staticMetaObject, qt_meta_stringdata_QPlainTextDocumentLayout,
      qt_meta_data_QPlainTextDocumentLayout, 0 }
};

const QMetaObject *QPlainTextDocumentLayout::metaObject() const
{
    return &staticMetaObject;
}

void *QPlainTextDocumentLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPlainTextDocumentLayout))
	return static_cast<void*>(const_cast< QPlainTextDocumentLayout*>(this));
    return QAbstractTextDocumentLayout::qt_metacast(_clname);
}

int QPlainTextDocumentLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTextDocumentLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = cursorWidth(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCursorWidth(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
