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

#include "q3progressdialog.h"

#ifndef QT_NO_PROGRESSDIALOG

#include "q3progressbar.h"
#include "qapplication.h"
#include "qcursor.h"
#include "qdatetime.h"
#include "qlabel.h"
#include "qpainter.h"
#include "qpushbutton.h"
#include "qshortcut.h"
#include "qstyle.h"
#include "qtimer.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

// If the operation is expected to take this long (as predicted by
// progress time), show the progress dialog.
static const int defaultShowTime    = 4000;
// Wait at least this long before attempting to make a prediction.
static const int minWaitTime = 50;

// Various layout values
static const int margin_lr   = 10;
static const int margin_tb   = 10;
static const int spacing     = 4;


class Q3ProgressDialogData
{
public:
    Q3ProgressDialogData(Q3ProgressDialog* that, QWidget* parent,
                   const QString& labelText,
                   int totalSteps) :
        creator(parent),
        label(new QLabel(labelText,that)),
        cancel(0),
        bar(new Q3ProgressBar(totalSteps, that)),
        shown_once(false),
        cancellation_flag(false),
        showTime(defaultShowTime)
    {
        int align = that->style()->styleHint(QStyle::SH_ProgressDialog_TextLabelAlignment, 0, that);
        label->setAlignment(Qt::Alignment(align));
    }

    QWidget         *creator;
    QLabel         *label;
    QPushButton         *cancel;
    Q3ProgressBar *bar;
    bool          shown_once;
    bool          cancellation_flag;
    QTime          starttime;
#ifndef QT_NO_CURSOR
    QCursor          parentCursor;
#endif
    int                  showTime;
    bool autoClose;
    bool autoReset;
    bool forceHide;
};


/*!
  \class Q3ProgressDialog qprogressdialog.h
  \brief The Q3ProgressDialog class provides feedback on the progress of a slow operation.

  \compat

  A progress dialog is used to give the user an indication of how long
  an operation is going to take, and to demonstrate that the
  application has not frozen. It can also give the user an opportunity
  to abort the operation.

  A common problem with progress dialogs is that it is difficult to know
  when to use them; operations take different amounts of time on different
  hardware.  Q3ProgressDialog offers a solution to this problem:
  it estimates the time the operation will take (based on time for
  steps), and only shows itself if that estimate is beyond minimumDuration()
  (4 seconds by default).

  Use setTotalSteps() (or the constructor) to set the number of
  "steps" in the operation and call setProgress() as the operation
  progresses. The step value can be chosen arbitrarily. It can be the
  number of files copied, the number of bytes received, the number of
  iterations through the main loop of your algorithm, or some other
  suitable unit.  Progress starts at 0, and the progress dialog shows
  that the operation has finished when you call setProgress() with
  totalSteps() as its argument.

  The dialog automatically resets and hides itself at the end of the
  operation. Use setAutoReset() and setAutoClose() to change this
  behavior.

  There are two ways of using Q3ProgressDialog: modal and modeless.

  Using a modal Q3ProgressDialog is simpler for the programmer, but you
  must call QApplication::processEvents() or
  QEventLoop::processEvents(ExcludeUserInput) to keep the event loop
  running to ensure that the application doesn't freeze. Do the
  operation in a loop, call \l setProgress() at intervals, and check
  for cancellation with wasCanceled(). For example:
\snippet doc/src/snippets/code/src_qt3support_dialogs_q3progressdialog.cpp 0

  A modeless progress dialog is suitable for operations that take
  place in the background, where the user is able to interact with the
  application. Such operations are typically based on QTimer (or
  QObject::timerEvent()), QSocketNotifier, or QUrlOperator; or performed
  in a separate thread. A Q3ProgressBar in the status bar of your main window
  is often an alternative to a modeless progress dialog.

  You need to have an event loop to be running, connect the
  canceled() signal to a slot that stops the operation, and call \l
  setProgress() at intervals. For example:
\snippet doc/src/snippets/code/src_qt3support_dialogs_q3progressdialog.cpp 1


  In both modes the progress dialog may be customized by
  replacing the child widgets with custom widgets by using setLabel(),
  setBar(), and setCancelButton().
  The functions setLabelText() and setCancelButtonText()
  set the texts shown.

  \inlineimage qprogdlg-m.png Screenshot in Motif style
  \inlineimage qprogdlg-w.png Screenshot in Windows style

  \sa QDialog, Q3ProgressBar, {fowler}{GUI Design Handbook: Progress Indicator}
*/


