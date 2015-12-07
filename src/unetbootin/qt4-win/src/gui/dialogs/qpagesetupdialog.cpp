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

#include <private/qabstractpagesetupdialog_p.h>

#ifndef QT_NO_PRINTDIALOG

QT_BEGIN_NAMESPACE

/*!
    \enum QPageSetupDialog::PageSetupDialogOption

    Used to specify options to the page setup dialog

    \value None None of the options are enabled.
    \value DontUseSheet Do not make the native print dialog a sheet. By default
    on Mac OS X, the native dialog is made a sheet if it has a parent that can
    accept sheets and is visible. Internally, Mac OS X tracks whether
    a printing <em>session</em> and not which particular dialog should have a sheet or not.
    Therefore, make sure this value matches between the page setup dialog and
    the print dialog or you can potentially end up in a modal loop that you can't break.
*/

/*!
    Adds the option \a option to the set of enabled options in this dialog.
*/
void QPageSetupDialog::addEnabledOption(PageSetupDialogOption option)
{
    reinterpret_cast<QAbstractPageSetupDialogPrivate *>(d_func())->addEnabledOption(option);
}

/*!
    Sets the set of options that should be enabled for the page setup dialog
    to \a options.
*/
void QPageSetupDialog::setEnabledOptions(PageSetupDialogOptions options)
{
    reinterpret_cast<QAbstractPageSetupDialogPrivate *>(d_func())->setEnabledOptions(options);
}

/*!
    Returns the set of enabled options in this dialog.
*/
QPageSetupDialog::PageSetupDialogOptions QPageSetupDialog::enabledOptions() const
{
    return reinterpret_cast<const QAbstractPageSetupDialogPrivate *>(d_func())->enabledOptions();
}

/*!
    Returns true if the specified \a option is enabled; otherwise returns false
*/
bool QPageSetupDialog::isOptionEnabled(PageSetupDialogOption option) const
{
    return reinterpret_cast<const QAbstractPageSetupDialogPrivate *>(d_func())->isOptionEnabled(option);
}

QT_END_NAMESPACE

#endif
