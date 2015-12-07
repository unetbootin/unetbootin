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

#include "filterwidget_p.h"
#include "iconloader_p.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QFocusEvent>
#include <QtGui/QPalette>
#include <QtGui/QCursor>

#include <QtCore/QDebug>

enum { debugFilter = 0 };

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

HintLineEdit::HintLineEdit(QWidget *parent) :
    QLineEdit(parent),
    m_defaultFocusPolicy(focusPolicy()),
    m_hintColor(QColor(0xbbbbbb)),
    m_refuseFocus(false),
    m_showingHintText(false)
{
}

bool HintLineEdit::refuseFocus() const
{
    return m_refuseFocus;
}

void HintLineEdit::setRefuseFocus(bool v)
{
    if (v == m_refuseFocus)
        return;
    m_refuseFocus = v;
    setFocusPolicy(m_refuseFocus ? Qt::NoFocus : m_defaultFocusPolicy);
}

void HintLineEdit::mousePressEvent(QMouseEvent *e)
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;
    // Explicitly focus on click.
    if (m_refuseFocus && !hasFocus())
        setFocus(Qt::OtherFocusReason);
    QLineEdit::mousePressEvent(e);
}

void HintLineEdit::focusInEvent(QFocusEvent *e)
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;
    if (m_refuseFocus) {
        // Refuse the focus if the mouse it outside. In addition to the mouse
        // press logic, this prevents a re-focussing which occurs once
        // we actually had focus
        const Qt::FocusReason reason = e->reason();
        if (reason == Qt::ActiveWindowFocusReason || reason == Qt::PopupFocusReason) {
            const QPoint mousePos = mapFromGlobal(QCursor::pos());
            const bool refuse = !geometry().contains(mousePos);
            if (debugFilter)
                qDebug() << Q_FUNC_INFO << refuse ;
            if (refuse) {
                e->ignore();
                return;
            }
        }
    }

    hideHintText();
    QLineEdit::focusInEvent(e);
}

void HintLineEdit::focusOutEvent(QFocusEvent *e)
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;
    // Focus out: Switch to displaying the hint text unless there is user input
    showHintText();
    QLineEdit::focusOutEvent(e);
}

QString HintLineEdit::hintText() const
{
    return m_hintText;
}

void HintLineEdit::setHintText(const QString &ht)
{
    if (ht == m_hintText)
        return;
    hideHintText();
    m_hintText = ht;
    if (!hasFocus() && !ht.isEmpty())
        showHintText();
}

void HintLineEdit::showHintText(bool force)
{
    if (m_showingHintText || m_hintText.isEmpty())
        return;
    if (force || text().isEmpty()) {
        m_showingHintText = true;
        setText(m_hintText);
        setTextColor(m_hintColor, &m_textColor);
    }
}
void HintLineEdit::hideHintText()
{
    if (m_showingHintText && !m_hintText.isEmpty()) {
        m_showingHintText = false;
        setText(QString());
        setTextColor(m_textColor);
    }
}

bool  HintLineEdit::isShowingHintText() const
{
    return m_showingHintText;
}

QString  HintLineEdit::typedText() const
{
    return m_showingHintText ? QString() : text();
}

void HintLineEdit::setTextColor(const QColor &newColor, QColor *oldColor)
{
    QPalette pal = palette();
    if (oldColor)
        *oldColor = pal.color(QPalette::Text);
    pal.setColor(QPalette::Text, newColor);
    setPalette(pal);}

// ------------------- FilterWidget
FilterWidget::FilterWidget(QWidget *parent, LayoutMode lm)  :
    QWidget(parent),
    m_button(new QPushButton),
    m_editor(new HintLineEdit)
{
    m_editor->setHintText(tr("<Filter>"));
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    if (lm == LayoutAlignRight)
        l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    l->addWidget(m_editor);

    m_button->setIcon(createIconSet(QLatin1String("resetproperty.png")));
    m_button->setIconSize(QSize(8, 8));
    m_button->setFlat(true);
    l->addWidget(m_button);

    connect(m_button, SIGNAL(clicked()), this, SLOT(reset()));
    connect(m_editor, SIGNAL(textChanged(QString)), this, SLOT(checkButton(QString)));
    connect(m_editor, SIGNAL(textEdited(QString)), this, SIGNAL(filterChanged(QString)));
}

QString FilterWidget::text() const
{
    return m_editor->typedText();
}

void FilterWidget::checkButton(const QString &)
{
    m_button->setEnabled(!text().isEmpty());
}

void FilterWidget::reset()
{
    if (debugFilter)
        qDebug() << Q_FUNC_INFO;
    if (!text().isEmpty()) {
        // Editor has lost focus once this is pressed
        m_editor->showHintText(true);
        emit filterChanged(QString());
    }
}

bool FilterWidget::refuseFocus() const
{
    return m_editor->refuseFocus();
}

void FilterWidget::setRefuseFocus(bool v)
{
    m_editor->setRefuseFocus(v);
}
} // namespace qdesigner_internal

QT_END_NAMESPACE
