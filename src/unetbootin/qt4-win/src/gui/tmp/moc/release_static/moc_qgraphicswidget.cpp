/****************************************************************************
** Meta object code from reading C++ file 'qgraphicswidget.h'
**
** Created: Fri May 30 22:34:19 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../graphicsview/qgraphicswidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qgraphicswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QGraphicsWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       9,   15, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      22,   16,   17,   16, 0x0a,

 // properties: name, type, flags
      39,   30, 0x44095103,
      53,   47, 0x40095103,
      78,   58, 0x0009510f,
     101,   94, 0x16095003,
     122,  106, 0x0009510b,
     134,   17, 0x01095103,
     142,   17, 0x01095103,
     166,  150, 0x0009510b,
     186,  178, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QGraphicsWidget[] = {
    "QGraphicsWidget\0\0bool\0close()\0QPalette\0"
    "palette\0QFont\0font\0Qt::LayoutDirection\0"
    "layoutDirection\0QSizeF\0size\0Qt::FocusPolicy\0"
    "focusPolicy\0enabled\0visible\0Qt::WindowFlags\0"
    "windowFlags\0QString\0windowTitle\0"
};

const QMetaObject QGraphicsWidget::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QGraphicsWidget,
      qt_meta_data_QGraphicsWidget, 0 }
};

const QMetaObject *QGraphicsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QGraphicsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGraphicsWidget))
	return static_cast<void*>(const_cast< QGraphicsWidget*>(this));
    if (!strcmp(_clname, "QGraphicsItem"))
	return static_cast< QGraphicsItem*>(const_cast< QGraphicsWidget*>(this));
    if (!strcmp(_clname, "QGraphicsLayoutItem"))
	return static_cast< QGraphicsLayoutItem*>(const_cast< QGraphicsWidget*>(this));
    return QObject::qt_metacast(_clname);
}

int QGraphicsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = close();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QPalette*>(_v) = palette(); break;
        case 1: *reinterpret_cast< QFont*>(_v) = font(); break;
        case 2: *reinterpret_cast< Qt::LayoutDirection*>(_v) = layoutDirection(); break;
        case 3: *reinterpret_cast< QSizeF*>(_v) = size(); break;
        case 4: *reinterpret_cast< Qt::FocusPolicy*>(_v) = focusPolicy(); break;
        case 5: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 6: *reinterpret_cast< bool*>(_v) = isVisible(); break;
        case 7: *reinterpret_cast< Qt::WindowFlags*>(_v) = windowFlags(); break;
        case 8: *reinterpret_cast< QString*>(_v) = windowTitle(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPalette(*reinterpret_cast< QPalette*>(_v)); break;
        case 1: setFont(*reinterpret_cast< QFont*>(_v)); break;
        case 2: setLayoutDirection(*reinterpret_cast< Qt::LayoutDirection*>(_v)); break;
        case 3: resize(*reinterpret_cast< QSizeF*>(_v)); break;
        case 4: setFocusPolicy(*reinterpret_cast< Qt::FocusPolicy*>(_v)); break;
        case 5: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 6: setVisible(*reinterpret_cast< bool*>(_v)); break;
        case 7: setWindowFlags(*reinterpret_cast< Qt::WindowFlags*>(_v)); break;
        case 8: setWindowTitle(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        switch (_id) {
        case 2: unsetLayoutDirection(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
