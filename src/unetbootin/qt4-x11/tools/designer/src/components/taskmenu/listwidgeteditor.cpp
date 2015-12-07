/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

/*
TRANSLATOR qdesigner_internal::ListWidgetEditor
*/

#include "listwidgeteditor.h"
#include <designerpropertymanager.h>
#include <abstractformbuilder.h>

#include <QtDesigner/private/abstractsettings_p.h>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QDialogButtonBox>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

ListWidgetEditor::ListWidgetEditor(QDesignerFormWindowInterface *form,
                                   QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

    m_itemsEditor = new ItemListEditor(form, 0);
    m_itemsEditor->layout()->setMargin(0);
    m_itemsEditor->setNewItemText(tr("New Item"));

    QFrame *sep = new QFrame;
    sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QBoxLayout *box = new QVBoxLayout(this);
    box->addWidget(m_itemsEditor);
    box->addWidget(sep);
    box->addWidget(buttonBox);

    // Numbers copied from itemlisteditor.ui
    // (Automatic resizing doesn't work because ui has parent).
    resize(550, 360);
}

static AbstractItemEditor::PropertyDefinition listBoxPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { Qt::ToolTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "toolTip" },
    { Qt::StatusTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "statusTip" },
    { Qt::WhatsThisPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "whatsThis" },
    { Qt::FontRole, QVariant::Font, 0, "font" },
    { Qt::TextAlignmentRole, 0, DesignerPropertyManager::designerAlignmentTypeId, "textAlignment" },
    { Qt::BackgroundRole, QVariant::Brush, 0, "background" },
    { Qt::ForegroundRole, QVariant::Brush, 0, "foreground" },
    { ItemFlagsShadowRole, 0, QtVariantPropertyManager::flagTypeId, "flags" },
    { Qt::CheckStateRole, 0, QtVariantPropertyManager::enumTypeId, "checkState" },
    { 0, 0, 0, 0 }
};

ListContents ListWidgetEditor::fillContentsFromListWidget(QListWidget *listWidget)
{
    setWindowTitle(tr("Edit List Widget"));

    ListContents retVal;
    retVal.createFromListWidget(listWidget, false);
    retVal.applyToListWidget(m_itemsEditor->listWidget(), m_itemsEditor->iconCache(), true);

    m_itemsEditor->setupEditor(listWidget, listBoxPropList);

    return retVal;
}

static AbstractItemEditor::PropertyDefinition comboBoxPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { 0, 0, 0, 0 }
};

ListContents ListWidgetEditor::fillContentsFromComboBox(QComboBox *comboBox)
{
    setWindowTitle(tr("Edit Combobox"));

    ListContents retVal;
    retVal.createFromComboBox(comboBox);
    retVal.applyToListWidget(m_itemsEditor->listWidget(), m_itemsEditor->iconCache(), true);

    m_itemsEditor->setupEditor(comboBox, comboBoxPropList);

    return retVal;
}

ListContents ListWidgetEditor::contents() const
{
    ListContents retVal;
    retVal.createFromListWidget(m_itemsEditor->listWidget(), true);
    return retVal;
}

QT_END_NAMESPACE
