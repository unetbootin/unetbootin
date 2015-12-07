/****************************************************************************
** Meta object code from reading C++ file 'qdbusconnection.h'
**
** Created: Thu Mar 5 19:07:19 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qdbusconnection.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdbusconnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QDBusConnection[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       3,   12, // enums/sets
       0,    0, // constructors

 // enums: name, flags, count, data
      16, 0x0,    3,   24,
      24, 0x1,   15,   30,
      40, 0x0,    2,   60,

 // enum data: key, value
      55, uint(QDBusConnection::SessionBus),
      66, uint(QDBusConnection::SystemBus),
      76, uint(QDBusConnection::ActivationBus),
      90, uint(QDBusConnection::ExportAdaptors),
     105, uint(QDBusConnection::ExportScriptableSlots),
     127, uint(QDBusConnection::ExportScriptableSignals),
     151, uint(QDBusConnection::ExportScriptableProperties),
     178, uint(QDBusConnection::ExportScriptableContents),
     203, uint(QDBusConnection::ExportNonScriptableSlots),
     228, uint(QDBusConnection::ExportNonScriptableSignals),
     255, uint(QDBusConnection::ExportNonScriptableProperties),
     285, uint(QDBusConnection::ExportNonScriptableContents),
     313, uint(QDBusConnection::ExportAllSlots),
     328, uint(QDBusConnection::ExportAllSignals),
     345, uint(QDBusConnection::ExportAllProperties),
     365, uint(QDBusConnection::ExportAllContents),
     383, uint(QDBusConnection::ExportAllSignal),
     399, uint(QDBusConnection::ExportChildObjects),
     418, uint(QDBusConnection::UnregisterNode),
     433, uint(QDBusConnection::UnregisterTree),

       0        // eod
};

static const char qt_meta_stringdata_QDBusConnection[] = {
    "QDBusConnection\0BusType\0RegisterOptions\0"
    "UnregisterMode\0SessionBus\0SystemBus\0"
    "ActivationBus\0ExportAdaptors\0"
    "ExportScriptableSlots\0ExportScriptableSignals\0"
    "ExportScriptableProperties\0"
    "ExportScriptableContents\0"
    "ExportNonScriptableSlots\0"
    "ExportNonScriptableSignals\0"
    "ExportNonScriptableProperties\0"
    "ExportNonScriptableContents\0ExportAllSlots\0"
    "ExportAllSignals\0ExportAllProperties\0"
    "ExportAllContents\0ExportAllSignal\0"
    "ExportChildObjects\0UnregisterNode\0"
    "UnregisterTree\0"
};

const QMetaObject QDBusConnection::staticMetaObject = {
    { 0, qt_meta_stringdata_QDBusConnection,
      qt_meta_data_QDBusConnection, 0 }
};
QT_END_MOC_NAMESPACE
