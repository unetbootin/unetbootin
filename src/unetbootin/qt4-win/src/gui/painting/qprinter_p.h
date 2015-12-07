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

#ifndef QPRINTER_P_H
#define QPRINTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include "QtCore/qglobal.h"

#ifndef QT_NO_PRINTER

#include "QtGui/qprinter.h"
#include "QtGui/qprintengine.h"
#include "QtGui/qprintdialog.h"
#include "QtCore/qpointer.h"

#include <limits.h>

QT_BEGIN_NAMESPACE

class QPrintEngine;
class QPreviewPaintEngine;
class QPicture;

class QPrinterPrivate
{
    Q_DECLARE_PUBLIC(QPrinter)
public:
    QPrinterPrivate(QPrinter *printer)
        : printEngine(0)
        , paintEngine(0)
        , q_ptr(printer)
        , options(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange |
                QAbstractPrintDialog::PrintCollateCopies | QAbstractPrintDialog::PrintShowPageSize)
        , printRange(QAbstractPrintDialog::AllPages)
        , minPage(1)
        , maxPage(INT_MAX)
        , fromPage(0)
        , toPage(0)
        , use_default_engine(true)
        , validPrinter(false)
        , hasCustomPageMargins(false)
    {
    }

    ~QPrinterPrivate() {

    }

    void createDefaultEngines();
#ifndef QT_NO_PRINTPREVIEWWIDGET
    QList<const QPicture *> previewPages() const;
    void setPreviewMode(bool);
#endif

    void addToManualSetList(QPrintEngine::PrintEnginePropertyKey key);

    QPrinter::PrinterMode printerMode;
    QPrinter::OutputFormat outputFormat;
    QPrintEngine *printEngine;
    QPaintEngine *paintEngine;

    QPrintEngine *realPrintEngine;
    QPaintEngine *realPaintEngine;
#ifndef QT_NO_PRINTPREVIEWWIDGET
    QPreviewPaintEngine *previewEngine;
#endif

    QPrinter *q_ptr;

    QAbstractPrintDialog::PrintDialogOptions options;
    QAbstractPrintDialog::PrintRange printRange;
    int minPage, maxPage, fromPage, toPage;

    uint use_default_engine : 1;
    uint had_default_engines : 1;

    uint validPrinter : 1;
    uint hasCustomPageMargins : 1;

    // Used to remember which properties have been manually set by the user.
    QList<QPrintEngine::PrintEnginePropertyKey> manualSetList;
};

QT_END_NAMESPACE

#endif // QT_NO_PRINTER

#endif // QPRINTER_P_H
