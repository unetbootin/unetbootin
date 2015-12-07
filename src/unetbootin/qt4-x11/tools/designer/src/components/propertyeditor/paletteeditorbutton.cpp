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
TRANSLATOR qdesigner_internal::PaletteEditorButton
*/

#include "paletteeditorbutton.h"
#include "paletteeditor.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

PaletteEditorButton::PaletteEditorButton(QDesignerFormEditorInterface *core, const QPalette &palette, QWidget *parent)
    : QToolButton(parent),
      m_palette(palette)
{
    m_core = core;
    setFocusPolicy(Qt::NoFocus);
    setText(tr("Change Palette"));
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    connect(this, SIGNAL(clicked()), this, SLOT(showPaletteEditor()));
}

PaletteEditorButton::~PaletteEditorButton()
{
}

void PaletteEditorButton::setPalette(const QPalette &palette)
{
    m_palette = palette;
}

void PaletteEditorButton::setSuperPalette(const QPalette &palette)
{
    m_superPalette = palette;
}

void PaletteEditorButton::showPaletteEditor()
{
    int result;
    QPalette p = QPalette();
    QPalette pal = PaletteEditor::getPalette(m_core, 0, m_palette, m_superPalette, &result);
    if (result == QDialog::Accepted) {
        m_palette = pal;
        emit paletteChanged(m_palette);
    }
}

QT_END_NAMESPACE
