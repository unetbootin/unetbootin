/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

/*  TRANSLATOR MsgEdit

  This is the right panel of the main window.
*/

#include "messageeditor.h"
#include "messageeditorwidgets.h"
#include "simtexth.h"
#include "phrasemodel.h"
#include "languagesmanager.h"

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QDockWidget>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMainWindow>
#include <QTreeView>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE


// Allow translators to provide localized names for QLocale::languageToString
// At least the own language should be translated ... This is a "hack" until
// functionality is provided within Qt (see task 196275).
static const char * language_strings[] = 
{
    QT_TRANSLATE_NOOP("EditorPage", "German"),
    QT_TRANSLATE_NOOP("EditorPage", "Japanese"),
    QT_TRANSLATE_NOOP("EditorPage", "French"),
    QT_TRANSLATE_NOOP("EditorPage", "Polish"),
    QT_TRANSLATE_NOOP("EditorPage", "Chinese")
};


/*
   EditorPage class impl.

   A frame that contains the source text, translated text and any
   source code comments and hints.
*/
EditorPage::EditorPage(QLocale::Language targetLanguage, MessageEditor *parent, const char *name)
    : QFrame(parent),
      m_layout(new QVBoxLayout),
      m_targetLanguage(targetLanguage)
{
    setObjectName(QLatin1String(name));

    // Due to CSS being rather broken on the Mac style at the moment, only
    // use the border-image on non-Mac systems.
    setStyleSheet(QLatin1String(
#ifndef Q_WS_MAC
            "EditorPage { border-image: url(:/images/transbox.png) 12 16 16 12 repeat;"
            "             border-width: 12px 16px 16px 12px; }"
#endif
            "EditorPage { background-color: white; }"
            "QLabel { font-weight: bold; }"
            ));
#ifdef Q_WS_MAC
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
#endif
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    m_layout->setMargin(2);

    // Use white explicitly as the background color for the editor page.
    QPalette p = palette();
    p.setColor(QPalette::Active,   QPalette::Base,   Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Base,   Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Base,   Qt::white);
    p.setColor(QPalette::Active,   QPalette::Window, Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Window, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Window, Qt::white);
    parent->setPalette(p);


    m_source = new FormWidget(tr("Source text"), false, this);
    m_source->setWhatsThis(tr("This area shows the source text."));
    m_layout->addWidget(m_source);

    connect(m_source, SIGNAL(selectionChanged()), SLOT(sourceSelectionChanged()));

    cmtText = new FormatTextEdit(this);
    cmtText->setObjectName(QLatin1String("comment/context view"));
    cmtText->setEditable(false);
    p = cmtText->palette();
    p.setColor(QPalette::Active, QPalette::Base, QColor(236, 245, 255));
    p.setColor(QPalette::Inactive, QPalette::Base, QColor(236, 245, 255));
    cmtText->setPalette(p);
    cmtText->setWhatsThis(tr("This area shows a comment that"
                        " may guide you, and the context in which the text"
                        " occurs.") );
    m_layout->addWidget(cmtText);
    m_layout->addStretch(1);

    setTargetLanguage(targetLanguage);
    m_pluralEditMode = false;
    addPluralForm(m_invariantForm);

    updateCommentField();
    setLayout(m_layout);
}

void EditorPage::showNothing()
{
    m_source->clearTranslation();
    cmtText->clear();
    foreach (FormWidget *widget, m_transTexts)
        widget->clearTranslation();
    foreach (FormWidget *widget, auxTranslations)
        widget->clearTranslation();
    handleChanges();
}

void EditorPage::setAuxLanguages(const QStringList &languages)
{
    qDeleteAll(auxTranslations);
    auxTranslations.clear();

    foreach (const QString &language, languages) {
        FormWidget *auxWidget = new FormWidget(tr("Existing %1 translation").arg(language), false, this);
        auxWidget->setHideWhenEmpty(true);
        m_layout->insertWidget(m_layout->indexOf(cmtText), auxWidget);

        connect(auxWidget, SIGNAL(selectionChanged()), SLOT(sourceSelectionChanged()));

        auxTranslations.append(auxWidget);
    }
}

