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

#include "qdnd_p.h"

#ifndef QT_NO_DRAGANDDROP

#if defined(Q_OS_WINCE)
#include "qguifunctions_wince.h"
#endif

QT_BEGIN_NAMESPACE

QOleEnumFmtEtc::QOleEnumFmtEtc(const QVector<FORMATETC> &fmtetcs)
{
    m_isNull = false;
    m_dwRefs = 1;
    m_nIndex = 0;

    for (int idx = 0; idx < fmtetcs.count(); ++idx) {
        LPFORMATETC destetc = new FORMATETC();
        if (copyFormatEtc(destetc, (LPFORMATETC)&(fmtetcs.at(idx)))) {
            m_lpfmtetcs.append(destetc);
        } else {
            m_isNull = true;
            delete destetc;
            break;
        }
    }
}

QOleEnumFmtEtc::QOleEnumFmtEtc(const QVector<LPFORMATETC> &lpfmtetcs)
{
    m_isNull = false;
    m_dwRefs = 1;
    m_nIndex = 0;

    for (int idx = 0; idx < lpfmtetcs.count(); ++idx) {
        LPFORMATETC srcetc = lpfmtetcs.at(idx);
        LPFORMATETC destetc = new FORMATETC();
        if (copyFormatEtc(destetc, srcetc)) {
            m_lpfmtetcs.append(destetc);
        } else {
            m_isNull = true;
            delete destetc;
            break;
        }
    }
}

QOleEnumFmtEtc::~QOleEnumFmtEtc()
{
    LPMALLOC pmalloc;

#if !defined(Q_OS_WINCE)
    if (CoGetMalloc(MEMCTX_TASK, &pmalloc) == NOERROR) {
#else
    if (SHGetMalloc(&pmalloc) == NOERROR) {
#endif
        for (int idx = 0; idx < m_lpfmtetcs.count(); ++idx) {
            LPFORMATETC tmpetc = m_lpfmtetcs.at(idx);
            if (tmpetc->ptd)
                pmalloc->Free(tmpetc->ptd);
            delete tmpetc;
        }

        pmalloc->Release();
    }
    m_lpfmtetcs.clear();
}

bool QOleEnumFmtEtc::isNull() const
{
    return m_isNull;
}

// IUnknown methods
STDMETHODIMP
QOleEnumFmtEtc::QueryInterface(REFIID riid, void FAR* FAR* ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IEnumFORMATETC) {
        *ppvObj = this;
        AddRef();
        return NOERROR;
    }
    *ppvObj = NULL;
    return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG)
QOleEnumFmtEtc::AddRef(void)
{
    return ++m_dwRefs;
}

STDMETHODIMP_(ULONG)
QOleEnumFmtEtc::Release(void)
{
    if (--m_dwRefs == 0) {
        delete this;
        return 0;
    }
    return m_dwRefs;
}

// IEnumFORMATETC methods
STDMETHODIMP
QOleEnumFmtEtc::Next(ULONG celt, LPFORMATETC rgelt, ULONG FAR* pceltFetched)
{
    ULONG i=0;
    ULONG nOffset;

    if (rgelt == NULL)
        return ResultFromScode(E_INVALIDARG);

    while (i < celt) {
        nOffset = m_nIndex + i;

        if (nOffset < ULONG(m_lpfmtetcs.count())) {
            copyFormatEtc((LPFORMATETC)&(rgelt[i]), m_lpfmtetcs.at(nOffset));
            i++;
        } else {
            break;
        }
    }

    m_nIndex += (WORD)i;

    if (pceltFetched != NULL)
        *pceltFetched = i;

    if (i != celt)
        return ResultFromScode(S_FALSE);

    return NOERROR;    
}

STDMETHODIMP
QOleEnumFmtEtc::Skip(ULONG celt)
{
    ULONG i=0;
    ULONG nOffset;

    while (i < celt) {
        nOffset = m_nIndex + i;

        if (nOffset < ULONG(m_lpfmtetcs.count())) {
            i++;
        } else {
            break;
        }
    }

    m_nIndex += (WORD)i;

    if (i != celt)
        return ResultFromScode(S_FALSE);

    return NOERROR;
}

STDMETHODIMP
QOleEnumFmtEtc::Reset()
{
    m_nIndex = 0;
    return NOERROR;
}

STDMETHODIMP
QOleEnumFmtEtc::Clone(LPENUMFORMATETC FAR* newEnum)
{
    if (newEnum == NULL)
        return ResultFromScode(E_INVALIDARG);

    QOleEnumFmtEtc *result = new QOleEnumFmtEtc(m_lpfmtetcs);
    result->m_nIndex = m_nIndex;

    if (result->isNull()) {
        delete result;
        return ResultFromScode(E_OUTOFMEMORY);
    } else {
        *newEnum = result;
    }

    return NOERROR;
}

bool QOleEnumFmtEtc::copyFormatEtc(LPFORMATETC dest, LPFORMATETC src) const
{
    if (dest == NULL || src == NULL)
        return false;

    *dest = *src;

    if (src->ptd) {
        LPVOID pout;
        LPMALLOC pmalloc;

#if !defined(Q_OS_WINCE)
        if (CoGetMalloc(MEMCTX_TASK, &pmalloc) != NOERROR)
#else
        if (SHGetMalloc(&pmalloc) != NOERROR)
#endif
            return false;

        pout = (LPVOID)pmalloc->Alloc(src->ptd->tdSize);
        memcpy(dest->ptd, src->ptd, size_t(src->ptd->tdSize));

        pmalloc->Release();
    }

    return true;
}

QT_END_NAMESPACE
#endif //QT_NO_DRAGANDDROP
