/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QWINDOWSVISTASTYLE_P_H
#define QWINDOWSVISTASTYLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "qwindowsvistastyle.h"

#if !defined(QT_NO_STYLE_WINDOWSVISTA)
#include <private/qwindowsxpstyle_p.h>
#include <private/qpaintengine_raster_p.h>
#include <qlibrary.h>
#include <qpaintengine.h>
#include <qwidget.h>
#include <qapplication.h>
#include <qpixmapcache.h>
#include <qstyleoption.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qscrollbar.h>
#include <qprogressbar.h>
#include <qdockwidget.h>
#include <qtreeview.h>
#include <qtextedit.h>
#include <qmessagebox.h>
#include <qdialogbuttonbox.h>
#include <qinputdialog.h>
#include <qtreeview.h>
#include <qlistview.h>
#include <qbasictimer.h>
#include <qcommandlinkbutton.h>

QT_BEGIN_NAMESPACE

#if !defined(SCHEMA_VERIFY_VSSYM32)
#define TMT_ANIMATIONDURATION       5006
#define TMT_TRANSITIONDURATIONS     6000
#define EP_EDITBORDER_NOSCROLL      6
#define EP_EDITBORDER_HVSCROLL      9
#define EP_BACKGROUND               3
#define EBS_NORMAL                  1
#define EBS_HOT                     2
#define EBS_DISABLED                3
#define EBS_READONLY                5
#define PBS_DEFAULTED_ANIMATING     6
#define MBI_NORMAL                  1
#define MBI_HOT                     2
#define MBI_PUSHED                  3
#define MBI_DISABLED                4
#define MB_ACTIVE                   1
#define MB_INACTIVE                 2
#define PP_FILL                     5
#define PP_FILLVERT                 6
#define PP_MOVEOVERLAY              8
#define PP_MOVEOVERLAYVERT          10
#define MENU_BARBACKGROUND          7
#define MENU_BARITEM                8
#define MENU_POPUPCHECK             11
#define MENU_POPUPCHECKBACKGROUND   12
#define MENU_POPUPGUTTER            13
#define MENU_POPUPITEM              14
#define MENU_POPUPBORDERS           10
#define MENU_POPUPSEPARATOR         15
#define MC_CHECKMARKNORMAL          1
#define MC_CHECKMARKDISABLED        2
#define MC_BULLETNORMAL             3
#define MC_BULLETDISABLED           4
#define ABS_UPHOVER                 17
#define ABS_DOWNHOVER               18
#define ABS_LEFTHOVER               19
#define ABS_RIGHTHOVER              20
#define CP_DROPDOWNBUTTONRIGHT      6
#define CP_DROPDOWNBUTTONLEFT       7
#define SCRBS_HOVER                 5
#define TVP_HOTGLYPH                4
#define SPI_GETCLIENTAREAANIMATION  0x1042
#define TDLG_PRIMARYPANEL           1
#define TDLG_SECONDARYPANEL         8
#endif

class Animation
{
public :
    Animation() : _running(true) { }
    virtual ~Animation() { }
    QWidget * widget() const { return _widget; }
    bool running() const { return _running; }
    const QTime &startTime() const { return _startTime; }
    void setRunning(bool val) { _running = val; }
    void setWidget(QWidget *widget) { _widget = widget; }
    void setStartTime(const QTime &startTime) { _startTime = startTime; }
    virtual void paint(QPainter *painter, const QStyleOption *option);

protected:
    void drawBlendedImage(QPainter *painter, QRect rect, float value);
    QTime _startTime;
    QPointer<QWidget> _widget;
    QImage _primaryImage;
    QImage _secondaryImage;
    QImage _tempImage;
    bool _running;
};


// Handles state transition animations
class Transition : public Animation
{
public :
    Transition() : Animation() {}
    virtual ~Transition() { }
    void setDuration(int duration) { _duration = duration; }
    void setStartImage(const QImage &image) { _primaryImage = image; }
    void setEndImage(const QImage &image) { _secondaryImage = image; }
    virtual void paint(QPainter *painter, const QStyleOption *option);
    int duration() const { return _duration; }
    int _duration; //set time in ms to complete a state transition
};


// Handles pulse animations (default buttons)
class Pulse: public Animation
{
public :
    Pulse() : Animation() {}
    virtual ~Pulse() { }
    void setDuration(int duration) { _duration = duration; }
    void setPrimaryImage(const QImage &image) { _primaryImage = image; }
    void setAlternateImage(const QImage &image) { _secondaryImage = image; }
    virtual void paint(QPainter *painter, const QStyleOption *option);
    int duration() const { return _duration; }
    int _duration; //time in ms to complete a pulse cycle
};


class QWindowsVistaStylePrivate :  public QWindowsXPStylePrivate
{
    Q_DECLARE_PUBLIC(QWindowsVistaStyle)

public:
    QWindowsVistaStylePrivate();
    ~QWindowsVistaStylePrivate();
    static bool resolveSymbols();
    static inline bool useVista();
    void startAnimation(Animation *);
    void stopAnimation(const QWidget *);
    Animation* widgetAnimation(const QWidget *) const;
    void timerEvent();
    bool transitionsEnabled() const;
    QWidget *treeViewHelper();

private:
    QList <Animation*> animations;
    QBasicTimer animationTimer;
    QWidget *m_treeViewHelper;
};

QT_END_NAMESPACE

#endif // QT_NO_STYLE_WINDOWSVISTA

#endif // QWINDOWSVISTASTYLE_P_H
