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

#include "qaccessible2.h"
#include "qapplication.h"
#include "qclipboard.h"

#ifndef QT_NO_ACCESSIBILITY

QT_BEGIN_NAMESPACE

/*!
    \namespace QAccessible2
    \ingroup accessibility
    \internal
    \preliminary

    \brief The QAccessible2 namespace defines constants relating to
    IAccessible2-based interfaces

    \link http://www.linux-foundation.org/en/Accessibility/IAccessible2 IAccessible2 Specification \endlink
*/

/*!
    \class QAccessibleTextInterface

    \ingroup accessibility
    \internal
    \preliminary

    \brief The QAccessibleTextInterface class implements support for
    the IAccessibleText interface.

    \link http://www.linux-foundation.org/en/Accessibility/IAccessible2 IAccessible2 Specification \endlink
*/

/*!
    \class QAccessibleEditableTextInterface
    \ingroup accessibility
    \internal
    \preliminary

    \brief The QAccessibleEditableTextInterface class implements support for
    the IAccessibleEditableText interface.

    \link http://www.linux-foundation.org/en/Accessibility/IAccessible2 IAccessible2 Specification \endlink
*/

/*!
    \class QAccessibleSimpleEditableTextInterface
    \ingroup accessibility
    \internal
    \preliminary

    \brief The QAccessibleSimpleEditableTextInterface class is a convenience class for
    text-based widgets.

    \link http://www.linux-foundation.org/en/Accessibility/IAccessible2 IAccessible2 Specification \endlink
*/

/*!
    \class QAccessibleValueInterface
    \ingroup accessibility
    \internal
    \preliminary

    \brief The QAccessibleValueInterface class implements support for
    the IAccessibleValue interface.

    \link http://www.linux-foundation.org/en/Accessibility/IAccessible2 IAccessible2 Specification \endlink
*/

QAccessibleSimpleEditableTextInterface::QAccessibleSimpleEditableTextInterface(
                QAccessibleInterface *accessibleInterface)
    : iface(accessibleInterface)
{
    Q_ASSERT(iface);
}

#ifndef QT_NO_CLIPBOARD
static QString textForRange(QAccessibleInterface *iface, int startOffset, int endOffset)
{
    return iface->text(QAccessible::Value, 0).mid(startOffset, endOffset - startOffset);
}
#endif

void QAccessibleSimpleEditableTextInterface::copyText(int startOffset, int endOffset)
{
#ifdef QT_NO_CLIPBOARD
    Q_UNUSED(startOffset);
    Q_UNUSED(endOffset);
#else
    QApplication::clipboard()->setText(textForRange(iface, startOffset, endOffset));
#endif
}

void QAccessibleSimpleEditableTextInterface::deleteText(int startOffset, int endOffset)
{
    QString txt = iface->text(QAccessible::Value, 0);
    txt.remove(startOffset, endOffset - startOffset);
    iface->setText(QAccessible::Value, 0, txt);
}

void QAccessibleSimpleEditableTextInterface::insertText(int offset, const QString &text)
{
    QString txt = iface->text(QAccessible::Value, 0);
    txt.insert(offset, text);
    iface->setText(QAccessible::Value, 0, txt);
}

void QAccessibleSimpleEditableTextInterface::cutText(int startOffset, int endOffset)
{
#ifdef QT_NO_CLIPBOARD
    Q_UNUSED(startOffset);
    Q_UNUSED(endOffset);
#else
    QString sub = textForRange(iface, startOffset, endOffset);
    deleteText(startOffset, endOffset);
    QApplication::clipboard()->setText(sub);
#endif
}

void QAccessibleSimpleEditableTextInterface::pasteText(int offset)
{
#ifdef QT_NO_CLIPBOARD
    Q_UNUSED(offset);
#else
    QString txt = iface->text(QAccessible::Value, 0);
    txt.insert(offset, QApplication::clipboard()->text());
    iface->setText(QAccessible::Value, 0, txt);
#endif
}

void QAccessibleSimpleEditableTextInterface::replaceText(int startOffset, int endOffset, const QString &text)
{
    QString txt = iface->text(QAccessible::Value, 0);
    txt.replace(startOffset, endOffset - startOffset, text);
    iface->setText(QAccessible::Value, 0, txt);
}

QT_END_NAMESPACE

#endif // QT_NO_ACCESSIBILITY