/*!
  Returns the QLabel currently being displayed above the progress bar.
  This QLabel is owned by the Q3ProgressDialog.

  \sa setLabel()
*/
QLabel *Q3ProgressDialog::label() const
{
    return d->label;
}

/*!
  Returns the Q3ProgressBar currently being used to display progress.
  This Q3ProgressBar is owned by the Q3ProgressDialog.

  \sa setBar()
*/
Q3ProgressBar *Q3ProgressDialog::bar() const
{
    return d->bar;
}


/*!
  Constructs a progress dialog.

  Default settings:
  \list
  \i The label text is empty.
  \i The cancel button text is (translated) "Cancel".
  \i The total number of steps is 100.
  \endlist

  The \a creator argument is the widget to use as the dialog's parent.
  The \a name, \a modal, and the widget flags, \a f, are
  passed to the QDialog::QDialog() constructor. If \a modal is false (the
  default), you must have an event loop proceeding for any redrawing
  of the dialog to occur. If \a modal is true, the dialog ensures that
  events are processed when needed.

  \sa setLabelText(), setLabel(), setCancelButtonText(), setCancelButton(),
  setTotalSteps()
*/

Q3ProgressDialog::Q3ProgressDialog(QWidget *creator, const char *name,
                                  bool modal, Qt::WindowFlags f)
    : QDialog(creator, f)
{
    setObjectName(QLatin1String(name));
    setModal(modal);
    init(creator, QString::fromLatin1(""), tr("Cancel"), 100);
}

/*!
  Constructs a progress dialog.

   The \a labelText is text used to remind the user what is progressing.

   The \a cancelButtonText is the text to display on the cancel button,
            or 0 if no cancel button is to be shown.

   The \a totalSteps is the total number of steps in the operation for
   which this progress dialog shows progress.  For example, if the
   operation is to examine 50 files, this value would be 50. Before
   examining the first file, call setProgress(0). As each file is
   processed call setProgress(1), setProgress(2), etc., finally
   calling setProgress(50) after examining the last file.

   The \a creator argument is the widget to use as the dialog's parent.
   The \a name, \a modal, and widget flags, \a f, are passed to the
   QDialog::QDialog() constructor. If \a modal is false (the default),
   you will must have an event loop proceeding for any redrawing of
   the dialog to occur. If \a modal is true, the dialog ensures that
   events are processed when needed.


  \sa setLabelText(), setLabel(), setCancelButtonText(), setCancelButton(),
  setTotalSteps()
*/

Q3ProgressDialog::Q3ProgressDialog(const QString &labelText,
                                  const QString &cancelButtonText,
                                  int totalSteps,
                                  QWidget *creator, const char *name,
                                  bool modal, Qt::WindowFlags f)
    : QDialog(creator, f)
{
    setObjectName(QLatin1String(name));
    setModal(modal);
    init(creator, labelText, cancelButtonText, totalSteps);
}

/*!
  Constructs a progress dialog.

  Default settings:
  \list
      \i The label text is empty.
      \i The cancel button text is (translated) "Cancel".
      \i The total number of steps is 100.
  \endlist

  The \a creator argument is the widget to use as the dialog's parent.
  The widget flags, \a f, are passed to the QDialog::QDialog() constructor.

  \sa setLabelText(), setLabel(), setCancelButtonText(), setCancelButton(),
  setTotalSteps()
*/
Q3ProgressDialog::Q3ProgressDialog(QWidget *creator, Qt::WindowFlags f)
    : QDialog(creator, f)
{
    init(creator, QString::fromLatin1(""), tr("Cancel"), 100);
}

/*!
  Constructs a progress dialog.

   The \a labelText is text used to remind the user what is progressing.

   The \a cancelButtonText is the text to display on the cancel button,
            or 0 if no cancel button is to be shown.

   The \a totalSteps is the total number of steps in the operation for
   which this progress dialog shows progress.  For example, if the
   operation is to examine 50 files, this value would be 50. Before
   examining the first file, call setProgress(0). As each file is
   processed call setProgress(1), setProgress(2), etc., finally
   calling setProgress(50) after examining the last file.

   The \a creator argument is the widget to use as the dialog's parent.
   The widget flags, \a f, are passed to the
   QDialog::QDialog() constructor.

  \sa setLabelText(), setLabel(), setCancelButtonText(), setCancelButton(),
  setTotalSteps()
*/
Q3ProgressDialog::Q3ProgressDialog(const QString &labelText,
                                  const QString &cancelButtonText,
                                  int totalSteps, QWidget *creator, Qt::WindowFlags f)
    : QDialog(creator, f)
{
    init(creator, labelText, cancelButtonText, totalSteps);
}

