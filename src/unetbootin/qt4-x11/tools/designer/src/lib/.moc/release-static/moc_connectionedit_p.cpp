/****************************************************************************
** Meta object code from reading C++ file 'connectionedit_p.h'
**
** Created: Thu Mar 5 20:46:52 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/connectionedit_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connectionedit_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__ConnectionEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      40,   36,   35,   35, 0x05,
      70,   66,   35,   35, 0x05,
      99,   66,   35,   35, 0x05,
     136,   36,   35,   35, 0x05,
     159,   66,   35,   35, 0x05,
     195,  191,   35,   35, 0x05,
     221,   66,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
     252,   35,   35,   35, 0x0a,
     265,   35,   35,   35, 0x0a,
     277,   35,   35,   35, 0x0a,
     305,  294,   35,   35, 0x0a,
     329,   35,   35,   35, 0x0a,
     350,  348,   35,   35, 0x0a,
     376,  374,   35,   35, 0x0a,
     400,   35,   35,   35, 0x0a,
     421,  414,   35,   35, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__ConnectionEdit[] = {
    "qdesigner_internal::ConnectionEdit\0\0"
    "idx\0aboutToAddConnection(int)\0con\0"
    "connectionAdded(Connection*)\0"
    "aboutToRemoveConnection(Connection*)\0"
    "connectionRemoved(int)\0"
    "connectionSelected(Connection*)\0wgt\0"
    "widgetActivated(QWidget*)\0"
    "connectionChanged(Connection*)\0"
    "selectNone()\0selectAll()\0deleteSelected()\0"
    "background\0setBackground(QWidget*)\0"
    "updateBackground()\0w\0widgetRemoved(QWidget*)\0"
    "o\0objectRemoved(QObject*)\0updateLines()\0"
    "enable\0enableUpdateBackground(bool)\0"
};

const QMetaObject qdesigner_internal::ConnectionEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ConnectionEdit,
      qt_meta_data_qdesigner_internal__ConnectionEdit, 0 }
};

const QMetaObject *qdesigner_internal::ConnectionEdit::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ConnectionEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ConnectionEdit))
        return static_cast<void*>(const_cast< ConnectionEdit*>(this));
    if (!strcmp(_clname, "CETypes"))
        return static_cast< CETypes*>(const_cast< ConnectionEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::ConnectionEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aboutToAddConnection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: connectionAdded((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        case 2: aboutToRemoveConnection((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        case 3: connectionRemoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: connectionSelected((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        case 5: widgetActivated((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 6: connectionChanged((*reinterpret_cast< Connection*(*)>(_a[1]))); break;
        case 7: selectNone(); break;
        case 8: selectAll(); break;
        case 9: deleteSelected(); break;
        case 10: setBackground((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 11: updateBackground(); break;
        case 12: widgetRemoved((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 13: objectRemoved((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 14: updateLines(); break;
        case 15: enableUpdateBackground((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::ConnectionEdit::aboutToAddConnection(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qdesigner_internal::ConnectionEdit::connectionAdded(Connection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qdesigner_internal::ConnectionEdit::aboutToRemoveConnection(Connection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void qdesigner_internal::ConnectionEdit::connectionRemoved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void qdesigner_internal::ConnectionEdit::connectionSelected(Connection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void qdesigner_internal::ConnectionEdit::widgetActivated(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void qdesigner_internal::ConnectionEdit::connectionChanged(Connection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
