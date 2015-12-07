/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#include <math.h>

#include "button.h"
#include "calculator.h"

//! [0]
Calculator::Calculator(QWidget *parent)
    : QDialog(parent)
{
    sumInMemory = 0.0;
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    waitingForOperand = true;
//! [0]

//! [1]
    display = new QLineEdit("0");
//! [1] //! [2]
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);
//! [2]

//! [3]
    QColor digitColor(150, 205, 205);
//! [3] //! [4]
    QColor backspaceColor(225, 185, 135);
    QColor memoryColor(100, 155, 155);
    QColor operatorColor(155, 175, 195);

    for (int i = 0; i < NumDigitButtons; ++i) {
	digitButtons[i] = createButton(QString::number(i), digitColor,
                                       SLOT(digitClicked()));
    }

    Button *pointButton = createButton(tr("."), digitColor, SLOT(pointClicked()));
    Button *changeSignButton = createButton(tr("\261"), digitColor, SLOT(changeSignClicked()));

    Button *backspaceButton = createButton(tr("Backspace"), backspaceColor,
                                   SLOT(backspaceClicked()));
    Button *clearButton = createButton(tr("Clear"), backspaceColor, SLOT(clear()));
    Button *clearAllButton = createButton(tr("Clear All"), backspaceColor.light(120),
                                  SLOT(clearAll()));

    Button *clearMemoryButton = createButton(tr("MC"), memoryColor,
                                     SLOT(clearMemory()));
    Button *readMemoryButton = createButton(tr("MR"), memoryColor, SLOT(readMemory()));
    Button *setMemoryButton = createButton(tr("MS"), memoryColor, SLOT(setMemory()));
    Button *addToMemoryButton = createButton(tr("M+"), memoryColor,
                                     SLOT(addToMemory()));

    Button *divisionButton = createButton(tr("\367"), operatorColor,
                                  SLOT(multiplicativeOperatorClicked()));
    Button *timesButton = createButton(tr("\327"), operatorColor,
                               SLOT(multiplicativeOperatorClicked()));
    Button *minusButton = createButton(tr("-"), operatorColor,
                               SLOT(additiveOperatorClicked()));
    Button *plusButton = createButton(tr("+"), operatorColor,
                              SLOT(additiveOperatorClicked()));

    Button *squareRootButton = createButton(tr("Sqrt"), operatorColor,
                                    SLOT(unaryOperatorClicked()));
    Button *powerButton = createButton(tr("x\262"), operatorColor,
                               SLOT(unaryOperatorClicked()));
    Button *reciprocalButton = createButton(tr("1/x"), operatorColor,
                                    SLOT(unaryOperatorClicked()));
    Button *equalButton = createButton(tr("="), operatorColor.light(120),
                               SLOT(equalClicked()));
//! [4]

//! [5]
    QGridLayout *mainLayout = new QGridLayout;
//! [5] //! [6]
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainLayout->addWidget(display, 0, 0, 1, 6);
    mainLayout->addWidget(backspaceButton, 1, 0, 1, 2);
    mainLayout->addWidget(clearButton, 1, 2, 1, 2);
    mainLayout->addWidget(clearAllButton, 1, 4, 1, 2);

    mainLayout->addWidget(clearMemoryButton, 2, 0);
    mainLayout->addWidget(readMemoryButton, 3, 0);
    mainLayout->addWidget(setMemoryButton, 4, 0);
    mainLayout->addWidget(addToMemoryButton, 5, 0);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 5, 1);
    mainLayout->addWidget(pointButton, 5, 2);
    mainLayout->addWidget(changeSignButton, 5, 3);

    mainLayout->addWidget(divisionButton, 2, 4);
    mainLayout->addWidget(timesButton, 3, 4);
    mainLayout->addWidget(minusButton, 4, 4);
    mainLayout->addWidget(plusButton, 5, 4);

    mainLayout->addWidget(squareRootButton, 2, 5);
    mainLayout->addWidget(powerButton, 3, 5);
    mainLayout->addWidget(reciprocalButton, 4, 5);
    mainLayout->addWidget(equalButton, 5, 5);
    setLayout(mainLayout);

    setWindowTitle(tr("Calculator"));
}
//! [6]

//! [7]
void Calculator::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if (display->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        display->clear();
	waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}
//! [7]

//! [8]
void Calculator::unaryOperatorClicked()
//! [8] //! [9]
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = sqrt(operand);
    } else if (clickedOperator == tr("x\262")) {
        result = pow(operand, 2.0);
    } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
	    abortOperation();
	    return;
        }
        result = 1.0 / operand;
    }
    display->setText(QString::number(result));
    waitingForOperand = true;
}
//! [9]

//! [10]
void Calculator::additiveOperatorClicked()
//! [10] //! [11]
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

//! [11] //! [12]
    if (!pendingMultiplicativeOperator.isEmpty()) {
//! [12] //! [13]
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
	    return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

//! [13] //! [14]
    if (!pendingAdditiveOperator.isEmpty()) {
//! [14] //! [15]
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
	    return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

//! [15] //! [16]
    pendingAdditiveOperator = clickedOperator;
//! [16] //! [17]
    waitingForOperand = true;
}
//! [17]

//! [18]
void Calculator::multiplicativeOperatorClicked()
//! [18] //! [19]
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
	    return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}
//! [19]

//! [20]
void Calculator::equalClicked()
//! [20] //! [21]
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
	    return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
	    return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}
//! [21]

//! [22]
void Calculator::pointClicked()
//! [22] //! [23]
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains("."))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}
//! [23]

//! [24]
void Calculator::changeSignClicked()
//! [24] //! [25]
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}
//! [25]

//! [26]
void Calculator::backspaceClicked()
//! [26] //! [27]
{
    if (waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    display->setText(text);
}
//! [27]

//! [28]
void Calculator::clear()
//! [28] //! [29]
{
    if (waitingForOperand)
        return;

    display->setText("0");
    waitingForOperand = true;
}
//! [29]

//! [30]
void Calculator::clearAll()
//! [30] //! [31]
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}
//! [31]

//! [32]
void Calculator::clearMemory()
//! [32] //! [33]
{
    sumInMemory = 0.0;
}

void Calculator::readMemory()
{
    display->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

void Calculator::setMemory()
{
    equalClicked();
    sumInMemory = display->text().toDouble();
}

void Calculator::addToMemory()
{
    equalClicked();
    sumInMemory += display->text().toDouble();
}

//! [33] //! [34]
Button *Calculator::createButton(const QString &text, const QColor &color,
//! [34] //! [35]
                                 const char *member)
{
    Button *button = new Button(text, color);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
//! [35]

//! [36]
void Calculator::abortOperation()
//! [36] //! [37]
{
    clearAll();
    display->setText(tr("####"));
}
//! [37]

//! [38]
bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
//! [38] //! [39]
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\327")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\367")) {
	if (rightOperand == 0.0)
	    return false;
	factorSoFar /= rightOperand;
    }
    return true;
}
//! [39]
