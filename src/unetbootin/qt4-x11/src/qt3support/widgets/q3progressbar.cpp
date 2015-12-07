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

#include "q3progressbar.h"
#ifndef QT_NO_PROGRESSBAR
#include "qpainter.h"
#include "qdrawutil.h"
#include "qpixmap.h"
#include "qstyle.h"
#include "qstyleoption.h"
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif
#include "qevent.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

/*!
    \class Q3ProgressBar q3progressbar.h
    \brief The Q3ProgressBar widget provides a horizontal progress bar.

    \compat

    A progress bar is used to give the user an indication of the
    progress of an operation and to reassure them that the application
    is still running.

    The progress bar uses the concept of \e steps; you give it the
    total number of steps and the number of steps completed so far and
    it will display the percentage of steps that have been completed.
    You can specify the total number of steps in the constructor or
    later with setTotalSteps(). The current number of steps is set
    with setProgress(). The progress bar can be rewound to the
    beginning with reset().

    If the total is given as 0 the progress bar shows a busy indicator
    instead of a percentage of steps. This is useful, for example,
    when using QFtp or QHttp to download items when they are unable to
    determine the size of the item being downloaded.

    \sa QProgressDialog

    \inlineimage qprogbar-m.png Screenshot in Motif style
    \inlineimage qprogbar-w.png Screenshot in Windows style

    \sa QProgressDialog
*/


/*! \obsolete
    Constructs a progress bar.

    The total number of steps is set to 100 by default.

    The \a parent, \a name and widget flags, \a f, are passed on to
    the QFrame::QFrame() constructor.

    \sa setTotalSteps()
*/

Q3ProgressBar::Q3ProgressBar(QWidget *parent, const char *name, Qt::WindowFlags f)
    : QFrame(parent, f),
      total_steps(100),
      progress_val(-1),
      percentage(-1),
      center_indicator(true),
      percentage_visible(true),
      d(0)
{
    setObjectName(QLatin1String(name));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    initFrame();
}


/*! \obsolete
    Constructs a progress bar.

    The \a totalSteps is the total number of steps that need to be
    completed for the operation which this progress bar represents.
    For example, if the operation is to examine 50 files, this value
    would be 50. Before examining the first file, call setProgress(0);
    call setProgress(50) after examining the last file.

    The \a parent, \a name and widget flags, \a f, are passed to the
    QFrame::QFrame() constructor.

    \sa setTotalSteps(), setProgress()
*/

Q3ProgressBar::Q3ProgressBar(int totalSteps, QWidget *parent, const char *name, Qt::WindowFlags f)
    : QFrame(parent, f),
      total_steps(totalSteps),
      progress_val(-1),
      percentage(-1),
      center_indicator(true),
      percentage_visible(true),
      d(0)
{
    setObjectName(QLatin1String(name));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    initFrame();
}
#endif

/*!
    Constructs a progress bar.

    The total number of steps is set to 100 by default.

    The \a parent, and widget flags, \a f, are passed on to
    the QFrame::QFrame() constructor.

    \sa setTotalSteps()
*/

Q3ProgressBar::Q3ProgressBar(QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f),
      total_steps(100),
      progress_val(-1),
      percentage(-1),
      center_indicator(true),
      percentage_visible(true),
      d(0)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    initFrame();
}


/*!
    Constructs a progress bar.

    The \a totalSteps is the total number of steps that need to be
    completed for the operation which this progress bar represents.
    For example, if the operation is to examine 50 files, this value
    would be 50. Before examining the first file, call setProgress(0);
    call setProgress(50) after examining the last file.

    The \a parent, and widget flags, \a f, are passed to the
    QFrame::QFrame() constructor.

    \sa setTotalSteps(), setProgress()
*/

Q3ProgressBar::Q3ProgressBar(int totalSteps, QWidget *parent, Qt::WindowFlags f)
    : QFrame(parent, f),
      total_steps(totalSteps),
      progress_val(-1),
      percentage(-1),
      center_indicator(true),
      percentage_visible(true),
      d(0)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    initFrame();
}


/*!
    Reset the progress bar. The progress bar "rewinds" and shows no
    progress.
*/

void Q3ProgressBar::reset()
{
    progress_val = -1;
    percentage = -1;
    setIndicator(progress_str, progress_val, total_steps);
    repaint();
}


/*!
    \property Q3ProgressBar::totalSteps
    \brief The total number of steps.

    If totalSteps is 0, the progress bar will display a busy
    indicator.
*/

void Q3ProgressBar::setTotalSteps(int totalSteps)
{
    total_steps = totalSteps;

    // Current progress is invalid if larger than total
    if (total_steps < progress_val)
        progress_val = -1;

    if (isVisible() &&
         (setIndicator(progress_str, progress_val, total_steps) || !total_steps))
        repaint();
}


/*!
    \property Q3ProgressBar::progress
    \brief The current amount of progress

    This property is -1 if progress counting has not started.
*/

void Q3ProgressBar::setProgress(int progress)
{
    if (progress == progress_val ||
         progress < 0 || ((progress > total_steps) && total_steps))
        return;

    int old_progress_val = progress_val;
    progress_val = progress;

    if (setIndicator(progress_str, progress_val, total_steps)
        || ( total_steps == 0 || (width() * progress_val / total_steps != width() * old_progress_val / total_steps )))
        repaint();

#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(this, 0, QAccessible::ValueChanged);
#endif
}

