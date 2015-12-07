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

#include "qapplication.h"

#ifndef QT_NO_DRAGANDDROP

#include "qapplication_p.h"
#include "qevent.h"
#include "qpainter.h"
#include "qwidget.h"
#include "qbuffer.h"
#include "qdatastream.h"
#include "qcursor.h"
#include "qt_windows.h"
#include <shlobj.h>
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif
#include "qdnd_p.h"
#include "qdebug.h"

#if defined(Q_OS_WINCE)
#include "qguifunctions_wince.h"
#endif

QT_BEGIN_NAMESPACE

//#define QDND_DEBUG

#ifdef QDND_DEBUG
extern QString dragActionsToString(Qt::DropActions actions);
#endif

Qt::DropActions translateToQDragDropActions(DWORD pdwEffects)
{
    Qt::DropActions actions = Qt::IgnoreAction;
    if (pdwEffects & DROPEFFECT_LINK)
        actions |= Qt::LinkAction;
    if (pdwEffects & DROPEFFECT_COPY)
        actions |= Qt::CopyAction;
    if (pdwEffects & DROPEFFECT_MOVE)
        actions |= Qt::MoveAction;
    return actions;
}

Qt::DropAction translateToQDragDropAction(DWORD pdwEffect)
{
    if (pdwEffect & DROPEFFECT_LINK)
        return Qt::LinkAction;
    if (pdwEffect & DROPEFFECT_COPY)
        return Qt::CopyAction;
    if (pdwEffect & DROPEFFECT_MOVE)
        return Qt::MoveAction;
    return Qt::IgnoreAction;
}

DWORD translateToWinDragEffects(Qt::DropActions action)
{
    DWORD effect = DROPEFFECT_NONE;
    if (action & Qt::LinkAction)
        effect |= DROPEFFECT_LINK;
    if (action & Qt::CopyAction)
        effect |= DROPEFFECT_COPY;
    if (action & Qt::MoveAction)
        effect |= DROPEFFECT_MOVE;
    return effect;
}

Qt::KeyboardModifiers toQtKeyboardModifiers(DWORD keyState)
{
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;

    if (keyState & MK_SHIFT)
        modifiers |= Qt::ShiftModifier;
    if (keyState & MK_CONTROL)
        modifiers |= Qt::ControlModifier;
    if (keyState & MK_ALT)
        modifiers |= Qt::AltModifier;

    return modifiers;
}

Qt::MouseButtons toQtMouseButtons(DWORD keyState)
{
    Qt::MouseButtons buttons = Qt::NoButton;

    if (keyState & MK_LBUTTON)
        buttons |= Qt::LeftButton;
    if (keyState & MK_RBUTTON)
        buttons |= Qt::RightButton;
    if (keyState & MK_MBUTTON)
        buttons |= Qt::MidButton;

    return buttons;
}

class QOleDropSource : public IDropSource
{
public:
    QOleDropSource();
    virtual ~QOleDropSource();

    void createCursors();

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void ** ppvObj);
    STDMETHOD_(ULONG,AddRef)(void);
    STDMETHOD_(ULONG,Release)(void);

    // IDropSource methods
    STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
    STDMETHOD(GiveFeedback)(DWORD dwEffect);

private:
    Qt::DropAction currentAction;
    QMap <Qt::DropAction, QCursor> cursors;

    ULONG m_refs;
};


QOleDropSource::QOleDropSource()
{
    m_refs = 1;
    currentAction = Qt::IgnoreAction;
}

QOleDropSource::~QOleDropSource()
{
}

