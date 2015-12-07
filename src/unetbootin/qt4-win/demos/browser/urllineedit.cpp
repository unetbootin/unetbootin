/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
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

#include "urllineedit.h"

#include "browserapplication.h"
#include "searchlineedit.h"
#include "webview.h"

#include <QtCore/QEvent>

#include <QtGui/QApplication>
#include <QtGui/QCompleter>
#include <QtGui/QFocusEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionFrameV2>

#include <QtCore/QDebug>

ExLineEdit::ExLineEdit(QWidget *parent)
    : QWidget(parent)
    , m_leftWidget(0)
    , m_lineEdit(new QLineEdit(this))
    , m_clearButton(0)
{
    setFocusPolicy(m_lineEdit->focusPolicy());
    setAttribute(Qt::WA_InputMethodEnabled);
    setSizePolicy(m_lineEdit->sizePolicy());
    setBackgroundRole(m_lineEdit->backgroundRole());
    setMouseTracking(true);
    setAcceptDrops(true);
    setAttribute(Qt::WA_MacShowFocusRect, true);
    QPalette p = m_lineEdit->palette();
    setPalette(p);

    // line edit
    m_lineEdit->setFrame(false);
    m_lineEdit->setFocusProxy(this);
    m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    QPalette clearPalette = m_lineEdit->palette();
    clearPalette.setBrush(QPalette::Base, QBrush(Qt::transparent));
    m_lineEdit->setPalette(clearPalette);

    // clearButton
    m_clearButton = new ClearButton(this);
    connect(m_clearButton, SIGNAL(clicked()),
            m_lineEdit, SLOT(clear()));
    connect(m_lineEdit, SIGNAL(textChanged(const QString&)),
            m_clearButton, SLOT(textChanged(const QString&)));
}

void ExLineEdit::setLeftWidget(QWidget *widget)
{
    m_leftWidget = widget;
}

QWidget *ExLineEdit::leftWidget() const
{
    return m_leftWidget;
}

void ExLineEdit::resizeEvent(QResizeEvent *event)
{
    Q_ASSERT(m_leftWidget);
    updateGeometries();
    QWidget::resizeEvent(event);
}

void ExLineEdit::updateGeometries()
{
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    QRect rect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

    int height = rect.height();
    int width = rect.width();

    int m_leftWidgetHeight = m_leftWidget->height();
    m_leftWidget->setGeometry(rect.x() + 2,          rect.y() + (height - m_leftWidgetHeight)/2,
                              m_leftWidget->width(), m_leftWidget->height());

    int clearButtonWidth = this->height();
    m_lineEdit->setGeometry(m_leftWidget->x() + m_leftWidget->width(),        0,
                            width - clearButtonWidth - m_leftWidget->width(), this->height());

    m_clearButton->setGeometry(this->width() - clearButtonWidth, 0,
                               clearButtonWidth, this->height());
}

void ExLineEdit::initStyleOption(QStyleOptionFrameV2 *option) const
{
    option->initFrom(this);
    option->rect = contentsRect();
    option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;
    if (m_lineEdit->isReadOnly())
        option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
    if (hasEditFocus())
        option->state |= QStyle::State_HasEditFocus;
#endif
    option->features = QStyleOptionFrameV2::None;
}

QSize ExLineEdit::sizeHint() const
{
    m_lineEdit->setFrame(true);
    QSize size = m_lineEdit->sizeHint();
    m_lineEdit->setFrame(false);
    return size;
}

void ExLineEdit::focusInEvent(QFocusEvent *event)
{
    m_lineEdit->event(event);
    QWidget::focusInEvent(event);
}

void ExLineEdit::focusOutEvent(QFocusEvent *event)
{
    m_lineEdit->event(event);

    if (m_lineEdit->completer()) {
        connect(m_lineEdit->completer(), SIGNAL(activated(QString)),
                         m_lineEdit, SLOT(setText(QString)));
        connect(m_lineEdit->completer(), SIGNAL(highlighted(QString)),
                         m_lineEdit, SLOT(_q_completionHighlighted(QString)));
    }
    QWidget::focusOutEvent(event);
}

