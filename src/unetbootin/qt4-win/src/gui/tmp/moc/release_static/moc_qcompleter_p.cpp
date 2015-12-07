/****************************************************************************
** Meta object code from reading C++ file 'qcompleter_p.h'
**
** Created: Fri May 30 22:34:20 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../util/qcompleter_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcompleter_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCompletionModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   17,   17,   17, 0x0a,
      43,   17,   17,   17, 0x0a,
      58,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QCompletionModel[] = {
    "QCompletionModel\0\0rowsAdded()\0"
    "invalidate()\0rowsInserted()\0"
    "modelDestroyed()\0"
};

const QMetaObject QCompletionModel::staticMetaObject = {
    { &QAbstractProxyModel::staticMetaObject, qt_meta_stringdata_QCompletionModel,
      qt_meta_data_QCompletionModel, 0 }
};

const QMetaObject *QCompletionModel::metaObject() const
{
    return &staticMetaObject;
}

void *QCompletionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCompletionModel))
	return static_cast<void*>(const_cast< QCompletionModel*>(this));
    return QAbstractProxyModel::qt_metacast(_clname);
}

int QCompletionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rowsAdded(); break;
        case 1: invalidate(); break;
        case 2: rowsInserted(); break;
        case 3: modelDestroyed(); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QCompletionModel::rowsAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
