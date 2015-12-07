/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "qaccessible.h"
#ifndef QT_NO_ACCESSIBILITY

#ifdef Q_OS_WINCE
#include "qguifunctions_wince.h"
#endif

#include "qapplication.h"
#include "qlibrary.h"
#include "qmessagebox.h" // ### dependency
#include "qt_windows.h"
#include "qwidget.h"
#include "qsettings.h"

#if WINVER >= 0x0600
#include <winuser.h>
#else
#  if defined(Q_OS_WINCE)
#    include <bldver.h>
#  endif
#include <winable.h>
#endif

#include <oleacc.h>
#if !defined(Q_CC_BOR) && !defined (Q_CC_GNU)
#include <comdef.h>
#endif

QT_BEGIN_NAMESPACE

//#define DEBUG_SHOW_ATCLIENT_COMMANDS
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
QT_BEGIN_INCLUDE_NAMESPACE
#include <qdebug.h>
QT_END_INCLUDE_NAMESPACE

void showDebug(const char* funcName, const QAccessibleInterface *iface)
{
    qDebug() << "Role:" << iface->role(0) << "Name:" << iface->text(QAccessible::Name, 0) << QLatin1String(funcName);
}
#endif

void QAccessible::initialize()
{

}
void QAccessible::cleanup()
{

}

void QAccessible::updateAccessibility(QObject *o, int who, Event reason)
{
    Q_ASSERT(o);

    if (updateHandler) {
        updateHandler(o, who, reason);
        return;
    }

    QByteArray soundName;
    switch (reason) {
    case PopupMenuStart:
        soundName = "MenuPopup";
        break;

    case MenuCommand:
        soundName = "MenuCommand";
        break;

    case Alert:
        {
#ifndef QT_NO_MESSAGEBOX
            QMessageBox *mb = qobject_cast<QMessageBox*>(o);
            if (mb) {
                switch (mb->icon()) {
                case QMessageBox::Warning:
                    soundName = "SystemExclamation";
                    break;
                case QMessageBox::Critical:
                    soundName = "SystemHand";
                    break;
                case QMessageBox::Information:
                    soundName = "SystemAsterisk";
                    break;
                default:
                    break;
                }
            } else
#endif // QT_NO_MESSAGEBOX
            {
                soundName = "SystemAsterisk";
            }

        }
        break;
    default:
        break;
    }

    if (soundName.size()) {
#ifndef QT_NO_SETTINGS
        QSettings settings(QLatin1String("HKEY_CURRENT_USER\\AppEvents\\Schemes\\Apps\\.Default\\") +
                                         QString::fromLatin1(soundName.constData()), QSettings::NativeFormat);
        QString file = settings.value(QLatin1String(".Current/.")).toString();
#else
		QString file;
#endif
		if (!file.isEmpty()) {
	        QT_WA({
				PlaySoundW(reinterpret_cast<const wchar_t *> (QString::fromLatin1(soundName).utf16()), 0, SND_ALIAS | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT );
			} , {
				PlaySoundA(soundName.constData(), 0, SND_ALIAS | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT );
			});
		}
	}

    if (!isActive())
        return;

    typedef void (WINAPI *PtrNotifyWinEvent)(DWORD, HWND, LONG, LONG);

#if defined(Q_OS_WINCE) // ### TODO: check for NotifyWinEvent in CE 6.0
    // There is no user32.lib nor NotifyWinEvent for CE
    return;
#else
    static PtrNotifyWinEvent ptrNotifyWinEvent = 0;
    static bool resolvedNWE = false;
    if (!resolvedNWE) {
        resolvedNWE = true;
        ptrNotifyWinEvent = (PtrNotifyWinEvent)QLibrary::resolve(QLatin1String("user32"), "NotifyWinEvent");
    }
    if (!ptrNotifyWinEvent)
        return;

    // An event has to be associated with a window,
    // so find the first parent that is a widget.
    QWidget *w = 0;
    if (o->isWidgetType()) {
        w = (QWidget*)o;
    } else {
        QObject *p = o;
        while ((p = p->parent()) != 0) {
            if (p->isWidgetType()) {
                w = (QWidget*)p;
                break;
            }
        }
    }

    if (!w) {
        if (reason != QAccessible::ContextHelpStart &&
             reason != QAccessible::ContextHelpEnd)
            w = qApp->focusWidget();
        if (!w) {
            w = qApp->activeWindow();

            if (!w)
                return;

// ### Fixme
//             if (!w) {
//                 w = qApp->mainWidget();
//                 if (!w)
//                     return;
//             }
        }
    }

    if (reason != MenuCommand) { // MenuCommand is faked
        ptrNotifyWinEvent(reason, w->winId(), OBJID_CLIENT, who);
    }
#endif // Q_OS_WINCE
}

