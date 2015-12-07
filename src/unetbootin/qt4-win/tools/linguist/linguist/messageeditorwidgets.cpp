/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "messageeditorwidgets.h"
#include "messagehighlighter.h"

#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QScrollArea>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE

const char * const friendlyBackTab[] = {
        QT_TRANSLATE_NOOP("MessageEditor", "bell"),
        QT_TRANSLATE_NOOP("MessageEditor", "backspace"),
        QT_TRANSLATE_NOOP("MessageEditor", "new page"),
        QT_TRANSLATE_NOOP("MessageEditor", "new line"),
        QT_TRANSLATE_NOOP("MessageEditor", "carriage return"),
        QT_TRANSLATE_NOOP("MessageEditor", "tab")
    };

const char backTab[] = "\a\b\f\n\r\t";

const char *bellImageName = "trolltech/bellImage";
const char *backspaceImageName = "trolltech/bsImage";
const char *newpageImageName = "trolltech/newpageImage";
const char *newlineImageName = "trolltech/newlineImage";
const char *crImageName = "trolltech/crImage";
const char *tabImageName = "trolltech/tabImage";
const char *backTabImages[] = {
    bellImageName,
    backspaceImageName,
    newpageImageName,
    newlineImageName,
    crImageName,
    tabImageName};

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

    // Do not set different background if disabled
    QPalette p = palette();
    p.setColor(QPalette::Disabled, QPalette::Base, p.color(QPalette::Active, QPalette::Base));
    setPalette(p);

    setEditable(true);

    m_highlighter = new MessageHighlighter(this);
}

QString FormatTextEdit::getPlainText()
{
    return plainText(document());
}

QVariant FormatTextEdit::loadResource(int type, const QUrl &name)
{
    QImage img;
    if (type == QTextDocument::ImageResource) {
        img = m_backTabOmages.value(name);
        if (img.isNull()) {
            for (uint i = 0; i < qstrlen(backTab); ++i) {
                if (backTabImages[i] && name == QUrl(QLatin1String(backTabImages[i]))) {

                    QFont fnt = font();
                    fnt.setItalic(true);
                    QFontMetrics fm(fnt);
                    int h = fm.height();

                    QString str = QString::fromAscii("(%1)").arg(QLatin1String(friendlyBackTab[i]));
                    int w = fm.boundingRect(str).width() + 1;   //###
                    QImage textimg(w, h, QImage::Format_RGB32);
                    textimg.fill(qRgb(255,255,255));

                    QPainter p(&textimg);
                    p.setPen(QColor(Qt::blue));
                    p.setFont(fnt);
                    p.drawText(0, fm.ascent(), str);            //###
                    document()->addResource(QTextDocument::ImageResource, QUrl(QLatin1String(backTabImages[i])), textimg);

                    m_backTabOmages.insert(name, textimg);
                    return textimg;
                }
            }
        }
    }
    return img;
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

void FormatTextEdit::copySelection()
{
    QTextDocumentFragment fragment = textCursor().selection();
    // Convert fragment to document
    QTextDocument document;
    QTextCursor cursor(&document);
    cursor.insertFragment(fragment);

    QApplication::clipboard()->setText(plainText(&document));
}

void FormatTextEdit::contextMenuEvent(QContextMenuEvent * e)
{
    QMenu *menu = createStandardContextMenu(e->pos());
    // Change slot for copy action to our own method
    foreach (const QAction *action, menu->actions()) {
        if (action->text().endsWith(QString(QKeySequence(QKeySequence::Copy)))) {
            disconnect(action, SIGNAL(triggered()));
            connect(action, SIGNAL(triggered()), this, SLOT(copySelection()));
        }
    }
    menu->popup(mapToGlobal(e->pos()));
}

void FormatTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (!isReadOnly()) {
        bool eatevent = false;
        QTextCursor tc = textCursor();

        // Accept Key_Enter on the numpad as well as on the main pad
        if (e->key() == Qt::Key_Return && (e->modifiers() == Qt::KeypadModifier
                                        || e->modifiers() == Qt::NoModifier)) {
            tc = textCursor();
            document()->blockSignals(true);
            tc.beginEditBlock();
            tc.insertImage(QLatin1String(newlineImageName));
            document()->blockSignals(false);
            tc.insertBlock();
            tc.endEditBlock();
            eatevent = true;
        }
        else if (e->modifiers() == Qt::NoModifier) {
            switch (e->key()) {
            case Qt::Key_Tab:
                tc = textCursor();
                tc.insertImage(QLatin1String(tabImageName));
                eatevent = true;
                break;
            case Qt::Key_Backspace:
                if (tc.anchor() == tc.position()) {
                    QTextCursor tc = textCursor();
                    if (!tc.atStart() && tc.atBlockStart()) {
                        tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
                        QTextCharFormat fmt = tc.charFormat();
                        if (fmt.isImageFormat()) {
                            tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
                        }
                        tc.removeSelectedText();
                        eatevent = true;
                    }
                }
                break;
            case Qt::Key_Delete:
                if (tc.anchor() == tc.position()) {
                    QTextCursor tc = textCursor();
                    tc.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
                    QTextCharFormat fmt = tc.charFormat();
                    if (fmt.isImageFormat()) {
                        if (!tc.atEnd() && tc.atBlockEnd()) {
                            tc.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
                        }
                        tc.removeSelectedText();
                        eatevent = true;
                    }
                }
                break;
            }
        }
        else if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
            // Let user switch focus via Ctrl+Tab
            QKeyEvent ke(QEvent::KeyPress, e->key(), e->modifiers() & Qt::ShiftModifier);
            QCoreApplication::sendEvent(parent(), &ke);
            eatevent = true;
        }
        else if (e == QKeySequence::Copy) {
            // Use our own copySelection method
            copySelection();
            eatevent = true;
        }

        if (eatevent) {
            e->accept();
            return;
        }
    }
    ExpandingTextEdit::keyPressEvent(e);
}

