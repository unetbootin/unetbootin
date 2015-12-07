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

#include "paletteeditoradvanced.h"
#include "colorbutton.h"

#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QGroupBox>

QT_BEGIN_NAMESPACE

PaletteEditorAdvanced::PaletteEditorAdvanced( QWidget * parent,
                                              const char * name, bool modal, Qt::WindowFlags f )
    : PaletteEditorAdvancedBase( parent, name, modal, f ), selectedPalette(0)
{
    // work around buggy ui file
    comboEffect->setEnabled(false);
    buttonEffect->setEnabled(false);
    onToggleBuildEffects(true);

    editPalette = QApplication::palette();
    setPreviewPalette( editPalette );
}

PaletteEditorAdvanced::~PaletteEditorAdvanced()
{
}

void PaletteEditorAdvanced::onToggleBuildInactive( bool v )
{
    if (selectedPalette == 1) {
        groupCentral->setDisabled(v);
        groupEffect->setDisabled(v);
    }

    if (v) {
        buildInactive();
        updateColorButtons();
    }
}

void PaletteEditorAdvanced::onToggleBuildDisabled( bool v )
{
    if (selectedPalette == 2) {
        groupCentral->setDisabled(v);
        groupEffect->setDisabled(v);
    }

    if (v) {
        buildDisabled();
        updateColorButtons();
    }
}

void PaletteEditorAdvanced::paletteSelected(int p)
{
    selectedPalette = p;

    if(p == 1) { // inactive
        groupCentral->setDisabled(checkBuildInactive->isChecked());
        groupEffect->setDisabled(checkBuildInactive->isChecked());
    }
    else if (p == 2) { // disabled
        groupCentral->setDisabled(checkBuildDisabled->isChecked());
        groupEffect->setDisabled(checkBuildDisabled->isChecked());
    }
    else {
        groupCentral->setEnabled(true);
        groupEffect->setEnabled(true);
    }
    updateColorButtons();
}

void PaletteEditorAdvanced::onChooseCentralColor()
{
    switch(selectedPalette) {
    case 0:
    default:
        mapToActiveCentralRole( buttonCentral->color() );
        break;
    case 1:
        mapToInactiveCentralRole( buttonCentral->color() );
        break;
    case 2:
        mapToDisabledCentralRole( buttonCentral->color() );
        break;
    }
    updateColorButtons();
}

void PaletteEditorAdvanced::onChooseEffectColor()
{
    switch(selectedPalette) {
    case 0:
    default:
        mapToActiveEffectRole( buttonEffect->color() );
        break;
    case 1:
        mapToInactiveEffectRole( buttonEffect->color() );
        break;
    case 2:
        mapToDisabledEffectRole( buttonEffect->color() );
        break;
    }
    updateColorButtons();
}

void PaletteEditorAdvanced::onToggleBuildEffects( bool on )
{
    if (!on) return;
    buildActiveEffect();
    buildInactiveEffect();
    buildDisabledEffect();
}

QColorGroup::ColorRole PaletteEditorAdvanced::centralFromItem( int item )
{
    switch( item ) {
        case 0:
            return QColorGroup::Window;
        case 1:
            return QColorGroup::WindowText;
        case 2:
            return QColorGroup::Button;
        case 3:
            return QColorGroup::Base;
        case 4:
            return QColorGroup::Text;
        case 5:
            return QColorGroup::BrightText;
        case 6:
            return QColorGroup::ButtonText;
        case 7:
            return QColorGroup::Highlight;
        case 8:
            return QColorGroup::HighlightedText;
        default:
            return QColorGroup::NColorRoles;
    }
}

QColorGroup::ColorRole PaletteEditorAdvanced::effectFromItem( int item )
{
    switch( item ) {
    case 0:
        return QColorGroup::Light;
    case 1:
        return QColorGroup::Midlight;
    case 2:
        return QColorGroup::Mid;
    case 3:
        return QColorGroup::Dark;
    case 4:
        return QColorGroup::Shadow;
    default:
        return QColorGroup::NColorRoles;
    }
}

void PaletteEditorAdvanced::onCentral( int item )
{
    QColor c;

    switch(selectedPalette) {
    case 0:
    default:
        c = editPalette.active().color( centralFromItem(item) );
        break;
    case 1:
        c = editPalette.inactive().color( centralFromItem(item) );
        break;
    case 2:
        c =  editPalette.disabled().color( centralFromItem(item) );
        break;
    }

    buttonCentral->setColor(c);
}

void PaletteEditorAdvanced::onEffect( int item )
{
    QColor c;
    switch(selectedPalette) {
    case 0:
    default:
        c = editPalette.active().color( effectFromItem(item) );
        break;
    case 1:
        editPalette.inactive().color( effectFromItem(item) );
        break;
    case 2:
        editPalette.disabled().color( effectFromItem(item) );
        break;
    }
    buttonEffect->setColor(c);
}

