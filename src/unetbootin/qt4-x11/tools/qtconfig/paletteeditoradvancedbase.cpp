/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "paletteeditoradvancedbase.h"
#include "colorbutton.h"

#include <QVariant>

QT_BEGIN_NAMESPACE

/*
 *  Constructs a PaletteEditorAdvancedBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
PaletteEditorAdvancedBase::PaletteEditorAdvancedBase(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, name, modal, fl)
{
    setupUi(this);


    // signals and slots connections
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(paletteCombo, SIGNAL(activated(int)), this, SLOT(paletteSelected(int)));
    connect(comboCentral, SIGNAL(activated(int)), this, SLOT(onCentral(int)));
    connect(buttonCentral, SIGNAL(clicked()), this, SLOT(onChooseCentralColor()));
    connect(buttonEffect, SIGNAL(clicked()), this, SLOT(onChooseEffectColor()));
    connect(comboEffect, SIGNAL(activated(int)), this, SLOT(onEffect(int)));
    connect(checkBuildEffect, SIGNAL(toggled(bool)), this, SLOT(onToggleBuildEffects(bool)));
    connect(checkBuildEffect, SIGNAL(toggled(bool)), comboEffect, SLOT(setDisabled(bool)));
    connect(checkBuildEffect, SIGNAL(toggled(bool)), buttonEffect, SLOT(setDisabled(bool)));
    connect(checkBuildInactive, SIGNAL(toggled(bool)), this, SLOT(onToggleBuildInactive(bool)));
    connect(checkBuildDisabled, SIGNAL(toggled(bool)), this, SLOT(onToggleBuildDisabled(bool)));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
PaletteEditorAdvancedBase::~PaletteEditorAdvancedBase()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PaletteEditorAdvancedBase::languageChange()
{
    retranslateUi(this);
}

void PaletteEditorAdvancedBase::init()
{
}

void PaletteEditorAdvancedBase::destroy()
{
}

void PaletteEditorAdvancedBase::onCentral(int)
{
    qWarning("PaletteEditorAdvancedBase::onCentral(int): Not implemented yet");
}

void PaletteEditorAdvancedBase::onChooseCentralColor()
{
    qWarning("PaletteEditorAdvancedBase::onChooseCentralColor(): Not implemented yet");
}

void PaletteEditorAdvancedBase::onChooseEffectColor()
{
    qWarning("PaletteEditorAdvancedBase::onChooseEffectColor(): Not implemented yet");
}

void PaletteEditorAdvancedBase::onEffect(int)
{
    qWarning("PaletteEditorAdvancedBase::onEffect(int): Not implemented yet");
}

void PaletteEditorAdvancedBase::onToggleBuildDisabled(bool)
{
    qWarning("PaletteEditorAdvancedBase::onToggleBuildDisabled(bool): Not implemented yet");
}

void PaletteEditorAdvancedBase::onToggleBuildEffects(bool)
{
    qWarning("PaletteEditorAdvancedBase::onToggleBuildEffects(bool): Not implemented yet");
}

void PaletteEditorAdvancedBase::onToggleBuildInactive(bool)
{
    qWarning("PaletteEditorAdvancedBase::onToggleBuildInactive(bool): Not implemented yet");
}

void PaletteEditorAdvancedBase::paletteSelected(int)
{
    qWarning("PaletteEditorAdvancedBase::paletteSelected(int): Not implemented yet");
}

QT_END_NAMESPACE
