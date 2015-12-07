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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include "shared_global_p.h"

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

class QPushButton;

namespace qdesigner_internal {

/* A line edit that displays a grayed hintText (like "Type Here to Filter")
 * when not focussed and empty. When connecting to the changed signals and
 * querying text, one has to be aware that the text is set to that hint
 * text if isShowingHintText() returns true (that is, does not contain
 * valid user input). This widget should never have initial focus
 * (ie, be the first widget of a dialog, else, the hint cannot be displayed.
 * For situations, where it is the only focusable control (widget box),
 * there is a special "refuseFocus()" mode, in which it clears the focus
 * policy and focusses explicitly on click (note that setting Qt::ClickFocus
 * is not sufficient for that as an ActivationFocus will occur). */

class QDESIGNER_SHARED_EXPORT HintLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit HintLineEdit(QWidget *parent = 0);

    QString hintText() const;

    bool isShowingHintText() const;

    // Convenience for accessing the text that returns "" in case of isShowingHintText().
    QString typedText() const;

    bool refuseFocus() const;
    void setRefuseFocus(bool v);

public slots:
    void setHintText(const QString &ht);
    void showHintText(bool force = false);
    void hideHintText();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private:
    void setTextColor(const QColor &newColor, QColor *oldColor = 0);

    const Qt::FocusPolicy m_defaultFocusPolicy;
    const QColor m_hintColor;
    QColor m_textColor;
    bool m_refuseFocus;
    QString m_hintText;
    bool m_showingHintText;
};

// FilterWidget: For filtering item views, with reset button Uses HintLineEdit.

class  QDESIGNER_SHARED_EXPORT FilterWidget : public QWidget
{
    Q_OBJECT
public:
    enum LayoutMode {
        // For use in toolbars: Expand to the right
        LayoutAlignRight,
        // No special alignment
        LayoutAlignNone
    };

    explicit FilterWidget(QWidget *parent = 0, LayoutMode lm = LayoutAlignRight);

    QString text() const;

    bool refuseFocus() const; // see HintLineEdit
    void setRefuseFocus(bool v);

signals:
    void filterChanged(const QString &);

public slots:
    void reset();

private slots:
    void checkButton(const QString &text);

private:
    QPushButton *m_button;
    HintLineEdit *m_editor;
};
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif
