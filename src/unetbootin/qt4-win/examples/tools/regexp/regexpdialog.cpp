/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtGui>

#include "regexpdialog.h"

RegExpDialog::RegExpDialog(QWidget *parent)
    : QDialog(parent)
{
    patternComboBox = new QComboBox;
    patternComboBox->setEditable(true);
    patternComboBox->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Preferred);

    patternLabel = new QLabel(tr("&Pattern:"));
    patternLabel->setBuddy(patternComboBox);

    escapedPatternLineEdit = new QLineEdit;
    escapedPatternLineEdit->setReadOnly(true);
    QPalette palette = escapedPatternLineEdit->palette();
    palette.setBrush(QPalette::Base,
                     palette.brush(QPalette::Disabled, QPalette::Base));
    escapedPatternLineEdit->setPalette(palette);

    escapedPatternLabel = new QLabel(tr("&Escaped Pattern:"));
    escapedPatternLabel->setBuddy(escapedPatternLineEdit);

    syntaxComboBox = new QComboBox;
    syntaxComboBox->addItem(tr("Regular expression v1"), QRegExp::RegExp);
    syntaxComboBox->addItem(tr("Regular expression v2"), QRegExp::RegExp2);
    syntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    syntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);

    syntaxLabel = new QLabel(tr("&Pattern Syntax:"));
    syntaxLabel->setBuddy(syntaxComboBox);

    textComboBox = new QComboBox;
    textComboBox->setEditable(true);
    textComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    textLabel = new QLabel(tr("&Text:"));
    textLabel->setBuddy(textComboBox);

    caseSensitiveCheckBox = new QCheckBox(tr("Case &Sensitive"));
    caseSensitiveCheckBox->setChecked(true);
    minimalCheckBox = new QCheckBox(tr("&Minimal"));

    indexLabel = new QLabel(tr("Index of Match:"));
    indexEdit = new QLineEdit;
    indexEdit->setReadOnly(true);

    matchedLengthLabel = new QLabel(tr("Matched Length:"));
    matchedLengthEdit = new QLineEdit;
    matchedLengthEdit->setReadOnly(true);

    for (int i = 0; i < MaxCaptures; ++i) {
        captureLabels[i] = new QLabel(tr("Capture %1:").arg(i));
        captureEdits[i] = new QLineEdit;
        captureEdits[i]->setReadOnly(true);
    }
    captureLabels[0]->setText(tr("Match:"));

    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addWidget(caseSensitiveCheckBox);
    checkBoxLayout->addWidget(minimalCheckBox);
    checkBoxLayout->addStretch(1);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(patternLabel, 0, 0);
    mainLayout->addWidget(patternComboBox, 0, 1);
    mainLayout->addWidget(escapedPatternLabel, 1, 0);
    mainLayout->addWidget(escapedPatternLineEdit, 1, 1);
    mainLayout->addWidget(syntaxLabel, 2, 0);
    mainLayout->addWidget(syntaxComboBox, 2, 1);
    mainLayout->addLayout(checkBoxLayout, 3, 0, 1, 2);
    mainLayout->addWidget(textLabel, 4, 0);
    mainLayout->addWidget(textComboBox, 4, 1);
    mainLayout->addWidget(indexLabel, 5, 0);
    mainLayout->addWidget(indexEdit, 5, 1);
    mainLayout->addWidget(matchedLengthLabel, 6, 0);
    mainLayout->addWidget(matchedLengthEdit, 6, 1);

    for (int j = 0; j < MaxCaptures; ++j) {
        mainLayout->addWidget(captureLabels[j], 7 + j, 0);
        mainLayout->addWidget(captureEdits[j], 7 + j, 1);
    }
    setLayout(mainLayout);

    connect(patternComboBox, SIGNAL(editTextChanged(const QString &)),
            this, SLOT(refresh()));
    connect(textComboBox, SIGNAL(editTextChanged(const QString &)),
            this, SLOT(refresh()));
    connect(caseSensitiveCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(refresh()));
    connect(minimalCheckBox, SIGNAL(toggled(bool)), this, SLOT(refresh()));
    connect(syntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(refresh()));

    patternComboBox->addItem(tr("[A-Za-z_]+([A-Za-z_0-9]*)"));
    textComboBox->addItem(tr("(10 + delta4) * 32"));

    setWindowTitle(tr("RegExp"));
    setFixedHeight(sizeHint().height());
    refresh();
}

void RegExpDialog::refresh()
{
    setUpdatesEnabled(false);

    QString pattern = patternComboBox->currentText();
    QString text = textComboBox->currentText();

    QString escaped = pattern;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.prepend("\"");
    escaped.append("\"");
    escapedPatternLineEdit->setText(escaped);

    QRegExp rx(pattern);
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (caseSensitiveCheckBox->isChecked())
        cs = Qt::CaseSensitive;
    rx.setCaseSensitivity(cs);
    rx.setMinimal(minimalCheckBox->isChecked());
    QRegExp::PatternSyntax syntax = QRegExp::PatternSyntax(
            syntaxComboBox->itemData(syntaxComboBox->currentIndex()).toInt());
    rx.setPatternSyntax(syntax);

    QPalette palette = patternComboBox->palette();
    if (rx.isValid()) {
        palette.setColor(QPalette::Text,
                         textComboBox->palette().color(QPalette::Text));
    } else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    patternComboBox->setPalette(palette);

    indexEdit->setText(QString::number(rx.indexIn(text)));
    matchedLengthEdit->setText(QString::number(rx.matchedLength()));
    for (int i = 0; i < MaxCaptures; ++i) {
        captureLabels[i]->setEnabled(i <= rx.numCaptures());
        captureEdits[i]->setEnabled(i <= rx.numCaptures());
        captureEdits[i]->setText(rx.cap(i));
    }

    setUpdatesEnabled(true);
}