void FormatTextEdit::setPlainText(const QString &text, bool userAction)
{
    bool oldBlockState = false;
    QTextCursor tc(textCursor());
    if (!userAction) {
        // Prevent contentsChanged signal
        oldBlockState = document()->blockSignals(true);
        document()->setUndoRedoEnabled(false);
    } else {
        tc.beginEditBlock();
    }

    // delete old text
    tc.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor, 1);
    tc.removeSelectedText();

    QString plainText;
    for (int i = 0; i < (int) text.length(); ++i) {
        int ch = text[i].unicode();
        if (ch < 0x20) {
            if (!plainText.isEmpty()) {
                tc.insertText(plainText);
                plainText.clear();
            }
            const char *p = strchr(backTab, ch);
            if (p) {
                if (backTabImages[p - backTab])
                    tc.insertImage(QLatin1String(backTabImages[p - backTab]));
                if (backTab[p - backTab] == '\n')
                    tc.insertBlock();
            }
        } else {
            plainText += QString(ch);
        }
    }

    tc.insertText(plainText);
    if (!userAction) {
        // highlighter is out of sync because of blocked signals
        m_highlighter->rehighlight();
        document()->setUndoRedoEnabled(true);
        document()->blockSignals(oldBlockState);
    } else {
        tc.endEditBlock();
    }
}

QString FormatTextEdit::plainText(const QTextDocument *document)
{
    QString plain;

    QTextBlock tb = document->begin();
    for (int b = 0; b < document->blockCount(); ++b) {
        QTextBlock::iterator it = tb.begin();
        if (it.atEnd()) {
            plain += tb.text();
        } else {
            while ( !it.atEnd() ) {
                QTextCharFormat fmt = it.fragment().charFormat();
                if (fmt.isImageFormat()) {
                    QTextImageFormat tif = fmt.toImageFormat();
                    if (tif.name() == QLatin1String(tabImageName)) plain += QLatin1Char('\t');
                    else if (tif.name() == QLatin1String(newlineImageName)) plain += QLatin1Char('\n');
                } else {
                    plain += it.fragment().text();
                }
                ++it;
            }
        }
        tb = tb.next();
    }
    return plain;
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