void QOleDropSource::createCursors()
{
    QDragManager *manager = QDragManager::self();
    if (manager && manager->object
        && (!manager->object->pixmap().isNull()
        || manager->hasCustomDragCursors())) {
        QPixmap pm = manager->object->pixmap();
        QList<Qt::DropAction> actions;
        actions << Qt::MoveAction << Qt::CopyAction << Qt::LinkAction;
        if (!manager->object->pixmap().isNull())
            actions << Qt::IgnoreAction;
        QPoint hotSpot = manager->object->hotSpot();
        for (int cnum = 0; cnum < actions.size(); ++cnum) {
            QPixmap cpm = manager->dragCursor(actions.at(cnum));
            int w = cpm.width();
            int h = cpm.height();

            if (!pm.isNull()) {
                int x1 = qMin(-hotSpot.x(),0);
                int x2 = qMax(pm.width()-hotSpot.x(),cpm.width());
                int y1 = qMin(-hotSpot.y(),0);
                int y2 = qMax(pm.height()-hotSpot.y(),cpm.height());

                w = x2-x1+1;
                h = y2-y1+1;
            }

            QRect srcRect = pm.rect();
            QPoint pmDest = QPoint(qMax(0, -hotSpot.x()), qMax(0, -hotSpot.y()));
            QPoint newHotSpot = hotSpot;

#if !defined(Q_OS_WINCE) || defined(GWES_ICONCURS)
			bool limitedCursorSize = (QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based)
				                  || (QSysInfo::WindowsVersion == QSysInfo::WV_NT)
                                  || (QSysInfo::WindowsVersion == QSysInfo::WV_CE);

            if (limitedCursorSize) {
                // Limited cursor size
                int reqw = GetSystemMetrics(SM_CXCURSOR);
                int reqh = GetSystemMetrics(SM_CYCURSOR);

                QPoint hotspotInPM = newHotSpot - pmDest;
                if (reqw < w) {
                    // Not wide enough - move objectpm right
                    qreal r = qreal(newHotSpot.x()) / w;
                    newHotSpot = QPoint(int(r * reqw), newHotSpot.y()); 
                    if (newHotSpot.x() + cpm.width() > reqw)
                        newHotSpot.setX(reqw - cpm.width());

                    srcRect = QRect(QPoint(hotspotInPM.x() - newHotSpot.x(), srcRect.top()), QSize(reqw, srcRect.height()));
                }
                if (reqh < h) {
                    qreal r = qreal(newHotSpot.y()) / h;
                    newHotSpot = QPoint(newHotSpot.x(), int(r * reqh));
                    if (newHotSpot.y() + cpm.height() > reqh)
                        newHotSpot.setY(reqh - cpm.height());
                    
                    srcRect = QRect(QPoint(srcRect.left(), hotspotInPM.y() - newHotSpot.y()), QSize(srcRect.width(), reqh));
                }
                // Always use system cursor size
                w = reqw;
                h = reqh;
            }
#endif            
            QPixmap newCursor(w, h);
            if (!pm.isNull()) {
                newCursor.fill(QColor(0, 0, 0, 0));
                QPainter p(&newCursor);                
                p.drawPixmap(pmDest, pm, srcRect);
                p.drawPixmap(qMax(0,newHotSpot.x()),qMax(0,newHotSpot.y()),cpm);
            } else {
                newCursor = cpm;
            }

#ifndef QT_NO_CURSOR
            cursors[actions.at(cnum)] = QCursor(newCursor, pm.isNull() ? 0 : qMax(0,newHotSpot.x()),
                                                pm.isNull() ? 0 : qMax(0,newHotSpot.y()));
#endif
        }
    }
}



//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
QOleDropSource::QueryInterface(REFIID iid, void FAR* FAR* ppv)
{
    if(iid == IID_IUnknown || iid == IID_IDropSource)
    {
      *ppv = this;
      ++m_refs;
      return NOERROR;
    }
    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
QOleDropSource::AddRef(void)
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
QOleDropSource::Release(void)
{
    if(--m_refs == 0)
    {
      delete this;
      return 0;
    }
    return m_refs;
}

//---------------------------------------------------------------------
//                    IDropSource Methods
//---------------------------------------------------------------------
STDMETHODIMP
QOleDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
#ifdef QDND_DEBUG
    qDebug("QOleDropSource::QueryContinueDrag(fEscapePressed %d, grfKeyState %d)", fEscapePressed, grfKeyState);
#endif

    if (fEscapePressed) {
        return ResultFromScode(DRAGDROP_S_CANCEL);
    } else if (!(grfKeyState & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON))) {
        return ResultFromScode(DRAGDROP_S_DROP);
    } else {
#if defined(Q_OS_WINCE)
        // grfKeyState is broken on CE, therefore need to check
        // the state manually
        if ((GetAsyncKeyState(VK_LBUTTON) == 0) &&
            (GetAsyncKeyState(VK_MBUTTON) == 0) &&
            (GetAsyncKeyState(VK_RBUTTON) == 0)) {
            return ResultFromScode(DRAGDROP_S_DROP);
        }
#endif
        qApp->processEvents();
        return NOERROR;
    }
}

