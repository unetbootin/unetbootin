/****************************************************************************
** Meta object code from reading C++ file 'qhelpindexwidget.h'
**
** Created: Thu Mar 5 20:32:35 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qhelpindexwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhelpindexwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QHelpIndexModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      40,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   16,   16,   16, 0x08,
      82,   71,   16,   16, 0x08,
     104,   16,   16,   16, 0x28,

       0        // eod
};

static const char qt_meta_stringdata_QHelpIndexModel[] = {
    "QHelpIndexModel\0\0indexCreationStarted()\0"
    "indexCreated()\0insertIndices()\0"
    "onShutDown\0invalidateIndex(bool)\0"
    "invalidateIndex()\0"
};

const QMetaObject QHelpIndexModel::staticMetaObject = {
    { &QStringListModel::staticMetaObject, qt_meta_stringdata_QHelpIndexModel,
      qt_meta_data_QHelpIndexModel, 0 }
};

const QMetaObject *QHelpIndexModel::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpIndexModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpIndexModel))
        return static_cast<void*>(const_cast< QHelpIndexModel*>(this));
    return QStringListModel::qt_metacast(_clname);
}

int QHelpIndexModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStringListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: indexCreationStarted(); break;
        case 1: indexCreated(); break;
        case 2: insertIndices(); break;
        case 3: invalidateIndex((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: invalidateIndex(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QHelpIndexModel::indexCreationStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QHelpIndexModel::indexCreated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
static const uint qt_meta_data_QHelpIndexWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      31,   18,   17,   17, 0x05,
      73,   59,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     132,  116,   17,   17, 0x0a,
     170,  163,   17,   17, 0x2a,
     193,   17,   17,   17, 0x0a,
     221,  215,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QHelpIndexWidget[] = {
    "QHelpIndexWidget\0\0link,keyword\0"
    "linkActivated(QUrl,QString)\0links,keyword\0"
    "linksActivated(QMap<QString,QUrl>,QString)\0"
    "filter,wildcard\0filterIndices(QString,QString)\0"
    "filter\0filterIndices(QString)\0"
    "activateCurrentItem()\0index\0"
    "showLink(QModelIndex)\0"
};

const QMetaObject QHelpIndexWidget::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_QHelpIndexWidget,
      qt_meta_data_QHelpIndexWidget, 0 }
};

const QMetaObject *QHelpIndexWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QHelpIndexWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QHelpIndexWidget))
        return static_cast<void*>(const_cast< QHelpIndexWidget*>(this));
    return QListView::qt_metacast(_clname);
}

int QHelpIndexWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: linkActivated((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: linksActivated((*reinterpret_cast< const QMap<QString,QUrl>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: filterIndices((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: filterIndices((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: activateCurrentItem(); break;
        case 5: showLink((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QHelpIndexWidget::linkActivated(const QUrl & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QHelpIndexWidget::linksActivated(const QMap<QString,QUrl> & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
