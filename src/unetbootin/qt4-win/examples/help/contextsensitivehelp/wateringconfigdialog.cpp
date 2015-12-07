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

#include "wateringconfigdialog.h"

WateringConfigDialog::WateringConfigDialog()
{
    m_ui.setupUi(this);
    m_widgetInfo.insert(m_ui.plantComboBox, tr("plants"));
    m_widgetInfo.insert(m_ui.temperatureCheckBox, tr("temperature"));
    m_widgetInfo.insert(m_ui.temperatureSpinBox, tr("temperature"));
    m_widgetInfo.insert(m_ui.rainCheckBox, tr("rain"));
    m_widgetInfo.insert(m_ui.rainSpinBox, tr("rain"));
    m_widgetInfo.insert(m_ui.startTimeEdit, tr("starting time"));
    m_widgetInfo.insert(m_ui.amountSpinBox, tr("water amount"));
    m_widgetInfo.insert(m_ui.sourceComboBox, tr("water source"));
    m_widgetInfo.insert(m_ui.filterCheckBox, tr("water filtering"));

    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
        this, SLOT(focusChanged(QWidget*, QWidget*)));
}

void WateringConfigDialog::focusChanged(QWidget *, QWidget *now)
{
    if (m_widgetInfo.contains(now)) {
        m_ui.helpLabel->setText(tr("Information about %1:").arg(m_widgetInfo.value(now)));
        QStringList lst = m_widgetInfo.value(now).split(QLatin1Char(' '));
        m_ui.helpBrowser->showHelpForKeyword(lst.last());
    }
}

