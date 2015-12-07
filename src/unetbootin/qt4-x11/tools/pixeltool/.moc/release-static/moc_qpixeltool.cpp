/****************************************************************************
** Meta object code from reading C++ file 'qpixeltool.h'
**
** Created: Thu Mar 5 20:37:06 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qpixeltool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qpixeltool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPixelTool[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x0a,
      39,   30,   11,   11, 0x0a,
      56,   11,   11,   11, 0x0a,
      69,   11,   11,   11, 0x0a,
      92,   84,   11,   11, 0x0a,
     113,   11,   11,   11, 0x0a,
     131,   11,   11,   11, 0x0a,
     144,   11,   11,   11, 0x0a,
     163,   11,   11,   11, 0x0a,
     182,   11,   11,   11, 0x0a,
     197,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QPixelTool[] = {
    "QPixelTool\0\0zoom\0setZoom(int)\0gridSize\0"
    "setGridSize(int)\0toggleGrid()\0"
    "toggleFreeze()\0visible\0setZoomVisible(bool)\0"
    "copyToClipboard()\0saveToFile()\0"
    "increaseGridSize()\0decreaseGridSize()\0"
    "increaseZoom()\0decreaseZoom()\0"
};

const QMetaObject QPixelTool::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QPixelTool,
      qt_meta_data_QPixelTool, 0 }
};

const QMetaObject *QPixelTool::metaObject() const
{
    return &staticMetaObject;
}

void *QPixelTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPixelTool))
        return static_cast<void*>(const_cast< QPixelTool*>(this));
    return QWidget::qt_metacast(_clname);
}

int QPixelTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setZoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: setGridSize((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: toggleGrid(); break;
        case 3: toggleFreeze(); break;
        case 4: setZoomVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: copyToClipboard(); break;
        case 6: saveToFile(); break;
        case 7: increaseGridSize(); break;
        case 8: decreaseGridSize(); break;
        case 9: increaseZoom(); break;
        case 10: decreaseZoom(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
