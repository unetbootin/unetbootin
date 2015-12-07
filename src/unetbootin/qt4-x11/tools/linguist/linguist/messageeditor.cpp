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

/*  TRANSLATOR MsgEdit

  This is the right panel of the main window.
*/

#include "messageeditor.h"
#include "messageeditorwidgets.h"
#include "simtexth.h"
#include "phrasemodel.h"

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QDebug>
#include <QDockWidget>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPainter>
#include <QTreeView>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE

#ifdef NEVER_TRUE
// Allow translators to provide localized names for QLocale::languageToString
// At least the own language should be translated ... This is a "hack" until
// functionality is provided within Qt (see task 196275).
static const char * language_strings[] = 
{
    QT_TRANSLATE_NOOP("MessageEditor", "German"),
    QT_TRANSLATE_NOOP("MessageEditor", "Japanese"),
    QT_TRANSLATE_NOOP("MessageEditor", "French"),
    QT_TRANSLATE_NOOP("MessageEditor", "Polish"),
    QT_TRANSLATE_NOOP("MessageEditor", "Chinese")
};
#endif

/*
   MessageEditor class impl.

   Handles layout of dock windows and the editor page.
*/
MessageEditor::MessageEditor(MultiDataModel *dataModel, QMainWindow *parent)
    : QScrollArea(parent->centralWidget()),
      m_dataModel(dataModel),
      m_currentModel(-1),
      m_currentNumerus(-1),
      m_undoAvail(false),
      m_redoAvail(false),
      m_cutAvail(false),
      m_copyAvail(false),
      m_sourceSelected(false),
      m_pluralSourceSelected(false),
      m_currentSelected(false)
{
    setObjectName(QLatin1String("scroll area"));

    // Use white explicitly as the background color for the editor page.
    QPalette p;
    p.setColor(QPalette::Active,   QPalette::Base,   Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Base,   Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Base,   Qt::white);
    p.setColor(QPalette::Active,   QPalette::Window, Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Window, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Window, Qt::white);
    setPalette(p);

    setupEditorPage();

    // Signals
    connect(qApp->clipboard(), SIGNAL(dataChanged()),
            SLOT(clipboardChanged()));
    connect(m_dataModel, SIGNAL(modelAppended()),
            SLOT(messageModelAppended()));
    connect(m_dataModel, SIGNAL(modelDeleted(int)),
            SLOT(messageModelDeleted(int)));
    connect(m_dataModel, SIGNAL(allModelsDeleted()),
            SLOT(allModelsDeleted()));
    connect(m_dataModel, SIGNAL(languageChanged(int)),
            SLOT(setTargetLanguage(int)));

    clipboardChanged();

    setWhatsThis(tr("This whole panel allows you to view and edit "
                    "the translation of some source text."));
    showNothing();
}

void MessageEditor::setupEditorPage()
{
    QFrame *editorPage = new QFrame;
    editorPage->setObjectName(QLatin1String("editorPage"));

    // Due to CSS being rather broken on the Mac style at the moment, only
    // use the border-image on non-Mac systems.
    editorPage->setStyleSheet(QLatin1String(
#ifndef Q_WS_MAC
            "QFrame#editorPage { border-image: url(:/images/transbox.png) 12 16 16 12 repeat;"
            "                    border-width: 12px 16px 16px 12px; }"
#endif
            "QFrame#editorPage { background-color: white; }"
            "QLabel { font-weight: bold; }"
            ));
#ifdef Q_WS_MAC
    editorPage->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
#endif
    editorPage->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    m_source = new FormWidget(tr("Source text"), false);
    m_source->setHideWhenEmpty(true);
    m_source->setWhatsThis(tr("This area shows the source text."));
    connect(m_source, SIGNAL(selectionChanged()), SLOT(selectionChanged()));

    m_pluralSource = new FormWidget(tr("Source text (Plural)"), false);
    m_pluralSource->setHideWhenEmpty(true);
    m_pluralSource->setWhatsThis(tr("This area shows the plural form of the source text."));
    connect(m_pluralSource, SIGNAL(selectionChanged()), SLOT(selectionChanged()));

    m_commentText = new FormWidget(tr("Developer comments"), false);
    m_commentText->setHideWhenEmpty(true);
    m_commentText->setObjectName(QLatin1String("comment/context view"));
    m_commentText->setWhatsThis(tr("This area shows a comment that"
                        " may guide you, and the context in which the text"
                        " occurs.") );

    QBoxLayout *subLayout = new QVBoxLayout;

    subLayout->setMargin(5);
    subLayout->addWidget(m_source);
    subLayout->addWidget(m_pluralSource);
    subLayout->addWidget(m_commentText);

    m_layout = new QVBoxLayout;
    m_layout->setSpacing(2);
    m_layout->setMargin(2);
    m_layout->addLayout(subLayout);
    m_layout->addStretch(1);
    editorPage->setLayout(m_layout);

    setWidget(editorPage);
    setWidgetResizable(true);
}

