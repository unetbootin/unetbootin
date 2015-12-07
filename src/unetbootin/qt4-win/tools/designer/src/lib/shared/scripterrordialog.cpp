/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

/*
TRANSLATOR qdesigner_internal::ScriptErrorDialog
*/

#include "scripterrordialog_p.h"

#include <QtGui/QTextEdit>
#include <QtGui/QTextCursor>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPen>

QT_BEGIN_NAMESPACE

static void formatError(const QFormScriptRunner::Error &error,
                        QTextCursor &cursor)
{
    const QTextCharFormat oldFormat = cursor.charFormat();
    // Message
    cursor.insertText(QObject::tr("An error occurred while running the scripts for \"%1\":\n").arg(error.objectName));

    QTextCharFormat format(oldFormat);

    // verbatim listing
    format.setFontFamily(QLatin1String("Courier"));
    cursor.insertText(error.script, format);

    const QString newLine(QLatin1Char('\n'));

    cursor.insertText(newLine);

    // red error
    format = oldFormat;
    format.setTextOutline(QPen(Qt::red));
    cursor.insertText(error.errorMessage, format);
    cursor.insertText(newLine);
    cursor.setCharFormat (oldFormat);
}

namespace qdesigner_internal {

    // ScriptErrorDialog
    ScriptErrorDialog::ScriptErrorDialog(const Errors& errors, QWidget *parent) :
        QDialog(parent),
        m_textEdit(new QTextEdit)
    {
        setWindowTitle(tr("Script errors"));
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setModal(true);

        QVBoxLayout *vboxLayout = new QVBoxLayout(this);

        m_textEdit->setReadOnly(true);
        m_textEdit->setMinimumSize(QSize(600, 400));
        vboxLayout->addWidget(m_textEdit);
        // button box
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
        connect(buttonBox , SIGNAL(rejected()), this, SLOT(reject()));
        vboxLayout->addWidget(buttonBox);

        // Generate text
        QTextCursor cursor = m_textEdit->textCursor();
        cursor.movePosition (QTextCursor::End);
        foreach (const QFormScriptRunner::Error error, errors)
            formatError(error, cursor);
    }
} // namespace qdesigner_internal

QT_END_NAMESPACE
