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

#include "multipagewidget.h"

MultiPageWidget::MultiPageWidget(QWidget *parent)
    : QWidget(parent)
{
    comboBox = new QComboBox();
    comboBox->setObjectName("__qt__passive_comboBox");
    stackWidget = new QStackedWidget();

    connect(comboBox, SIGNAL(activated(int)),
            this, SLOT(setCurrentIndex(int)));

    layout = new QVBoxLayout();
    layout->addWidget(comboBox);
    layout->addWidget(stackWidget);
    setLayout(layout);
}

QSize MultiPageWidget::sizeHint() const
{
    return QSize(200, 150);
}

void MultiPageWidget::addPage(QWidget *page)
{
    insertPage(count(), page);
}

void MultiPageWidget::removePage(int index)
{
    QWidget *widget = stackWidget->widget(index);
    stackWidget->removeWidget(widget);

    comboBox->removeItem(index);
}

int MultiPageWidget::count() const
{
    return stackWidget->count();
}

int MultiPageWidget::currentIndex() const
{
    return stackWidget->currentIndex();
}

void MultiPageWidget::insertPage(int index, QWidget *page)
{
    page->setParent(stackWidget);

    stackWidget->insertWidget(index, page);

    QString title = page->windowTitle();
    if (title.isEmpty()) {
        title = tr("Page %1").arg(comboBox->count() + 1);
        page->setWindowTitle(title);
    }
    comboBox->insertItem(index, title);
}

void MultiPageWidget::setCurrentIndex(int index)
{
    if (index != currentIndex()) {
        stackWidget->setCurrentIndex(index);
        comboBox->setCurrentIndex(index);
        emit currentIndexChanged(index);
    }
}

QWidget* MultiPageWidget::widget(int index)
{
    return stackWidget->widget(index);
}

QString MultiPageWidget::pageTitle() const
{
    if (const QWidget *currentWidget = stackWidget->currentWidget())
        return currentWidget->windowTitle();
    return QString();
}

void MultiPageWidget::setPageTitle(QString const &newTitle)
{
    comboBox->setItemText(currentIndex(), newTitle);
    if (QWidget *currentWidget = stackWidget->currentWidget())
        currentWidget->setWindowTitle(newTitle);
    emit pageTitleChanged(newTitle);
}