QPalette MessageEditor::paletteForModel(int model) const
{
    QBrush brush = m_dataModel->brushForModel(model);
    QPalette pal;

    if (m_dataModel->isModelWritable(model)) {
        pal.setBrush(QPalette::Window, brush);
    } else {
        QPixmap pm(brush.texture().size());
        pm.fill();
        QPainter p(&pm);
        p.fillRect(brush.texture().rect(), brush);
        pal.setBrush(QPalette::Window, pm);
    }
    return pal;
}

void MessageEditor::messageModelAppended()
{
    int model = m_editors.size();
    m_editors.append(MessageEditorData());
    MessageEditorData &ed = m_editors.last();
    ed.pluralEditMode = false;
    ed.fontSize = font().pointSize();
    ed.container = new QWidget;
    if (model > 0) {
        ed.container->setPalette(paletteForModel(model));
        ed.container->setAutoFillBackground(true);
        if (model == 1) {
            m_editors[0].container->setPalette(paletteForModel(0));
            m_editors[0].container->setAutoFillBackground(true);
        }
    }
    bool writable = m_dataModel->isModelWritable(model);
    ed.transCommentText = new FormWidget(QString(), true);
    ed.transCommentText->setEditingEnabled(writable);
    ed.transCommentText->setHideWhenEmpty(!writable);
    ed.transCommentText->setWhatsThis(tr("Here you can enter comments for your own use."
                        " They have no effect on the translated applications.") );
    ed.transCommentText->getEditor()->installEventFilter(this);
    connect(ed.transCommentText, SIGNAL(selectionChanged()), SLOT(selectionChanged()));
    connect(ed.transCommentText, SIGNAL(textChanged()), SLOT(emitTranslatorCommentChanged()));
    connect(ed.transCommentText, SIGNAL(textChanged()), SLOT(resetHoverSelection()));
    connect(ed.transCommentText, SIGNAL(cursorPositionChanged()), SLOT(resetHoverSelection()));
    QBoxLayout *box = new QVBoxLayout(ed.container);
    box->setMargin(5);
    box->addWidget(ed.transCommentText);
    box->addSpacing(ed.transCommentText->getEditor()->fontMetrics().height() / 2);
    m_layout->addWidget(ed.container);
    setTargetLanguage(model);
}

void MessageEditor::allModelsDeleted()
{
    foreach (const MessageEditorData &med, m_editors)
        med.container->deleteLater();
    m_editors.clear();
    m_currentModel = -1;
    // Do not emit activeModelChanged() - the main window will refresh anyway
    m_currentNumerus = -1;
    showNothing();
}

