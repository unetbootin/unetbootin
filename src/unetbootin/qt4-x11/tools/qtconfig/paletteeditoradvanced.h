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

#ifndef PALETTEEDITORADVANCED_H
#define PALETTEEDITORADVANCED_H

#include "paletteeditoradvancedbase.h"

QT_BEGIN_NAMESPACE

class PaletteEditorAdvanced : public PaletteEditorAdvancedBase
{
    Q_OBJECT
public:
    PaletteEditorAdvanced( QWidget * parent=0, const char * name=0,
                           bool modal=false, Qt::WindowFlags f=0 );
    ~PaletteEditorAdvanced();

    static QPalette getPalette( bool *ok, const QPalette &pal, Qt::BackgroundMode mode = Qt::PaletteBackground,
                                QWidget* parent = 0, const char* name = 0 );

protected slots:
    void paletteSelected(int);

    void onCentral( int );
    void onEffect( int );

    void onChooseCentralColor();
    void onChooseEffectColor();

    void onToggleBuildEffects( bool );
    void onToggleBuildInactive( bool );
    void onToggleBuildDisabled( bool );

protected:
    void mapToActiveCentralRole( const QColor& );
    void mapToActiveEffectRole( const QColor& );
    void mapToActivePixmapRole( const QPixmap& );
    void mapToInactiveCentralRole( const QColor& );
    void mapToInactiveEffectRole( const QColor& );
    void mapToInactivePixmapRole( const QPixmap& );
    void mapToDisabledCentralRole( const QColor& );
    void mapToDisabledEffectRole( const QColor& );
    void mapToDisabledPixmapRole( const QPixmap& );


    void buildPalette();
    void buildActiveEffect();
    void buildInactive();
    void buildInactiveEffect();
    void buildDisabled();
    void buildDisabledEffect();

private:
    void setPreviewPalette( const QPalette& );
    void updateColorButtons();
    void setupBackgroundMode( Qt::BackgroundMode );

    QPalette pal() const;
    void setPal( const QPalette& );

    QColorGroup::ColorRole centralFromItem( int );
    QColorGroup::ColorRole effectFromItem( int );
    QPalette editPalette;
    QPalette previewPalette;

    int selectedPalette;
};

QT_END_NAMESPACE

#endif
