/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <qdebug.h>
#include <QApplication>
#include <QLineEdit>
#include <QValidator>

#include "ui_validators.h"

class ValidatorWidget : public QWidget, public Ui::ValidatorsForm
{
    Q_OBJECT
public:
    ValidatorWidget(QWidget *parent = 0);

private slots:
    void updateValidator();
    void updateDoubleValidator();
    void _setLocale(const QLocale &l) { setLocale(l); updateValidator(); updateDoubleValidator(); }

private:
    QIntValidator *validator;
    QDoubleValidator *doubleValidator;
};

ValidatorWidget::ValidatorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(localeSelector, SIGNAL(localeSelected(QLocale)), this, SLOT(_setLocale(QLocale)));

    connect(minVal, SIGNAL(editingFinished()), this, SLOT(updateValidator()));
    connect(maxVal, SIGNAL(editingFinished()), this, SLOT(updateValidator()));
    connect(editor, SIGNAL(editingFinished()), ledWidget, SLOT(flash()));

    connect(doubleMaxVal, SIGNAL(editingFinished()), this, SLOT(updateDoubleValidator()));
    connect(doubleMinVal, SIGNAL(editingFinished()), this, SLOT(updateDoubleValidator()));
    connect(doubleDecimals, SIGNAL(valueChanged(int)), this, SLOT(updateDoubleValidator()));
    connect(doubleFormat, SIGNAL(activated(int)), this, SLOT(updateDoubleValidator()));
    connect(doubleEditor, SIGNAL(editingFinished()), doubleLedWidget, SLOT(flash()));

    validator = 0;
    doubleValidator = 0;
    updateValidator();
    updateDoubleValidator();
};

void ValidatorWidget::updateValidator()
{
    QIntValidator *v = new QIntValidator(minVal->value(), maxVal->value(), this);
    v->setLocale(locale());
    editor->setValidator(v);
    delete validator;
    validator = v;

    QString s = editor->text();
    int i = 0;
    if (validator->validate(s, i) == QValidator::Invalid) {
        editor->clear();
    } else {
        editor->setText(s);
    }
}

void ValidatorWidget::updateDoubleValidator()
{
    QDoubleValidator *v
        = new QDoubleValidator(doubleMinVal->value(), doubleMaxVal->value(),
                                doubleDecimals->value(), this);
    v->setNotation(static_cast<QDoubleValidator::Notation>(doubleFormat->currentIndex()));
    v->setLocale(locale());
    doubleEditor->setValidator(v);
    delete doubleValidator;
    doubleValidator = v;

    QString s = doubleEditor->text();
    int i = 0;
    if (doubleValidator->validate(s, i) == QValidator::Invalid) {
        doubleEditor->clear();
    } else {
        doubleEditor->setText(s);
    }
}

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(validators);

    QApplication app(argc, argv);

    ValidatorWidget w;
    w.show();

    return app.exec();
}

#include "main.moc"
