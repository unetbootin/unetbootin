/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef COLORS_H
#define COLORS_H

#include <QtGui>
#include <QBrush>

class Colors
{
private:
    Colors(){};

public:
    static void parseArgs(int argc, char *argv[]);
    static void detectSystemResources();
    static void postConfigure();
    static void setLowSettings();

    // Colors:
    static QColor sceneBg1;
    static QColor sceneBg2;
    static QColor sceneBg1Line;
    static QColor paperBg;
    static QColor menuTextFg;
    static QColor buttonText;
    static QColor buttonBgLow;
    static QColor buttonBgHigh;
    static QColor tt_green;
    static QColor fadeOut;
    static QColor sceneLine;
    static QColor heading;
    static QString contentColor;
    static QString glVersion;

    // Guides:
    static int stageStartY;
    static int stageHeight;
    static int stageStartX;
    static int stageWidth;
    static int contentStartY;
    static int contentHeight;

    // properties:
    static bool openGlRendering;
    static bool direct3dRendering;
    static bool softwareRendering;
    static bool openGlAwailable;
    static bool direct3dAwailable;
    static bool xRenderPresent;
    static bool noAdapt;
    static bool noTicker;
    static bool noRescale;
    static bool noAnimations;
    static bool noBlending;
    static bool noScreenSync;
    static bool useLoop;
    static bool noWindowMask;
    static bool usePixmaps;
    static bool useEightBitPalette;
    static bool fullscreen;
    static bool showBoundingRect;
    static bool showFps;
    static bool noTimerUpdate;
    static bool noTickerMorph;
    static bool useButtonBalls;
    static bool adapted;
    static bool verbose;
    static bool pause;

    static float animSpeed;
    static float animSpeedButtons;
    static float benchmarkFps;
    static int tickerLetterCount;
    static int fps;
    static int menuCount;
    static float tickerMoveSpeed;
    static float tickerMorphSpeed;
    static QString tickerText;
    static QString rootMenuName;

    // fonts
    static QFont contentFont();
    static QFont headingFont();
    static QFont buttonFont();
    static QFont tickerFont();

};

#endif // COLORS_H

