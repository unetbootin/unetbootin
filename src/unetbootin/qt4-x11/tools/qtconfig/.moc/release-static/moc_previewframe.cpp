/****************************************************************************
** Meta object code from reading C++ file 'previewframe.h'
**
** Created: Thu Mar 5 20:59:28 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../previewframe.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'previewframe.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Workspace[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_Workspace[] = {
    "Workspace\0"
};

const QMetaObject Workspace::staticMetaObject = {
    { &QMdiArea::staticMetaObject, qt_meta_stringdata_Workspace,
      qt_meta_data_Workspace, 0 }
};

const QMetaObject *Workspace::metaObject() const
{
    return &staticMetaObject;
}

void *Workspace::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Workspace))
        return static_cast<void*>(const_cast< Workspace*>(this));
    return QMdiArea::qt_metacast(_clname);
}

int Workspace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMdiArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_PreviewFrame[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_PreviewFrame[] = {
    "PreviewFrame\0"
};

const QMetaObject PreviewFrame::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_PreviewFrame,
      qt_meta_data_PreviewFrame, 0 }
};

const QMetaObject *PreviewFrame::metaObject() const
{
    return &staticMetaObject;
}

void *PreviewFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PreviewFrame))
        return static_cast<void*>(const_cast< PreviewFrame*>(this));
    return QFrame::qt_metacast(_clname);
}

int PreviewFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
