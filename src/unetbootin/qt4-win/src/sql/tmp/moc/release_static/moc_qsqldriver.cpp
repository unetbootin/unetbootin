/****************************************************************************
** Meta object code from reading C++ file 'qsqldriver.h'
**
** Created: Fri May 30 22:34:51 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/qsqldriver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsqldriver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSqlDriver[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   12,   39,   11, 0x09,
      91,   12,   39,   11, 0x09,
     154,   11,  142,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_QSqlDriver[] = {
    "QSqlDriver\0\0name\0notification(QString)\0"
    "bool\0subscribeToNotificationImplementation(QString)\0"
    "unsubscribeFromNotificationImplementation(QString)\0"
    "QStringList\0subscribedToNotificationsImplementation()\0"
};

const QMetaObject QSqlDriver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QSqlDriver,
      qt_meta_data_QSqlDriver, 0 }
};

const QMetaObject *QSqlDriver::metaObject() const
{
    return &staticMetaObject;
}

void *QSqlDriver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSqlDriver))
	return static_cast<void*>(const_cast< QSqlDriver*>(this));
    return QObject::qt_metacast(_clname);
}

int QSqlDriver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: notification((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: { bool _r = subscribeToNotificationImplementation((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = unsubscribeFromNotificationImplementation((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { QStringList _r = subscribedToNotificationsImplementation();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QSqlDriver::notification(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