void MessageEditor::messageModelDeleted(int model)
{
    m_editors[model].container->deleteLater();
    m_editors.removeAt(model);
    if (model <= m_currentModel) {
        if (model < m_currentModel || m_currentModel == m_editors.size())
            --m_currentModel;
        // Do not emit activeModelChanged() - the main window will refresh anyway
        if (m_currentModel >= 0) {
            if (m_currentNumerus >= m_editors[m_currentModel].transTexts.size())
                m_currentNumerus = m_editors[m_currentModel].transTexts.size() - 1;
            activeEditor()->getEditor()->setFocus();
        } else {
            m_currentNumerus = -1;
        }
    }
    if (m_editors.size() == 1) {
        m_editors[0].container->setAutoFillBackground(false);
    } else {
        for (int i = model; i < m_editors.size(); ++i)
            m_editors[i].container->setPalette(paletteForModel(i));
    }
}

void MessageEditor::addPluralForm(int model, const QString &label, bool writable)
{
    FormWidget *transEditor = new FormWidget(label, true);
    QFont font;
    font.setPointSize(static_cast<int>(m_editors[model].fontSize));
    transEditor->getEditor()->setFont(font);
    transEditor->setEditingEnabled(writable);
    transEditor->setHideWhenEmpty(!writable);
    if (!m_editors[model].transTexts.isEmpty())
        transEditor->setVisible(false);
    static_cast<QBoxLayout *>(m_editors[model].container->layout())->insertWidget(
        m_editors[model].transTexts.count(), transEditor);

    transEditor->getEditor()->installEventFilter(this);
    connect(transEditor, SIGNAL(selectionChanged()), SLOT(selectionChanged()));
    connect(transEditor, SIGNAL(textChanged()), SLOT(emitTranslationChanged()));
    connect(transEditor, SIGNAL(textChanged()), SLOT(resetHoverSelection()));
    connect(transEditor, SIGNAL(cursorPositionChanged()), SLOT(resetHoverSelection()));

    m_editors[model].transTexts << transEditor;
}

/*! internal
    Returns all translations for an item.
    The number of translations is dependent on if we have a plural form or not.
    If we don't have a plural form, then this should only contain one item.
    Otherwise it will contain the number of numerus forms for the particular language.
*/
QStringList MessageEditor::translations(int model) const
{
    QStringList translations;
    for (int i = 0; i < m_editors[model].transTexts.count() &&
                    m_editors[model].transTexts.at(i)->isVisible(); ++i)
        translations << m_editors[model].transTexts[i]->getTranslation();
    return translations;
}

static bool clearFormSelection(FormWidget *fw, FormWidget *te)
{
    if (fw != te) {
        QTextEdit *t = fw->getEditor();
        bool oldBlockState = t->blockSignals(true);
        QTextCursor c = t->textCursor();
        c.clearSelection();
        t->setTextCursor(c);
        t->blockSignals(oldBlockState);
        return true;
    }
    return false;
}

// Clear the selection for all textedits except the sender
void MessageEditor::selectionChanged()
{
    if (!resetSelection(qobject_cast<FormWidget *>(sender())))
        updateCanCutCopy();
}

bool MessageEditor::resetHoverSelection(FormWidget *fw)
{
    if (m_sourceSelected) {
        if (clearFormSelection(m_source, fw)) {
            updateCanCutCopy();
            return true;
        }
    } else if (m_pluralSourceSelected) {
        if (clearFormSelection(m_pluralSource, fw)) {
            updateCanCutCopy();
            return true;
        }
    }
    return false;
}

bool MessageEditor::resetSelection(FormWidget *fw)
{
    if (resetHoverSelection(fw))
        return true;
    if (m_currentSelected) {
        MessageEditorData &ed = m_editors[m_currentModel];
        FormWidget *cfw = (m_currentNumerus < 0) ? ed.transCommentText
                                                 : ed.transTexts[m_currentNumerus];
        if (clearFormSelection(cfw, fw)) {
            updateCanCutCopy();
            return true;
        }
    }
    return false;
}