void EditorPage::handleChanges()
{
    updateCommentField();
}

void EditorPage::addPluralForm(const QString &label)
{
    FormWidget *transEditor = new FormWidget(label, true, this);
    if (m_transTexts.count())
        transEditor->setVisible(false);
    m_layout->insertWidget(m_layout->count() - 1, transEditor);

    // Setting tab order explicitly is needed for MS Windows
    FormWidget *prevFormWidget = 0;
    for (int prevWidgetPos = m_layout->indexOf(transEditor);
         --prevWidgetPos > 0 && !prevFormWidget;)
    {
        QLayoutItem *item = m_layout->itemAt(prevWidgetPos);
        prevFormWidget = qobject_cast<FormWidget*>(item->widget());
    }
    if (prevFormWidget)
        setTabOrder(prevFormWidget->getEditor(), transEditor->getEditor());

    connect(transEditor->getEditor(), SIGNAL(selectionChanged()), SLOT(translationSelectionChanged()));

    m_transTexts << transEditor;
}

int EditorPage::currentTranslationEditor()
{
    for (int i = 0; i < m_transTexts.count(); ++i) {
        QTextEdit *te = m_transTexts[i]->getEditor();
        if (te->hasFocus()) return i;
    }
    return -1;  //no focus
}

/*! internal
    Returns all translations for an item.
    The number of translations is dependent on if we have a plural form or not.
    If we don't have a plural form, then this should only contain one item.
    Otherwise it will contain the number of numerus forms for the particular language.
*/
QStringList EditorPage::translations() const
{
    QStringList translations;
    for (int i = 0; i < m_transTexts.count() && m_transTexts.at(i)->isVisible(); ++i) {
        translations << m_transTexts[i]->getTranslation();
    }
    return translations;
}

/*
   Don't show the comment field if there are no comments.
*/
void EditorPage::updateCommentField()
{
    cmtText->setVisible(!cmtText->toPlainText().isEmpty());
}

// makes sure only one of the textedits has a selection
void EditorPage::sourceSelectionChanged()
{
    for (int i = 0; i < m_transTexts.count(); ++i) {
        QTextEdit *te = m_transTexts[i]->getEditor();
        bool oldBlockState = te->blockSignals(true);
        QTextCursor c = te->textCursor();
        c.clearSelection();
        te->setTextCursor(c);
        te->blockSignals(oldBlockState);
    }
    emit selectionChanged();
}

void EditorPage::translationSelectionChanged()
{
    bool oldBlockState = m_source->getEditor()->blockSignals(true);
    QTextCursor c = m_source->getEditor()->textCursor();
    c.clearSelection();
    m_source->getEditor()->setTextCursor(c);
    m_source->getEditor()->blockSignals(oldBlockState);

    // clear the selection for all except the sender
    QTextEdit *te = qobject_cast<QTextEdit*>(sender());
    for (int i = 0; i < m_transTexts.count(); ++i) {
        QTextEdit *t = m_transTexts[i]->getEditor();
        if (t != te) {
            oldBlockState = t->blockSignals(true);
            QTextCursor c = t->textCursor();
            c.clearSelection();
            t->setTextCursor(c);
            t->blockSignals(oldBlockState);
        }
    }

    emit selectionChanged();
}

int EditorPage::activeTranslationNumerus() const
{
    for (int i = 0; i < m_transTexts.count(); ++i) {
        if (m_transTexts[i]->getEditor()->hasFocus()) {
            return i;
        }
    }
    //### hmmm.....
    if (m_transTexts.count()) return 0;
    return -1;
}

void EditorPage::setTargetLanguage(QLocale::Language lang)
{
    m_targetLanguage = lang;
    if (lang == QLocale::C || uint(lang) > uint(QLocale::LastLanguage))
        m_invariantForm = tr("Translation");
    else {
        const QString &langLocalized = tr(QLocale::languageToString(lang).toAscii());
        m_invariantForm = tr("%1 translation").arg(langLocalized);
    }
}