void PaletteEditorAdvanced::mapToActiveCentralRole( const QColor& c )
{
    QColorGroup cg = editPalette.active();
    cg.setColor( centralFromItem(comboCentral->currentItem()), c );
    editPalette.setActive( cg );

    buildActiveEffect();
    if(checkBuildInactive->isChecked())
        buildInactive();
    if(checkBuildDisabled->isChecked())
        buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToActiveEffectRole( const QColor& c )
{
    QColorGroup cg = editPalette.active();
    cg.setColor( effectFromItem(comboEffect->currentItem()), c );
    editPalette.setActive( cg );

    if(checkBuildInactive->isChecked())
        buildInactive();
    if(checkBuildDisabled->isChecked())
        buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToActivePixmapRole( const QPixmap& pm )
{
    QColorGroup::ColorRole role = centralFromItem(comboCentral->currentItem());
    QColorGroup cg = editPalette.active();
    if (  !pm.isNull()  )
        cg.setBrush( role, QBrush( cg.color( role ), pm ) );
    else
        cg.setBrush( role, QBrush( cg.color( role ) ) );
    editPalette.setActive( cg );


    buildActiveEffect();
    if(checkBuildInactive->isChecked())
        buildInactive();
    if(checkBuildDisabled->isChecked())
        buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToInactiveCentralRole( const QColor& c )
{
    QColorGroup cg = editPalette.inactive();
    cg.setColor( centralFromItem(comboCentral->currentItem()), c );
    editPalette.setInactive( cg );

    buildInactiveEffect();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToInactiveEffectRole( const QColor& c )
{
    QColorGroup cg = editPalette.inactive();
    cg.setColor( effectFromItem(comboEffect->currentItem()), c );
    editPalette.setInactive( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToInactivePixmapRole( const QPixmap& pm )
{
    QColorGroup::ColorRole role = centralFromItem(comboCentral->currentItem());
    QColorGroup cg = editPalette.inactive();
    if (  !pm.isNull()  )
        cg.setBrush( role, QBrush( cg.color( role ), pm ) );
    else
        cg.setBrush( role, QBrush( cg.color( role ) ) );
    editPalette.setInactive( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToDisabledCentralRole( const QColor& c )
{
    QColorGroup cg = editPalette.disabled();
    cg.setColor( centralFromItem(comboCentral->currentItem()), c );
    editPalette.setDisabled( cg );

    buildDisabledEffect();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToDisabledEffectRole( const QColor& c )
{
    QColorGroup cg = editPalette.disabled();
    cg.setColor( effectFromItem(comboEffect->currentItem()), c );
    editPalette.setDisabled( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToDisabledPixmapRole( const QPixmap& pm )
{
    QColorGroup::ColorRole role = centralFromItem(comboCentral->currentItem());
    QColorGroup cg = editPalette.disabled();
    if (  !pm.isNull()  )
        cg.setBrush( role, QBrush( cg.color( role ), pm ) );
    else
        cg.setBrush( role, QBrush( cg.color( role ) ) );

    editPalette.setDisabled( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::buildActiveEffect()
{
    QColorGroup cg = editPalette.active();
    QColor btn = cg.color( QColorGroup::Button );

    QPalette temp( btn, btn );

    for (int i = 0; i<5; i++)
        cg.setColor( effectFromItem(i), temp.active().color( effectFromItem(i) ) );

    editPalette.setActive( cg );
    setPreviewPalette( editPalette );

    updateColorButtons();
}

void PaletteEditorAdvanced::buildInactive()
{
    editPalette.setInactive( editPalette.active() );
    if ( checkBuildEffect->isChecked() )
        buildInactiveEffect();
    else {
        setPreviewPalette( editPalette );
        updateColorButtons();
    }

}

void PaletteEditorAdvanced::buildInactiveEffect()
{
    QColorGroup cg = editPalette.inactive();

    QColor light, midlight, mid, dark, shadow;
    QColor btn = cg.color( QColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = Qt::black;

    cg.setColor( QColorGroup::Light, light );
    cg.setColor( QColorGroup::Midlight, midlight );
    cg.setColor( QColorGroup::Mid, mid );
    cg.setColor( QColorGroup::Dark, dark );
    cg.setColor( QColorGroup::Shadow, shadow );

    editPalette.setInactive( cg );
    setPreviewPalette( editPalette );
    updateColorButtons();
}

void PaletteEditorAdvanced::buildDisabled()
{
    QColorGroup cg = editPalette.active();
    cg.setColor( QColorGroup::ButtonText, Qt::darkGray );
    cg.setColor( QColorGroup::WindowText, Qt::darkGray );
    cg.setColor( QColorGroup::Text, Qt::darkGray );
    cg.setColor( QColorGroup::HighlightedText, Qt::darkGray );
    editPalette.setDisabled( cg );

    if ( checkBuildEffect->isChecked() )
        buildDisabledEffect();
    else {
        setPreviewPalette( editPalette );
        updateColorButtons();
    }
}

void PaletteEditorAdvanced::buildDisabledEffect()
{
    QColorGroup cg = editPalette.disabled();

    QColor light, midlight, mid, dark, shadow;
    QColor btn = cg.color( QColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = Qt::black;

    cg.setColor( QColorGroup::Light, light );
    cg.setColor( QColorGroup::Midlight, midlight );
    cg.setColor( QColorGroup::Mid, mid );
    cg.setColor( QColorGroup::Dark, dark );
    cg.setColor( QColorGroup::Shadow, shadow );

    editPalette.setDisabled( cg );
    setPreviewPalette( editPalette );
    updateColorButtons();
}

void PaletteEditorAdvanced::setPreviewPalette( const QPalette& pal )
{
    QColorGroup cg;

    switch (selectedPalette) {
    case 0:
    default:
        cg = pal.active();
        break;
    case 1:
        cg = pal.inactive();
        break;
    case 2:
        cg = pal.disabled();
        break;
    }
    previewPalette.setActive( cg );
    previewPalette.setInactive( cg );
    previewPalette.setDisabled( cg );
}

void PaletteEditorAdvanced::updateColorButtons()
{
    QColor central, effect;
    switch (selectedPalette) {
    case 0:
    default:
        central = editPalette.active().color( centralFromItem( comboCentral->currentItem() ) );
        effect = editPalette.active().color( effectFromItem( comboEffect->currentItem() ) );
        break;
    case 1:
        central = editPalette.inactive().color( centralFromItem( comboCentral->currentItem() ) );
        effect = editPalette.inactive().color( effectFromItem( comboEffect->currentItem() ) );
        break;
    case 2:
        central = editPalette.disabled().color( centralFromItem( comboCentral->currentItem() ) );
        effect = editPalette.disabled().color( effectFromItem( comboEffect->currentItem() ) );
        break;
    }

    buttonCentral->setColor(central);
    buttonEffect->setColor(effect);
}

void PaletteEditorAdvanced::setPal( const QPalette& pal )
{
    editPalette = pal;
    setPreviewPalette( pal );
    updateColorButtons();
}

QPalette PaletteEditorAdvanced::pal() const
{
    return editPalette;
}

void PaletteEditorAdvanced::setupBackgroundMode( Qt::BackgroundMode mode )
{
    int initRole = 0;

    switch( mode ) {
    case Qt::PaletteBackground:
        initRole = 0;
        break;
    case Qt::PaletteForeground:
        initRole = 1;
        break;
    case Qt::PaletteButton:
        initRole = 2;
        break;
    case Qt::PaletteBase:
        initRole = 3;
        break;
    case Qt::PaletteText:
        initRole = 4;
        break;
    case Qt::PaletteBrightText:
        initRole = 5;
        break;
    case Qt::PaletteButtonText:
        initRole = 6;
        break;
    case Qt::PaletteHighlight:
        initRole = 7;
        break;
    case Qt::PaletteHighlightedText:
        initRole = 8;
        break;
    case Qt::PaletteLight:
        initRole = 9;
        break;
    case Qt::PaletteMidlight:
        initRole = 10;
        break;
    case Qt::PaletteDark:
        initRole = 11;
        break;
    case Qt::PaletteMid:
        initRole = 12;
        break;
    case Qt::PaletteShadow:
        initRole = 13;
        break;
    default:
        initRole = -1;
        break;
    }

    if ( initRole <= -1 ) return;

    if (initRole > 8 ) {
        comboEffect->setCurrentItem( initRole - 9 );
    }
    else {
        comboCentral->setCurrentItem( initRole );
    }
}

QPalette PaletteEditorAdvanced::getPalette( bool *ok, const QPalette &init,
                                            Qt::BackgroundMode mode, QWidget* parent,
                                            const char* name )
{
    PaletteEditorAdvanced* dlg = new PaletteEditorAdvanced( parent, name, true );
    dlg->setupBackgroundMode( mode );

    if ( init != QPalette() )
        dlg->setPal( init );
    int resultCode = dlg->exec();

    QPalette result = init;
    if ( resultCode == QDialog::Accepted ) {
        if ( ok )
            *ok = true;
        result = dlg->pal();
    } else {
        if ( ok )
            *ok = false;
    }
    delete dlg;
    return result;
}

QT_END_NAMESPACE
