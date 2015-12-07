/****************************************************************************
** Meta object code from reading C++ file 'qprintpreviewwidget.h'
**
** Created: Thu Mar 5 18:53:17 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/qprintpreviewwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qprintpreviewwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPrintPreviewWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      29,   21,   20,   20, 0x05,
      55,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      72,   20,   20,   20, 0x0a,
      85,   80,   20,   20, 0x0a,
      99,   20,   20,   20, 0x2a,
     108,   80,   20,   20, 0x0a,
     123,   20,   20,   20, 0x2a,
     144,  133,   20,   20, 0x0a,
     177,  165,   20,   20, 0x0a,
     224,  215,   20,   20, 0x0a,
     255,  246,   20,   20, 0x0a,
     288,  277,   20,   20, 0x0a,
     308,   20,   20,   20, 0x0a,
     321,   20,   20,   20, 0x0a,
     333,   20,   20,   20, 0x0a,
     359,   20,   20,   20, 0x0a,
     384,   20,   20,   20, 0x0a,
     408,   20,   20,   20, 0x0a,
     433,   20,   20,   20, 0x0a,
     455,   20,   20,   20, 0x0a,
     471,   20,   20,   20, 0x08,
     480,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QPrintPreviewWidget[] = {
    "QPrintPreviewWidget\0\0printer\0"
    "paintRequested(QPrinter*)\0previewChanged()\0"
    "print()\0zoom\0zoomIn(qreal)\0zoomIn()\0"
    "zoomOut(qreal)\0zoomOut()\0zoomFactor\0"
    "setZoomFactor(qreal)\0orientation\0"
    "setOrientation(QPrinter::Orientation)\0"
    "viewMode\0setViewMode(ViewMode)\0zoomMode\0"
    "setZoomMode(ZoomMode)\0pageNumber\0"
    "setCurrentPage(int)\0fitToWidth()\0"
    "fitInView()\0setLandscapeOrientation()\0"
    "setPortraitOrientation()\0"
    "setSinglePageViewMode()\0"
    "setFacingPagesViewMode()\0setAllPagesViewMode()\0"
    "updatePreview()\0_q_fit()\0"
    "_q_updateCurrentPage()\0"
};

const QMetaObject QPrintPreviewWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QPrintPreviewWidget,
      qt_meta_data_QPrintPreviewWidget, 0 }
};

const QMetaObject *QPrintPreviewWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QPrintPreviewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPrintPreviewWidget))
        return static_cast<void*>(const_cast< QPrintPreviewWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QPrintPreviewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: paintRequested((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        case 1: previewChanged(); break;
        case 2: print(); break;
        case 3: zoomIn((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 4: zoomIn(); break;
        case 5: zoomOut((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 6: zoomOut(); break;
        case 7: setZoomFactor((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 8: setOrientation((*reinterpret_cast< QPrinter::Orientation(*)>(_a[1]))); break;
        case 9: setViewMode((*reinterpret_cast< ViewMode(*)>(_a[1]))); break;
        case 10: setZoomMode((*reinterpret_cast< ZoomMode(*)>(_a[1]))); break;
        case 11: setCurrentPage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: fitToWidth(); break;
        case 13: fitInView(); break;
        case 14: setLandscapeOrientation(); break;
        case 15: setPortraitOrientation(); break;
        case 16: setSinglePageViewMode(); break;
        case 17: setFacingPagesViewMode(); break;
        case 18: setAllPagesViewMode(); break;
        case 19: updatePreview(); break;
        case 20: d_func()->_q_fit(); break;
        case 21: d_func()->_q_updateCurrentPage(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void QPrintPreviewWidget::paintRequested(QPrinter * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QPrintPreviewWidget::previewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