/*!
  Destroys the progress dialog.
*/

Q3ProgressDialog::~Q3ProgressDialog()
{
#ifndef QT_NO_CURSOR
    if (d->creator)
        d->creator->setCursor(d->parentCursor);
#endif
    delete d;
}

void Q3ProgressDialog::init(QWidget *creator,
                            const QString& lbl, const QString& canc,
                            int totstps)
{
    d = new Q3ProgressDialogData(this, creator, lbl, totstps);
    d->autoClose = true;
    d->autoReset = true;
    d->forceHide = false;
    setCancelButtonText(canc);
    connect(this, SIGNAL(canceled()), this, SIGNAL(cancelled()));
    connect(this, SIGNAL(canceled()), this, SLOT(cancel()));
    forceTimer = new QTimer(this);
    connect(forceTimer, SIGNAL(timeout()), this, SLOT(forceShow()));
    layout();
}

/*!
  \fn void Q3ProgressDialog::canceled()

  This signal is emitted when the cancel button is clicked.
  It is connected to the cancel() slot by default.

  \sa wasCanceled()
*/

/*!
    \fn void Q3ProgressDialog::cancelled()

    Use canceled() instead.
*/

/*!
  Sets the label to \a label. The progress dialog resizes to fit. The
  label becomes owned by the progress dialog and will be deleted when
  necessary, so do not pass the address of an object on the stack.

  \sa setLabelText()
*/

void Q3ProgressDialog::setLabel(QLabel *label)
{
    delete d->label;
    d->label = label;
    if (label) {
        if (label->parentWidget() == this) {
            label->hide(); // until we resize
        } else {
            label->setParent(this, 0);
        }
    }
    int w = qMax(isVisible() ? width() : 0, sizeHint().width());
    int h = qMax(isVisible() ? height() : 0, sizeHint().height());
    resize(w, h);
    if (label)
        label->show();
}


/*!
  \property Q3ProgressDialog::labelText
  \brief the label's text

  The default text is an empty string.
*/

QString Q3ProgressDialog::labelText() const
{
    if (label())
        return label()->text();
    return QString();
}

void Q3ProgressDialog::setLabelText(const QString &text)
{
    if (label()) {
        label()->setText(text);
        int w = qMax(isVisible() ? width() : 0, sizeHint().width());
        int h = qMax(isVisible() ? height() : 0, sizeHint().height());
        resize(w, h);
    }
}


/*!
  Sets the cancel button to the push button, \a cancelButton. The
  progress dialog takes ownership of this button which will be deleted
  when necessary, so do not pass the address of an object that is on
  the stack, i.e. use new() to create the button.

  \sa setCancelButtonText()
*/

void Q3ProgressDialog::setCancelButton(QPushButton *cancelButton)
{
    delete d->cancel;
    d->cancel = cancelButton;
    if (cancelButton) {
        if (cancelButton->parentWidget() == this) {
            cancelButton->hide(); // until we resize
        } else {
            cancelButton->setParent(this, 0);
        }
        connect(d->cancel, SIGNAL(clicked()), this, SIGNAL(canceled()));
        new QShortcut(Qt::Key_Escape, this, SIGNAL(canceled()));
    }
    int w = qMax(isVisible() ? width() : 0, sizeHint().width());
    int h = qMax(isVisible() ? height() : 0, sizeHint().height());
    resize(w, h);
    if (cancelButton)
        cancelButton->show();
}

/*!
  Sets the cancel button's text to \a cancelButtonText.
  \sa setCancelButton()
*/

void Q3ProgressDialog::setCancelButtonText(const QString &cancelButtonText)
{
    if (!cancelButtonText.isNull()) {
        if (d->cancel)
            d->cancel->setText(cancelButtonText);
        else
            setCancelButton(new QPushButton(cancelButtonText, this));
    } else {
        setCancelButton(0);
    }
    int w = qMax(isVisible() ? width() : 0, sizeHint().width());
    int h = qMax(isVisible() ? height() : 0, sizeHint().height());
    resize(w, h);
}


/*!
  Sets the progress bar widget to \a bar. The progress dialog resizes to
  fit. The progress dialog takes ownership of the progress \a bar which
  will be deleted when necessary, so do not use a progress bar
  allocated on the stack.
*/

void Q3ProgressDialog::setBar(Q3ProgressBar *bar)
{
#ifndef QT_NO_DEBUG
    if (progress() > 0)
        qWarning("Q3ProgressDialog::setBar: Cannot set a new progress bar "
                  "while the old one is active");
#endif
    delete d->bar;
    d->bar = bar;
    int w = qMax(isVisible() ? width() : 0, sizeHint().width());
    int h = qMax(isVisible() ? height() : 0, sizeHint().height());
    resize(w, h);
}


