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

#include "qfocusframe.h"
#include "qstyle.h"
#include "qbitmap.h"
#include "qstylepainter.h"
#include "qstyleoption.h"
#include "qdebug.h"
#include <private/qwidget_p.h>

QT_BEGIN_NAMESPACE

class QFocusFramePrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QFocusFrame)
    QWidget *widget;

public:
    QFocusFramePrivate() {
        widget = 0;
        sendChildEvents = false;
    }
    void updateSize();
    void update();
};

void QFocusFramePrivate::update()
{
    Q_Q(QFocusFrame);
    q->setParent(widget->parentWidget());
    updateSize();
    if (q->parentWidget()->rect().intersects(q->geometry())) {
        if (q->style()->styleHint(QStyle::SH_FocusFrame_AboveWidget, 0, q))
            q->raise();
        else
            q->stackUnder(widget);
        q->show();
    } else {
        q->hide();
    }
}

void QFocusFramePrivate::updateSize()
{
    Q_Q(QFocusFrame);
    int vmargin = q->style()->pixelMetric(QStyle::PM_FocusFrameVMargin),
        hmargin = q->style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
    QRect geom(widget->x()-hmargin, widget->y()-vmargin,
               widget->width()+(hmargin*2), widget->height()+(vmargin*2));
    if(q->geometry() == geom)
        return;

    q->setGeometry(geom);
    QStyleHintReturnMask mask;
    QStyleOption opt;
    q->initStyleOption(&opt);
    if (q->style()->styleHint(QStyle::SH_FocusFrame_Mask, &opt, q, &mask))
        q->setMask(mask.region);
}

/*!
    Initialize \a option with the values from this QFocusFrame. This method is useful
    for subclasses when they need a QStyleOption, but don't want to fill
    in all the information themselves.

    \sa QStyleOption::initFrom()
*/
void QFocusFrame::initStyleOption(QStyleOption *option) const
{
    if (!option)
        return;

    option->initFrom(this);
}

/*!
    \class QFocusFrame
    \brief The QFocusFrame widget provides a focus frame which can be
    outside of a widget's normal paintable area.

    \ingroup basicwidgets
    \mainclass

    Normally an application will not need to create its own
    QFocusFrame as QStyle will handle this detail for
    you. A style writer can optionally use a QFocusFrame to have a
    focus area outside of the widget's paintable geometry. In this way
    space need not be reserved for the widget to have focus but only
    set on a QWidget with QFocusFrame::setWidget. It is, however,
    legal to create your own QFocusFrame on a custom widget and set
    its geometry manually via QWidget::setGeometry however you will
    not get auto-placement when the focused widget changes size or
    placement.
*/

/*!
    Constructs a QFocusFrame.

    The focus frame will not monitor \a parent for updates but rather
    can be placed manually or by using QFocusFrame::setWidget. A
    QFocusFrame sets Qt::WA_NoChildEventsForParent attribute; as a
    result the parent will not receive a QEvent::ChildInserted event,
    this will make it possible to manually set the geometry of the
    QFocusFrame inside of a QSplitter or other child event monitoring
    widget.

    \sa QFocusFrame::setWidget()
*/

QFocusFrame::QFocusFrame(QWidget *parent)
    : QWidget(*new QFocusFramePrivate, parent, 0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_NoChildEventsForParent, true);
    setAttribute(Qt::WA_AcceptDrops, style()->styleHint(QStyle::SH_FocusFrame_AboveWidget, 0, this));
}

/*!
    Destructor.
*/

QFocusFrame::~QFocusFrame()
{
}

/*!
  QFocusFrame will track changes to \a widget and resize itself automatically.
  If the monitored widget's parent changes, QFocusFrame will follow the widget
  and place itself around the widget automatically. If the monitored widget is deleted,
  QFocusFrame will set it to zero.

  \sa QFocusFrame::widget()
*/

void
QFocusFrame::setWidget(QWidget *widget)
{
    Q_D(QFocusFrame);
    if(widget == d->widget)
        return;

    if(d->widget)
        d->widget->removeEventFilter(this);
    if(widget && !widget->isWindow() && widget->parentWidget()->windowType() != Qt::SubWindow) {
        d->widget = widget;
        widget->installEventFilter(this);
        d->update();
    } else {
        d->widget = 0;
        hide();
    }
}

/*!
  Returns the currently monitored widget for automatically resize and
  update.

   \sa QFocusFrame::setWidget()
*/

QWidget *
QFocusFrame::widget() const
{
    Q_D(const QFocusFrame);
    return d->widget;
}


/*! \reimp */
void
QFocusFrame::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOption option;
    initStyleOption(&option);
    p.drawControl(QStyle::CE_FocusFrame, option);
}


/*! \reimp */
bool
QFocusFrame::eventFilter(QObject *o, QEvent *e)
{
    Q_D(QFocusFrame);
    if(o == d->widget) {
        switch(e->type()) {
        case QEvent::Move:
        case QEvent::Resize:
            d->updateSize();
            break;
        case QEvent::Hide:
        case QEvent::StyleChange:
            hide();
            break;
        case QEvent::ParentChange:
            d->update();
            break;
        case QEvent::Show:
            d->update();
            show();
            break;
        case QEvent::PaletteChange:
            setPalette(d->widget->palette());
            break;
        case QEvent::ZOrderChange:
            if (style()->styleHint(QStyle::SH_FocusFrame_AboveWidget, 0, this))
                raise();
            else
                stackUnder(d->widget);
            break;
        case QEvent::Destroy:
            setWidget(0);
            break;
        default:
            break;
        }
    }
    return false;
}

/*! \reimp */
bool QFocusFrame::event(QEvent *e)
{
    return QWidget::event(e);
}

QT_END_NAMESPACE
