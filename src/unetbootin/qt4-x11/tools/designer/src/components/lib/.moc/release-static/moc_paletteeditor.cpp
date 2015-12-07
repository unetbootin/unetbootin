/****************************************************************************
** Meta object code from reading C++ file 'paletteeditor.h'
**
** Created: Thu Mar 5 20:51:49 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../propertyeditor/paletteeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paletteeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qdesigner_internal__PaletteEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      35,   34,   34,   34, 0x08,
      71,   34,   34,   34, 0x08,
      96,   34,   34,   34, 0x08,
     123,   34,   34,   34, 0x08,
     150,   34,   34,   34, 0x08,
     176,   34,   34,   34, 0x08,
     210,  202,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PaletteEditor[] = {
    "qdesigner_internal::PaletteEditor\0\0"
    "on_buildButton_colorChanged(QColor)\0"
    "on_activeRadio_clicked()\0"
    "on_inactiveRadio_clicked()\0"
    "on_disabledRadio_clicked()\0"
    "on_computeRadio_clicked()\0"
    "on_detailsRadio_clicked()\0palette\0"
    "paletteChanged(QPalette)\0"
};

const QMetaObject qdesigner_internal::PaletteEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PaletteEditor,
      qt_meta_data_qdesigner_internal__PaletteEditor, 0 }
};

const QMetaObject *qdesigner_internal::PaletteEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PaletteEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PaletteEditor))
        return static_cast<void*>(const_cast< PaletteEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int qdesigner_internal::PaletteEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_buildButton_colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: on_activeRadio_clicked(); break;
        case 2: on_inactiveRadio_clicked(); break;
        case 3: on_disabledRadio_clicked(); break;
        case 4: on_computeRadio_clicked(); break;
        case 5: on_detailsRadio_clicked(); break;
        case 6: paletteChanged((*reinterpret_cast< const QPalette(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
static const uint qt_meta_data_qdesigner_internal__PaletteModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       1,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      42,   34,   33,   33, 0x05,

 // properties: name, type, flags
      87,   67, 0x00095009,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__PaletteModel[] = {
    "qdesigner_internal::PaletteModel\0\0"
    "palette\0paletteChanged(QPalette)\0"
    "QPalette::ColorRole\0colorRole\0"
};

static const QMetaObject *qt_meta_extradata_qdesigner_internal__PaletteModel[] = {
        &QPalette::staticMetaObject,0
};

static const QMetaObjectExtraData qt_meta_extradata2_qdesigner_internal__PaletteModel = {
    qt_meta_extradata_qdesigner_internal__PaletteModel, 0 
};

const QMetaObject qdesigner_internal::PaletteModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_qdesigner_internal__PaletteModel,
      qt_meta_data_qdesigner_internal__PaletteModel, &qt_meta_extradata2_qdesigner_internal__PaletteModel }
};

const QMetaObject *qdesigner_internal::PaletteModel::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::PaletteModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__PaletteModel))
        return static_cast<void*>(const_cast< PaletteModel*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int qdesigner_internal::PaletteModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: paletteChanged((*reinterpret_cast< const QPalette(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QPalette::ColorRole*>(_v) = colorRole(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void qdesigner_internal::PaletteModel::paletteChanged(const QPalette & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__BrushEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      40,   33,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   32,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__BrushEditor[] = {
    "qdesigner_internal::BrushEditor\0\0"
    "widget\0changed(QWidget*)\0brushChanged()\0"
};

const QMetaObject qdesigner_internal::BrushEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__BrushEditor,
      qt_meta_data_qdesigner_internal__BrushEditor, 0 }
};

const QMetaObject *qdesigner_internal::BrushEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::BrushEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__BrushEditor))
        return static_cast<void*>(const_cast< BrushEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::BrushEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changed((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: brushChanged(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::BrushEditor::changed(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__RoleEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      39,   32,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qdesigner_internal__RoleEditor[] = {
    "qdesigner_internal::RoleEditor\0\0widget\0"
    "changed(QWidget*)\0emitResetProperty()\0"
};

const QMetaObject qdesigner_internal::RoleEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_qdesigner_internal__RoleEditor,
      qt_meta_data_qdesigner_internal__RoleEditor, 0 }
};

const QMetaObject *qdesigner_internal::RoleEditor::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::RoleEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__RoleEditor))
        return static_cast<void*>(const_cast< RoleEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int qdesigner_internal::RoleEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changed((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: emitResetProperty(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void qdesigner_internal::RoleEditor::changed(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_qdesigner_internal__ColorDelegate[] = {

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

static const char qt_meta_stringdata_qdesigner_internal__ColorDelegate[] = {
    "qdesigner_internal::ColorDelegate\0"
};

const QMetaObject qdesigner_internal::ColorDelegate::staticMetaObject = {
    { &QItemDelegate::staticMetaObject, qt_meta_stringdata_qdesigner_internal__ColorDelegate,
      qt_meta_data_qdesigner_internal__ColorDelegate, 0 }
};

const QMetaObject *qdesigner_internal::ColorDelegate::metaObject() const
{
    return &staticMetaObject;
}

void *qdesigner_internal::ColorDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qdesigner_internal__ColorDelegate))
        return static_cast<void*>(const_cast< ColorDelegate*>(this));
    return QItemDelegate::qt_metacast(_clname);
}

int qdesigner_internal::ColorDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
