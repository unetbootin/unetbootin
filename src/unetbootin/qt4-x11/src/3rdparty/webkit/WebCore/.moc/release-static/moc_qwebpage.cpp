/****************************************************************************
** Meta object code from reading C++ file 'qwebpage.h'
**
** Created: Thu Mar 5 19:56:07 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../WebKit/qt/Api/qwebpage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwebpage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QWebPage[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   12, // methods
       7,  137, // properties
       3,  158, // enums/sets
       0,    0, // constructors

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
     639,    9,    9,    9, 0x05,
     676,  657,    9,    9, 0x05,
     729,  718,    9,    9, 0x05,
     782,  183,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     821,    9,    9,    9, 0x08,
     859,  851,    9,    9, 0x08,

 // properties: name, type, flags
     892,  887, 0x01095001,
     909,  901, 0x0a095001,
     928,  922, 0x15095103,
     941,  887, 0x01095103,
     988,  967, 0x0009510b,
    1018, 1009, 0x44095103,
    1026,  887, 0x01095103,

 // enums: name, flags, count, data
    1042, 0x0,    6,  170,
    1057, 0x0,   55,  182,
     967, 0x0,    3,  292,

 // enum data: key, value
    1067, uint(QWebPage::NavigationTypeLinkClicked),
    1093, uint(QWebPage::NavigationTypeFormSubmitted),
    1121, uint(QWebPage::NavigationTypeBackOrForward),
    1149, uint(QWebPage::NavigationTypeReload),
    1170, uint(QWebPage::NavigationTypeFormResubmitted),
    1200, uint(QWebPage::NavigationTypeOther),
    1220, uint(QWebPage::NoWebAction),
    1232, uint(QWebPage::OpenLink),
    1241, uint(QWebPage::OpenLinkInNewWindow),
    1261, uint(QWebPage::OpenFrameInNewWindow),
    1282, uint(QWebPage::DownloadLinkToDisk),
    1301, uint(QWebPage::CopyLinkToClipboard),
    1321, uint(QWebPage::OpenImageInNewWindow),
    1342, uint(QWebPage::DownloadImageToDisk),
    1362, uint(QWebPage::CopyImageToClipboard),
    1383, uint(QWebPage::Back),
    1388, uint(QWebPage::Forward),
    1396, uint(QWebPage::Stop),
    1401, uint(QWebPage::Reload),
    1408, uint(QWebPage::Cut),
    1412, uint(QWebPage::Copy),
    1417, uint(QWebPage::Paste),
    1423, uint(QWebPage::Undo),
    1428, uint(QWebPage::Redo),
    1433, uint(QWebPage::MoveToNextChar),
    1448, uint(QWebPage::MoveToPreviousChar),
    1467, uint(QWebPage::MoveToNextWord),
    1482, uint(QWebPage::MoveToPreviousWord),
    1501, uint(QWebPage::MoveToNextLine),
    1516, uint(QWebPage::MoveToPreviousLine),
    1535, uint(QWebPage::MoveToStartOfLine),
    1553, uint(QWebPage::MoveToEndOfLine),
    1569, uint(QWebPage::MoveToStartOfBlock),
    1588, uint(QWebPage::MoveToEndOfBlock),
    1605, uint(QWebPage::MoveToStartOfDocument),
    1627, uint(QWebPage::MoveToEndOfDocument),
    1647, uint(QWebPage::SelectNextChar),
    1662, uint(QWebPage::SelectPreviousChar),
    1681, uint(QWebPage::SelectNextWord),
    1696, uint(QWebPage::SelectPreviousWord),
    1715, uint(QWebPage::SelectNextLine),
    1730, uint(QWebPage::SelectPreviousLine),
    1749, uint(QWebPage::SelectStartOfLine),
    1767, uint(QWebPage::SelectEndOfLine),
    1783, uint(QWebPage::SelectStartOfBlock),
    1802, uint(QWebPage::SelectEndOfBlock),
    1819, uint(QWebPage::SelectStartOfDocument),
    1841, uint(QWebPage::SelectEndOfDocument),
    1861, uint(QWebPage::DeleteStartOfWord),
    1879, uint(QWebPage::DeleteEndOfWord),
    1895, uint(QWebPage::SetTextDirectionDefault),
    1919, uint(QWebPage::SetTextDirectionLeftToRight),
    1947, uint(QWebPage::SetTextDirectionRightToLeft),
    1975, uint(QWebPage::ToggleBold),
    1986, uint(QWebPage::ToggleItalic),
    1999, uint(QWebPage::ToggleUnderline),
    2015, uint(QWebPage::InspectElement),
    2030, uint(QWebPage::InsertParagraphSeparator),
    2055, uint(QWebPage::InsertLineSeparator),
    2075, uint(QWebPage::SelectAll),
    2085, uint(QWebPage::WebActionCount),
    2100, uint(QWebPage::DontDelegateLinks),
    2118, uint(QWebPage::DelegateExternalLinks),
    2140, uint(QWebPage::DelegateAllLinks),

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
    "microFocusChanged()\0contentsChanged()\0"
    "frame,databaseName\0"
    "databaseQuotaExceeded(QWebFrame*,QString)\0"
    "frame,item\0"
    "saveFrameStateRequested(QWebFrame*,QWebHistoryItem*)\0"
    "restoreFrameStateRequested(QWebFrame*)\0"
    "_q_onLoadProgressChanged(int)\0checked\0"
    "_q_webActionTriggered(bool)\0bool\0"
    "modified\0QString\0selectedText\0QSize\0"
    "viewportSize\0forwardUnsupportedContent\0"
    "LinkDelegationPolicy\0linkDelegationPolicy\0"
    "QPalette\0palette\0contentEditable\0"
    "NavigationType\0WebAction\0"
    "NavigationTypeLinkClicked\0"
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
    "InspectElement\0InsertParagraphSeparator\0"
    "InsertLineSeparator\0SelectAll\0"
    "WebActionCount\0DontDelegateLinks\0"
    "DelegateExternalLinks\0DelegateAllLinks\0"
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
        case 19: contentsChanged(); break;
        case 20: databaseQuotaExceeded((*reinterpret_cast< QWebFrame*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 21: saveFrameStateRequested((*reinterpret_cast< QWebFrame*(*)>(_a[1])),(*reinterpret_cast< QWebHistoryItem*(*)>(_a[2]))); break;
        case 22: restoreFrameStateRequested((*reinterpret_cast< QWebFrame*(*)>(_a[1]))); break;
        case 23: d->_q_onLoadProgressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: d->_q_webActionTriggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 25;
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
        case 6: *reinterpret_cast< bool*>(_v) = isContentEditable(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setViewportSize(*reinterpret_cast< QSize*>(_v)); break;
        case 3: setForwardUnsupportedContent(*reinterpret_cast< bool*>(_v)); break;
        case 4: setLinkDelegationPolicy(*reinterpret_cast< LinkDelegationPolicy*>(_v)); break;
        case 5: setPalette(*reinterpret_cast< QPalette*>(_v)); break;
        case 6: setContentEditable(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
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

// SIGNAL 19
void QWebPage::contentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}

// SIGNAL 20
void QWebPage::databaseQuotaExceeded(QWebFrame * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void QWebPage::saveFrameStateRequested(QWebFrame * _t1, QWebHistoryItem * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void QWebPage::restoreFrameStateRequested(QWebFrame * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}
QT_END_MOC_NAMESPACE
