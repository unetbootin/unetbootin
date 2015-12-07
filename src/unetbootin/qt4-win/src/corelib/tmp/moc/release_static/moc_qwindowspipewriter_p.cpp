/****************************************************************************
** Meta object code from reading C++ file 'qwindowspipewriter_p.h'
**
** Created: Fri May 30 22:18:26 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../io/qwindowspipewriter_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwindowspipewriter_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWindowsPipeWriter[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QWindowsPipeWriter[] = {
    "QWindowsPipeWriter\0\0canWrite()\0"
};

const QMetaObject QWindowsPipeWriter::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_QWindowsPipeWriter,
      qt_meta_data_QWindowsPipeWriter, 0 }
};

const QMetaObject *QWindowsPipeWriter::metaObject() const
{
    return &staticMetaObject;
}

void *QWindowsPipeWriter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWindowsPipeWriter))
	return static_cast<void*>(const_cast< QWindowsPipeWriter*>(this));
    return QThread::qt_metacast(_clname);
}

int QWindowsPipeWriter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: canWrite(); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QWindowsPipeWriter::canWrite()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