void MessageEditor::activeModelAndNumerus(int *model, int *numerus) const
{
    for (int j = 0; j < m_editors.count(); ++j) {
        for (int i = 0; i < m_editors[j].transTexts.count(); ++i)
            if (m_editors[j].transTexts[i]->getEditor()->hasFocus()) {
                *model = j;
                *numerus = i;
                return;
            }
        if (m_editors[j].transCommentText->getEditor()->hasFocus()) {
            *model = j;
            *numerus = -1;
            return;
        }
    }
    *model = -1;
    *numerus = -1;
}

FormWidget *MessageEditor::activeTranslation() const
{
    if (m_currentNumerus < 0)
        return 0;
    return m_editors[m_currentModel].transTexts[m_currentNumerus];
}

FormWidget *MessageEditor::activeOr1stTranslation() const
{
    if (m_currentNumerus < 0) {
        for (int i = 0; i < m_editors.size(); ++i)
            if (m_editors[i].container->isVisible()
                && !m_editors[i].transTexts[0]->getEditor()->isReadOnly())
                return m_editors[i].transTexts[0];
        return 0;
    }
    return m_editors[m_currentModel].transTexts[m_currentNumerus];
}

FormWidget *MessageEditor::activeTransComment() const
{
    if (m_currentModel < 0 || m_currentNumerus >= 0)
        return 0;
    return m_editors[m_currentModel].transCommentText;
}

FormWidget *MessageEditor::activeEditor() const
{
    if (FormWidget *fw = activeTransComment())
        return fw;
    return activeTranslation();
}

FormWidget *MessageEditor::activeOr1stEditor() const
{
    if (FormWidget *fw = activeTransComment())
        return fw;
    return activeOr1stTranslation();
}

void MessageEditor::setTargetLanguage(int model)
{
    const QStringList &numerusForms = m_dataModel->model(model)->numerusForms();
    const QString &langLocalized = m_dataModel->model(model)->localizedLanguage();
    bool added = false;
    for (int i = 0; i < numerusForms.count(); ++i) {
        const QString &label = tr("%1 translation (%2)").arg(langLocalized, numerusForms[i]);
        if (!i)
            m_editors[model].firstForm = label;
        if (i >= m_editors[model].transTexts.count()) {
            addPluralForm(model, label, m_dataModel->isModelWritable(model));
            QWidget *prev;
            if (i > 0)
                prev = m_editors[model].transTexts[i - 1]->getEditor();
            else if (model)
                prev = m_editors[model - 1].transCommentText->getEditor();
            else
                prev = this;
            setTabOrder(prev, m_editors[model].transTexts[i]->getEditor());
            added = true;
        } else {
            m_editors[model].transTexts[i]->setLabel(label);
        }
        m_editors[model].transTexts[i]->setVisible(!i || m_editors[model].pluralEditMode);
        m_editors[model].transTexts[i]->setWhatsThis(
                tr("This is where you can enter or modify"
                   " the translation of the above source text.") );
    }
    for (int j = m_editors[model].transTexts.count() - numerusForms.count(); j > 0; --j)
        delete m_editors[model].transTexts.takeLast();
    m_editors[model].invariantForm = tr("%1 translation").arg(langLocalized);
    m_editors[model].transCommentText->setLabel(tr("%1 translator comments").arg(langLocalized));
    if (added)
        setTabOrder(m_editors[model].transTexts.last()->getEditor(), m_editors[model].transCommentText->getEditor());
}

MessageEditorData *MessageEditor::modelForWidget(const QObject *o)
{
    for (int j = 0; j < m_editors.count(); ++j) {
        for (int i = 0; i < m_editors[j].transTexts.count(); ++i)
            if (m_editors[j].transTexts[i]->getEditor() == o)
                return &m_editors[j];
        if (m_editors[j].transCommentText->getEditor() == o)
            return &m_editors[j];
    }
    return 0;
}

static bool applyFont(MessageEditorData *med)
{
    QFont font;
    font.setPointSize(static_cast<int>(med->fontSize));
    for (int i = 0; i < med->transTexts.count(); ++i)
        med->transTexts[i]->getEditor()->setFont(font);
    med->transCommentText->getEditor()->setFont(font);
    return true;
}

