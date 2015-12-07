/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*! \class AbstractFindWidget

    \brief A search bar that is commonly added below a searchable widget.

    \internal

    This widget implements a search bar which becomes visible when the user
    wants to start searching. It is a modern replacement for the commonly used
    search dialog. It is usually placed below the target widget using a QVBoxLayout.

    The search is incremental and can be set to case sensitive or whole words
    using buttons available on the search bar.
 */

#include "abstractfindwidget.h"

#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QTimer>

#include <QtGui/QCheckBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

static QIcon createIconSet(const QString &name)
{
    QStringList candidates = QStringList()
        << (QString::fromUtf8(":/trolltech/shared/images/") + name)
#ifdef Q_WS_MAC
        << (QString::fromUtf8(":/trolltech/shared/images/mac/") + name);
#else
        << (QString::fromUtf8(":/trolltech/shared/images/win/") + name);
#endif

    foreach (const QString &f, candidates) {
        if (QFile::exists(f))
            return QIcon(f);
    }

    return QIcon();
}

/*!
    Constructs an AbstractFindWidget.

    \a flags can change the layout and turn off certain features.
    \a parent is passed to the QWidget constructor.
 */
AbstractFindWidget::AbstractFindWidget(FindFlags flags, QWidget *parent)
    : QWidget(parent)
{
    QBoxLayout *topLayOut;
    QBoxLayout *layOut;
    if (flags & NarrowLayout) {
        topLayOut = new QVBoxLayout(this);
        layOut = new QHBoxLayout;
        topLayOut->addLayout(layOut);
    } else {
        topLayOut = layOut = new QHBoxLayout(this);
    }
#ifndef Q_OS_MAC
    topLayOut->setSpacing(6);
    topLayOut->setMargin(0);
#endif

    m_toolClose = new QToolButton(this);
    m_toolClose->setIcon(createIconSet(QLatin1String("closetab.png")));
    m_toolClose->setAutoRaise(true);
    layOut->addWidget(m_toolClose);
    connect(m_toolClose, SIGNAL(clicked()), SLOT(deactivate()));

    m_editFind = new QLineEdit(this);
    layOut->addWidget(m_editFind);
    connect(m_editFind, SIGNAL(returnPressed()), SLOT(findNext()));
    connect(m_editFind, SIGNAL(textChanged(QString)), SLOT(findCurrentText()));
    connect(m_editFind, SIGNAL(textChanged(QString)), SLOT(updateButtons()));

    m_toolPrevious = new QToolButton(this);
    m_toolPrevious->setAutoRaise(true);
    m_toolPrevious->setText(tr("&Previous"));
    m_toolPrevious->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolPrevious->setIcon(createIconSet(QLatin1String("previous.png")));
    layOut->addWidget(m_toolPrevious);
    connect(m_toolPrevious, SIGNAL(clicked()), SLOT(findPrevious()));

    m_toolNext = new QToolButton(this);
    m_toolNext->setAutoRaise(true);
    m_toolNext->setText(tr("&Next"));
    m_toolNext->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolNext->setIcon(createIconSet(QLatin1String("next.png")));
    layOut->addWidget(m_toolNext);
    connect(m_toolNext, SIGNAL(clicked()), SLOT(findNext()));

    if (flags & NarrowLayout) {
        QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
        m_toolPrevious->setSizePolicy(sp);
        m_toolPrevious->setMinimumWidth(m_toolPrevious->minimumSizeHint().height());
        m_toolNext->setSizePolicy(sp);
        m_toolNext->setMinimumWidth(m_toolNext->minimumSizeHint().height());

        QSpacerItem *spacerItem =
            new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layOut->addItem(spacerItem);

        layOut = new QHBoxLayout;
        topLayOut->addLayout(layOut);
    } else {
        m_editFind->setMinimumWidth(150);
    }

    if (!(flags & NoCaseSensitive)) {
        m_checkCase = new QCheckBox(tr("&Case sensitive"), this);
        layOut->addWidget(m_checkCase);
        connect(m_checkCase, SIGNAL(toggled(bool)), SLOT(findCurrentText()));
    } else {
        m_checkCase = 0;
    }

    if (!(flags & NoWholeWords)) {
        m_checkWholeWords = new QCheckBox(tr("Whole &words"), this);
        layOut->addWidget(m_checkWholeWords);
        connect(m_checkWholeWords, SIGNAL(toggled(bool)), SLOT(findCurrentText()));
    } else {
        m_checkWholeWords = 0;
    }

    m_labelWrapped = new QLabel(this);
    m_labelWrapped->setTextFormat(Qt::RichText);
    m_labelWrapped->setAlignment(
            Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    m_labelWrapped->setText(
            tr("<img src=\":/trolltech/shared/images/wrap.png\">"
                "&nbsp;Search wrapped"));
    m_labelWrapped->hide();
    layOut->addWidget(m_labelWrapped);

    QSpacerItem *spacerItem =
        new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layOut->addItem(spacerItem);

    setMinimumWidth(minimumSizeHint().width());

    updateButtons();
    hide();
}

/*!
    Destroys the AbstractFindWidget.
 */
AbstractFindWidget::~AbstractFindWidget()
{
}

/*!
    Returns the icon set to be used for the action that initiates a search.
 */
QIcon AbstractFindWidget::findIconSet()
{
    return createIconSet(QLatin1String("searchfind.png"));
}

/*!
    Activates the find widget, making it visible and having focus on its input
    field.
 */
void AbstractFindWidget::activate()
{
    show();
    m_editFind->selectAll();
    m_editFind->setFocus(Qt::ShortcutFocusReason);
}

/*!
    Deactivates the find widget, making it invisible and handing focus to any
    associated QTextEdit.
 */
void AbstractFindWidget::deactivate()
{
    hide();
}

void AbstractFindWidget::findNext()
{
    findInternal(m_editFind->text(), true, false);
}

void AbstractFindWidget::findPrevious()
{
    findInternal(m_editFind->text(), true, true);
}

void AbstractFindWidget::findCurrentText()
{
    findInternal(m_editFind->text(), false, false);
}

void AbstractFindWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        deactivate();
        return;
    }

    QWidget::keyPressEvent(event);
}

void AbstractFindWidget::updateButtons()
{
    const bool en = !m_editFind->text().isEmpty();
    m_toolPrevious->setEnabled(en);
    m_toolNext->setEnabled(en);
}

void AbstractFindWidget::findInternal(const QString &ttf, bool skipCurrent, bool backward)
{
    bool found = false;
    bool wrapped = false;
    find(ttf, skipCurrent, backward, &found, &wrapped);
    QPalette p;
    p.setColor(QPalette::Active, QPalette::Base, found ? Qt::white : QColor(255, 102, 102));
    m_editFind->setPalette(p);
    m_labelWrapped->setVisible(wrapped);
}

bool AbstractFindWidget::caseSensitive() const
{
    return m_checkCase && m_checkCase->isChecked();
}

bool AbstractFindWidget::wholeWords() const
{
    return m_checkWholeWords && m_checkWholeWords->isChecked();
}

bool AbstractFindWidget::eventFilter(QObject *object, QEvent *e)
{
    if (isVisible() && e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Escape) {
            hide();
            return true;
        }
    }

    return QWidget::eventFilter(object, e);
}

QT_END_NAMESPACE