STDMETHODIMP
QOleDropSource::GiveFeedback(DWORD dwEffect)
{
    Qt::DropAction action = translateToQDragDropAction(dwEffect);

#ifdef QDND_DEBUG
    qDebug("QOleDropSource::GiveFeedback(DWORD dwEffect)");
    qDebug("dwEffect = %s", dragActionsToString(action).toLatin1().data());
#endif

    if (currentAction != action) {
        currentAction = action;
        QDragManager::self()->emitActionChanged(currentAction);
    }

    if (cursors.contains(currentAction)) {
#ifndef QT_NO_CURSOR
        SetCursor(cursors[currentAction].handle());
#endif
        return ResultFromScode(S_OK);
    }

    return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
}


//---------------------------------------------------------------------
//                    QOleDataObject Constructor
//---------------------------------------------------------------------

QOleDataObject::QOleDataObject(QMimeData *mimeData)
{
    m_refs = 1;
    data = mimeData;
    CF_PERFORMEDDROPEFFECT = RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
    performedEffect = DROPEFFECT_NONE;
}

QOleDataObject::~QOleDataObject()
{
}

void QOleDataObject::releaseQt()
{
    data = 0;
}

const QMimeData *QOleDataObject::mimeData() const
{
    return data;
}

DWORD QOleDataObject::reportedPerformedEffect() const
{
    return performedEffect;
}

//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------

STDMETHODIMP
QOleDataObject::QueryInterface(REFIID iid, void FAR* FAR* ppv)
{
    if (iid == IID_IUnknown || iid == IID_IDataObject) {
        *ppv = this;
        AddRef();
        return NOERROR;
    }
    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG)
QOleDataObject::AddRef(void)
{
    return ++m_refs;
}

STDMETHODIMP_(ULONG)
QOleDataObject::Release(void)
{
    if (--m_refs == 0) {
        releaseQt();
        delete this;
        return 0;
    }
    return m_refs;
}

//---------------------------------------------------------------------
//                    IDataObject Methods
//
// The following methods are NOT supported for data transfer using the
// clipboard or drag-drop:
//
//      IDataObject::SetData    -- return E_NOTIMPL
//      IDataObject::DAdvise    -- return OLE_E_ADVISENOTSUPPORTED
//                 ::DUnadvise
//                 ::EnumDAdvise
//      IDataObject::GetCanonicalFormatEtc -- return E_NOTIMPL
//                     (NOTE: must set pformatetcOut->ptd = NULL)
//---------------------------------------------------------------------

STDMETHODIMP
QOleDataObject::GetData(LPFORMATETC pformatetc, LPSTGMEDIUM pmedium)
{
#ifdef QDND_DEBUG
    qDebug("QOleDataObject::GetData(LPFORMATETC pformatetc, LPSTGMEDIUM pmedium)");
#ifndef Q_OS_WINCE
    char buf[256] = {0};
    GetClipboardFormatNameA(pformatetc->cfFormat, buf, 255);
    qDebug("CF = %d : %s", pformatetc->cfFormat, buf);
#endif
#endif

    if (!data)
        return ResultFromScode(DATA_E_FORMATETC);

    QWindowsMime *converter = QWindowsMime::converterFromMime(*pformatetc, data);

    if (converter && converter->convertFromMime(*pformatetc, data, pmedium))
        return ResultFromScode(S_OK);
    else
        return ResultFromScode(DATA_E_FORMATETC);
}

STDMETHODIMP
QOleDataObject::GetDataHere(LPFORMATETC, LPSTGMEDIUM)
{
    return ResultFromScode(DATA_E_FORMATETC);
}

