/****************************************************************************
** Meta object code from reading C++ file 'qtextdocument.h'
**
** Created: Thu Mar 5 18:57:05 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../text/qtextdocument.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtextdocument.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTextDocument[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   12, // methods
      12,   82, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      44,   15,   14,   14, 0x05,
      72,   14,   14,   14, 0x05,
      90,   14,   14,   14, 0x05,
     110,   14,   14,   14, 0x05,
     130,   14,   14,   14, 0x05,
     151,  149,   14,   14, 0x05,
     184,  177,   14,   14, 0x05,
     233,  219,   14,   14, 0x05,
     256,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     280,   14,   14,   14, 0x0a,
     287,   14,   14,   14, 0x0a,
     294,   14,   14,   14, 0x0a,
     329,  149,   14,   14, 0x0a,
     347,   14,   14,   14, 0x2a,

 // properties: name, type, flags
     366,  361, 0x01095103,
     382,  361, 0x01094103,
     398,  391, 0x16095103,
     413,  407, 0x40095103,
     425,  361, 0x01095103,
     442,  391, 0x16095001,
     453,  447, 0x06095103,
     467,  463, 0x02095001,
     478,  447, 0x06095103,
     498,  490, 0x0a095103,
     516,  463, 0x02095103,
     534,  447, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_QTextDocument[] = {
    "QTextDocument\0\0from,charsRemoves,charsAdded\0"
    "contentsChange(int,int,int)\0"
    "contentsChanged()\0undoAvailable(bool)\0"
    "redoAvailable(bool)\0undoCommandAdded()\0"
    "m\0modificationChanged(bool)\0cursor\0"
    "cursorPositionChanged(QTextCursor)\0"
    "newBlockCount\0blockCountChanged(int)\0"
    "documentLayoutChanged()\0undo()\0redo()\0"
    "appendUndoItem(QAbstractUndoItem*)\0"
    "setModified(bool)\0setModified()\0bool\0"
    "undoRedoEnabled\0modified\0QSizeF\0"
    "pageSize\0QFont\0defaultFont\0useDesignMetrics\0"
    "size\0qreal\0textWidth\0int\0blockCount\0"
    "indentWidth\0QString\0defaultStyleSheet\0"
    "maximumBlockCount\0documentMargin\0"
};

const QMetaObject QTextDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QTextDocument,
      qt_meta_data_QTextDocument, 0 }
};

const QMetaObject *QTextDocument::metaObject() const
{
    return &staticMetaObject;
}

void *QTextDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTextDocument))
        return static_cast<void*>(const_cast< QTextDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int QTextDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contentsChange((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: contentsChanged(); break;
        case 2: undoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: redoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: undoCommandAdded(); break;
        case 5: modificationChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: cursorPositionChanged((*reinterpret_cast< const QTextCursor(*)>(_a[1]))); break;
        case 7: blockCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: documentLayoutChanged(); break;
        case 9: undo(); break;
        case 10: redo(); break;
        case 11: appendUndoItem((*reinterpret_cast< QAbstractUndoItem*(*)>(_a[1]))); break;
        case 12: setModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: setModified(); break;
        default: ;
        }
        _id -= 14;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isUndoRedoEnabled(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isModified(); break;
        case 2: *reinterpret_cast< QSizeF*>(_v) = pageSize(); break;
        case 3: *reinterpret_cast< QFont*>(_v) = defaultFont(); break;
        case 4: *reinterpret_cast< bool*>(_v) = useDesignMetrics(); break;
        case 5: *reinterpret_cast< QSizeF*>(_v) = size(); break;
        case 6: *reinterpret_cast< qreal*>(_v) = textWidth(); break;
        case 7: *reinterpret_cast< int*>(_v) = blockCount(); break;
        case 8: *reinterpret_cast< qreal*>(_v) = indentWidth(); break;
        case 9: *reinterpret_cast< QString*>(_v) = defaultStyleSheet(); break;
        case 10: *reinterpret_cast< int*>(_v) = maximumBlockCount(); break;
        case 11: *reinterpret_cast< qreal*>(_v) = documentMargin(); break;
        }
        _id -= 12;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setUndoRedoEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 1: setModified(*reinterpret_cast< bool*>(_v)); break;
        case 2: setPageSize(*reinterpret_cast< QSizeF*>(_v)); break;
        case 3: setDefaultFont(*reinterpret_cast< QFont*>(_v)); break;
        case 4: setUseDesignMetrics(*reinterpret_cast< bool*>(_v)); break;
        case 6: setTextWidth(*reinterpret_cast< qreal*>(_v)); break;
        case 8: setIndentWidth(*reinterpret_cast< qreal*>(_v)); break;
        case 9: setDefaultStyleSheet(*reinterpret_cast< QString*>(_v)); break;
        case 10: setMaximumBlockCount(*reinterpret_cast< int*>(_v)); break;
        case 11: setDocumentMargin(*reinterpret_cast< qreal*>(_v)); break;
        }
        _id -= 12;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 12;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QTextDocument::contentsChange(int _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTextDocument::contentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QTextDocument::undoAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QTextDocument::redoAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QTextDocument::undoCommandAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QTextDocument::modificationChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QTextDocument::cursorPositionChanged(const QTextCursor & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QTextDocument::blockCountChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QTextDocument::documentLayoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}
QT_END_MOC_NAMESPACE
