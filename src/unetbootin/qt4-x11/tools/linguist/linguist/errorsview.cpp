/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "errorsview.h"

#include "messagemodel.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include <QtGui/QListView>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

ErrorsView::ErrorsView(MultiDataModel *dataModel, QWidget *parent) :
    QListView(parent),
    m_dataModel(dataModel)
{
    m_list = new QStandardItemModel(this);
    setModel(m_list);
}

void ErrorsView::clear()
{
    m_list->clear();
}

void ErrorsView::addError(int model, const ErrorType type, const QString &arg)
{
    QString error;
    switch (type) {
      case SuperfluousAccelerator:
        addError(model, tr("Accelerator possibly superfluous in translation."));
        break;
      case MissingAccelerator:
        addError(model, tr("Accelerator possibly missing in translation."));
        break;
      case PunctuationDiffer:
        addError(model, tr("Translation does not end with the same punctuation as the source text."));
        break;
      case IgnoredPhrasebook:
        addError(model, tr("A phrase book suggestion for '%1' was ignored.").arg(arg));
        break;
      case PlaceMarkersDiffer:
        addError(model, tr("Translation does not refer to the same place markers as in the source text."));
        break;
      case NumerusMarkerMissing:
        addError(model, tr("Translation does not contain the necessary %n place marker."));
        break;
      default:
        addError(model, tr("Unknown error"));
        break;
    }
}

QString ErrorsView::firstError()
{
    return (m_list->rowCount() == 0) ? QString() : m_list->item(0)->text();
}

void ErrorsView::addError(int model, const QString &error)
{
    // NOTE: Three statics instead of one just for GCC 3.3.5
    static QLatin1String imageLocation(":/images/s_check_danger.png");
    static QPixmap image(imageLocation);
    static QIcon pxDanger(image);
    QString lang;
    if (m_dataModel->modelCount() > 1)
        lang = m_dataModel->model(model)->localizedLanguage() + QLatin1String(": ");
    QStandardItem *item = new QStandardItem(pxDanger, lang + error);
    item->setEditable(false);
    m_list->appendRow(QList<QStandardItem*>() << item);
}

QT_END_NAMESPACE