STDMETHODIMP
QOleDataObject::QueryGetData(LPFORMATETC pformatetc)
{
#ifdef QDND_DEBUG
    qDebug("QOleDataObject::QueryGetData(LPFORMATETC pformatetc)");
#endif

    if (!data)
        return ResultFromScode(DATA_E_FORMATETC);

    if (QWindowsMime::converterFromMime(*pformatetc, data))
        return ResultFromScode(S_OK);
    return ResultFromScode(S_FALSE);
}

STDMETHODIMP
QOleDataObject::GetCanonicalFormatEtc(LPFORMATETC, LPFORMATETC pformatetcOut)
{
    pformatetcOut->ptd = NULL;
    return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP
QOleDataObject::SetData(LPFORMATETC pFormatetc, STGMEDIUM *pMedium, BOOL fRelease)
{
    if (pFormatetc->cfFormat == CF_PERFORMEDDROPEFFECT && pMedium->tymed == TYMED_HGLOBAL) {
        DWORD * val = (DWORD*)GlobalLock(pMedium->hGlobal);
        performedEffect = *val;
        GlobalUnlock(pMedium->hGlobal);
        if (fRelease)
            ReleaseStgMedium(pMedium);
        return ResultFromScode(S_OK);
    }
    return ResultFromScode(E_NOTIMPL);
}


STDMETHODIMP
QOleDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC FAR* ppenumFormatEtc)
{
#ifdef QDND_DEBUG
    qDebug("QOleDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC FAR* ppenumFormatEtc)");
#endif

    if (!data)
        return ResultFromScode(DATA_E_FORMATETC);

    SCODE sc = S_OK;

    QVector<FORMATETC> fmtetcs;
    if (dwDirection == DATADIR_GET) {
        fmtetcs = QWindowsMime::allFormatsForMime(data);
    } else {
        FORMATETC formatetc;
        formatetc.cfFormat = CF_PERFORMEDDROPEFFECT;
        formatetc.dwAspect = DVASPECT_CONTENT;
        formatetc.lindex = -1;
        formatetc.ptd = NULL;
        formatetc.tymed = TYMED_HGLOBAL;
        fmtetcs.append(formatetc);
    }

    QOleEnumFmtEtc *enumFmtEtc = new QOleEnumFmtEtc(fmtetcs);
    *ppenumFormatEtc = enumFmtEtc;
    if (enumFmtEtc->isNull()) {
        delete enumFmtEtc;
        *ppenumFormatEtc = NULL;
        sc = E_OUTOFMEMORY;
    }

    return ResultFromScode(sc);
}

STDMETHODIMP
QOleDataObject::DAdvise(FORMATETC FAR*, DWORD,
                       LPADVISESINK, DWORD FAR*)
{
    return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);
}


STDMETHODIMP
QOleDataObject::DUnadvise(DWORD)
{
    return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);
}

STDMETHODIMP
QOleDataObject::EnumDAdvise(LPENUMSTATDATA FAR*)
{
    return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);
}


//---------------------------------------------------------------------
//                    QOleDropTarget
//---------------------------------------------------------------------

QOleDropTarget::QOleDropTarget(QWidget* w)
:   widget(w)
{
   m_refs = 1;
}

void QOleDropTarget::releaseQt()
{
    widget = 0;
}

