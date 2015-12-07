/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QMap>
#include <QtCore/QUrl>

#include "topicchooser.h"

QT_BEGIN_NAMESPACE

TopicChooser::TopicChooser(QWidget *parent, const QString &keyword,
                         const QMap<QString, QUrl> &links)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.label->setText(tr("Choose a topic for <b>%1</b>:").arg(keyword));

    m_links = links;
    QMap<QString, QUrl>::const_iterator it = m_links.constBegin();
    for (; it != m_links.constEnd(); ++it)
        ui.listWidget->addItem(it.key());
    
    if (ui.listWidget->count() != 0)
        ui.listWidget->setCurrentRow(0);
    ui.listWidget->setFocus();

    connect(ui.buttonDisplay, SIGNAL(clicked()),
        this, SLOT(accept()));
    connect(ui.buttonCancel, SIGNAL(clicked()),
        this, SLOT(reject()));
    connect(ui.listWidget, SIGNAL(itemActivated(QListWidgetItem*)),
        this, SLOT(accept()));
}

QUrl TopicChooser::link() const
{
    QListWidgetItem *item = ui.listWidget->currentItem();
    if (!item)
        return QUrl();

    QString title = item->text();
    if (title.isEmpty() || !m_links.contains(title))
        return QUrl();

    return m_links.value(title);
}

QT_END_NAMESPACE
