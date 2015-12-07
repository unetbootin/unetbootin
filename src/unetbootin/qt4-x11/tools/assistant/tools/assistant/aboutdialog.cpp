/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QBuffer>

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>

#include "aboutdialog.h"

QT_BEGIN_NAMESPACE

AboutLabel::AboutLabel(QWidget *parent)
    : QTextBrowser(parent)
{
    setFrameStyle(QFrame::NoFrame);
    QPalette p;
    p.setColor(QPalette::Base, p.color(QPalette::Background));
    setPalette(p);
}

void AboutLabel::setText(const QString &text, const QByteArray &resources)
{
    QDataStream in(resources);
    in >> m_resourceMap;
    
    QTextBrowser::setText(text);
}

QSize AboutLabel::minimumSizeHint() const
{
    QTextDocument *doc = document();
    doc->adjustSize();
    return QSize(int(doc->size().width()), int(doc->size().height()));
}

QVariant AboutLabel::loadResource(int type, const QUrl &name)
{
    if (type == 2 || type == 3) {
        if (m_resourceMap.contains(name.toString())) {
            return m_resourceMap.value(name.toString());
        }
    }
    return QVariant();
}

void AboutLabel::setSource(const QUrl &url)
{
    if (url.isValid() 
        && (url.scheme() == QLatin1String("http") || url.scheme() == QLatin1String("ftp") 
            || url.scheme() == QLatin1String("mailto") || url.path().endsWith(QLatin1String("pdf")))) {
        if (!QDesktopServices::openUrl(url)) {
            QMessageBox::warning(this, tr("Warning"),
                         tr("Unable to launch external application.\n"),
                         tr("OK"));
        }
    }
}

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent, Qt::MSWindowsFixedSizeDialogHint|Qt::WindowTitleHint|Qt::WindowSystemMenuHint)
{
    m_pixmapLabel = 0;
    m_aboutLabel = new AboutLabel();
    
    m_closeButton = new QPushButton();
    m_closeButton->setText(tr("&Close"));
    connect(m_closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_aboutLabel, 1, 0, 1, -1);
    m_layout->addItem(new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed), 2, 1, 1, 1);
    m_layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 3, 0, 1, 1);
    m_layout->addWidget(m_closeButton, 3, 1, 1, 1);
    m_layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 3, 2, 1, 1);    
}

void AboutDialog::setText(const QString &text, const QByteArray &resources)
{
    m_aboutLabel->setText(text, resources);
    updateSize();
}

void AboutDialog::setPixmap(const QPixmap &pixmap)
{
    if (!m_pixmapLabel) {
        m_pixmapLabel = new QLabel();
        m_layout->addWidget(m_pixmapLabel, 0, 0, 1, -1, Qt::AlignCenter);
    }
    m_pixmapLabel->setPixmap(pixmap);
    updateSize();
}

QString AboutDialog::documentTitle() const
{
    return m_aboutLabel->documentTitle();
}

void AboutDialog::updateSize()
{
    QSize screenSize = QApplication::desktop()->availableGeometry(QCursor::pos()).size();
    int limit = qMin(screenSize.width()/2, 500);

#ifdef Q_WS_MAC
    limit = qMin(screenSize.width()/2, 420);
#endif

    layout()->activate();
    int width = layout()->totalMinimumSize().width();

    if (width > limit)
        width = limit;
    
    QFontMetrics fm(qApp->font("QWorkspaceTitleBar"));
    int windowTitleWidth = qMin(fm.width(windowTitle()) + 50, limit);
    if (windowTitleWidth > width)
        width = windowTitleWidth;

    layout()->activate();
    int height = (layout()->hasHeightForWidth())
                     ? layout()->totalHeightForWidth(width)
                     : layout()->totalMinimumSize().height();
    setFixedSize(width, height);
    QCoreApplication::removePostedEvents(this, QEvent::LayoutRequest);
}

QT_END_NAMESPACE