//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
QOleDropTarget::QueryInterface(REFIID iid, void FAR* FAR* ppv)
{
    if(iid == IID_IUnknown || iid == IID_IDropTarget)
    {
      *ppv = this;
      AddRef();
      return NOERROR;
    }
    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
QOleDropTarget::AddRef(void)
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
QOleDropTarget::Release(void)
{
    if(--m_refs == 0)
    {
      delete this;
      return 0;
    }
    return m_refs;
}

//---------------------------------------------------------------------
//                    IDropTarget Methods
//---------------------------------------------------------------------

STDMETHODIMP
QOleDropTarget::DragEnter(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
#ifdef QDND_DEBUG
    qDebug("QOleDropTarget::DragEnter(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)");
#endif

    if (!QApplicationPrivate::tryModalHelper(widget)) {
        *pdwEffect = DROPEFFECT_NONE;
        return NOERROR;
    }

    QDragManager *manager = QDragManager::self();
    manager->dropData->currentDataObject = pDataObj;
    manager->dropData->currentDataObject->AddRef();
    sendDragEnterEvent(widget, grfKeyState, pt, pdwEffect);
    *pdwEffect = choosenEffect;

    return NOERROR;
}

void QOleDropTarget::sendDragEnterEvent(QWidget *dragEnterWidget, DWORD grfKeyState,
                                        POINTL pt, LPDWORD pdwEffect)
{
    Q_ASSERT(dragEnterWidget);
    lastPoint = dragEnterWidget->mapFromGlobal(QPoint(pt.x,pt.y));
    lastKeyState = grfKeyState;

    choosenEffect = DROPEFFECT_NONE;
    currentWidget = dragEnterWidget;

    QDragManager *manager = QDragManager::self();
    QMimeData * md = manager->source() ? manager->dragPrivate()->data : manager->dropData;
    QDragEnterEvent enterEvent(lastPoint, translateToQDragDropActions(*pdwEffect), md,
                      toQtMouseButtons(grfKeyState), toQtKeyboardModifiers(grfKeyState));
    QApplication::sendEvent(dragEnterWidget, &enterEvent);
    answerRect = enterEvent.answerRect();

    if (enterEvent.isAccepted()) {
        choosenEffect = translateToWinDragEffects(enterEvent.dropAction());
    }

    // Documentation states that a drag move event is sendt immidiatly after
    // a drag enter event. This will honor widgets overriding dragMoveEvent only:
    if (enterEvent.isAccepted()) {
        QDragMoveEvent moveEvent(lastPoint, translateToQDragDropActions(*pdwEffect), md,
                                 toQtMouseButtons(grfKeyState), toQtKeyboardModifiers(grfKeyState));
        answerRect = enterEvent.answerRect();
        moveEvent.setDropAction(enterEvent.dropAction());
        moveEvent.accept(); // accept by default, since enter event was accepted.

        QApplication::sendEvent(dragEnterWidget, &moveEvent);
        if (moveEvent.isAccepted()) {
            answerRect = moveEvent.answerRect();
            choosenEffect = translateToWinDragEffects(moveEvent.dropAction());
        } else {
            choosenEffect = DROPEFFECT_NONE;
        }
    }

}

STDMETHODIMP
QOleDropTarget::DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
#ifdef QDND_DEBUG
    qDebug("QOleDropTarget::DragOver(grfKeyState %d, pt (%d,%d), pdwEffect %d)", grfKeyState, pt.x, pt.y, pdwEffect);
#endif

    QWidget *dragOverWidget = widget->childAt(widget->mapFromGlobal(QPoint(pt.x, pt.y)));
    if (!dragOverWidget)
        dragOverWidget = widget;


    if (!QApplicationPrivate::tryModalHelper(dragOverWidget)
            || !dragOverWidget->testAttribute(Qt::WA_DropSiteRegistered)) {
        *pdwEffect = DROPEFFECT_NONE;
        return NOERROR;
    }

    QPoint tmpPoint = dragOverWidget->mapFromGlobal(QPoint(pt.x, pt.y));
    // see if we should compress this event
    if ((tmpPoint == lastPoint || answerRect.contains(tmpPoint)) && lastKeyState == grfKeyState) {
        *pdwEffect = choosenEffect;
        return NOERROR;
    }

    if (!dragOverWidget->internalWinId() && currentWidget && dragOverWidget != currentWidget) {
        // Send drag leave event to the previous drag widget.
        QDragLeaveEvent dragLeave;
        QApplication::sendEvent(currentWidget, &dragLeave);
        // Send drag enter event to the current drag widget.
        sendDragEnterEvent(dragOverWidget, grfKeyState, pt, pdwEffect);
    }

    lastPoint = tmpPoint;
    lastKeyState = grfKeyState;

    QDragManager *manager = QDragManager::self();
    QMimeData *md = manager->source() ? manager->dragPrivate()->data : manager->dropData;
    QDragMoveEvent e(lastPoint, translateToQDragDropActions(*pdwEffect), md,
                     toQtMouseButtons(grfKeyState), toQtKeyboardModifiers(grfKeyState));
    if (choosenEffect != DROPEFFECT_NONE) {
        e.setDropAction(translateToQDragDropAction(choosenEffect));
        e.accept();
    }
    QApplication::sendEvent(dragOverWidget, &e);

    answerRect = e.answerRect();
    if (e.isAccepted())
        choosenEffect = translateToWinDragEffects(e.dropAction());
    else
        choosenEffect = DROPEFFECT_NONE;
    *pdwEffect = choosenEffect;

    return NOERROR;
}