/*!
  \property Q3ProgressDialog::wasCanceled
  \brief whether the dialog was canceled

  \sa setProgress()
*/

bool Q3ProgressDialog::wasCanceled() const
{
    return d->cancellation_flag;
}

/*!
  \property Q3ProgressDialog::wasCancelled
  \brief whether the dialog was canceled
  \since 4.2

  Use \l wasCanceled instead.
*/

/*!
    Use wasCanceled() instead.
*/
bool Q3ProgressDialog::wasCancelled() const
{
    return d->cancellation_flag;
}

/*!
  \property Q3ProgressDialog::totalSteps
  \brief the total number of steps

  The default is 0.
*/

int Q3ProgressDialog::totalSteps() const
{
    if (d && d->bar)
        return bar()->totalSteps();
    return 0;
}

void Q3ProgressDialog::setTotalSteps(int totalSteps)
{
    bar()->setTotalSteps(totalSteps);
}


/*!
  Resets the progress dialog.
  The progress dialog becomes hidden if autoClose() is true.

  \sa setAutoClose(), setAutoReset()
*/

void Q3ProgressDialog::reset()
{
#ifndef QT_NO_CURSOR
    if (progress() >= 0) {
        if (d->creator)
            d->creator->setCursor(d->parentCursor);
    }
#endif
    if (d->autoClose || d->forceHide)
        hide();
    bar()->reset();
    d->cancellation_flag = false;
    d->shown_once = false;
    forceTimer->stop();
}

/*!
  Resets the progress dialog. wasCanceled() becomes true until
  the progress dialog is reset.
  The progress dialog becomes hidden.
*/

void Q3ProgressDialog::cancel()
{
    d->forceHide = true;
    reset();
    d->forceHide = false;
    d->cancellation_flag = true;
}

/*!
  \property Q3ProgressDialog::progress
  \brief the current amount of progress made.

  For the progress dialog to work as expected, you should initially set
  this property to 0 and finally set it to
  Q3ProgressDialog::totalSteps(); you can call setProgress() any number of times
  in-between.

  \warning If the progress dialog is modal
    (see Q3ProgressDialog::Q3ProgressDialog()),
    this function calls QApplication::processEvents(), so take care that
    this does not cause undesirable re-entrancy in your code. For example,
    don't use a Q3ProgressDialog inside a paintEvent()!

  \sa totalSteps
*/

int Q3ProgressDialog::progress() const
{
    return bar()->progress();
}

void Q3ProgressDialog::setProgress(int progress)
{
    if (progress == bar()->progress()
        || (bar()->progress() == -1 && progress == bar()->totalSteps()))
        return;

    bar()->setProgress(progress);

    if (d->shown_once) {
        if (isModal())
            qApp->processEvents();
    } else {
        if (progress == 0) {
#ifndef QT_NO_CURSOR
            if (d->creator) {
                d->parentCursor = d->creator->cursor();
                d->creator->setCursor(Qt::WaitCursor);
            }
#endif
            d->starttime.start();
            forceTimer->start(d->showTime);
            return;
        } else {
            bool need_show;
            int elapsed = d->starttime.elapsed();
            if (elapsed >= d->showTime) {
                need_show = true;
            } else {
                if (elapsed > minWaitTime) {
                    int estimate;
                    if ((totalSteps() - progress) >= INT_MAX / elapsed)
                        estimate = (totalSteps() - progress) / progress * elapsed;
                    else
                        estimate = elapsed * (totalSteps() - progress) / progress;
                    need_show = estimate >= d->showTime;
                } else {
                    need_show = false;
                }
            }
            if (need_show) {
                int w = qMax(isVisible() ? width() : 0, sizeHint().width());
                int h = qMax(isVisible() ? height() : 0, sizeHint().height());
                resize(w, h);
                show();
                d->shown_once = true;
            }
        }
#ifdef Q_WS_MAC
        QApplication::flush();
#endif
    }

    if (progress == bar()->totalSteps() && d->autoReset)
        reset();
}

/*!
  \overload

  Sets the current amount of progress to \a progress and the total number of
  steps to \a totalSteps.

  \sa setTotalSteps()
*/

void Q3ProgressDialog::setProgress(int progress, int totalSteps)
{
    setTotalSteps(totalSteps);
    setProgress(progress);
}

