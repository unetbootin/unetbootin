/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include "gridpanel_p.h"
#include "ui_gridpanel.h"
#include "grid_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

GridPanel::GridPanel(QWidget *parentWidget) :
    QWidget(parentWidget)
{
    m_ui = new Ui::GridPanel;
    m_ui->setupUi(this);

    connect(m_ui->m_resetButton, SIGNAL(clicked()), this, SLOT(reset()));
}

GridPanel::~GridPanel()
{
    delete m_ui;
}

void GridPanel::setGrid(const Grid &g)
{
    m_ui->m_deltaXSpinBox->setValue(g.deltaX());
    m_ui->m_deltaYSpinBox->setValue(g.deltaY());
    m_ui->m_visibleCheckBox->setCheckState(g.visible() ? Qt::Checked : Qt::Unchecked);
    m_ui->m_snapXCheckBox->setCheckState(g.snapX()  ? Qt::Checked : Qt::Unchecked);
    m_ui->m_snapYCheckBox->setCheckState(g.snapY()  ? Qt::Checked : Qt::Unchecked);
}

void GridPanel::setTitle(const QString &title)
{
    m_ui->m_gridGroupBox->setTitle(title);
}

Grid GridPanel::grid() const
{
    Grid rc;
    rc.setDeltaX(m_ui->m_deltaXSpinBox->value());
    rc.setDeltaY(m_ui->m_deltaYSpinBox->value());
    rc.setSnapX(m_ui->m_snapXCheckBox->checkState() == Qt::Checked);
    rc.setSnapY(m_ui->m_snapYCheckBox->checkState() == Qt::Checked);
    rc.setVisible(m_ui->m_visibleCheckBox->checkState() == Qt::Checked);
    return rc;
}

void GridPanel::reset()
{
    setGrid(Grid());
}

void GridPanel::setCheckable (bool c)
{
    m_ui->m_gridGroupBox->setCheckable(c);
}

bool GridPanel::isCheckable () const
{
    return m_ui->m_gridGroupBox->isCheckable ();
}

bool GridPanel::isChecked () const
{
    return m_ui->m_gridGroupBox->isChecked ();
}

void GridPanel::setChecked(bool c)
{
    m_ui->m_gridGroupBox->setChecked(c);
}

void GridPanel::setResetButtonVisible(bool v)
{
    m_ui->m_resetButton->setVisible(v);
}

}

QT_END_NAMESPACE