void EditorPage::setNumerusForms(const QStringList &numerusForms)
{
    m_numerusForms = numerusForms;
    const QString &lang = tr(QLocale::languageToString(m_targetLanguage).toAscii());

    if (!m_pluralEditMode) {
        m_transTexts[0]->setLabel(m_invariantForm);
    } else {
        m_transTexts[0]->setLabel(tr("%1 translation (%2)").arg(lang, m_numerusForms[0]));
    }
    for (int i = 1; i < m_numerusForms.count(); ++i) {
        QString label = tr("%1 translation (%2)").arg(lang, m_numerusForms[i]);
        if (i >= m_transTexts.count()) {
            addPluralForm(label);
        } else {
            m_transTexts[i]->setLabel(label);
        }
        m_transTexts[i]->setVisible(m_pluralEditMode);
        m_transTexts[i]->setWhatsThis(tr("This is where you can enter or modify"
                                         " the translation of some source text.") );
    }

    // delete superfluous translation text widgets
    // (if plural forms are not specified for languages, numerusForms will be 0)
    int numerusFormCount = numerusForms.count() ? numerusForms.count() : 1;
    for (int j = m_transTexts.count() - numerusFormCount; j > 0; --j) {
        FormWidget *te = m_transTexts.takeLast();
        delete te;
    }
}

FormWidget *EditorPage::activeTranslation() const
{
    int numerus = activeTranslationNumerus();
    if (numerus != -1) {
        return m_transTexts[numerus];
    }
    return 0;
}

/*
   MessageEditor class impl.

   Handles layout of dock windows and the editor page.
*/
MessageEditor::MessageEditor(LanguagesManager *languages, QMainWindow *parent)
    : QScrollArea(parent->centralWidget()),
      m_languages(languages),
      cutAvail(false),
      copyAvail(false)
{
    setObjectName(QLatin1String("scroll area"));

    editorPage = new EditorPage(m_languages->mainModel()->language(), this, "editor page");

    setWidget(editorPage);
    setWidgetResizable(true);

    // Signals
    connect(this, SIGNAL(translationChanged(const QStringList &)),
            this, SLOT(updateButtons()));

    connect(this, SIGNAL(translationChanged(const QStringList &)),
            this, SLOT(updateUndoRedo()));

    connect(editorPage, SIGNAL(selectionChanged()),
            this, SLOT(updateCutAndCopy()));

    connect(qApp->clipboard(), SIGNAL(dataChanged()),
            this, SLOT(clipboardChanged()));

    connect(m_languages, SIGNAL(listChanged()), this, SLOT(messageModelListChanged()));
    connect(m_languages->mainModel(), SIGNAL(languageChanged(QLocale::Language)), editorPage, SLOT(setTargetLanguage(QLocale::Language)));

    clipboardChanged();

    this->setWhatsThis(tr("This whole panel allows you to view and edit "
                          "the translation of some source text."));
    showNothing();
}

void MessageEditor::updateUndoRedo()
{
    FormWidget *translation = editorPage->activeTranslation();
    emit undoAvailable(translation->getEditor()->document()->isUndoAvailable());
    emit redoAvailable(translation->getEditor()->document()->isRedoAvailable());
}

void MessageEditor::messageModelListChanged()
{
    QStringList languages;
    foreach (MessageModel *model, m_languages->auxModels()) {
        languages << tr(QLocale::languageToString(model->language()).toAscii());
    }
    editorPage->setAuxLanguages(languages);
    editorPage->setTargetLanguage(m_languages->mainModel()->language());
    showMessage(m_currentMessage, m_currentContext);
}

