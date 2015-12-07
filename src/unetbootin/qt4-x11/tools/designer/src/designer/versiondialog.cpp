/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include <QtCore/QVector>
#include <QtGui/QMouseEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QStyleOption>
#include "versiondialog.h"

QT_BEGIN_NAMESPACE

class VersionLabel : public QLabel
{
    Q_OBJECT
public:
    VersionLabel(QWidget *parent = 0);

signals:
    void triggered();

protected:
    void mousePressEvent(QMouseEvent *me);
    void mouseMoveEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void paintEvent(QPaintEvent *pe);
private:
    QVector<QPoint> hitPoints;
    QVector<QPoint> missPoints;
    QPainterPath m_path;
    bool secondStage;
    bool m_pushed;
};

VersionLabel::VersionLabel(QWidget *parent)
        : QLabel(parent), secondStage(false), m_pushed(false)
{
    setPixmap(QPixmap(QLatin1String(":/trolltech/designer/images/designer.png")));
    hitPoints.append(QPoint(56, 25));
    hitPoints.append(QPoint(29, 55));
    hitPoints.append(QPoint(56, 87));
    hitPoints.append(QPoint(82, 55));
    hitPoints.append(QPoint(58, 56));

    secondStage = false;
    m_pushed = false;
}

void VersionLabel::mousePressEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton) {
        if (!secondStage) {
            m_path = QPainterPath(me->pos());
        } else {
            m_pushed = true;
            update();
        }
    }
}

void VersionLabel::mouseMoveEvent(QMouseEvent *me)
{
    if (me->buttons() & Qt::LeftButton)
        if (!secondStage)
            m_path.lineTo(me->pos());
}

void VersionLabel::mouseReleaseEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton) {
        if (!secondStage) {
            m_path.lineTo(me->pos());
            bool gotIt = true;
            QPoint pt;
            foreach(pt, hitPoints) {
                if (!m_path.contains(pt)) {
                    gotIt = false;
                    break;
                }
            }
            if (gotIt) {
                foreach(pt, missPoints) {
                    if (m_path.contains(pt)) {
                        gotIt = false;
                        break;
                    }
                }
            }
            if (gotIt && !secondStage) {
                secondStage = true;
                m_path = QPainterPath();
                update();
            }
        } else {
            m_pushed = false;
            update();
            emit triggered();
        }
    }
}

void VersionLabel::paintEvent(QPaintEvent *pe)
{
    if (secondStage) {
        QPainter p(this);
        QStyleOptionButton opt;
        opt.init(this);
        if (!m_pushed)
            opt.state |= QStyle::State_Raised;
        else
            opt.state |= QStyle::State_Sunken;
        opt.state &= ~QStyle::State_HasFocus;
        style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &p, this);
    }
    QLabel::paintEvent(pe);
}

VersionDialog::VersionDialog(QWidget *parent)
    : QDialog(parent
#ifdef Q_WS_MAC
            , Qt::Tool
#endif
            )
{
    setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::MSWindowsFixedSizeDialogHint);
    QGridLayout *layout = new QGridLayout(this);
    VersionLabel *label = new VersionLabel;
    QLabel *lbl = new QLabel;
    QString version = tr("<h3>%1</h3><br/><br/>Version %2");
#if QT_EDITION == QT_EDITION_OPENSOURCE
    QString open = tr(" Open Source Edition");
    version.append(open);
#endif
    version = version.arg(tr("Qt Designer")).arg(QLatin1String(QT_VERSION_STR));
    version.append(tr("<br/>Qt Designer is a graphical user interface designer for Qt applications.<br/>"));

    QString edition =
#if QT_EDITION == QT_EDITION_OPENSOURCE
                    tr("This version of Qt Designer is part of the Qt Open Source Edition, for use "
                    "in the development of Open Source applications. "
                    "Qt is a comprehensive C++ framework for cross-platform application "
                    "development.<br/><br/>"
                    "You need a commercial Qt license for development of proprietary (closed "
                    "source) applications. Please see <a href=\"http://qtsoftware.com/company/about/businessmodel\">http://qtsoftware.com/company/about/businessmodel"
                    ".html</a> for an overview of Qt licensing.<br/>");
#elif defined(QT_PRODUCT_LICENSE)
                    tr("This program is licensed to you under the terms of the "
                    "Qt %1 License Agreement. For details, see the license file "
                    "that came with this software distribution.<br/>").arg(QT_PRODUCT_LICENSE);
#else
                    tr("This program is licensed to you under the terms of the "
                    "Qt Commercial License Agreement. For details, see the file LICENSE "
                    "that came with this software distribution.<br/>");
#endif

    lbl->setText(tr("%1"
                    "<br/>%2"
                    "<br/>Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)."
                    "<br/><br/>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                    " INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A"
                    " PARTICULAR PURPOSE.<br/> ").arg(version).arg(edition));

    lbl->setWordWrap(true);
    lbl->setOpenExternalLinks(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox , SIGNAL(rejected()), this, SLOT(reject()));
    connect(label, SIGNAL(triggered()), this, SLOT(accept()));
    layout->addWidget(label, 0, 0, 1, 1);
    layout->addWidget(lbl, 0, 1, 4, 4);
    layout->addWidget(buttonBox, 4, 2, 1, 1);
}

QT_END_NAMESPACE

#include "versiondialog.moc"
