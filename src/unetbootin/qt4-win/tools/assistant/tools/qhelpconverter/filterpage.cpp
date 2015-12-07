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

#include <QtGui/QMessageBox>
#include "filterpage.h"

QT_BEGIN_NAMESPACE

FilterPage::FilterPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Filter Settings"));
    setSubTitle(tr("Specify the filter attributes for the "
        "documentation. If filter attributes are used, "
        "also define a custom filter for it. Both, the "
        "filter attributes and the custom filters are "
        "optional."));

    m_ui.setupUi(this);
    m_ui.customFilterWidget->headerItem()->setText(0, tr("Filter Name"));
    m_ui.customFilterWidget->headerItem()->setText(1, tr("Filter Attributes"));
    m_ui.customFilterWidget->setRootIsDecorated(false);
    m_ui.removeButton->setDisabled(true);
    connect(m_ui.addButton, SIGNAL(clicked()),
        this, SLOT(addFilter()));
    connect(m_ui.removeButton, SIGNAL(clicked()),
        this, SLOT(removeFilter()));
}

bool FilterPage::validatePage()
{
    m_filterAttributes.clear();
    foreach (QString f, m_ui.filterLineEdit->text().split(QLatin1Char(','))) {
        if (!f.trimmed().isEmpty())
            m_filterAttributes.append(f.trimmed());
    }

    m_customFilters.clear();
    QSet<QString> names;
    QSet<QString> atts;
    QString str;
    CustomFilter customFilter;
    QTreeWidgetItem *item = 0;
    for (int i=0; i<m_ui.customFilterWidget->topLevelItemCount(); ++i) {
        item = m_ui.customFilterWidget->topLevelItem(i);
        str = item->text(0);
        if (str.isEmpty() || names.contains(str)) {
            QMessageBox::critical(this, tr("Custom Filters"),
                tr("The custom filter \'%1\' is defined multiple times.")
                .arg(str));
            return false;
        }
        names.insert(str);
        customFilter.name = str;

        str.clear();
        QStringList lst;
        foreach (QString s, item->text(1).split(QLatin1Char(','))) {
            QString st = s.trimmed();
            if (!st.isEmpty()) {
                str += QLatin1Char(',') + st;
                lst.append(st);
            }
        }
        if (atts.contains(str)) {
            QMessageBox::critical(this, tr("Custom Filters"),
                tr("The attributes for custom filter \'%1\' are defined multiple times.")
                .arg(customFilter.name));
            return false;
        }
        atts.insert(str);
        customFilter.filterAttributes = lst;
        m_customFilters.append(customFilter);
    }
    return true;
}

QStringList FilterPage::filterAttributes() const
{
    return m_filterAttributes;
}

QList<CustomFilter> FilterPage::customFilters() const
{
    return m_customFilters;
}

void FilterPage::addFilter()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui.customFilterWidget);
    item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
    item->setText(0, QLatin1String("unfiltered"));
    item->setText(1, QLatin1String(""));
    m_ui.customFilterWidget->editItem(item, 0);
    m_ui.removeButton->setDisabled(false);
}

void FilterPage::removeFilter()
{
    QModelIndex idx = m_ui.customFilterWidget->currentIndex();
    if (!idx.isValid())
        return;
    QTreeWidgetItem *item = m_ui.customFilterWidget->takeTopLevelItem(idx.row());
    delete item;
    if (!m_ui.customFilterWidget->topLevelItemCount())
        m_ui.removeButton->setDisabled(true);
}

QT_END_NAMESPACE