void ExLineEdit::keyPressEvent(QKeyEvent *event)
{
    m_lineEdit->event(event);
    QWidget::keyPressEvent(event);
}

bool ExLineEdit::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride)
        m_lineEdit->event(event);
    return QWidget::event(event);
}

void ExLineEdit::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &p, this);
}


class UrlIconLabel : public QLabel
{

public:
    UrlIconLabel(QWidget *parent);

    WebView *m_webView;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint m_dragStartPos;

};

UrlIconLabel::UrlIconLabel(QWidget *parent)
    : QLabel(parent)
    , m_webView(0)
{
    setMinimumWidth(16);
    setMinimumHeight(16);
}

void UrlIconLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragStartPos = event->pos();
    QLabel::mousePressEvent(event);
}

void UrlIconLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton
        && (event->pos() - m_dragStartPos).manhattanLength() > QApplication::startDragDistance()
         && m_webView) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_webView->url().toString());
        QList<QUrl> urls;
        urls.append(m_webView->url());
        mimeData->setUrls(urls);
        drag->setMimeData(mimeData);
        drag->exec();
    }
}

UrlLineEdit::UrlLineEdit(QWidget *parent)
    : ExLineEdit(parent)
    , m_webView(0)
    , m_iconLabel(0)
{
    // icon
    m_iconLabel = new UrlIconLabel(this);
    m_iconLabel->resize(16, 16);
    setLeftWidget(m_iconLabel);
    m_defaultBaseColor = palette().color(QPalette::Base);

    webViewIconChanged();
}

void UrlLineEdit::setWebView(WebView *webView)
{
    Q_ASSERT(!m_webView);
    m_webView = webView;
    m_iconLabel->m_webView = webView;
    connect(webView, SIGNAL(urlChanged(const QUrl &)),
        this, SLOT(webViewUrlChanged(const QUrl &)));
    connect(webView, SIGNAL(loadFinished(bool)),
        this, SLOT(webViewIconChanged()));
    connect(webView, SIGNAL(iconChanged()),
        this, SLOT(webViewIconChanged()));
    connect(webView, SIGNAL(loadProgress(int)),
        this, SLOT(update()));
}

void UrlLineEdit::webViewUrlChanged(const QUrl &url)
{
    m_lineEdit->setText(url.toString());
    m_lineEdit->setCursorPosition(0);
}

void UrlLineEdit::webViewIconChanged()
{
    QUrl url = (m_webView)  ? m_webView->url() : QUrl();
    QIcon icon = BrowserApplication::instance()->icon(url);
    QPixmap pixmap(icon.pixmap(16, 16));
    m_iconLabel->setPixmap(pixmap);
}

QLinearGradient UrlLineEdit::generateGradient(const QColor &color) const
{
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, m_defaultBaseColor);
    gradient.setColorAt(0.15, color.lighter(120));
    gradient.setColorAt(0.5, color);
    gradient.setColorAt(0.85, color.lighter(120));
    gradient.setColorAt(1, m_defaultBaseColor);
    return gradient;
}

void UrlLineEdit::focusOutEvent(QFocusEvent *event)
{
    if (m_lineEdit->text().isEmpty() && m_webView)
        m_lineEdit->setText(m_webView->url().toString());
    ExLineEdit::focusOutEvent(event);
}

void UrlLineEdit::paintEvent(QPaintEvent *event)
{
    QPalette p = palette();
    if (m_webView && m_webView->url().scheme() == QLatin1String("https")) {
        QColor lightYellow(248, 248, 210);
        p.setBrush(QPalette::Base, generateGradient(lightYellow));
    } else {
        p.setBrush(QPalette::Base, m_defaultBaseColor);
    }
    setPalette(p);
    ExLineEdit::paintEvent(event);

    QPainter painter(this);
    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    QRect backgroundRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    if (m_webView && !hasFocus()) {
        int progress = m_webView->progress();
        QColor loadingColor = QColor(116, 192, 250);
        painter.setBrush(generateGradient(loadingColor));
        painter.setPen(Qt::transparent);
        int mid = backgroundRect.width() / 100 * progress;
        QRect progressRect(backgroundRect.x(), backgroundRect.y(), mid, backgroundRect.height());
        painter.drawRect(progressRect);
    }
}

