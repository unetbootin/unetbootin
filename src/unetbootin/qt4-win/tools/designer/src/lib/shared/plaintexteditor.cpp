/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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
TRANSLATOR qdesigner_internal::PlainTextEditorDialog
*/

#include "plaintexteditor_p.h"

#include <QtGui/QPlainTextEdit>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

PlainTextEditorDialog::PlainTextEditorDialog(QWidget *parent)  :
    QDialog(parent),
    m_editor(new QPlainTextEdit)
{
    setWindowTitle(tr("Edit text"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(m_editor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    QPushButton *ok_button = buttonBox->button(QDialogButtonBox::Ok);
    ok_button->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    vlayout->addWidget(buttonBox);
}

int PlainTextEditorDialog::showDialog()
{
    m_editor->setFocus();
    return exec();
}

void PlainTextEditorDialog::setDefaultFont(const QFont &font)
{
    m_editor->setFont(font);
}

void PlainTextEditorDialog::setText(const QString &text)
{
    m_editor->setPlainText(text);
}

QString PlainTextEditorDialog::text() const
{
    return m_editor->toPlainText();
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
