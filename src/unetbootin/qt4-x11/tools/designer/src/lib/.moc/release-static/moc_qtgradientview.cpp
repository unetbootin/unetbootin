/****************************************************************************
** Meta object code from reading C++ file 'qtgradientview.h'
**
** Created: Thu Mar 5 20:46:48 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../shared/qtgradienteditor/qtgradientview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtgradientview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtGradientView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      19,   16,   15,   15, 0x05,
      51,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      90,   78,   15,   15, 0x08,
     136,  127,   15,   15, 0x08,
     188,  173,   15,   15, 0x08,
     227,   16,   15,   15, 0x08,
     256,   15,   15,   15, 0x08,
     274,   15,   15,   15, 0x08,
     293,   15,   15,   15, 0x08,
     314,   15,   15,   15, 0x08,
     340,  335,   15,   15, 0x08,
     377,  335,   15,   15, 0x08,
     418,  335,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtGradientView[] = {
    "QtGradientView\0\0id\0currentGradientChanged(QString)\0"
    "gradientActivated(QString)\0id,gradient\0"
    "slotGradientAdded(QString,QGradient)\0"
    "id,newId\0slotGradientRenamed(QString,QString)\0"
    "id,newGradient\0slotGradientChanged(QString,QGradient)\0"
    "slotGradientRemoved(QString)\0"
    "slotNewGradient()\0slotEditGradient()\0"
    "slotRemoveGradient()\0slotRenameGradient()\0"
    "item\0slotRenameGradient(QListWidgetItem*)\0"
    "slotCurrentItemChanged(QListWidgetItem*)\0"
    "slotGradientActivated(QListWidgetItem*)\0"
};

const QMetaObject QtGradientView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtGradientView,
      qt_meta_data_QtGradientView, 0 }
};

const QMetaObject *QtGradientView::metaObject() const
{
    return &staticMetaObject;
}

void *QtGradientView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtGradientView))
        return static_cast<void*>(const_cast< QtGradientView*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtGradientView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentGradientChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: gradientActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: slotGradientAdded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QGradient(*)>(_a[2]))); break;
        case 3: slotGradientRenamed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: slotGradientChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QGradient(*)>(_a[2]))); break;
        case 5: slotGradientRemoved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: slotNewGradient(); break;
        case 7: slotEditGradient(); break;
        case 8: slotRemoveGradient(); break;
        case 9: slotRenameGradient(); break;
        case 10: slotRenameGradient((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 11: slotCurrentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 12: slotGradientActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void QtGradientView::currentGradientChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtGradientView::gradientActivated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