void QAccessible::setRootObject(QObject *o)
{
    if (rootObjectHandler) {
        rootObjectHandler(o);
    }
}

class QWindowsEnumerate : public IEnumVARIANT
{
public:
    QWindowsEnumerate(const QVector<int> &a)
        : ref(0), current(0),array(a)
    {
    }

    virtual ~QWindowsEnumerate() {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID *);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE Clone(IEnumVARIANT **ppEnum);
    HRESULT STDMETHODCALLTYPE Next(unsigned long  celt, VARIANT FAR*  rgVar, unsigned long FAR*  pCeltFetched);
    HRESULT STDMETHODCALLTYPE Reset();
    HRESULT STDMETHODCALLTYPE Skip(unsigned long celt);

private:
    ULONG ref;
    ULONG current;
    QVector<int> array;
};

HRESULT STDMETHODCALLTYPE QWindowsEnumerate::QueryInterface(REFIID id, LPVOID *iface)
{
    *iface = 0;
    if (id == IID_IUnknown)
        *iface = (IUnknown*)this;
    else if (id == IID_IEnumVARIANT)
        *iface = (IEnumVARIANT*)this;

    if (*iface) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE QWindowsEnumerate::AddRef()
{
    return ++ref;
}

ULONG STDMETHODCALLTYPE QWindowsEnumerate::Release()
{
    if (!--ref) {
        delete this;
        return 0;
    }
    return ref;
}

HRESULT STDMETHODCALLTYPE QWindowsEnumerate::Clone(IEnumVARIANT **ppEnum)
{
    QWindowsEnumerate *penum = 0;
    *ppEnum = 0;

    penum = new QWindowsEnumerate(array);
    if (!penum)
        return E_OUTOFMEMORY;
    penum->current = current;
    penum->array = array;
    penum->AddRef();
    *ppEnum = penum;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsEnumerate::Next(unsigned long  celt, VARIANT FAR*  rgVar, unsigned long FAR*  pCeltFetched)
{
    if (pCeltFetched)
        *pCeltFetched = 0;

    ULONG l;
    for (l = 0; l < celt; l++) {
        VariantInit(&rgVar[l]);
        if ((current+1) > (ULONG)array.size()) {
            *pCeltFetched = l;
            return S_FALSE;
        }

        rgVar[l].vt = VT_I4;
        rgVar[l].lVal = array[(int)current];
        ++current;
    }
    *pCeltFetched = l;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsEnumerate::Reset()
{
    current = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsEnumerate::Skip(unsigned long celt)
{
    current += celt;
    if (current > (ULONG)array.size()) {
        current = array.size();
        return S_FALSE;
    }
    return S_OK;
}

/*
*/
class QWindowsAccessible : public IAccessible, IOleWindow, QAccessible
{
public:
    QWindowsAccessible(QAccessibleInterface *a)
        : ref(0), accessible(a)
    {
    }

    virtual ~QWindowsAccessible()
    {
        delete accessible;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, LPVOID *);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int *);
    HRESULT STDMETHODCALLTYPE GetTypeInfo(unsigned int, unsigned long, ITypeInfo **);
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(const _GUID &, wchar_t **, unsigned int, unsigned long, long *);
    HRESULT STDMETHODCALLTYPE Invoke(long, const _GUID &, unsigned long, unsigned short, tagDISPPARAMS *, tagVARIANT *, tagEXCEPINFO *, unsigned int *);

    HRESULT STDMETHODCALLTYPE accHitTest(long xLeft, long yTop, VARIANT *pvarID);
    HRESULT STDMETHODCALLTYPE accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varID);
    HRESULT STDMETHODCALLTYPE accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEnd);
    HRESULT STDMETHODCALLTYPE get_accChild(VARIANT varChildID, IDispatch** ppdispChild);
    HRESULT STDMETHODCALLTYPE get_accChildCount(long* pcountChildren);
    HRESULT STDMETHODCALLTYPE get_accParent(IDispatch** ppdispParent);

    HRESULT STDMETHODCALLTYPE accDoDefaultAction(VARIANT varID);
    HRESULT STDMETHODCALLTYPE get_accDefaultAction(VARIANT varID, BSTR* pszDefaultAction);
    HRESULT STDMETHODCALLTYPE get_accDescription(VARIANT varID, BSTR* pszDescription);
    HRESULT STDMETHODCALLTYPE get_accHelp(VARIANT varID, BSTR *pszHelp);
    HRESULT STDMETHODCALLTYPE get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic);
    HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut(VARIANT varID, BSTR *pszKeyboardShortcut);
    HRESULT STDMETHODCALLTYPE get_accName(VARIANT varID, BSTR* pszName);
    HRESULT STDMETHODCALLTYPE put_accName(VARIANT varChild, BSTR szName);
    HRESULT STDMETHODCALLTYPE get_accRole(VARIANT varID, VARIANT *pvarRole);
    HRESULT STDMETHODCALLTYPE get_accState(VARIANT varID, VARIANT *pvarState);
    HRESULT STDMETHODCALLTYPE get_accValue(VARIANT varID, BSTR* pszValue);
    HRESULT STDMETHODCALLTYPE put_accValue(VARIANT varChild, BSTR szValue);

    HRESULT STDMETHODCALLTYPE accSelect(long flagsSelect, VARIANT varID);
    HRESULT STDMETHODCALLTYPE get_accFocus(VARIANT *pvarID);
    HRESULT STDMETHODCALLTYPE get_accSelection(VARIANT *pvarChildren);

    HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

private:
    ULONG ref;
    QAccessibleInterface *accessible;
};

