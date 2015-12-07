/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "qplatformdefs.h"

#ifndef QT_NO_TITLEBAR

#include "qapplication.h"
#include "qcursor.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qimage.h"
#include "qpainter.h"
#include "qiodevice.h"
#include "qpixmap.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"
#include "qtooltip.h"
#include "qdebug.h"
#if defined(Q_WS_WIN)
#include "qt_windows.h"
#endif

#include "private/qapplication_p.h"
#include "private/q3titlebar_p.h"
#include "private/qwidget_p.h"

QT_BEGIN_NAMESPACE

class Q3TitleBarPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(Q3TitleBar)
public:
    Q3TitleBarPrivate()
        : toolTip(0), act(0), window(0), movable(1), pressed(0), autoraise(0), inevent(0)
    {
    }

    Qt::WindowFlags flags;
    QStyle::SubControl buttonDown;
    QPoint moveOffset;
    QToolTip *toolTip;
    bool act                    :1;
    QWidget* window;
    bool movable            :1;
    bool pressed            :1;
    bool autoraise          :1;
    bool inevent : 1;

    int titleBarState() const;
    QStyleOptionTitleBar getStyleOption() const;
    void readColors();
};

inline int Q3TitleBarPrivate::titleBarState() const
{
    uint state = window ? window->windowState() : static_cast<Qt::WindowStates>(Qt::WindowNoState);
    state |= uint(act ? QStyle::State_Active : QStyle::State_None);
    return (int)state;
}

QStyleOptionTitleBar Q3TitleBarPrivate::getStyleOption() const
{
    Q_Q(const Q3TitleBar);
    QStyleOptionTitleBar opt;
    opt.init(q);
    opt.text = q->windowTitle();
    //################
    QIcon icon = q->windowIcon();
    QSize s = icon.actualSize(QSize(64, 64));
    opt.icon = icon.pixmap(s);
    opt.subControls = QStyle::SC_All;
    opt.activeSubControls = QStyle::SC_None;
    opt.titleBarState = titleBarState();
    opt.titleBarFlags = flags;
    return opt;
}

Q3TitleBar::Q3TitleBar(QWidget *w, QWidget *parent, Qt::WindowFlags f)
    : QWidget(*new Q3TitleBarPrivate, parent, Qt::WStyle_Customize | Qt::WStyle_NoBorder)
{
    Q_D(Q3TitleBar);
    if (f == 0 && w)
        f = w->windowFlags();
    d->flags = f;
    d->window = w;
    d->buttonDown = QStyle::SC_None;
    d->act = 0;
    if (w) {
        if (w->minimumSize() == w->maximumSize())
            d->flags &= ~Qt::WindowMaximizeButtonHint;
        setWindowTitle(w->windowTitle());
    }

    d->readColors();
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    setMouseTracking(true);
    setAutoRaise(style()->styleHint(QStyle::SH_TitleBar_AutoRaise, 0, this));
}

void Q3TitleBar::setFakeWindowFlags(Qt::WindowFlags f)
{
    Q_D(Q3TitleBar);
    d->flags = f;
}

Qt::WindowFlags Q3TitleBar::fakeWindowFlags() const
{
    Q_D(const Q3TitleBar);
    return d->flags;
}

Q3TitleBar::~Q3TitleBar()
{
}

QStyleOptionTitleBar Q3TitleBar::getStyleOption() const
{
    return d_func()->getStyleOption();
}

#ifdef Q_WS_WIN
static inline QRgb colorref2qrgb(COLORREF col)
{
    return qRgb(GetRValue(col),GetGValue(col),GetBValue(col));
}
#endif

