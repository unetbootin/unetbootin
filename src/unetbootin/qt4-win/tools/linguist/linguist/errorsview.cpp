/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
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

#include "errorsview.h"
#include "messagemodel.h"
#include <QList>
#include <QListView>
#include <QString>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE


ErrorsView::ErrorsView(QWidget *parent) :
    QWidget(parent)
{
    m_list = new QStandardItemModel(this);
    QListView *listView = new QListView(this);
    listView->setModel(m_list);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(listView);
}

void ErrorsView::clear()
{
    m_list->clear();
}

void ErrorsView::addError(const ErrorType type, const QString &arg)
{
    QString error;
    switch (type) {
      case SuperfluousAccelerator:
        addError(tr("Accelerator possibly superfluous in translation."));
        break;
      case MissingAccelerator:
        addError(tr("Accelerator possibly missing in translation."));
        break;
      case PunctuationDiffer:
        addError(tr("Translation does not end with the same punctuation as the source text."));
        break;
      case IgnoredPhrasebook:
        addError(tr("A phrase book suggestion for '%1' was ignored.").arg(arg));
        break;
      case PlaceMarkersDiffer:
        addError(tr("Translation does not refer to the same place markers as in the source text."));
        break;
      default:
        addError(tr("Unknown error"));
        break;
    }
}

QList<QString> ErrorsView::errorList()
{
    QList<QString> errors;
    for (int i = 0; i < m_list->rowCount(); i++) {
        QStandardItem *item = m_list->item(i);
        errors.append(item->text());
    }
    return errors;
}

void ErrorsView::addError(const QString &error)
{
    QStandardItem *item = new QStandardItem(QIcon(*MessageModel::pxDanger), error);
    item->setEditable(false);
    m_list->appendRow(QList<QStandardItem*>() << item);
}

QT_END_NAMESPACE
