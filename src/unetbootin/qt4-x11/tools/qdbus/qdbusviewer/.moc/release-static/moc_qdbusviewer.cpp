/****************************************************************************
** Meta object code from reading C++ file 'qdbusviewer.h'
**
** Created: Thu Mar 5 21:00:26 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdbusviewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdbusviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusViewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      23,   12,   12,   12, 0x0a,
      36,   31,   12,   12, 0x08,
      69,   12,   12,   12, 0x08,
      97,   93,   12,   12, 0x08,
     131,   93,   12,   12, 0x08,
     156,   93,   12,   12, 0x08,
     182,   93,   12,   12, 0x08,
     212,  208,   12,   12, 0x08,
     238,   12,   12,   12, 0x08,
     264,  256,   12,   12, 0x08,
     291,  256,   12,   12, 0x08,
     343,  320,   12,   12, 0x08,
     388,   31,   12,   12, 0x08,
     410,  208,   12,   12, 0x08,
     432,  428,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QDBusViewer[] = {
    "QDBusViewer\0\0refresh()\0about()\0item\0"
    "serviceChanged(QTreeWidgetItem*)\0"
    "showContextMenu(QPoint)\0sig\0"
    "connectionRequested(BusSignature)\0"
    "callMethod(BusSignature)\0"
    "getProperty(BusSignature)\0"
    "setProperty(BusSignature)\0msg\0"
    "dumpMessage(QDBusMessage)\0refreshChildren()\0"
    "service\0serviceRegistered(QString)\0"
    "serviceUnregistered(QString)\0"
    "name,oldOwner,newOwner\0"
    "serviceOwnerChanged(QString,QString,QString)\0"
    "activate(QModelIndex)\0logError(QString)\0"
    "url\0anchorClicked(QUrl)\0"
};

const QMetaObject QDBusViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QDBusViewer,
      qt_meta_data_QDBusViewer, 0 }
};

const QMetaObject *QDBusViewer::metaObject() const
{
    return &staticMetaObject;
}

void *QDBusViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QDBusViewer))
        return static_cast<void*>(const_cast< QDBusViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int QDBusViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refresh(); break;
        case 1: about(); break;
        case 2: serviceChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 3: showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 4: connectionRequested((*reinterpret_cast< const BusSignature(*)>(_a[1]))); break;
        case 5: callMethod((*reinterpret_cast< const BusSignature(*)>(_a[1]))); break;
        case 6: getProperty((*reinterpret_cast< const BusSignature(*)>(_a[1]))); break;
        case 7: setProperty((*reinterpret_cast< const BusSignature(*)>(_a[1]))); break;
        case 8: dumpMessage((*reinterpret_cast< const QDBusMessage(*)>(_a[1]))); break;
        case 9: refreshChildren(); break;
        case 10: serviceRegistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: serviceUnregistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: serviceOwnerChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 13: activate((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 14: logError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: anchorClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