void Q3TitleBarPrivate::readColors()
{
    Q_Q(Q3TitleBar);
    QPalette pal = q->palette();

    bool colorsInitialized = false;

#ifdef Q_WS_WIN // ask system properties on windows
#ifndef SPI_GETGRADIENTCAPTIONS
#define SPI_GETGRADIENTCAPTIONS 0x1008
#endif
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION 27
#endif
#ifndef COLOR_GRADIENTINACTIVECAPTION
#define COLOR_GRADIENTINACTIVECAPTION 28
#endif
    if (QApplication::desktopSettingsAware()) {
        pal.setColor(QPalette::Active, QPalette::Highlight, colorref2qrgb(GetSysColor(COLOR_ACTIVECAPTION)));
        pal.setColor(QPalette::Inactive, QPalette::Highlight, colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTION)));
        pal.setColor(QPalette::Active, QPalette::HighlightedText, colorref2qrgb(GetSysColor(COLOR_CAPTIONTEXT)));
        pal.setColor(QPalette::Inactive, QPalette::HighlightedText, colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTIONTEXT)));
        if (QSysInfo::WindowsVersion != QSysInfo::WV_95 && QSysInfo::WindowsVersion != QSysInfo::WV_NT) {
            colorsInitialized = true;
            BOOL gradient;
            QT_WA({
                SystemParametersInfo(SPI_GETGRADIENTCAPTIONS, 0, &gradient, 0);
            } , {
                SystemParametersInfoA(SPI_GETGRADIENTCAPTIONS, 0, &gradient, 0);
            });
            if (gradient) {
                pal.setColor(QPalette::Active, QPalette::Base, colorref2qrgb(GetSysColor(COLOR_GRADIENTACTIVECAPTION)));
                pal.setColor(QPalette::Inactive, QPalette::Base, colorref2qrgb(GetSysColor(COLOR_GRADIENTINACTIVECAPTION)));
            } else {
                pal.setColor(QPalette::Active, QPalette::Base, pal.color(QPalette::Active, QPalette::Highlight));
                pal.setColor(QPalette::Inactive, QPalette::Base, pal.color(QPalette::Inactive, QPalette::Highlight));
            }
        }
    }
#endif // Q_WS_WIN
    if (!colorsInitialized) {
        pal.setColor(QPalette::Active, QPalette::Highlight,
                      pal.color(QPalette::Active, QPalette::Highlight));
        pal.setColor(QPalette::Active, QPalette::Base,
                      pal.color(QPalette::Active, QPalette::Highlight));
        pal.setColor(QPalette::Inactive, QPalette::Highlight,
                      pal.color(QPalette::Inactive, QPalette::Dark));
        pal.setColor(QPalette::Inactive, QPalette::Base,
                      pal.color(QPalette::Inactive, QPalette::Dark));
        pal.setColor(QPalette::Inactive, QPalette::HighlightedText,
                      pal.color(QPalette::Inactive, QPalette::Window));
    }

    q->setPalette(pal);
    q->setActive(act);
}

void Q3TitleBar::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::ModifiedChange)
        update();
    QWidget::changeEvent(ev);
}

void Q3TitleBar::mousePressEvent(QMouseEvent *e)
{
    Q_D(Q3TitleBar);
    if (!d->act)
        emit doActivate();
    if (e->button() == Qt::LeftButton) {
        d->pressed = true;
        QStyleOptionTitleBar opt = d->getStyleOption();
        QStyle::SubControl ctrl = style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt,
                                                                 e->pos(), this);
        switch (ctrl) {
        case QStyle::SC_TitleBarSysMenu:
            if (d->flags & Qt::WindowSystemMenuHint) {
                d->buttonDown = QStyle::SC_None;
                static QTime *t = 0;
                static Q3TitleBar *tc = 0;
                if (!t)
                    t = new QTime;
                if (tc != this || t->elapsed() > QApplication::doubleClickInterval()) {
                    emit showOperationMenu();
                    t->start();
                    tc = this;
                } else {
                    tc = 0;
                    emit doClose();
                    return;
                }
            }
            break;

        case QStyle::SC_TitleBarShadeButton:
        case QStyle::SC_TitleBarUnshadeButton:
            if (d->flags & Qt::WindowShadeButtonHint)
                d->buttonDown = ctrl;
            break;

        case QStyle::SC_TitleBarNormalButton:
            if (d->flags & Qt::WindowMinMaxButtonsHint)
                d->buttonDown = ctrl;
            break;

        case QStyle::SC_TitleBarMinButton:
            if (d->flags & Qt::WindowMinimizeButtonHint)
                d->buttonDown = ctrl;
            break;

        case QStyle::SC_TitleBarMaxButton:
            if (d->flags & Qt::WindowMaximizeButtonHint)
                d->buttonDown = ctrl;
            break;

        case QStyle::SC_TitleBarCloseButton:
            if (d->flags & Qt::WindowSystemMenuHint)
                d->buttonDown = ctrl;
            break;

        case QStyle::SC_TitleBarLabel:
            d->buttonDown = ctrl;
            d->moveOffset = mapToParent(e->pos());
            break;

        default:
            break;
        }
        repaint();
    } else {
        d->pressed = false;
    }
}