/*!
  Returns a size that fits the contents of the progress dialog.
  The progress dialog resizes itself as required, so you should not
  need to call this yourself.
*/

QSize Q3ProgressDialog::sizeHint() const
{
    QSize sh = label()->sizeHint();
    QSize bh = bar()->sizeHint();
    int h = margin_tb*2 + bh.height() + sh.height() + spacing;
    if (d->cancel)
        h += d->cancel->sizeHint().height() + spacing;
    return QSize(qMax(200, sh.width() + 2*margin_lr), h);
}

/*!\reimp
*/
void Q3ProgressDialog::resizeEvent(QResizeEvent *)
{
    layout();
}

/*!
  \reimp
*/
void Q3ProgressDialog::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange)
        layout();
    QDialog::changeEvent(ev);
}

void Q3ProgressDialog::layout()
{
    int sp = spacing;
    int mtb = margin_tb;
    int mlr = qMin(width()/10, margin_lr);
    const bool centered =
        bool(style()->styleHint(QStyle::SH_ProgressDialog_CenterCancelButton, 0, this));

    QSize cs = d->cancel ? d->cancel->sizeHint() : QSize(0,0);
    QSize bh = bar()->sizeHint();
    int cspc;
    int lh = 0;

    // Find spacing and sizes that fit.  It is important that a progress
    // dialog can be made very small if the user demands it so.
    for (int attempt=5; attempt--;) {
        cspc = d->cancel ? cs.height() + sp : 0;
        lh = qMax(0, height() - mtb - bh.height() - sp - cspc);

        if (lh < height()/4) {
            // Getting cramped
            sp /= 2;
            mtb /= 2;
            if (d->cancel) {
                cs.setHeight(qMax(4,cs.height()-sp-2));
            }
            bh.setHeight(qMax(4,bh.height()-sp-1));
        } else {
            break;
        }
    }

    if (d->cancel) {
        d->cancel->setGeometry(
            centered ? width()/2 - cs.width()/2 : width() - mlr - cs.width(),
            height() - mtb - cs.height() + sp,
            cs.width(), cs.height());
    }

    label()->setGeometry(mlr, 0, width()-mlr*2, lh);
    bar()->setGeometry(mlr, lh+sp, width()-mlr*2, bh.height());
}

/*!
    \property Q3ProgressDialog::minimumDuration
    \brief the time that must pass before the dialog appears

    If the expected duration of the task is less than the
    minimumDuration, the dialog will not appear at all. This prevents
    the dialog popping up for tasks that are quickly over. For tasks
    that are expected to exceed the minimumDuration, the dialog will
    pop up after the minimumDuration time or as soon as any progress
    is set.

    If set to 0, the dialog is always shown as soon as any progress is
    set. The default is 4000 milliseconds.
*/
void Q3ProgressDialog::setMinimumDuration(int ms)
{
    d->showTime = ms;
    if (bar()->progress() == 0) {
        forceTimer->stop();
        forceTimer->start(ms);
    }
}

int Q3ProgressDialog::minimumDuration() const
{
    return d->showTime;
}


/*!
  \reimp
*/

void Q3ProgressDialog::closeEvent(QCloseEvent *e)
{
    emit canceled();
    QDialog::closeEvent(e);
}

/*!
  \property Q3ProgressDialog::autoReset
  \brief whether the progress dialog calls reset() as soon as progress() equals totalSteps()

  The default is true.

  \sa setAutoClose()
*/

void Q3ProgressDialog::setAutoReset(bool b)
{
    d->autoReset = b;
}

bool Q3ProgressDialog::autoReset() const
{
    return d->autoReset;
}

/*!
  \property Q3ProgressDialog::autoClose
  \brief whether the dialog gets hidden by reset()

  The default is true.

  \sa setAutoReset()
*/

void Q3ProgressDialog::setAutoClose(bool b)
{
    d->autoClose = b;
}

bool Q3ProgressDialog::autoClose() const
{
    return d->autoClose;
}

/*!
  \reimp
*/

void Q3ProgressDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    int w = qMax(isVisible() ? width() : 0, sizeHint().width());
    int h = qMax(isVisible() ? height() : 0, sizeHint().height());
    resize(w, h);
    forceTimer->stop();
}

/*!
  Shows the dialog if it is still hidden after the algorithm has been started
  and minimumDuration milliseconds have passed.

  \sa setMinimumDuration()
*/

void Q3ProgressDialog::forceShow()
{
    if (d->shown_once || d->cancellation_flag)
        return;

    show();
    d->shown_once = true;
}


QT_END_NAMESPACE

#endif