static bool incFont(MessageEditorData *med)
{
    if (!med || med->fontSize >= 32)
        return true;
    med->fontSize *= 1.2;
    return applyFont(med);
}

static bool decFont(MessageEditorData *med)
{
    if (!med || med->fontSize <= 8)
        return true;
    med->fontSize /= 1.2;
    return applyFont(med);
}

bool MessageEditor::eventFilter(QObject *o, QEvent *e)
{
    // handle copying from the source
    if (e->type() == QEvent::ShortcutOverride) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        if (ke->modifiers() & Qt::ControlModifier) {
            if (ke->key() == Qt::Key_C) {
                if (m_source->getEditor()->textCursor().hasSelection()) {
                    m_source->getEditor()->copy();
                    return true;
                }
                if (m_pluralSource->getEditor()->textCursor().hasSelection()) {
                    m_pluralSource->getEditor()->copy();
                    return true;
                }
            }
        }
    } else if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (ke->modifiers() & Qt::ControlModifier) {
            if (ke->key() == Qt::Key_Plus || ke->key() == Qt::Key_Equal)
                return incFont(modelForWidget(o));
            if (ke->key() == Qt::Key_Minus)
                return decFont(modelForWidget(o));
        } else {
            // Ctrl-Tab is still passed through to the textedit and causes a tab to be inserted.
            if (ke->key() == Qt::Key_Tab) {
                focusNextChild();
                return true;
            }
        }
    } else if (e->type() == QEvent::Wheel) {
        QWheelEvent *we = static_cast<QWheelEvent *>(e);
        if (we->modifiers() & Qt::ControlModifier) {
            if (we->delta() > 0)
                return incFont(modelForWidget(o));
            return decFont(modelForWidget(o));
        }
    } else if (e->type() == QEvent::FocusIn) {
        int model, numerus;
        activeModelAndNumerus(&model, &numerus);
        if (model != m_currentModel || numerus != m_currentNumerus) {
            resetSelection();
            m_currentModel = model;
            m_currentNumerus = numerus;
            emit activeModelChanged(activeModel());
            updateBeginFromSource();
            updateUndoRedo();
            updateCanPaste();
        }
    }

    return QScrollArea::eventFilter(o, e);
}

void MessageEditor::showNothing()
{
    m_source->clearTranslation();
    m_pluralSource->clearTranslation();
    m_commentText->clearTranslation();
    for (int j = 0; j < m_editors.count(); ++j) {
        setEditingEnabled(j, false);
        foreach (FormWidget *widget, m_editors[j].transTexts)
            widget->clearTranslation();
        m_editors[j].transCommentText->clearTranslation();
    }
    emit pasteAvailable(false);
    updateBeginFromSource();
    updateUndoRedo();
}