/*!
    \overload

    Sets the amount of progress to \a progress and the total number of
    steps to \a totalSteps.

    \sa setTotalSteps()
*/

void Q3ProgressBar::setProgress(int progress, int totalSteps)
{
    if (total_steps != totalSteps)
        setTotalSteps(totalSteps);
    setProgress(progress);
}

/*!
  \property Q3ProgressBar::progressString
  \brief the amount of progress as a string

    This property is an empty string if progress counting has not started.
*/

static QStyleOptionProgressBar getStyleOption(const Q3ProgressBar *pb)
{
    QStyleOptionProgressBar opt;
    opt.init(pb);
    opt.rect = pb->contentsRect();
    opt.minimum = 0;
    opt.maximum = pb->totalSteps();
    opt.progress = pb->progress();
    if (pb->centerIndicator())
        opt.textAlignment = Qt::AlignCenter;
    else
        opt.textAlignment = Qt::AlignAuto;
    opt.textVisible = pb->percentageVisible();
    opt.text = pb->progressString();
    return opt;
}

/*!
    \reimp
*/
QSize Q3ProgressBar::sizeHint() const
{
    ensurePolished();
    QFontMetrics fm = fontMetrics();
    QStyleOptionProgressBar opt = getStyleOption(this);
    int cw = style()->pixelMetric(QStyle::PM_ProgressBarChunkWidth, &opt, this);
    return style()->sizeFromContents(QStyle::CT_ProgressBar, &opt,
                                    QSize(cw * 7 + fm.width(QLatin1Char('0')) * 4, fm.height() + 8), this);
}

/*!
    \reimp
*/
QSize Q3ProgressBar::minimumSizeHint() const
{
    return sizeHint();
}

/*!
    \property Q3ProgressBar::centerIndicator
    \brief whether the indicator string should be centered
*/

void Q3ProgressBar::setCenterIndicator(bool on)
{
    if (on == center_indicator)
        return;
    center_indicator = on;
    repaint();
}

/*!
    \property Q3ProgressBar::percentageVisible
    \brief whether the current progress value is displayed

    The default is true.

    \sa centerIndicator
*/
void Q3ProgressBar::setPercentageVisible(bool on)
{
    if (on == percentage_visible)
        return;
    percentage_visible = on;
    repaint();
}

/*!
    \reimp
*/
void Q3ProgressBar::setVisible(bool visible)
{
    if (visible)
        setIndicator(progress_str, progress_val, total_steps);
    QFrame::setVisible(visible);
}

void Q3ProgressBar::initFrame()
{
    setFrameStyle(QFrame::NoFrame);
}

/*!
    \reimp
*/
void Q3ProgressBar::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange)
        initFrame();
    QFrame::changeEvent(ev);
}


/*!
    This method is called to generate the text displayed in the center
    (or in some styles, to the left) of the progress bar.

    The \a progress may be negative, indicating that the progress bar
    is in the "reset" state before any progress is set.

    The default implementation is the percentage of completion or
    blank in the reset state. The percentage is calculated based on
    the \a progress and \a totalSteps. You can set the \a indicator
    text if you wish.

    To allow efficient repainting of the progress bar, this method
    should return false if the string is unchanged from the last call
    to this function.
*/

bool Q3ProgressBar::setIndicator(QString & indicator, int progress,
                                 int totalSteps)
{
    if (!totalSteps)
        return false;
    if (progress < 0) {
        indicator = QString::fromLatin1("");
        return true;
    } else {
        // Get the values down to something usable.
        if (totalSteps > INT_MAX/1000) {
            progress /= 1000;
            totalSteps /= 1000;
        }

        int np = progress * 100 / totalSteps;
        if (np != percentage) {
            percentage = np;
            indicator.sprintf("%d%%", np);
            return true;
        } else {
            return false;
        }
    }
}


/*!
    \reimp
*/
void Q3ProgressBar::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    QPainter *p = &paint;
    drawFrame(p);

    QStyleOptionProgressBar opt = getStyleOption(this);
    opt.rect = style()->subElementRect(QStyle::SE_ProgressBarGroove, &opt, this);

    style()->drawControl(QStyle::CE_ProgressBarGroove, &opt, p, this);
    opt.rect = contentsRect();
    opt.rect = style()->subElementRect(QStyle::SE_ProgressBarContents, &opt, this);
    style()->drawControl(QStyle::CE_ProgressBarContents, &opt, p, this);

    if (percentageVisible()) {
        opt.rect = contentsRect();
        opt.rect = style()->subElementRect(QStyle::SE_ProgressBarLabel, &opt, this);
        style()->drawControl(QStyle::CE_ProgressBarLabel, &opt, p, this);
    }
}

/*!
    \fn void Q3ProgressBar::setMargin(int margin)
    \since 4.2

    Sets the width of the margin around the contents of the widget to \a margin.
    
    This function uses QWidget::setContentsMargins() to set the margin.
    \sa margin(), QWidget::setContentsMargins()
*/

/*!
    \fn int Q3ProgressBar::margin() const
    \since 4.2

    Returns the with of the the margin around the contents of the widget.
    
    This function uses QWidget::getContentsMargins() to get the margin.
    \sa setMargin(), QWidget::getContentsMargins()
*/

QT_END_NAMESPACE
