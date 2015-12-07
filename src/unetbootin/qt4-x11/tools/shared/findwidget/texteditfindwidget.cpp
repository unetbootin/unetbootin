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

/*! \class TextEditFindWidget

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

#include "texteditfindwidget.h"

#include <QtGui/QCheckBox>
#include <QtGui/QTextCursor>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

/*!
    Constructs a TextEditFindWidget.

    \a flags is passed to the AbstractFindWidget constructor.
    \a parent is passed to the QWidget constructor.
 */
TextEditFindWidget::TextEditFindWidget(FindFlags flags, QWidget *parent)
    : AbstractFindWidget(flags, parent)
    , m_textEdit(0)
{
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
void TextEditFindWidget::setTextEdit(QTextEdit *textEdit)
{
    if (m_textEdit)
        m_textEdit->removeEventFilter(this);

    m_textEdit = textEdit;

    if (m_textEdit)
        m_textEdit->installEventFilter(this);
}

/*!
    \reimp
 */
void TextEditFindWidget::deactivate()
{
    // Pass focus to the text edit
    if (m_textEdit)
        m_textEdit->setFocus();

    AbstractFindWidget::deactivate();
}

/*!
    \reimp
 */
void TextEditFindWidget::find(const QString &ttf, bool skipCurrent, bool backward, bool *found, bool *wrapped)
{
    if (!m_textEdit)
        return;

    QTextCursor cursor = m_textEdit->textCursor();
    QTextDocument *doc = m_textEdit->document();

    if (!doc || cursor.isNull())
        return;

    if (cursor.hasSelection())
        cursor.setPosition((skipCurrent && !backward) ? cursor.position() : cursor.anchor());

    *found = true;
    QTextCursor newCursor = cursor;

    if (!ttf.isEmpty()) {
        QTextDocument::FindFlags options;

        if (backward)
            options |= QTextDocument::FindBackward;

        if (caseSensitive())
            options |= QTextDocument::FindCaseSensitively;

        if (wholeWords())
            options |= QTextDocument::FindWholeWords;

        newCursor = doc->find(ttf, cursor, options);
        if (newCursor.isNull()) {
            QTextCursor ac(doc);
            ac.movePosition(options & QTextDocument::FindBackward
                    ? QTextCursor::End : QTextCursor::Start);
            newCursor = doc->find(ttf, ac, options);
            if (newCursor.isNull()) {
                *found = false;
                newCursor = cursor;
            } else {
                *wrapped = true;
            }
        }
    }

    if (!isVisible())
        show();

    m_textEdit->setTextCursor(newCursor);
}

QT_END_NAMESPACE