void MessageEditor::showMessage(const MultiDataIndex &index)
{
    m_currentIndex = index;

    bool hadMsg = false;
    for (int j = 0; j < m_editors.size(); ++j) {

        MessageEditorData &ed = m_editors[j];

        MessageItem *item = m_dataModel->messageItem(index, j);
        if (!item) {
            ed.container->hide();
            continue;
        }
        ed.container->show();

        if (!hadMsg) {

            // Source text form
            m_source->setTranslation(item->text());
            m_pluralSource->setTranslation(item->pluralText());
            // Use location from first non-obsolete message
            if (!item->fileName().isEmpty()) {
                QString toolTip = tr("'%1'\nLine: %2").arg(item->fileName(), QString::number(item->lineNumber()));
                m_source->setToolTip(toolTip);
            } else {
                m_source->setToolTip(QLatin1String(""));
            }

            // Comment field
            QString commentText = item->comment().simplified();

            if (!item->extraComment().isEmpty()) {
                if (!commentText.isEmpty())
                    commentText += QLatin1String("\n");
                commentText += item->extraComment().simplified();
            }

            m_commentText->setTranslation(commentText);

            hadMsg = true;
        }

        setEditingEnabled(j, m_dataModel->isModelWritable(j)
                             && item->message().type() != TranslatorMessage::Obsolete);

        // Translation label
        ed.pluralEditMode = item->translations().count() > 1;
        ed.transTexts.first()->setLabel(ed.pluralEditMode ? ed.firstForm : ed.invariantForm);

        // Translation forms
        if (item->text().isEmpty() && !item->context().isEmpty()) {
            for (int i = 0; i < ed.transTexts.size(); ++i)
                ed.transTexts.at(i)->setVisible(false);
        } else {
            QStringList normalizedTranslations =
                m_dataModel->model(j)->normalizedTranslations(*item);
            for (int i = 0; i < ed.transTexts.size(); ++i) {
                bool shouldShow = (i < normalizedTranslations.count());
                if (shouldShow)
                    setTranslation(j, normalizedTranslations.at(i), i);
                else
                    setTranslation(j, QString(), i);
                ed.transTexts.at(i)->setVisible(i == 0 || shouldShow);
            }
        }

        ed.transCommentText->setTranslation(item->translatorComment().trimmed(), false);
    }

    updateUndoRedo();
}

void MessageEditor::setTranslation(int model, const QString &translation, int numerus)
{
    MessageEditorData &ed = m_editors[model];
    if (numerus >= ed.transTexts.count())
        numerus = 0;
    FormWidget *transForm = ed.transTexts[numerus];
    transForm->setTranslation(translation, false);

    updateBeginFromSource();
}

void MessageEditor::setTranslation(int latestModel, const QString &translation)
{
    int numerus;
    if (m_currentNumerus < 0) {
        numerus = 0;
    } else {
        latestModel = m_currentModel;
        numerus = m_currentNumerus;
    }
    FormWidget *transForm = m_editors[latestModel].transTexts[numerus];
    transForm->getEditor()->setFocus();
    transForm->setTranslation(translation, true);

    updateBeginFromSource();
}

void MessageEditor::setEditingEnabled(int model, bool enabled)
{
    MessageEditorData &ed = m_editors[model];
    foreach (FormWidget *widget, ed.transTexts)
        widget->setEditingEnabled(enabled);
    ed.transCommentText->setEditingEnabled(enabled);

    updateCanPaste();
}

void MessageEditor::undo()
{
    activeEditor()->getEditor()->document()->undo();
}

void MessageEditor::redo()
{
    activeEditor()->getEditor()->document()->redo();
}

void MessageEditor::updateUndoRedo()
{
    bool newUndoAvail = false;
    bool newRedoAvail = false;
    if (FormWidget *fw = activeEditor()) {
        QTextDocument *doc = fw->getEditor()->document();
        newUndoAvail = doc->isUndoAvailable();
        newRedoAvail = doc->isRedoAvailable();
    }

    if (newUndoAvail != m_undoAvail) {
        m_undoAvail = newUndoAvail;
        emit undoAvailable(newUndoAvail);
    }

    if (newRedoAvail != m_redoAvail) {
        m_redoAvail = newRedoAvail;
        emit redoAvailable(newRedoAvail);
    }
}

void MessageEditor::cut()
{
    QTextEdit *editor = activeEditor()->getEditor();
    if (editor->textCursor().hasSelection())
        editor->cut();
}

void MessageEditor::copy()
{
    QTextEdit *te;
    if ((te = m_source->getEditor())->textCursor().hasSelection()
        || (te = m_pluralSource->getEditor())->textCursor().hasSelection()
        || (te = activeEditor()->getEditor())->textCursor().hasSelection())
        te->copy();
}