bool MessageEditor::eventFilter(QObject *o, QEvent *e)
{
    // handle copying from the source
    if ((e->type() == QEvent::KeyPress) ||
        (e->type() == QEvent::ShortcutOverride))
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        if (ke->modifiers() & Qt::ControlModifier)
        {
            if ((ke->key() == Qt::Key_A) &&
                editorPage->m_source->getEditor()->underMouse())
            {
                //editorPage->m_source->getEditor()->selectAll();
                return false;
            }
            if ((ke->key() == Qt::Key_C) &&
                editorPage->m_source->getEditor()->textCursor().hasSelection())
            {
                editorPage->m_source->getEditor()->copySelection();
                return true;
            }
        }
    }

    return QScrollArea::eventFilter(o, e);
}

void MessageEditor::showNothing()
{
    setEditingEnabled(false);
    editorPage->showNothing();
}

void MessageEditor::showMessage(const MessageItem *item, const ContextItem *context)
{
    m_currentMessage = item;
    m_currentContext = context;

    setEditingEnabled(!(item->message().type() == TranslatorMessage::Obsolete));

    // Source text form
    editorPage->m_source->setTranslation(item->sourceText());
    if (!item->fileName().isEmpty()) {
        QString toolTip = tr("'%1'\nLine: %2").arg(item->fileName(), QString::number(item->lineNumber()));
        editorPage->m_source->setToolTip(toolTip);
    } else {
        editorPage->m_source->setToolTip(QLatin1String(""));
    }

    // Auxiliary language forms
    QList<MessageModel *> models = m_languages->auxModels();
    for (int i = 0; i < models.size(); i++) {
        editorPage->auxTranslations.at(i)->setTranslation(models.at(i)->translator()->translate(
            item->context().toLatin1(),
            item->sourceText().toLatin1(),
            item->comment().toLatin1()));
    }

    // Comment field
    QString commentText;
    if (!context->fullContext().isEmpty() && !item->comment().isEmpty())
        commentText = context->fullContext().simplified()
                    + QLatin1String("\n")
                    + item->comment().simplified();
    else if (!context->fullContext().isEmpty() && item->comment().isEmpty())
        commentText = context->fullContext().simplified();
    else if (context->fullContext().isEmpty() && !item->comment().isEmpty())
        commentText = item->comment().simplified();

    // Skip the formatting of special characters
    editorPage->cmtText->ExpandingTextEdit::setPlainText(commentText);

    // Translation label
    editorPage->m_pluralEditMode = item->translations().count() > 1;
    if (!editorPage->m_pluralEditMode) {
        editorPage->m_transTexts.first()->setLabel(editorPage->m_invariantForm);
    } else {
        const QString &lang = tr(QLocale::languageToString(editorPage->m_targetLanguage).toAscii());
        editorPage->m_transTexts.first()->setLabel(tr("%1 Translation (%2)").arg(lang,
                                                                            editorPage->m_numerusForms.first()));
    }

    // Translation forms
    QList<QString> normalizedTranslations = m_languages->mainModel()->normalizedTranslations(*item);
    for (int i = 0; i < qMax(1, editorPage->m_numerusForms.count()); ++i) {
        bool shouldShow = i < normalizedTranslations.count();
        if (shouldShow) {
            setTranslation(normalizedTranslations.at(i), i);
        } else {
            setTranslation(QString(), i);
        }
        if (i >= 1)
            editorPage->m_transTexts.at(i)->setVisible(shouldShow);
    }

    editorPage->handleChanges();
    updateUndoRedo();
}

void MessageEditor::setNumerusForms(const QStringList &numerusForms)
{
    // uninstall the emitTranslationChanged slots and remove event filters
    for (int i = 0; i  < editorPage->m_transTexts.count(); ++i) {
        QTextEdit *transText = editorPage->m_transTexts[i]->getEditor();
        disconnect(transText->document(), SIGNAL(contentsChanged()), this, SLOT(emitTranslationChanged()));
        transText->removeEventFilter(this);
    }
    editorPage->setNumerusForms(numerusForms);

    // reinstall event filters and set up the emitTranslationChanged slot
    for (int i = 0; i  < editorPage->m_transTexts.count(); ++i) {
        QTextEdit *transText = editorPage->m_transTexts[i]->getEditor();
        transText->installEventFilter(this);
        connect(transText->document(), SIGNAL(contentsChanged()),
                this, SLOT(emitTranslationChanged()));
    }
}

