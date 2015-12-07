/****************************************************************************
** Meta object code from reading C++ file 'previewmanager_p.h'
**
** Created: Thu Mar 5 20:47:17 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../shared/previewmanager_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'previewmanager_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PreviewManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      36,   35,   35,   35, 0x05,
      57,   35,   35,   35, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   35,   35,   35, 0x0a,
      96,   35,   35,   35, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PreviewManager[] = {
    "qdesigner_internal::PreviewManager\0\0"
    "firstPreviewOpened()\0lastPreviewClosed()\0"
    "closeAllPreviews()\0slotZoomChanged(int)\0"
};

const QMetaObject qdesigner_internal::PreviewManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PreviewManager,
      qt_meta_data_qdesigner_internal__PreviewManager, 0 }
};

const QMetaObject *qdesigner_internal::PreviewManager::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PreviewManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PreviewManager))
        return static_cast<void*>(const_cast< PreviewManager*>(this));
    return QObject::qt_metacast(_clname);
}

int qdesigner_internal::PreviewManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: firstPreviewOpened(); break;
        case 1: lastPreviewClosed(); break;
        case 2: closeAllPreviews(); break;
        case 3: slotZoomChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::PreviewManager::firstPreviewOpened()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void qdesigner_internal::PreviewManager::lastPreviewClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