void MessageEditor::updateCanCutCopy()
{
    bool newCopyState = false;
    bool newCutState = false;

    m_sourceSelected = m_source->getEditor()->textCursor().hasSelection();
    m_pluralSourceSelected = m_pluralSource->getEditor()->textCursor().hasSelection();
    m_currentSelected = false;

    if (m_sourceSelected || m_pluralSourceSelected) {
        newCopyState = true;
    } else if (FormWidget *fw = activeEditor()) {
        QTextEdit *te = fw->getEditor();
        if (te->textCursor().hasSelection()) {
            m_currentSelected = true;
            newCopyState = true;
            newCutState = !te->isReadOnly();
        }
    }

    if (newCopyState != m_copyAvail) {
        m_copyAvail = newCopyState;
        emit copyAvailable(m_copyAvail);
    }

    if (newCutState != m_cutAvail) {
        m_cutAvail = newCutState;
        emit cutAvailable(m_cutAvail);
    }
}

void MessageEditor::paste()
{
    activeEditor()->getEditor()->paste();
}

void MessageEditor::updateCanPaste()
{
    FormWidget *fw;
    emit pasteAvailable(!m_clipboardEmpty
                        && (fw = activeEditor()) && !fw->getEditor()->isReadOnly());
}

void MessageEditor::clipboardChanged()
{
    // this is expensive, so move it out of the common path in updateCanPaste
    m_clipboardEmpty = qApp->clipboard()->text().isNull();
    updateCanPaste();
}

void MessageEditor::selectAll()
{
    // make sure we don't select the selection of a translator textedit,
    // if we really want the source text editor to be selected.
    QTextEdit *te;
    FormWidget *fw;
    if ((te = m_source->getEditor())->underMouse()
        || (te = m_pluralSource->getEditor())->underMouse()
        || ((fw = activeEditor()) && (te = fw->getEditor())->hasFocus()))
        te->selectAll();
}

void MessageEditor::emitTranslationChanged()
{
    updateBeginFromSource();
    updateUndoRedo();
    emit translationChanged(translations(m_currentModel));
}

void MessageEditor::emitTranslatorCommentChanged()
{
    updateUndoRedo();
    emit translatorCommentChanged(m_editors[m_currentModel].transCommentText->getTranslation());
}

void MessageEditor::updateBeginFromSource()
{
    bool overwrite = false;
    if (FormWidget *transForm = activeTranslation()) {
        QTextEdit *activeEditor = transForm->getEditor();
        overwrite = !activeEditor->isReadOnly()
            && activeEditor->toPlainText().trimmed().isEmpty();
    }
    emit beginFromSourceAvailable(overwrite);
}

void MessageEditor::beginFromSource()
{
    MessageItem *item = m_dataModel->messageItem(m_currentIndex, m_currentModel);
    setTranslation(m_currentModel,
                   m_currentNumerus > 0 && !item->pluralText().isEmpty() ?
                        item->pluralText() : item->text());
}

void MessageEditor::setEditorFocus()
{
    if (!widget()->hasFocus())
        if (FormWidget *transForm = activeOr1stEditor())
            transForm->getEditor()->setFocus();
}

void MessageEditor::setEditorFocus(int model)
{
    if (m_currentModel != model) {
        if (model < 0) {
            resetSelection();
            m_currentNumerus = -1;
            m_currentModel = -1;
            emit activeModelChanged(activeModel());
            updateBeginFromSource();
            updateUndoRedo();
            updateCanPaste();
        } else {
            m_editors[model].transTexts[0]->getEditor()->setFocus();
        }
    }
}

bool MessageEditor::focusNextUnfinished(int start)
{
    for (int j = start; j < m_editors.count(); ++j)
        if (m_dataModel->isModelWritable(j))
            if (MessageItem *item = m_dataModel->messageItem(m_currentIndex, j))
                if (item->type() == TranslatorMessage::Unfinished) {
                    m_editors[j].transTexts[0]->getEditor()->setFocus();
                    return true;
                }
    return false;
}

void MessageEditor::setUnfinishedEditorFocus()
{
    focusNextUnfinished(0);
}

bool MessageEditor::focusNextUnfinished()
{
    return focusNextUnfinished(m_currentModel + 1);
}

QT_END_NAMESPACE