void MessageEditor::setTranslation(const QString &translation, int numerus)
{
    if (numerus >= editorPage->m_transTexts.count()) numerus = 0;
    FormWidget *transForm = editorPage->m_transTexts[numerus];
    transForm->setTranslation(translation, false);

    emit cutAvailable(false);
    emit copyAvailable(false);
    updateButtons();
}

void MessageEditor::setTranslation(const QString &translation)
{
    FormWidget *transForm = editorPage->activeTranslation();
    transForm->getEditor()->setFocus();
    transForm->setTranslation(translation, true);

    emit cutAvailable(false);
    emit copyAvailable(false);
    updateButtons();
}

void MessageEditor::setEditingEnabled(bool enabled)
{
    foreach (FormWidget *widget, editorPage->m_transTexts)
        widget->setEditingEnabled(enabled);

    updateCanPaste();
}

void MessageEditor::undo()
{
    editorPage->activeTranslation()->getEditor()->document()->undo();
}

void MessageEditor::redo()
{
    editorPage->activeTranslation()->getEditor()->document()->redo();
}

void MessageEditor::cut()
{
    QTextEdit *editor = editorPage->activeTranslation()->getEditor();
    if (editor->textCursor().hasSelection())
        editor->cut();
}

void MessageEditor::copy()
{
    if (editorPage->m_source->getEditor()->textCursor().hasSelection()) {
        editorPage->m_source->getEditor()->copySelection();
    } else if (editorPage->activeTranslation()->getEditor()->textCursor().hasSelection()) {
        editorPage->activeTranslation()->getEditor()->copySelection();
    }
}

void MessageEditor::paste()
{
    editorPage->activeTranslation()->getEditor()->paste();
}

void MessageEditor::selectAll()
{
    // make sure we don't select the selection of a translator textedit, if we really want the
    // source text editor to be selected.
    if (!editorPage->m_source->getEditor()->underMouse())
        editorPage->activeTranslation()->getEditor()->selectAll();
    else
        editorPage->m_source->getEditor()->selectAll();
}

void MessageEditor::emitTranslationChanged()
{
    emit translationChanged(editorPage->translations());
}

void MessageEditor::updateButtons()
{
    QTextEdit *activeEditor = editorPage->activeTranslation()->getEditor();
    bool overwrite = (!activeEditor->isReadOnly() &&
             (activeEditor->toPlainText().trimmed().isEmpty() ||
              mayOverwriteTranslation));
    mayOverwriteTranslation = false;
    emit updateActions(overwrite);
}

void MessageEditor::beginFromSource()
{
    mayOverwriteTranslation = true;
    setTranslation(m_currentMessage->sourceText());
    setEditorFocus();
}

void MessageEditor::setEditorFocus()
{
    if (!editorPage->hasFocus())
        editorPage->activeTranslation()->getEditor()->setFocus();
}

void MessageEditor::updateCutAndCopy()
{
    bool newCopyState = false;
    bool newCutState = false;
    if (editorPage->m_source->getEditor()->textCursor().hasSelection()) {
        newCopyState = true;
    } else if (editorPage->activeTranslation()->getEditor()->textCursor().hasSelection()) {
        newCopyState = true;
        newCutState = true;
    }

    if (newCopyState != copyAvail) {
        copyAvail = newCopyState;
        emit copyAvailable(copyAvail);
    }

    if (newCutState != cutAvail) {
        cutAvail = newCutState;
        emit cutAvailable(cutAvail);
    }
}

void MessageEditor::clipboardChanged()
{
    // this is expensive, so move it out of the common path in updateCanPaste
    clipboardEmpty = qApp->clipboard()->text().isNull();
    updateCanPaste();
}

void MessageEditor::updateCanPaste()
{
    emit pasteAvailable(!editorPage->activeTranslation()->getEditor()->isReadOnly() && !clipboardEmpty);
}

QT_END_NAMESPACE
