/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*! \class FindWidget

    \brief A search bar that is commonly added below the searchable text.

    \internal

    This widget implements a search bar which becomes visible when the user
    wants to start searching. It is a modern replacement for the commonly used
    search dialog. It is usually placed below a QTextEdit using a QVBoxLayout.

    The QTextEdit instance will need to be associated with this class using
    setTextEdit().

    The search is incremental and can be set to case sensitive or whole words
    using buttons available on the search bar.

    \sa QTextEdit
 */

#include "findwidget.h"

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
#include <QtGui/QTextCursor>
#include <QtGui/QTextEdit>

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

    foreach (QString f, candidates) {
        if (QFile::exists(f))
            return QIcon(f);
    }

    return QIcon();
}

/*!
    Constructs a FindWidget.

    \a parent is passed to the QWidget constructor.
 */
FindWidget::FindWidget(QWidget *parent)
    : QWidget(parent)
    , m_textEdit(0)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
#ifndef Q_OS_MAC
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
#endif

    m_toolClose = new QToolButton(this);
    m_toolClose->setIcon(createIconSet("closetab.png"));
    m_toolClose->setAutoRaise(true);
    hboxLayout->addWidget(m_toolClose);

    m_editFind = new QLineEdit(this);
    m_editFind->setMinimumSize(QSize(150, 0));
    connect(m_editFind, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateButtons()));
    hboxLayout->addWidget(m_editFind);

    m_toolPrevious = new QToolButton(this);
    m_toolPrevious->setAutoRaise(true);
    m_toolPrevious->setText(tr("Previous"));
    m_toolPrevious->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolPrevious->setIcon(createIconSet("previous.png"));
    hboxLayout->addWidget(m_toolPrevious);

    m_toolNext = new QToolButton(this);
    m_toolNext->setAutoRaise(true);
    m_toolNext->setText(tr("Next"));
    m_toolNext->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toolNext->setIcon(createIconSet("next.png"));
    hboxLayout->addWidget(m_toolNext);

    m_checkCase = new QCheckBox(tr("Case sensitive"), this);
    hboxLayout->addWidget(m_checkCase);

    m_checkWholeWords = new QCheckBox(tr("Whole words"), this);
    hboxLayout->addWidget(m_checkWholeWords);

    m_labelWrapped = new QLabel(this);
    m_labelWrapped->setTextFormat(Qt::RichText);
    m_labelWrapped->setAlignment(
            Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    m_labelWrapped->setText(
            tr("<img src=\":/trolltech/shared/images/wrap.png\">"
                "&nbsp;Search wrapped"));
    hboxLayout->addWidget(m_labelWrapped);

    QSpacerItem *spacerItem =
        new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);
    setMinimumWidth(minimumSizeHint().width());
    m_labelWrapped->hide();

    connect(m_toolClose, SIGNAL(clicked()), SLOT(deactivate()));
    connect(m_toolNext, SIGNAL(clicked()), SLOT(findNext()));
    connect(m_editFind, SIGNAL(returnPressed()), SLOT(findNext()));
    connect(m_editFind, SIGNAL(textChanged(QString)),
                        SLOT(findCurrentText(QString)));
    connect(m_toolPrevious, SIGNAL(clicked()), SLOT(findPrevious()));

    updateButtons();
    hide();
}

/*!
    Destroys the FindWidget.
 */
FindWidget::~FindWidget()
{
}

/*!
    Returns the icon set to be used for the action that initiates a search.
 */
QIcon FindWidget::findIconSet()
{
    return createIconSet("searchfind.png");
}

/*!
    Associates a QTextEdit with this find widget. Searches done using this find
    widget will then apply to the given QTextEdit.

    An event filter is set on the QTextEdit which intercepts the ESC key while
    the find widget is active, and uses it to deactivate the find widget.

    If the find widget is already associated with a QTextEdit, the event filter
    is removed from this QTextEdit first.

    \a textEdit may be NULL.
 */
void FindWidget::setTextEdit(QTextEdit *textEdit)
{
    if (m_textEdit)
        m_textEdit->removeEventFilter(this);

    m_textEdit = textEdit;

    if (m_textEdit)
        m_textEdit->installEventFilter(this);
}

/*!
    Activates the find widget, making it visible and having focus on its input
    field.
 */
void FindWidget::activate()
{
    show();
    m_editFind->selectAll();
    m_editFind->setFocus(Qt::ShortcutFocusReason);
}

/*!
    Deactivates the find widget, making it invisible and handing focus to any
    associated QTextEdit.
 */
void FindWidget::deactivate()
{
    // Pass focus to the text edit
    if (m_textEdit)
        m_textEdit->setFocus();

    hide();
}

void FindWidget::findNext()
{
    find(m_editFind->text(), true, false);
}

void FindWidget::findPrevious()
{
    find(m_editFind->text(), false, true);
}

void FindWidget::findCurrentText(const QString &text)
{
    find(text, false, false);
}

void FindWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        deactivate();
        return;
    }

    QWidget::keyPressEvent(event);
}

void FindWidget::updateButtons()
{
    if (m_editFind->text().isEmpty()) {
        m_toolPrevious->setEnabled(false);
        m_toolNext->setEnabled(false);
    } else {
        m_toolPrevious->setEnabled(true);
        m_toolNext->setEnabled(true);
    }
}

void FindWidget::find(const QString &ttf, bool forward, bool backward)
{
    if (!m_textEdit)
        return;

    QTextCursor cursor = m_textEdit->textCursor();
    QTextDocument *doc = m_textEdit->document();

    if (!doc || cursor.isNull())
        return;

    QPalette p = m_editFind->palette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);

    if (cursor.hasSelection())
        cursor.setPosition(forward ? cursor.position() : cursor.anchor());

    QTextCursor newCursor = cursor;

    if (!ttf.isEmpty()) {
        QTextDocument::FindFlags options;

        if (backward)
            options |= QTextDocument::FindBackward;

        if (m_checkCase->isChecked())
            options |= QTextDocument::FindCaseSensitively;

        if (m_checkWholeWords->isChecked())
            options |= QTextDocument::FindWholeWords;

        newCursor = doc->find(ttf, cursor, options);
        m_labelWrapped->hide();

        if (newCursor.isNull()) {
            QTextCursor ac(doc);
            ac.movePosition(options & QTextDocument::FindBackward
                    ? QTextCursor::End : QTextCursor::Start);
            newCursor = doc->find(ttf, ac, options);
            if (newCursor.isNull()) {
                p.setColor(QPalette::Active, QPalette::Base,
                           QColor(255, 102, 102));
                newCursor = cursor;
            } else {
                m_labelWrapped->show();
            }
        }
    }

    if (!isVisible())
        show();

    m_textEdit->setTextCursor(newCursor);
    m_editFind->setPalette(p);
}

bool FindWidget::eventFilter(QObject *object, QEvent *e)
{
    if (isVisible()
            && object == m_textEdit
            && e->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Escape) {
            hide();
            return true;
        }
    }

    return QWidget::eventFilter(object, e);
}

QT_END_NAMESPACE
