/****************************************************************************
** Meta object code from reading C++ file 'qwizard.h'
**
** Created: Fri May 30 22:30:24 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dialogs/qwizard.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWizard[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       6,   50, // properties
       3,   68, // enums/sets

 // signals: signature, parameters, type, tag, flags
      12,    9,    8,    8, 0x05,
      34,    8,    8,    8, 0x05,
      56,   50,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      81,    8,    8,    8, 0x0a,
      88,    8,    8,    8, 0x0a,
      95,    8,    8,    8, 0x0a,
     105,    8,    8,    8, 0x08,
     134,    8,    8,    8, 0x08,

 // properties: name, type, flags
     170,  158, 0x0009510b,
     196,  182, 0x0009510b,
     219,  204, 0x0009510b,
     231,  204, 0x0009510b,
     250,  246, 0x02095103,
     258,  246, 0x02095001,

 // enums: name, flags, count, data
     158, 0x0,    5,   80,
     268, 0x0,   16,   90,
     182, 0x1,   16,  122,

 // enum data: key, value
     281, uint(QWizard::ClassicStyle),
     294, uint(QWizard::ModernStyle),
     306, uint(QWizard::MacStyle),
     315, uint(QWizard::AeroStyle),
     325, uint(QWizard::NStyles),
     333, uint(QWizard::IndependentPages),
     350, uint(QWizard::IgnoreSubTitles),
     366, uint(QWizard::ExtendedWatermarkPixmap),
     390, uint(QWizard::NoDefaultButton),
     406, uint(QWizard::NoBackButtonOnStartPage),
     430, uint(QWizard::NoBackButtonOnLastPage),
     453, uint(QWizard::DisabledBackButtonOnLastPage),
     482, uint(QWizard::HaveNextButtonOnLastPage),
     507, uint(QWizard::HaveFinishButtonOnEarlyPages),
     536, uint(QWizard::NoCancelButton),
     551, uint(QWizard::CancelButtonOnLeft),
     570, uint(QWizard::HaveHelpButton),
     585, uint(QWizard::HelpButtonOnRight),
     603, uint(QWizard::HaveCustomButton1),
     621, uint(QWizard::HaveCustomButton2),
     639, uint(QWizard::HaveCustomButton3),
     333, uint(QWizard::IndependentPages),
     350, uint(QWizard::IgnoreSubTitles),
     366, uint(QWizard::ExtendedWatermarkPixmap),
     390, uint(QWizard::NoDefaultButton),
     406, uint(QWizard::NoBackButtonOnStartPage),
     430, uint(QWizard::NoBackButtonOnLastPage),
     453, uint(QWizard::DisabledBackButtonOnLastPage),
     482, uint(QWizard::HaveNextButtonOnLastPage),
     507, uint(QWizard::HaveFinishButtonOnEarlyPages),
     536, uint(QWizard::NoCancelButton),
     551, uint(QWizard::CancelButtonOnLeft),
     570, uint(QWizard::HaveHelpButton),
     585, uint(QWizard::HelpButtonOnRight),
     603, uint(QWizard::HaveCustomButton1),
     621, uint(QWizard::HaveCustomButton2),
     639, uint(QWizard::HaveCustomButton3),

       0        // eod
};

static const char qt_meta_stringdata_QWizard[] = {
    "QWizard\0\0id\0currentIdChanged(int)\0"
    "helpRequested()\0which\0customButtonClicked(int)\0"
    "back()\0next()\0restart()\0"
    "_q_emitCustomButtonClicked()\0"
    "_q_updateButtonStates()\0WizardStyle\0"
    "wizardStyle\0WizardOptions\0options\0"
    "Qt::TextFormat\0titleFormat\0subTitleFormat\0"
    "int\0startId\0currentId\0WizardOption\0"
    "ClassicStyle\0ModernStyle\0MacStyle\0"
    "AeroStyle\0NStyles\0IndependentPages\0"
    "IgnoreSubTitles\0ExtendedWatermarkPixmap\0"
    "NoDefaultButton\0NoBackButtonOnStartPage\0"
    "NoBackButtonOnLastPage\0"
    "DisabledBackButtonOnLastPage\0"
    "HaveNextButtonOnLastPage\0"
    "HaveFinishButtonOnEarlyPages\0"
    "NoCancelButton\0CancelButtonOnLeft\0"
    "HaveHelpButton\0HelpButtonOnRight\0"
    "HaveCustomButton1\0HaveCustomButton2\0"
    "HaveCustomButton3\0"
};

const QMetaObject QWizard::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QWizard,
      qt_meta_data_QWizard, 0 }
};

const QMetaObject *QWizard::metaObject() const
{
    return &staticMetaObject;
}

void *QWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWizard))
	return static_cast<void*>(const_cast< QWizard*>(this));
    return QDialog::qt_metacast(_clname);
}

int QWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentIdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: helpRequested(); break;
        case 2: customButtonClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: back(); break;
        case 4: next(); break;
        case 5: restart(); break;
        case 6: d_func()->_q_emitCustomButtonClicked(); break;
        case 7: d_func()->_q_updateButtonStates(); break;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< WizardStyle*>(_v) = wizardStyle(); break;
        case 1: *reinterpret_cast<int*>(_v) = QFlag(options()); break;
        case 2: *reinterpret_cast< Qt::TextFormat*>(_v) = titleFormat(); break;
        case 3: *reinterpret_cast< Qt::TextFormat*>(_v) = subTitleFormat(); break;
        case 4: *reinterpret_cast< int*>(_v) = startId(); break;
        case 5: *reinterpret_cast< int*>(_v) = currentId(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setWizardStyle(*reinterpret_cast< WizardStyle*>(_v)); break;
        case 1: setOptions(QFlag(*reinterpret_cast<int*>(_v))); break;
        case 2: setTitleFormat(*reinterpret_cast< Qt::TextFormat*>(_v)); break;
        case 3: setSubTitleFormat(*reinterpret_cast< Qt::TextFormat*>(_v)); break;
        case 4: setStartId(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QWizard::currentIdChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QWizard::helpRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QWizard::customButtonClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_QWizardPage[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       2,   25, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   12,   12,   12, 0x08,
      61,   12,   12,   12, 0x08,

 // properties: name, type, flags
     100,   92, 0x0a095103,
     106,   92, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_QWizardPage[] = {
    "QWizardPage\0\0completeChanged()\0"
    "_q_maybeEmitCompleteChanged()\0"
    "_q_updateCachedCompleteState()\0QString\0"
    "title\0subTitle\0"
};

const QMetaObject QWizardPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QWizardPage,
      qt_meta_data_QWizardPage, 0 }
};

const QMetaObject *QWizardPage::metaObject() const
{
    return &staticMetaObject;
}

void *QWizardPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWizardPage))
	return static_cast<void*>(const_cast< QWizardPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int QWizardPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: completeChanged(); break;
        case 1: d_func()->_q_maybeEmitCompleteChanged(); break;
        case 2: d_func()->_q_updateCachedCompleteState(); break;
        }
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = title(); break;
        case 1: *reinterpret_cast< QString*>(_v) = subTitle(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTitle(*reinterpret_cast< QString*>(_v)); break;
        case 1: setSubTitle(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QWizardPage::completeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
