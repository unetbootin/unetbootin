/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "messageeditorwidgets.h"
#include "messagehighlighter.h"

#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollArea>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE

ExpandingTextEdit::ExpandingTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QAbstractTextDocumentLayout *docLayout = document()->documentLayout();
    connect(docLayout, SIGNAL(documentSizeChanged(QSizeF)), SLOT(updateHeight(QSizeF)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(reallyEnsureCursorVisible()));

    m_minimumHeight = qRound(docLayout->documentSize().height()) + frameWidth() * 2;
}

void ExpandingTextEdit::updateHeight(const QSizeF &documentSize)
{
    m_minimumHeight = qRound(documentSize.height()) + frameWidth() * 2;
    updateGeometry();
}

QSize ExpandingTextEdit::sizeHint() const
{
    return QSize(100, m_minimumHeight);
}

QSize ExpandingTextEdit::minimumSizeHint() const
{
    return QSize(100, m_minimumHeight);
}

void ExpandingTextEdit::reallyEnsureCursorVisible()
{
    QObject *ancestor = parent();
    while (ancestor) {
        QScrollArea *scrollArea = qobject_cast<QScrollArea*>(ancestor);
        if (scrollArea &&
                (scrollArea->verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff &&
                 scrollArea->horizontalScrollBarPolicy() != Qt::ScrollBarAlwaysOff)) {
            const QRect &r = cursorRect();
            const QPoint &c = mapTo(scrollArea->widget(), r.center());
            scrollArea->ensureVisible(c.x(), c.y());
            break;
        }
        ancestor = ancestor->parent();
    }
}

FormatTextEdit::FormatTextEdit(QWidget *parent)
    : ExpandingTextEdit(parent)
{
    setLineWrapMode(QTextEdit::WidgetWidth);
    setAcceptRichText(false);
    QTextOption option = document()->defaultTextOption();
    option.setFlags(option.flags()
                    | QTextOption::ShowLineAndParagraphSeparators
                    | QTextOption::ShowTabsAndSpaces);
    document()->setDefaultTextOption(option);

    // Do not set different background if disabled
    QPalette p = palette();
    p.setColor(QPalette::Disabled, QPalette::Base, p.color(QPalette::Active, QPalette::Base));
    setPalette(p);

    setEditable(true);

    m_highlighter = new MessageHighlighter(this);
}

void FormatTextEdit::setEditable(bool editable)
{
    // save default frame style
    static int framed = frameStyle();
    static Qt::FocusPolicy defaultFocus = focusPolicy();

    if (editable) {
        setFrameStyle(framed);
        setFocusPolicy(defaultFocus);
    } else {
        setFrameStyle(QFrame::NoFrame | QFrame::Plain);
        setFocusPolicy(Qt::NoFocus);
    }

    setReadOnly(!editable);
}

void FormatTextEdit::setPlainText(const QString &text, bool userAction)
{
    bool oldBlockState = false;
    if (!userAction) {
        // Prevent contentsChanged signal
        oldBlockState = document()->blockSignals(true);
        document()->setUndoRedoEnabled(false);
        ExpandingTextEdit::setPlainText(text);
        // highlighter is out of sync because of blocked signals
        m_highlighter->rehighlight();
        document()->setUndoRedoEnabled(true);
        document()->blockSignals(oldBlockState);
    } else {
        ExpandingTextEdit::setPlainText(text);
    }
}

FormWidget::FormWidget(const QString &label, bool isEditable, QWidget *parent)
        : QWidget(parent),
          m_hideWhenEmpty(false)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);

    m_label = new QLabel(this);
    m_label->setText(label);
    layout->addWidget(m_label);

    m_editor = new FormatTextEdit(this);
    m_editor->setEditable(isEditable);
    //m_textEdit->setWhatsThis(tr("This area shows text from an auxillary translation."));
    layout->addWidget(m_editor);

    setLayout(layout);

    connect(m_editor->document(), SIGNAL(contentsChanged()), SIGNAL(textChanged()));
    connect(m_editor, SIGNAL(selectionChanged()), SIGNAL(selectionChanged()));
    connect(m_editor, SIGNAL(cursorPositionChanged()), SIGNAL(cursorPositionChanged()));
}

void FormWidget::setTranslation(const QString &text, bool userAction)
{
    m_editor->setPlainText(text, userAction);
    if (m_hideWhenEmpty)
        setHidden(text.isEmpty());
}

void FormWidget::setEditingEnabled(bool enable)
{
    // Use read-only state so that the text can still be copied
    m_editor->setReadOnly(!enable);
    m_label->setEnabled(enable);
}


QT_END_NAMESPACE
