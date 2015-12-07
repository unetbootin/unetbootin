/****************************************************************************
** Meta object code from reading C++ file 'qaction.h'
**
** Created: Thu Mar 5 18:43:14 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../kernel/qaction.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qaction.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAction[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   12, // methods
      16,   87, // properties
       1,  151, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      27,   19,    8,    8, 0x05,
      43,    8,    8,    8, 0x25,
      55,    8,    8,    8, 0x05,
      65,    8,    8,    8, 0x05,
      79,    8,    8,    8, 0x15,
      94,    8,    8,    8, 0x35,

 // slots: signature, parameters, type, tag, flags
     108,  106,    8,    8, 0x1a,
     120,    8,    8,    8, 0x0a,
     130,    8,    8,    8, 0x0a,
     138,    8,    8,    8, 0x0a,
     155,    8,    8,    8, 0x0a,
     164,    8,    8,    8, 0x0a,
     181,  106,    8,    8, 0x0a,
     199,    8,    8,    8, 0x0a,

 // properties: name, type, flags
     221,  216, 0x01095103,
      19,  216, 0x01495103,
     231,  216, 0x01095103,
     245,  239, 0x45095103,
     258,  250, 0x0a095103,
     263,  250, 0x0a095103,
     272,  250, 0x0a095103,
     280,  250, 0x0a095103,
     290,  250, 0x0a095103,
     306,  300, 0x40095103,
     324,  311, 0x4c095103,
     353,  333, 0x0009510b,
     369,  216, 0x01095103,
     380,  216, 0x01095103,
     397,  388, 0x0009510b,
     406,  216, 0x01095103,

 // properties: notify_signal_id
       0,
       4,
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
       0,
       0,
       0,

 // enums: name, flags, count, data
     388, 0x0,    7,  155,

 // enum data: key, value
     424, uint(QAction::NoRole),
     431, uint(QAction::TextHeuristicRole),
     449, uint(QAction::ApplicationSpecificRole),
     473, uint(QAction::AboutQtRole),
     485, uint(QAction::AboutRole),
     495, uint(QAction::PreferencesRole),
     511, uint(QAction::QuitRole),

       0        // eod
};

static const char qt_meta_stringdata_QAction[] = {
    "QAction\0\0changed()\0checked\0triggered(bool)\0"
    "triggered()\0hovered()\0toggled(bool)\0"
    "activated(int)\0activated()\0b\0setOn(bool)\0"
    "trigger()\0hover()\0setChecked(bool)\0"
    "toggle()\0setEnabled(bool)\0setDisabled(bool)\0"
    "setVisible(bool)\0bool\0checkable\0enabled\0"
    "QIcon\0icon\0QString\0text\0iconText\0"
    "toolTip\0statusTip\0whatsThis\0QFont\0"
    "font\0QKeySequence\0shortcut\0"
    "Qt::ShortcutContext\0shortcutContext\0"
    "autoRepeat\0visible\0MenuRole\0menuRole\0"
    "iconVisibleInMenu\0NoRole\0TextHeuristicRole\0"
    "ApplicationSpecificRole\0AboutQtRole\0"
    "AboutRole\0PreferencesRole\0QuitRole\0"
};

const QMetaObject QAction::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAction,
      qt_meta_data_QAction, 0 }
};

const QMetaObject *QAction::metaObject() const
{
    return &staticMetaObject;
}

void *QAction::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAction))
        return static_cast<void*>(const_cast< QAction*>(this));
    return QObject::qt_metacast(_clname);
}

int QAction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changed(); break;
        case 1: triggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: triggered(); break;
        case 3: hovered(); break;
        case 4: toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: activated(); break;
        case 7: setOn((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: trigger(); break;
        case 9: hover(); break;
        case 10: setChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: toggle(); break;
        case 12: setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: setDisabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: setVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 15;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isCheckable(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isChecked(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 3: *reinterpret_cast< QIcon*>(_v) = icon(); break;
        case 4: *reinterpret_cast< QString*>(_v) = text(); break;
        case 5: *reinterpret_cast< QString*>(_v) = iconText(); break;
        case 6: *reinterpret_cast< QString*>(_v) = toolTip(); break;
        case 7: *reinterpret_cast< QString*>(_v) = statusTip(); break;
        case 8: *reinterpret_cast< QString*>(_v) = whatsThis(); break;
        case 9: *reinterpret_cast< QFont*>(_v) = font(); break;
        case 10: *reinterpret_cast< QKeySequence*>(_v) = shortcut(); break;
        case 11: *reinterpret_cast< Qt::ShortcutContext*>(_v) = shortcutContext(); break;
        case 12: *reinterpret_cast< bool*>(_v) = autoRepeat(); break;
        case 13: *reinterpret_cast< bool*>(_v) = isVisible(); break;
        case 14: *reinterpret_cast< MenuRole*>(_v) = menuRole(); break;
        case 15: *reinterpret_cast< bool*>(_v) = isIconVisibleInMenu(); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCheckable(*reinterpret_cast< bool*>(_v)); break;
        case 1: setChecked(*reinterpret_cast< bool*>(_v)); break;
        case 2: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setIcon(*reinterpret_cast< QIcon*>(_v)); break;
        case 4: setText(*reinterpret_cast< QString*>(_v)); break;
        case 5: setIconText(*reinterpret_cast< QString*>(_v)); break;
        case 6: setToolTip(*reinterpret_cast< QString*>(_v)); break;
        case 7: setStatusTip(*reinterpret_cast< QString*>(_v)); break;
        case 8: setWhatsThis(*reinterpret_cast< QString*>(_v)); break;
        case 9: setFont(*reinterpret_cast< QFont*>(_v)); break;
        case 10: setShortcut(*reinterpret_cast< QKeySequence*>(_v)); break;
        case 11: setShortcutContext(*reinterpret_cast< Qt::ShortcutContext*>(_v)); break;
        case 12: setAutoRepeat(*reinterpret_cast< bool*>(_v)); break;
        case 13: setVisible(*reinterpret_cast< bool*>(_v)); break;
        case 14: setMenuRole(*reinterpret_cast< MenuRole*>(_v)); break;
        case 15: setIconVisibleInMenu(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        bool *_b = reinterpret_cast<bool*>(_a[0]);
        switch (_id) {
        case 1: *_b = isCheckable(); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 16;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QAction::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QAction::triggered(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, 2, _a);
}

// SIGNAL 3
void QAction::hovered()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void QAction::toggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QAction::activated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, 6, _a);
}
QT_END_MOC_NAMESPACE
