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

#ifndef MESSAGEEDITOR_H
#define MESSAGEEDITOR_H

#include "messagemodel.h"

#include <QtCore/QLocale>

#include <QtGui/QFrame>
#include <QtGui/QScrollArea>

QT_BEGIN_NAMESPACE

class QBoxLayout;
class QMainWindow;
class QTextEdit;

class MessageEditor;
class FormatTextEdit;
class FormWidget;

struct MessageEditorData {
    QWidget *container;
    FormWidget *transCommentText;
    QList<FormWidget*> transTexts;
    QString invariantForm;
    QString firstForm;
    float fontSize;
    bool pluralEditMode;
};

class MessageEditor : public QScrollArea
{
    Q_OBJECT

public:
    MessageEditor(MultiDataModel *dataModel, QMainWindow *parent = 0);

    void showNothing();
    void showMessage(const MultiDataIndex &index);
    void setNumerusForms(int model, const QStringList &numerusForms);
    bool eventFilter(QObject *, QEvent *);
    void setTranslation(int model, const QString &translation, int numerus);
    int activeModel() const { return (m_editors.count() != 1) ? m_currentModel : 0; }
    void setEditorFocus(int model);
    void setUnfinishedEditorFocus();
    bool focusNextUnfinished();

signals:
    void translationChanged(const QStringList &translations);
    void translatorCommentChanged(const QString &comment);
    void activeModelChanged(int model);

    void undoAvailable(bool avail);
    void redoAvailable(bool avail);
    void cutAvailable(bool avail);
    void copyAvailable(bool avail);
    void pasteAvailable(bool avail);
    void beginFromSourceAvailable(bool enable);

public slots:
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void beginFromSource();
    void setEditorFocus();
    void setTranslation(int latestModel, const QString &translation);

private slots:
    void selectionChanged();
    bool resetHoverSelection(FormWidget *fw = 0);
    void emitTranslationChanged();
    void emitTranslatorCommentChanged();
    void updateCanPaste();
    void clipboardChanged();
    void messageModelAppended();
    void messageModelDeleted(int model);
    void allModelsDeleted();
    void setTargetLanguage(int model);

private:
    void setupEditorPage();
    void setEditingEnabled(int model, bool enabled);
    bool focusNextUnfinished(int start);
    bool resetSelection(FormWidget *fw = 0);
    void activeModelAndNumerus(int *model, int *numerus) const;
    FormWidget *activeTranslation() const;
    FormWidget *activeOr1stTranslation() const;
    FormWidget *activeTransComment() const;
    FormWidget *activeEditor() const;
    FormWidget *activeOr1stEditor() const;
    MessageEditorData *modelForWidget(const QObject *o);
    int activeTranslationNumerus() const;
    QStringList translations(int model) const;
    void updateBeginFromSource();
    void updateUndoRedo();
    void updateCanCutCopy();
    void addPluralForm(int model, const QString &label, bool writable);
    QPalette paletteForModel(int model) const;

    MultiDataModel *m_dataModel;

    MultiDataIndex m_currentIndex;
    int m_currentModel;
    int m_currentNumerus;

    bool m_undoAvail;
    bool m_redoAvail;
    bool m_cutAvail;
    bool m_copyAvail;
    bool m_sourceSelected;
    bool m_pluralSourceSelected;
    bool m_currentSelected;

    bool m_clipboardEmpty;

    QBoxLayout *m_layout;
    FormWidget *m_source;
    FormWidget *m_pluralSource;
    FormWidget *m_commentText;
    QList<MessageEditorData> m_editors;
};

QT_END_NAMESPACE

#endif // MESSAGEEDITOR_H