void Q3TitleBar::contextMenuEvent(QContextMenuEvent *e)
{
    Q_D(Q3TitleBar);
    QStyleOptionTitleBar opt = d->getStyleOption();
    QStyle::SubControl ctrl = style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt, e->pos(),
                                                             this);
    if(ctrl == QStyle::SC_TitleBarLabel || ctrl == QStyle::SC_TitleBarSysMenu) {
        e->accept();
        emit popupOperationMenu(e->globalPos());
    } else {
        e->ignore();
    }
}

void Q3TitleBar::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(Q3TitleBar);
    if (e->button() == Qt::LeftButton && d->pressed) {
        e->accept();
        QStyleOptionTitleBar opt = d->getStyleOption();
        QStyle::SubControl ctrl = style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt,
                                                                 e->pos(), this);
        d->pressed = false;
        if (ctrl == d->buttonDown) {
            d->buttonDown = QStyle::SC_None;
            repaint();
            switch(ctrl) {
            case QStyle::SC_TitleBarShadeButton:
            case QStyle::SC_TitleBarUnshadeButton:
                if(d->flags & Qt::WindowShadeButtonHint)
                    emit doShade();
                break;

            case QStyle::SC_TitleBarNormalButton:
                if(d->flags & Qt::WindowMaximizeButtonHint)
                    emit doNormal();
                break;

            case QStyle::SC_TitleBarMinButton:
                if(d->flags & Qt::WindowMinimizeButtonHint) {
                    if (d->window && d->window->isMinimized())
                        emit doNormal();
                    else
                        emit doMinimize();
                }
                break;

            case QStyle::SC_TitleBarMaxButton:
                if(d->flags & Qt::WindowMaximizeButtonHint) {
                    if(d->window && d->window->isMaximized())
                        emit doNormal();
                    else
                        emit doMaximize();
                }
                break;

            case QStyle::SC_TitleBarCloseButton:
                if(d->flags & Qt::WindowSystemMenuHint) {
                    d->buttonDown = QStyle::SC_None;
                    repaint();
                    emit doClose();
                    return;
                }
                break;

            default:
                break;
            }
        }
    } else {
        e->ignore();
    }
}