STDMETHODIMP
QOleDropTarget::DragLeave()
{
#ifdef QDND_DEBUG
    qDebug("QOleDropTarget::DragLeave()");
#endif

    if (!QApplicationPrivate::tryModalHelper(widget)) {
        return NOERROR;
    }

    currentWidget = 0;
    QDragLeaveEvent e;
    QApplication::sendEvent(widget, &e);

    QDragManager *manager = QDragManager::self();

    if (manager->dropData->currentDataObject) { // Sanity
        manager->dropData->currentDataObject->Release();
        manager->dropData->currentDataObject = 0;
    }

    return NOERROR;
}

#define KEY_STATE_BUTTON_MASK (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)

STDMETHODIMP
QOleDropTarget::Drop(LPDATAOBJECT /*pDataObj*/, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
#ifdef QDND_DEBUG
    qDebug("QOleDropTarget::Drop(LPDATAOBJECT /*pDataObj*/, grfKeyState %d, POINTL pt, LPDWORD pdwEffect)", grfKeyState);
#endif

    QWidget *dropWidget = widget->childAt(widget->mapFromGlobal(QPoint(pt.x, pt.y)));
    if (!dropWidget)
        dropWidget = widget;

    if (!QApplicationPrivate::tryModalHelper(dropWidget)
            || !dropWidget->testAttribute(Qt::WA_DropSiteRegistered)) {
        *pdwEffect = DROPEFFECT_NONE;
        return NOERROR;
    }

    lastPoint = dropWidget->mapFromGlobal(QPoint(pt.x,pt.y));
    // grfKeyState does not all ways contain button state in the drop so if
    // it doesn't then use the last known button state;
    if ((grfKeyState & KEY_STATE_BUTTON_MASK) == 0)
        grfKeyState |= lastKeyState & KEY_STATE_BUTTON_MASK;
    lastKeyState = grfKeyState;

    QDragManager *manager = QDragManager::self();
    QMimeData *md = manager->source() ? manager->dragPrivate()->data : manager->dropData;
    QDropEvent e(lastPoint, translateToQDragDropActions(*pdwEffect), md,
                 toQtMouseButtons(grfKeyState), toQtKeyboardModifiers(grfKeyState));
    QApplication::sendEvent(dropWidget, &e);

    if (e.isAccepted()) {
        if (e.dropAction() == Qt::MoveAction || e.dropAction() == Qt::TargetMoveAction) {
            if (e.dropAction() == Qt::MoveAction)
                choosenEffect = DROPEFFECT_MOVE;
            else
                choosenEffect = DROPEFFECT_COPY;
            HGLOBAL hData = GlobalAlloc(0, sizeof(DWORD));
            if (hData) {
                DWORD *moveEffect = (DWORD *)GlobalLock(hData);;
                *moveEffect = DROPEFFECT_MOVE;
                GlobalUnlock(hData);
                STGMEDIUM medium;
                memset(&medium, 0, sizeof(STGMEDIUM));
                medium.tymed = TYMED_HGLOBAL;
                medium.hGlobal = hData;
                FORMATETC format;
                format.cfFormat = RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
                format.tymed = TYMED_HGLOBAL;
                format.ptd = 0;
                format.dwAspect = 1;
                format.lindex = -1;
                manager->dropData->currentDataObject->SetData(&format, &medium, true);
            }
        } else {
            choosenEffect = translateToWinDragEffects(e.dropAction());
        }
    } else {
        choosenEffect = DROPEFFECT_NONE;
    }
    *pdwEffect = choosenEffect;


    if (manager->dropData->currentDataObject) {
        manager->dropData->currentDataObject->Release();
        manager->dropData->currentDataObject = 0;
    }

    return NOERROR;

        // We won't get any mouserelease-event, so manually adjust qApp state:
///### test this        QApplication::winMouseButtonUp();
}

