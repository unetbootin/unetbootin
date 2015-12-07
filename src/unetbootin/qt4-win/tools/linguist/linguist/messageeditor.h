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

#ifndef MSGEDIT_H
#define MSGEDIT_H

#include "messagemodel.h"

#include <QFrame>
#include <QLocale>
#include <QScrollArea>
#include <QShortcut>

QT_BEGIN_NAMESPACE

class QBoxLayout;
class QDockWidget;
class QMainWindow;
template <class T, typename U> class QMap;
class QMenu;

class QTextEdit;
class QTreeView;

class Phrase;
class PhraseModel;
class EditorPage;
class ExpandingTextEdit;
class LanguagesManager;
class MessageEditor;
class FormatTextEdit;
class FormWidget;

class EditorPage : public QFrame
{
    Q_OBJECT

public:
    EditorPage(QLocale::Language targetLanguage, MessageEditor *parent = 0, const char *name = 0);
    FormWidget *activeTranslation() const;
    int activeTranslationNumerus() const;
    QStringList translations() const;
    void setNumerusForms(const QStringList &numerusForms);
    int currentTranslationEditor();

public slots:
    void setTargetLanguage(QLocale::Language lang);

protected:
    void updateCommentField();

private:
    void addPluralForm(const QString &label);
    void handleChanges();
    void showNothing();
    void setAuxLanguages(const QStringList &languages);

    QBoxLayout *m_layout;
    FormWidget *m_source;
    QList<FormWidget *> auxTranslations;
    FormatTextEdit *cmtText;
    QLocale::Language m_targetLanguage;
    QStringList m_numerusForms;
    QString     m_invariantForm;
    bool        m_pluralEditMode;
    QList<FormWidget*> m_transTexts;
    friend class MessageEditor;

private slots:
    void sourceSelectionChanged();
    void translationSelectionChanged();

signals:
    void selectionChanged();
    void currentTranslationEditorChanged();
};

class MessageEditor : public QScrollArea
{
    Q_OBJECT
public:
    MessageEditor(LanguagesManager *languages, QMainWindow *parent = 0);

    void showNothing();
    void showMessage(const MessageItem *item, const ContextItem *context);
    void setNumerusForms(const QStringList &numerusForms);
    bool eventFilter(QObject *, QEvent *);
    void setTranslation(const QString &translation, int numerus);

signals:
    void translationChanged(const QStringList &translations);
    void finished(bool finished);
    void prevUnfinished();
    void nextUnfinished();
    void updateActions(bool enable);

    void undoAvailable(bool avail);
    void redoAvailable(bool avail);
    void cutAvailable(bool avail);
    void copyAvailable(bool avail);
    void pasteAvailable(bool avail);

public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void beginFromSource();
    void setEditorFocus();
    void updateUndoRedo();
    void messageModelListChanged();
    void setTranslation(const QString &translation);

private slots:
    void emitTranslationChanged();
    void updateButtons();
    void updateCanPaste();
    void clipboardChanged();

    void updateCutAndCopy();

private:
    void setEditingEnabled(bool enabled);

    LanguagesManager *m_languages;

    EditorPage *editorPage;

    const MessageItem *m_currentMessage;
    const ContextItem *m_currentContext;

    bool cutAvail;
    bool copyAvail;

    bool mayOverwriteTranslation;
    bool clipboardEmpty;
};

QT_END_NAMESPACE

#endif