void Q3TitleBar::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(Q3TitleBar);
    e->accept();
    switch (d->buttonDown) {
    case QStyle::SC_None:
        if(autoRaise())
            repaint();
        break;
    case QStyle::SC_TitleBarSysMenu:
        break;
    case QStyle::SC_TitleBarShadeButton:
    case QStyle::SC_TitleBarUnshadeButton:
    case QStyle::SC_TitleBarNormalButton:
    case QStyle::SC_TitleBarMinButton:
    case QStyle::SC_TitleBarMaxButton:
    case QStyle::SC_TitleBarCloseButton:
        {
            QStyle::SubControl last_ctrl = d->buttonDown;
            QStyleOptionTitleBar opt = d->getStyleOption();
            d->buttonDown = style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt, e->pos(), this);
            if (d->buttonDown != last_ctrl)
                d->buttonDown = QStyle::SC_None;
            repaint();
            d->buttonDown = last_ctrl;
        }
        break;

    case QStyle::SC_TitleBarLabel:
        if (d->buttonDown == QStyle::SC_TitleBarLabel && d->movable && d->pressed) {
            if ((d->moveOffset - mapToParent(e->pos())).manhattanLength() >= 4) {
                QPoint p = mapFromGlobal(e->globalPos());

                QWidget *parent = d->window ? d->window->parentWidget() : 0;
                if(parent && parent->inherits("Q3WorkspaceChild")) {
                    QWidget *workspace = parent->parentWidget();
                    p = workspace->mapFromGlobal(e->globalPos());
                    if (!workspace->rect().contains(p)) {
                        if (p.x() < 0)
                            p.rx() = 0;
                        if (p.y() < 0)
                            p.ry() = 0;
                        if (p.x() > workspace->width())
                            p.rx() = workspace->width();
                        if (p.y() > workspace->height())
                            p.ry() = workspace->height();
                    }
                }

                QPoint pp = p - d->moveOffset;
                if (!parentWidget()->isMaximized())
                    parentWidget()->move(pp);
            }
        } else {
            QStyle::SubControl last_ctrl = d->buttonDown;
            d->buttonDown = QStyle::SC_None;
            if(d->buttonDown != last_ctrl)
                repaint();
        }
        break;
    default:
        break;
    }
}

void Q3TitleBar::resizeEvent(QResizeEvent *r)
{
    QWidget::resizeEvent(r);
    cutText();
}

bool Q3TitleBar::isTool() const
{
    return (d_func()->flags & Qt::WindowType_Mask) == Qt::Tool;
}

void Q3TitleBar::paintEvent(QPaintEvent *)
{
    Q_D(Q3TitleBar);
    QStyleOptionTitleBar opt = d->getStyleOption();
    opt.subControls = QStyle::SC_TitleBarLabel;
    opt.activeSubControls = d->buttonDown;
    if (d->flags & Qt::WindowSystemMenuHint) {
        opt.subControls |= QStyle::SC_TitleBarSysMenu | QStyle::SC_TitleBarCloseButton;
        if (d->window && (d->flags & Qt::WindowShadeButtonHint)) {
            if (d->window->isMinimized())
                opt.subControls |= QStyle::SC_TitleBarUnshadeButton;
            else
                opt.subControls |= QStyle::SC_TitleBarShadeButton;
        }
        if (d->window && (d->flags & Qt::WindowMinMaxButtonsHint)) {
            if(d->window && d->window->isMinimized())
                opt.subControls |= QStyle::SC_TitleBarNormalButton;
            else
                opt.subControls |= QStyle::SC_TitleBarMinButton;
        }
        if (d->window && (d->flags & Qt::WindowMaximizeButtonHint) && !d->window->isMaximized())
            opt.subControls |= QStyle::SC_TitleBarMaxButton;
    }
    QStyle::SubControl under_mouse = QStyle::SC_None;
    
    if (underMouse()) {
        under_mouse = style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt,
                                                     mapFromGlobal(QCursor::pos()), this);
        opt.activeSubControls |= under_mouse;
        if (d->pressed)
            opt.state |= QStyle::State_Sunken;
        else if(autoRaise())
            opt.state |= QStyle::State_MouseOver;
    }
    
    opt.palette.setCurrentColorGroup(usesActiveColor() ? QPalette::Active : QPalette::Inactive);

    QPainter p(this);
    if (!windowTitle().isEmpty())
        opt.titleBarFlags |= Qt::WindowTitleHint;
    style()->drawComplexControl(QStyle::CC_TitleBar, &opt, &p, this);
}