//---------------------------------------------------------------------
//                    QDropData
//---------------------------------------------------------------------

bool QDropData::hasFormat_sys(const QString &mimeType) const
{
    if (!currentDataObject) // Sanity
        return false;

    return QWindowsMime::converterToMime(mimeType, currentDataObject) != 0;
}

QStringList QDropData::formats_sys() const
{
    QStringList fmts;
    if (!currentDataObject) // Sanity
        return fmts;

    fmts = QWindowsMime::allMimesForFormats(currentDataObject);

    return fmts;
}

QVariant QDropData::retrieveData_sys(const QString &mimeType, QVariant::Type type) const
{
    QVariant result;

    if (!currentDataObject) // Sanity
        return result;

    QWindowsMime *converter = QWindowsMime::converterToMime(mimeType, currentDataObject);

    if (converter)
        result = converter->convertToMime(mimeType, currentDataObject, type);

    return result;
}

Qt::DropAction QDragManager::drag(QDrag *o)

{
#ifdef QDND_DEBUG
    qDebug("QDragManager::drag(QDrag *drag)");
#endif

    if (object == o || !o || !o->d_func()->source)
        return Qt::IgnoreAction;

    if (object) {
        cancel();
        qApp->removeEventFilter(this);
        beingCancelled = false;
    }

    object = o;

#ifdef QDND_DEBUG
    qDebug("actions = %s", dragActionsToString(dragPrivate()->possible_actions).toLatin1().data());
#endif

    dragPrivate()->target = 0;

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(this, 0, QAccessible::DragDropStart);
#endif

    DWORD resultEffect;
    QOleDropSource *src = new QOleDropSource();
    src->createCursors();
    QOleDataObject *obj = new QOleDataObject(o->mimeData());
    DWORD allowedEffects = translateToWinDragEffects(dragPrivate()->possible_actions);

#if !defined(Q_OS_WINCE) || defined(GWES_ICONCURS)
    HRESULT r = DoDragDrop(obj, src, allowedEffects, &resultEffect);
#else
    HRESULT r = DRAGDROP_S_CANCEL;
    resultEffect = DROPEFFECT_MOVE;
#endif

    Qt::DropAction ret = Qt::IgnoreAction;
    if (r == DRAGDROP_S_DROP) {
        if (obj->reportedPerformedEffect() == DROPEFFECT_MOVE && resultEffect != DROPEFFECT_MOVE) {
            ret = Qt::TargetMoveAction;
            resultEffect = DROPEFFECT_MOVE;
        } else {
            ret = translateToQDragDropAction(resultEffect);
        }
        // Force it to be a copy if an unsupported operation occurred.
        // This indicates a bug in the drop target.
        if (resultEffect != DROPEFFECT_NONE && !(resultEffect & allowedEffects))
            ret = Qt::CopyAction;
    } else {
        dragPrivate()->target = 0;
    }

    // clean up
    obj->releaseQt();
    obj->Release();        // Will delete obj if refcount becomes 0
    src->Release();        // Will delete src if refcount becomes 0
    object = 0;
    o->setMimeData(0);
    o->deleteLater();

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(this, 0, QAccessible::DragDropEnd);
#endif

    return ret;
}

void QDragManager::cancel(bool /* deleteSource */)
{
    if (object) {
        beingCancelled = true;
        object = 0;
    }

#ifndef QT_NO_CURSOR
    // insert cancel code here ######## todo

    if (restoreCursor) {
        QApplication::restoreOverrideCursor();
        restoreCursor = false;
    }
#endif
#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(this, 0, QAccessible::DragDropEnd);
#endif
}

void QDragManager::updatePixmap()
{
    // not used in windows implementation
}

bool QDragManager::eventFilter(QObject *, QEvent *)
{
    // not used in windows implementation
    return false;
}

void QDragManager::timerEvent(QTimerEvent*)
{
    // not used in windows implementation
}

void QDragManager::move(const QPoint &)
{
    // not used in windows implementation
}

void QDragManager::drop()
{
    // not used in windows implementation
}

QT_END_NAMESPACE

#endif // QT_NO_DRAGANDDROP
