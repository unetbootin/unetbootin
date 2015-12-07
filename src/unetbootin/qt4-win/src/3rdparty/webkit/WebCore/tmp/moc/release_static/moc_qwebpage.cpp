/****************************************************************************
** Meta object code from reading C++ file 'qwebpage.h'
**
** Created: Fri May 30 23:18:08 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../WebKit/qt/Api/qwebpage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwebpage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWebPage[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   10, // methods
       6,  115, // properties
       3,  133, // enums/sets

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      33,   24,    9,    9, 0x05,
      54,   51,    9,    9, 0x05,
      96,   73,    9,    9, 0x05,
     138,  133,    9,    9, 0x05,
     164,    9,    9,    9, 0x05,
     189,  183,    9,    9, 0x05,
     219,  214,    9,    9, 0x05,
     260,  250,    9,    9, 0x05,
     305,  284,    9,    9, 0x05,
     336,    9,    9,    9, 0x05,
     359,  183,    9,    9, 0x05,
     390,  386,    9,    9, 0x05,
     416,  408,    9,    9, 0x05,
     455,  408,    9,    9, 0x05,
     496,  408,    9,    9, 0x05,
     541,  535,    9,    9, 0x05,
     584,  576,    9,    9, 0x05,
     619,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     639,    9,    9,    9, 0x08,
     677,  669,    9,    9, 0x08,

 // properties: name, type, flags
     710,  705, 0x01095001,
     727,  719, 0x0a095001,
     746,  740, 0x15095103,
     759,  705, 0x01095103,
     806,  785, 0x0009510b,
     836,  827, 0x44095103,

 // enums: name, flags, count, data
     844, 0x0,    6,  145,
     859, 0x0,   52,  157,
     785, 0x0,    3,  261,

 // enum data: key, value
     869, uint(QWebPage::NavigationTypeLinkClicked),
     895, uint(QWebPage::NavigationTypeFormSubmitted),
     923, uint(QWebPage::NavigationTypeBackOrForward),
     951, uint(QWebPage::NavigationTypeReload),
     972, uint(QWebPage::NavigationTypeFormResubmitted),
    1002, uint(QWebPage::NavigationTypeOther),
    1022, uint(QWebPage::NoWebAction),
    1034, uint(QWebPage::OpenLink),
    1043, uint(QWebPage::OpenLinkInNewWindow),
    1063, uint(QWebPage::OpenFrameInNewWindow),
    1084, uint(QWebPage::DownloadLinkToDisk),
    1103, uint(QWebPage::CopyLinkToClipboard),
    1123, uint(QWebPage::OpenImageInNewWindow),
    1144, uint(QWebPage::DownloadImageToDisk),
    1164, uint(QWebPage::CopyImageToClipboard),
    1185, uint(QWebPage::Back),
    1190, uint(QWebPage::Forward),
    1198, uint(QWebPage::Stop),
    1203, uint(QWebPage::Reload),
    1210, uint(QWebPage::Cut),
    1214, uint(QWebPage::Copy),
    1219, uint(QWebPage::Paste),
    1225, uint(QWebPage::Undo),
    1230, uint(QWebPage::Redo),
    1235, uint(QWebPage::MoveToNextChar),
    1250, uint(QWebPage::MoveToPreviousChar),
    1269, uint(QWebPage::MoveToNextWord),
    1284, uint(QWebPage::MoveToPreviousWord),
    1303, uint(QWebPage::MoveToNextLine),
    1318, uint(QWebPage::MoveToPreviousLine),
    1337, uint(QWebPage::MoveToStartOfLine),
    1355, uint(QWebPage::MoveToEndOfLine),
    1371, uint(QWebPage::MoveToStartOfBlock),
    1390, uint(QWebPage::MoveToEndOfBlock),
    1407, uint(QWebPage::MoveToStartOfDocument),
    1429, uint(QWebPage::MoveToEndOfDocument),
    1449, uint(QWebPage::SelectNextChar),
    1464, uint(QWebPage::SelectPreviousChar),
    1483, uint(QWebPage::SelectNextWord),
    1498, uint(QWebPage::SelectPreviousWord),
    1517, uint(QWebPage::SelectNextLine),
    1532, uint(QWebPage::SelectPreviousLine),
    1551, uint(QWebPage::SelectStartOfLine),
    1569, uint(QWebPage::SelectEndOfLine),
    1585, uint(QWebPage::SelectStartOfBlock),
    1604, uint(QWebPage::SelectEndOfBlock),
    1621, uint(QWebPage::SelectStartOfDocument),
    1643, uint(QWebPage::SelectEndOfDocument),
    1663, uint(QWebPage::DeleteStartOfWord),
    1681, uint(QWebPage::DeleteEndOfWord),
    1697, uint(QWebPage::SetTextDirectionDefault),
    1721, uint(QWebPage::SetTextDirectionLeftToRight),
    1749, uint(QWebPage::SetTextDirectionRightToLeft),
    1777, uint(QWebPage::ToggleBold),
    1788, uint(QWebPage::ToggleItalic),
    1801, uint(QWebPage::ToggleUnderline),
    1817, uint(QWebPage::InspectElement),
    1832, uint(QWebPage::WebActionCount),
    1847, uint(QWebPage::DontDelegateLinks),
    1865, uint(QWebPage::DelegateExternalLinks),
    1887, uint(QWebPage::DelegateAllLinks),

       0        // eod
};

static const char qt_meta_stringdata_QWebPage[] = {
    "QWebPage\0\0loadStarted()\0progress\0"
    "loadProgress(int)\0ok\0loadFinished(bool)\0"
    "link,title,textContent\0"
    "linkHovered(QString,QString,QString)\0"
    "text\0statusBarMessage(QString)\0"
    "selectionChanged()\0frame\0"
    "frameCreated(QWebFrame*)\0geom\0"
    "geometryChangeRequested(QRect)\0dirtyRect\0"
    "repaintRequested(QRect)\0dx,dy,scrollViewRect\0"
    "scrollRequested(int,int,QRect)\0"
    "windowCloseRequested()\0"
    "printRequested(QWebFrame*)\0url\0"
    "linkClicked(QUrl)\0visible\0"
    "toolBarVisibilityChangeRequested(bool)\0"
    "statusBarVisibilityChangeRequested(bool)\0"
    "menuBarVisibilityChangeRequested(bool)\0"
    "reply\0unsupportedContent(QNetworkReply*)\0"
    "request\0downloadRequested(QNetworkRequest)\0"
    "microFocusChanged()\0_q_onLoadProgressChanged(int)\0"
    "checked\0_q_webActionTriggered(bool)\0"
    "bool\0modified\0QString\0selectedText\0"
    "QSize\0viewportSize\0forwardUnsupportedContent\0"
    "LinkDelegationPolicy\0linkDelegationPolicy\0"
    "QPalette\0palette\0NavigationType\0"
    "WebAction\0NavigationTypeLinkClicked\0"
    "NavigationTypeFormSubmitted\0"
    "NavigationTypeBackOrForward\0"
    "NavigationTypeReload\0NavigationTypeFormResubmitted\0"
    "NavigationTypeOther\0NoWebAction\0"
    "OpenLink\0OpenLinkInNewWindow\0"
    "OpenFrameInNewWindow\0DownloadLinkToDisk\0"
    "CopyLinkToClipboard\0OpenImageInNewWindow\0"
    "DownloadImageToDisk\0CopyImageToClipboard\0"
    "Back\0Forward\0Stop\0Reload\0Cut\0Copy\0"
    "Paste\0Undo\0Redo\0MoveToNextChar\0"
    "MoveToPreviousChar\0MoveToNextWord\0"
    "MoveToPreviousWord\0MoveToNextLine\0"
    "MoveToPreviousLine\0MoveToStartOfLine\0"
    "MoveToEndOfLine\0MoveToStartOfBlock\0"
    "MoveToEndOfBlock\0MoveToStartOfDocument\0"
    "MoveToEndOfDocument\0SelectNextChar\0"
    "SelectPreviousChar\0SelectNextWord\0"
    "SelectPreviousWord\0SelectNextLine\0"
    "SelectPreviousLine\0SelectStartOfLine\0"
    "SelectEndOfLine\0SelectStartOfBlock\0"
    "SelectEndOfBlock\0SelectStartOfDocument\0"
    "SelectEndOfDocument\0DeleteStartOfWord\0"
    "DeleteEndOfWord\0SetTextDirectionDefault\0"
    "SetTextDirectionLeftToRight\0"
    "SetTextDirectionRightToLeft\0ToggleBold\0"
    "ToggleItalic\0ToggleUnderline\0"
    "InspectElement\0WebActionCount\0"
    "DontDelegateLinks\0DelegateExternalLinks\0"
    "DelegateAllLinks\0"
};

const QMetaObject QWebPage::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QWebPage,
      qt_meta_data_QWebPage, 0 }
};

const QMetaObject *QWebPage::metaObject() const
{
    return &staticMetaObject;
}

void *QWebPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWebPage))
	return static_cast<void*>(const_cast< QWebPage*>(this));
    return QObject::qt_metacast(_clname);
}

int QWebPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadStarted(); break;
        case 1: loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: loadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: linkHovered((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 4: statusBarMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: selectionChanged(); break;
        case 6: frameCreated((*reinterpret_cast< QWebFrame*(*)>(_a[1]))); break;
        case 7: geometryChangeRequested((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 8: repaintRequested((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 9: scrollRequested((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QRect(*)>(_a[3]))); break;
        case 10: windowCloseRequested(); break;
        case 11: printRequested((*reinterpret_cast< QWebFrame*(*)>(_a[1]))); break;
        case 12: linkClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 13: toolBarVisibilityChangeRequested((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: statusBarVisibilityChangeRequested((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: menuBarVisibilityChangeRequested((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: unsupportedContent((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 17: downloadRequested((*reinterpret_cast< const QNetworkRequest(*)>(_a[1]))); break;
        case 18: microFocusChanged(); break;
        case 19: d->_q_onLoadProgressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: d->_q_webActionTriggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 21;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isModified(); break;
        case 1: *reinterpret_cast< QString*>(_v) = selectedText(); break;
        case 2: *reinterpret_cast< QSize*>(_v) = viewportSize(); break;
        case 3: *reinterpret_cast< bool*>(_v) = forwardUnsupportedContent(); break;
        case 4: *reinterpret_cast< LinkDelegationPolicy*>(_v) = linkDelegationPolicy(); break;
        case 5: *reinterpret_cast< QPalette*>(_v) = palette(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setViewportSize(*reinterpret_cast< QSize*>(_v)); break;
        case 3: setForwardUnsupportedContent(*reinterpret_cast< bool*>(_v)); break;
        case 4: setLinkDelegationPolicy(*reinterpret_cast< LinkDelegationPolicy*>(_v)); break;
        case 5: setPalette(*reinterpret_cast< QPalette*>(_v)); break;
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
void QWebPage::loadStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QWebPage::loadProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QWebPage::loadFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QWebPage::linkHovered(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QWebPage::statusBarMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QWebPage::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QWebPage::frameCreated(QWebFrame * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QWebPage::geometryChangeRequested(const QRect & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QWebPage::repaintRequested(const QRect & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QWebPage::scrollRequested(int _t1, int _t2, const QRect & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QWebPage::windowCloseRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void QWebPage::printRequested(QWebFrame * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void QWebPage::linkClicked(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void QWebPage::toolBarVisibilityChangeRequested(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void QWebPage::statusBarVisibilityChangeRequested(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void QWebPage::menuBarVisibilityChangeRequested(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void QWebPage::unsupportedContent(QNetworkReply * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void QWebPage::downloadRequested(const QNetworkRequest & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void QWebPage::microFocusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}
QT_END_MOC_NAMESPACE