static inline BSTR QStringToBSTR(const QString &str)
{
    BSTR bstrVal;

    int wlen = str.length()+1;
    bstrVal = SysAllocStringByteLen(0, wlen*2);
    memcpy(bstrVal, str.unicode(), sizeof(QChar)*(wlen));
    bstrVal[wlen] = 0;

    return bstrVal;
}

/*
*/
IAccessible *qt_createWindowsAccessible(QAccessibleInterface *access)
{
    QWindowsAccessible *acc = new QWindowsAccessible(access);
    IAccessible *iface;
    acc->QueryInterface(IID_IAccessible, (void**)&iface);

    return iface;
}

/*
  IUnknown
*/
HRESULT STDMETHODCALLTYPE QWindowsAccessible::QueryInterface(REFIID id, LPVOID *iface)
{
    *iface = 0;
    if (id == IID_IUnknown)
        *iface = (IUnknown*)(IDispatch*)this;
    else if (id == IID_IDispatch)
        *iface = (IDispatch*)this;
    else if (id == IID_IAccessible)
        *iface = (IAccessible*)this;
    else if (id == IID_IOleWindow)
        *iface = (IOleWindow*)this;
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE QWindowsAccessible::AddRef()
{
    return ++ref;
}

ULONG STDMETHODCALLTYPE QWindowsAccessible::Release()
{
    if (!--ref) {
        delete this;
        return 0;
    }
    return ref;
}

/*
  IDispatch
*/

HRESULT STDMETHODCALLTYPE QWindowsAccessible::GetTypeInfoCount(unsigned int * pctinfo)
{
    // We don't use a type library
    *pctinfo = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::GetTypeInfo(unsigned int, unsigned long, ITypeInfo **pptinfo)
{
    // We don't use a type library
    *pptinfo = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::GetIDsOfNames(const _GUID &, wchar_t **rgszNames, unsigned int, unsigned long, long *rgdispid)
{
#if !defined(Q_CC_BOR) && !defined(Q_CC_GNU)
    // PROPERTIES:  Hierarchical
    if (_bstr_t(rgszNames[0]) == _bstr_t(L"accParent"))
        rgdispid[0] = DISPID_ACC_PARENT;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accChildCount"))
        rgdispid[0] = DISPID_ACC_CHILDCOUNT;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accChild"))
        rgdispid[0] = DISPID_ACC_CHILD;

    // PROPERTIES:  Descriptional
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accName("))
        rgdispid[0] = DISPID_ACC_NAME;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accValue"))
        rgdispid[0] = DISPID_ACC_VALUE;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accDescription"))
        rgdispid[0] = DISPID_ACC_DESCRIPTION;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accRole"))
        rgdispid[0] = DISPID_ACC_ROLE;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accState"))
        rgdispid[0] = DISPID_ACC_STATE;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accHelp"))
        rgdispid[0] = DISPID_ACC_HELP;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accHelpTopic"))
        rgdispid[0] = DISPID_ACC_HELPTOPIC;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accKeyboardShortcut"))
        rgdispid[0] = DISPID_ACC_KEYBOARDSHORTCUT;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accFocus"))
        rgdispid[0] = DISPID_ACC_FOCUS;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accSelection"))
        rgdispid[0] = DISPID_ACC_SELECTION;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accDefaultAction"))
        rgdispid[0] = DISPID_ACC_DEFAULTACTION;

    // METHODS
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accSelect"))
        rgdispid[0] = DISPID_ACC_SELECT;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accLocation"))
        rgdispid[0] = DISPID_ACC_LOCATION;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accNavigate"))
        rgdispid[0] = DISPID_ACC_NAVIGATE;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accHitTest"))
        rgdispid[0] = DISPID_ACC_HITTEST;
    else if(_bstr_t(rgszNames[0]) == _bstr_t(L"accDoDefaultAction"))
        rgdispid[0] = DISPID_ACC_DODEFAULTACTION;
    else
        return DISP_E_UNKNOWNINTERFACE;

    return S_OK;
#else
    Q_UNUSED(rgszNames);
    Q_UNUSED(rgdispid);

    return DISP_E_MEMBERNOTFOUND;
#endif
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::Invoke(long dispIdMember, const _GUID &, unsigned long, unsigned short wFlags, tagDISPPARAMS *pDispParams, tagVARIANT *pVarResult, tagEXCEPINFO *, unsigned int *)
{
    HRESULT hr = DISP_E_MEMBERNOTFOUND;

    switch(dispIdMember)
    {
        case DISPID_ACC_PARENT:
            if (wFlags == DISPATCH_PROPERTYGET) {
                if (!pVarResult)
                    return E_INVALIDARG;
                hr = get_accParent(&pVarResult->pdispVal);
            } else {
                hr = DISP_E_MEMBERNOTFOUND;
            }
            break;

        case DISPID_ACC_CHILDCOUNT:
            if (wFlags == DISPATCH_PROPERTYGET) {
                if (!pVarResult)
                    return E_INVALIDARG;
                hr = get_accChildCount(&pVarResult->lVal);
            } else {
                hr = DISP_E_MEMBERNOTFOUND;
            }
            break;

        case DISPID_ACC_CHILD:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accChild(pDispParams->rgvarg[0], &pVarResult->pdispVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_NAME:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accName(pDispParams->rgvarg[0], &pVarResult->bstrVal);
            else if (wFlags == DISPATCH_PROPERTYPUT)
                hr = put_accName(pDispParams->rgvarg[0], pVarResult->bstrVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_VALUE:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accValue(pDispParams->rgvarg[0], &pVarResult->bstrVal);
            else if (wFlags == DISPATCH_PROPERTYPUT)
                hr = put_accValue(pDispParams->rgvarg[0], pVarResult->bstrVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_DESCRIPTION:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accDescription(pDispParams->rgvarg[0], &pVarResult->bstrVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_ROLE:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accRole(pDispParams->rgvarg[0], pVarResult);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_STATE:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accState(pDispParams->rgvarg[0], pVarResult);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_HELP:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accHelp(pDispParams->rgvarg[0], &pVarResult->bstrVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_HELPTOPIC:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accHelpTopic(&pDispParams->rgvarg[2].bstrVal, pDispParams->rgvarg[1], &pDispParams->rgvarg[0].lVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_KEYBOARDSHORTCUT:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accKeyboardShortcut(pDispParams->rgvarg[0], &pVarResult->bstrVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_FOCUS:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accFocus(pVarResult);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_SELECTION:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accSelection(pVarResult);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_DEFAULTACTION:
            if (wFlags == DISPATCH_PROPERTYGET)
                hr = get_accDefaultAction(pDispParams->rgvarg[0], &pVarResult->bstrVal);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_SELECT:
            if (wFlags == DISPATCH_METHOD)
                hr = accSelect(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0]);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_LOCATION:
            if (wFlags == DISPATCH_METHOD)
                hr = accLocation(&pDispParams->rgvarg[4].lVal, &pDispParams->rgvarg[3].lVal, &pDispParams->rgvarg[2].lVal, &pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0]);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_NAVIGATE:
            if (wFlags == DISPATCH_METHOD)
                hr = accNavigate(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0], pVarResult);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_HITTEST:
            if (wFlags == DISPATCH_METHOD)
                hr = accHitTest(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].lVal, pVarResult);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        case DISPID_ACC_DODEFAULTACTION:
            if (wFlags == DISPATCH_METHOD)
                hr = accDoDefaultAction(pDispParams->rgvarg[0]);
            else
                hr = DISP_E_MEMBERNOTFOUND;
            break;

        default:
            hr = DISP_E_MEMBERNOTFOUND;
            break;
    }

    if (!SUCCEEDED(hr)) {
        return hr;
    }
    return hr;
}

/*
  IAccessible
*/
HRESULT STDMETHODCALLTYPE QWindowsAccessible::accHitTest(long xLeft, long yTop, VARIANT *pvarID)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    int control = accessible->childAt(xLeft, yTop);
    if (control == -1) {
        (*pvarID).vt = VT_EMPTY;
        return S_FALSE;
    }
    QAccessibleInterface *acc = 0;
    if (control)
        accessible->navigate(Child, control, &acc);
    if (!acc) {
        (*pvarID).vt = VT_I4;
        (*pvarID).lVal = control;
        return S_OK;
    }

    QWindowsAccessible* wacc = new QWindowsAccessible(acc);
    IDispatch *iface = 0;
    wacc->QueryInterface(IID_IDispatch, (void**)&iface);
    if (iface) {
        (*pvarID).vt = VT_DISPATCH;
        (*pvarID).pdispVal = iface;
        return S_OK;
    } else {
        delete wacc;
    }

    (*pvarID).vt = VT_EMPTY;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varID)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QRect rect = accessible->rect(varID.lVal);
    if (rect.isValid()) {
        *pxLeft = rect.x();
        *pyTop = rect.y();
        *pcxWidth = rect.width();
        *pcyHeight = rect.height();
    } else {
        *pxLeft = 0;
        *pyTop = 0;
        *pcxWidth = 0;
        *pcyHeight = 0;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEnd)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QAccessibleInterface *acc = 0;
    int control = -1;
    switch(navDir) {
    case NAVDIR_FIRSTCHILD:
        control = accessible->navigate(Child, 1, &acc);
        break;
    case NAVDIR_LASTCHILD:
        control = accessible->navigate(Child, accessible->childCount(), &acc);
        break;
    case NAVDIR_NEXT:
    case NAVDIR_PREVIOUS:
        if (!varStart.lVal){
            QAccessibleInterface *parent = 0;
            accessible->navigate(Ancestor, 1, &parent);
            if (parent) {
                int index = parent->indexOfChild(accessible);
                index += (navDir == NAVDIR_NEXT) ? 1 : -1;
                if (index > 0 && index <= parent->childCount())
                    control = parent->navigate(Child, index, &acc);
                delete parent;
            }
        } else {
            int index = varStart.lVal;
            index += (navDir == NAVDIR_NEXT) ? 1 : -1;
            if (index > 0 && index <= accessible->childCount())
                control = accessible->navigate(Child, index, &acc);
        }
        break;
    case NAVDIR_UP:
        control = accessible->navigate(Up, varStart.lVal, &acc);
        break;
    case NAVDIR_DOWN:
        control = accessible->navigate(Down, varStart.lVal, &acc);
        break;
    case NAVDIR_LEFT:
        control = accessible->navigate(Left, varStart.lVal, &acc);
        break;
    case NAVDIR_RIGHT:
        control = accessible->navigate(Right, varStart.lVal, &acc);
        break;
    default:
        break;
    }
    if (control == -1) {
        (*pvarEnd).vt = VT_EMPTY;
        return S_FALSE;
    }
    if (!acc) {
        (*pvarEnd).vt = VT_I4;
        (*pvarEnd).lVal = control;
        return S_OK;
    }

    QWindowsAccessible* wacc = new QWindowsAccessible(acc);

    IDispatch *iface = 0;
    wacc->QueryInterface(IID_IDispatch, (void**)&iface);
    if (iface) {
        (*pvarEnd).vt = VT_DISPATCH;
        (*pvarEnd).pdispVal = iface;
        return S_OK;
    } else {
        delete wacc;
    }

    (*pvarEnd).vt = VT_EMPTY;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accChild(VARIANT varChildID, IDispatch** ppdispChild)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    if (varChildID.vt == VT_EMPTY)
        return E_INVALIDARG;

    QAccessibleInterface *acc = 0;
    RelationFlag rel = varChildID.lVal ? Child : Self;
    accessible->navigate(rel, varChildID.lVal, &acc);

    if (acc) {
        QWindowsAccessible* wacc = new QWindowsAccessible(acc);
        wacc->QueryInterface(IID_IDispatch, (void**)ppdispChild);
        return S_OK;
    }

    *ppdispChild = 0;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accChildCount(long* pcountChildren)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    *pcountChildren = accessible->childCount();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accParent(IDispatch** ppdispParent)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QAccessibleInterface *acc = 0;
    accessible->navigate(Ancestor, 1, &acc);
    if (acc) {
        QWindowsAccessible* wacc = new QWindowsAccessible(acc);
        wacc->QueryInterface(IID_IDispatch, (void**)ppdispParent);

        if (*ppdispParent)
            return S_OK;
    }

    *ppdispParent = 0;
    return S_FALSE;
}

/*
  Properties and methods
*/
HRESULT STDMETHODCALLTYPE QWindowsAccessible::accDoDefaultAction(VARIANT varID)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    return accessible->doAction(DefaultAction, varID.lVal, QVariantList()) ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accDefaultAction(VARIANT varID, BSTR* pszDefaultAction)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QString def = accessible->actionText(DefaultAction, Name, varID.lVal);
    if (def.isEmpty()) {
        *pszDefaultAction = 0;
        return S_FALSE;
    }

    *pszDefaultAction = QStringToBSTR(def);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accDescription(VARIANT varID, BSTR* pszDescription)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QString descr = accessible->text(Description, varID.lVal);
    if (descr.size()) {
        *pszDescription = QStringToBSTR(descr);
        return S_OK;
    }

    *pszDescription = 0;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accHelp(VARIANT varID, BSTR *pszHelp)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QString help = accessible->text(Help, varID.lVal);
    if (help.size()) {
        *pszHelp = QStringToBSTR(help);
        return S_OK;
    }

    *pszHelp = 0;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accHelpTopic(BSTR *, VARIANT, long *)
{
    return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accKeyboardShortcut(VARIANT varID, BSTR *pszKeyboardShortcut)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QString sc = accessible->text(Accelerator, varID.lVal);
    if (sc.size()) {
        *pszKeyboardShortcut = QStringToBSTR(sc);
        return S_OK;
    }

    *pszKeyboardShortcut = 0;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accName(VARIANT varID, BSTR* pszName)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QString n = accessible->text(Name, varID.lVal);
    if (n.size()) {
        *pszName = QStringToBSTR(n);
        return S_OK;
    }

    *pszName = 0;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::put_accName(VARIANT, BSTR)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accRole(VARIANT varID, VARIANT *pvarRole)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    Role role = accessible->role(varID.lVal);
    if (role != NoRole) {
        (*pvarRole).vt = VT_I4;
        (*pvarRole).lVal = role;
    } else {
        (*pvarRole).vt = VT_EMPTY;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accState(VARIANT varID, VARIANT *pvarState)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    (*pvarState).vt = VT_I4;
    (*pvarState).lVal = accessible->state(varID.lVal);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accValue(VARIANT varID, BSTR* pszValue)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QString value = accessible->text(Value, varID.lVal);
    if (!value.isNull()) {
        *pszValue = QStringToBSTR(value);
        return S_OK;
    }

    *pszValue = 0;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::put_accValue(VARIANT, BSTR)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    return DISP_E_MEMBERNOTFOUND;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::accSelect(long flagsSelect, VARIANT varID)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    bool res = false;

    if (flagsSelect & SELFLAG_TAKEFOCUS)
        res = accessible->doAction(SetFocus, varID.lVal, QVariantList());
    if (flagsSelect & SELFLAG_TAKESELECTION) {
        accessible->doAction(ClearSelection, 0, QVariantList());
        res = accessible->doAction(AddToSelection, varID.lVal, QVariantList());
    }
    if (flagsSelect & SELFLAG_EXTENDSELECTION)
        res = accessible->doAction(ExtendSelection, varID.lVal, QVariantList());
    if (flagsSelect & SELFLAG_ADDSELECTION)
        res = accessible->doAction(AddToSelection, varID.lVal, QVariantList());
    if (flagsSelect & SELFLAG_REMOVESELECTION)
        res = accessible->doAction(RemoveSelection, varID.lVal, QVariantList());

    return res ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accFocus(VARIANT *pvarID)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    QAccessibleInterface *acc = 0;
    int control = accessible->navigate(FocusChild, 1, &acc);
    if (control == -1) {
        (*pvarID).vt = VT_EMPTY;
        return S_FALSE;
    }
    if (!acc || control == 0) {
        (*pvarID).vt = VT_I4;
        (*pvarID).lVal = control ? control : CHILDID_SELF;
        return S_OK;
    }

    QWindowsAccessible* wacc = new QWindowsAccessible(acc);
    IDispatch *iface = 0;
    wacc->QueryInterface(IID_IDispatch, (void**)&iface);
    if (iface) {
        (*pvarID).vt = VT_DISPATCH;
        (*pvarID).pdispVal = iface;
        return S_OK;
    } else {
        delete wacc;
    }

    (*pvarID).vt = VT_EMPTY;
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::get_accSelection(VARIANT *pvarChildren)
{
#ifdef DEBUG_SHOW_ATCLIENT_COMMANDS
    showDebug(__FUNCTION__, accessible);
#endif //DEBUG_SHOW_ATCLIENT_COMMANDS
    if (!accessible->isValid())
        return E_FAIL;

    int cc = accessible->childCount();
    QVector<int> sel(cc);
    int selIndex = 0;
    for (int i = 1; i <= cc; ++i) {
        QAccessibleInterface *child = 0;
        int i2 = accessible->navigate(Child, i, &child);
        bool isSelected = false;
        if (child) {
            isSelected = child->state(0) & Selected;
            delete child;
            child = 0;
        } else {
            isSelected = accessible->state(i2) & Selected;
        }
        if (isSelected)
            sel[selIndex++] = i;
    }
    sel.resize(selIndex);
    if (sel.isEmpty()) {
        (*pvarChildren).vt = VT_EMPTY;
        return S_FALSE;
    }
    if (sel.size() == 1) {
        (*pvarChildren).vt = VT_I4;
        (*pvarChildren).lVal = sel[0];
        return S_OK;
    }
    IEnumVARIANT *iface = new QWindowsEnumerate(sel);
    IUnknown *uiface;
    iface->QueryInterface(IID_IUnknown, (void**)&uiface);
    (*pvarChildren).vt = VT_UNKNOWN;
    (*pvarChildren).punkVal = uiface;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::GetWindow(HWND *phwnd)
{
    *phwnd = 0;
    if (!accessible->isValid())
        return E_UNEXPECTED;

    QObject *o = accessible->object();
    if (!o || !o->isWidgetType())
        return E_FAIL;

    *phwnd = static_cast<QWidget*>(o)->winId();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE QWindowsAccessible::ContextSensitiveHelp(BOOL)
{
    return S_OK;
}

QT_END_NAMESPACE

#endif // QT_NO_ACCESSIBILITY
