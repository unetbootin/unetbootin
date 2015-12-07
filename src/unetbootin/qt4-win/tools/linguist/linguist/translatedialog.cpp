/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "translatedialog.h"

QT_BEGIN_NAMESPACE

TranslateDialog::TranslateDialog(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);
    connect(m_ui.findNxt, SIGNAL(clicked()), this, SLOT(emitFindNext()));
    connect(m_ui.translate, SIGNAL(clicked()), this, SLOT(emitTranslateAndFindNext()));
    connect(m_ui.translateAll, SIGNAL(clicked()), this, SLOT(emitTranslateAll()));
    connect(m_ui.ledFindWhat, SIGNAL(textChanged(const QString &)), this, SLOT(verifyText(const QString &)));

    m_ui.ledFindWhat->setFocus();

    // disable all buttons except 'cancel'
    verifyText(QLatin1String(""));
}

void TranslateDialog::verifyText(const QString &text)
{
    bool canFind = !text.isEmpty();
    m_ui.findNxt->setEnabled(canFind);
    m_ui.translate->setEnabled(canFind);
    m_ui.translateAll->setEnabled(canFind);
}

void TranslateDialog::emitFindNext()
{
    translateAndFindNext_helper(Skip);
}

void TranslateDialog::emitTranslateAndFindNext()
{
    translateAndFindNext_helper(Translate);
}

void TranslateDialog::emitTranslateAll()
{
    translateAndFindNext_helper(TranslateAll);
}

void TranslateDialog::translateAndFindNext_helper(int mode)
{
    bool markFinished = m_ui.ckMarkFinished->isChecked() ? true : false;
    int matchOption = m_ui.ckMatchCase->isChecked() ? MatchCase : 0;

    emit translateAndFindNext(m_ui.ledFindWhat->text(), m_ui.ledTranslateTo->text(), matchOption,
        mode, markFinished);
}

void TranslateDialog::setFindWhat(const QString &str)
{
    m_ui.ledFindWhat->setText(str);
}

QT_END_NAMESPACE