void Q3TitleBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(Q3TitleBar);
    if (e->button() != Qt::LeftButton) {
        e->ignore();
        return;
    }
    e->accept();
    QStyleOptionTitleBar opt = d->getStyleOption();
    switch (style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt, e->pos(), this)) {
    case QStyle::SC_TitleBarLabel:
        emit doubleClicked();
        break;

    case QStyle::SC_TitleBarSysMenu:
        if (d->flags & Qt::WStyle_SysMenu)
            emit doClose();
        break;

    default:
        break;
    }
}

void Q3TitleBar::cutText()
{
    Q_D(Q3TitleBar);
    QFontMetrics fm(font());
    QStyleOptionTitleBar opt = d->getStyleOption();
    int maxw = style()->subControlRect(QStyle::CC_TitleBar, &opt, QStyle::SC_TitleBarLabel,
                                       this).width();
    if (!d->window)
        return;

    QString txt = d->window->windowTitle();
    if (style()->styleHint(QStyle::SH_TitleBar_ModifyNotification, 0, this) && d->window
        && d->window->isWindowModified())
        txt += QLatin1String(" *");

    QString cuttext = txt;
    if (fm.width(txt + QLatin1Char('m')) > maxw) {
        int i = txt.length();
        int dotlength = fm.width(QLatin1String("..."));
        while (i>0 && fm.width(txt.left(i)) + dotlength > maxw)
            i--;
        if(i != (int)txt.length())
            cuttext = txt.left(i) + QLatin1String("...");
    }

    setWindowTitle(cuttext);
}


void Q3TitleBar::leaveEvent(QEvent *)
{
    if(autoRaise() && !d_func()->pressed)
        repaint();
}

void Q3TitleBar::enterEvent(QEvent *)
{
    if(autoRaise() && !d_func()->pressed)
        repaint();
    QEvent e(QEvent::Leave);
    QApplication::sendEvent(parentWidget(), &e);
}

void Q3TitleBar::setActive(bool active)
{
    Q_D(Q3TitleBar);
    if (d->act == active)
        return ;

    d->act = active;
    update();
}

bool Q3TitleBar::isActive() const
{
    return d_func()->act;
}

bool Q3TitleBar::usesActiveColor() const
{
    return (isActive() && isActiveWindow()) ||
        (!window() && QWidget::window()->isActiveWindow());
}

QWidget *Q3TitleBar::window() const
{
    return d_func()->window;
}

bool Q3TitleBar::event(QEvent *e)
{
    Q_D(Q3TitleBar);
    if (d->inevent)
        return QWidget::event(e);
    d->inevent = true;
    if (e->type() == QEvent::ApplicationPaletteChange) {
        d->readColors();
        return true;
    } else if (e->type() == QEvent::WindowActivate) {
        setActive(d->act);
    } else if (e->type() == QEvent::WindowDeactivate) {
        bool wasActive = d->act;
        setActive(false);
        d->act = wasActive;
    } else if (e->type() == QEvent::WindowIconChange) {
        update();
    } else if (e->type() == QEvent::WindowTitleChange) {
        cutText();
        update();
    }

    d->inevent = false;
    return QWidget::event(e);
}

void Q3TitleBar::setMovable(bool b)
{
    d_func()->movable = b;
}

bool Q3TitleBar::isMovable() const
{
    return d_func()->movable;
}

void Q3TitleBar::setAutoRaise(bool b)
{
    d_func()->autoraise = b;
}

bool Q3TitleBar::autoRaise() const
{
    return d_func()->autoraise;
}

QSize Q3TitleBar::sizeHint() const
{
    ensurePolished();
    QStyleOptionTitleBar opt = d_func()->getStyleOption();
    QRect menur = style()->subControlRect(QStyle::CC_TitleBar, &opt,
                                          QStyle::SC_TitleBarSysMenu, this);
    return QSize(menur.width(), style()->pixelMetric(QStyle::PM_TitleBarHeight, &opt, this));
}

QT_END_NAMESPACE

#endif //QT_NO_TITLEBAR
