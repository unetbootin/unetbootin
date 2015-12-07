/****************************************************************************
** Meta object code from reading C++ file 'qdatetimeedit_p.h'
**
** Created: Fri May 30 22:33:38 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/qdatetimeedit_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qdatetimeedit_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCalendarPopup[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      21,   16,   15,   15, 0x05,
      46,   38,   15,   15, 0x05,
      77,   69,   15,   15, 0x05,
      99,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     113,   16,   15,   15, 0x08,
     133,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QCalendarPopup[] = {
    "QCalendarPopup\0\0date\0activated(QDate)\0"
    "newDate\0newDateSelected(QDate)\0oldDate\0"
    "hidingCalendar(QDate)\0resetButton()\0"
    "dateSelected(QDate)\0dateSelectionChanged()\0"
};

const QMetaObject QCalendarPopup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QCalendarPopup,
      qt_meta_data_QCalendarPopup, 0 }
};

const QMetaObject *QCalendarPopup::metaObject() const
{
    return &staticMetaObject;
}

void *QCalendarPopup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCalendarPopup))
	return static_cast<void*>(const_cast< QCalendarPopup*>(this));
    return QWidget::qt_metacast(_clname);
}

int QCalendarPopup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 1: newDateSelected((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 2: hidingCalendar((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 3: resetButton(); break;
        case 4: dateSelected((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 5: dateSelectionChanged(); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QCalendarPopup::activated(const QDate & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QCalendarPopup::newDateSelected(const QDate & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QCalendarPopup::hidingCalendar(const QDate & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QCalendarPopup::resetButton()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
