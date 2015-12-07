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

#include "qpagesetupdialog.h"

#ifndef QT_NO_PRINTDIALOG
#include <qapplication.h>

#include <private/qprintengine_win_p.h>
#include <private/qabstractpagesetupdialog_p.h>

QT_BEGIN_NAMESPACE

class QPageSetupDialogPrivate : public QAbstractPageSetupDialogPrivate
{
};

QPageSetupDialog::QPageSetupDialog(QPrinter *printer, QWidget *parent)
    : QAbstractPageSetupDialog(*(new QPageSetupDialogPrivate), printer, parent)
{

}

int QPageSetupDialog::exec()
{
    Q_D(QPageSetupDialog);

    if (d->printer->outputFormat() != QPrinter::NativeFormat)
        return Rejected;

    QWin32PrintEngine *engine = static_cast<QWin32PrintEngine*>(d->printer->paintEngine());
    QWin32PrintEnginePrivate *ep = static_cast<QWin32PrintEnginePrivate *>(engine->d_ptr);

    PAGESETUPDLG psd;
    memset(&psd, 0, sizeof(PAGESETUPDLG));
    psd.lStructSize = sizeof(PAGESETUPDLG);

    // we need a temp DEVMODE struct if we don't have a global DEVMODE
    HGLOBAL hDevMode;
    int devModeSize;
    if (!ep->globalDevMode) {
        QT_WA( { devModeSize = sizeof(DEVMODEW) + ((DEVMODEW *) ep->devMode)->dmDriverExtra; },
               { devModeSize = sizeof(DEVMODEA) + ((DEVMODEA *) ep->devMode)->dmDriverExtra; });
        hDevMode = GlobalAlloc(GHND, devModeSize);
        if (hDevMode) {
            void *dest = GlobalLock(hDevMode);
            memcpy(dest, ep->devMode, devModeSize);
            GlobalUnlock(hDevMode);
        }
        psd.hDevMode = hDevMode;
    } else {
        psd.hDevMode = ep->devMode;
    }

    HGLOBAL *tempDevNames = ep->createDevNames();
    psd.hDevNames = tempDevNames;

    QWidget *parent = parentWidget();
    parent = parent ? parent->window() : qApp->activeWindow();
    Q_ASSERT(!parent ||parent->testAttribute(Qt::WA_WState_Created));
    psd.hwndOwner = parent ? parent->winId() : 0;

    QRect paperRect = d->printer->paperRect();
    QRect pageRect = d->printer->pageRect();

    psd.Flags = PSD_MARGINS;
    double multiplier = 1;
    switch (QLocale::system().measurementSystem()) {
    case QLocale::MetricSystem:
        psd.Flags |= PSD_INHUNDREDTHSOFMILLIMETERS;
        multiplier = 1;
        break;
    case QLocale::ImperialSystem:
        psd.Flags |= PSD_INTHOUSANDTHSOFINCHES;
        multiplier = 25.4/10;
        break;
    }

    QRect marginRect = ep->getPageMargins();
    psd.rtMargin.left   = marginRect.left()   / multiplier;
    psd.rtMargin.top    = marginRect.top()    / multiplier;
    psd.rtMargin.right  = marginRect.width()  / multiplier;;
    psd.rtMargin.bottom = marginRect.height() / multiplier;;

    bool result = PageSetupDlg(&psd);
    if (result) {
        ep->readDevnames(psd.hDevNames);
        ep->readDevmode(psd.hDevMode);

        QRect theseMargins = QRect(psd.rtMargin.left   * multiplier,
                                   psd.rtMargin.top    * multiplier,
                                   psd.rtMargin.right  * multiplier,
                                   psd.rtMargin.bottom * multiplier);

        if (theseMargins != marginRect) {
            ep->setPageMargins(psd.rtMargin.left   * multiplier,
                               psd.rtMargin.top    * multiplier,
                               psd.rtMargin.right  * multiplier,
                               psd.rtMargin.bottom * multiplier);
        }

        // copy from our temp DEVMODE struct
        if (!ep->globalDevMode && hDevMode) {
            void *src = GlobalLock(hDevMode);
            memcpy(ep->devMode, src, devModeSize);
            GlobalUnlock(hDevMode);
        }
    }

    if (!ep->globalDevMode && hDevMode)
        GlobalFree(hDevMode);
    GlobalFree(tempDevNames);
    return result;
}

QT_END_NAMESPACE
#endif
