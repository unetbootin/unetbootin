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

#ifndef ABSTRACTFINDWIDGET_H
#define ABSTRACTFINDWIDGET_H

#include <QtGui/QIcon>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QCheckBox;
class QEvent;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QObject;
class QToolButton;

class AbstractFindWidget : public QWidget
{
    Q_OBJECT

public:
    enum FindFlag {
        /// Use a layout that is roughly half as wide and twice as high as the regular one.
        NarrowLayout = 1,
        /// Do not show the "Whole words" checkbox.
        NoWholeWords = 2,
        /// Do not show the "Case sensitive" checkbox.
        NoCaseSensitive = 4
    };
    Q_DECLARE_FLAGS(FindFlags, FindFlag)

    AbstractFindWidget(FindFlags flags = FindFlags(), QWidget *parent = 0);
    virtual ~AbstractFindWidget();

    bool eventFilter(QObject *object, QEvent *e);

    static QIcon findIconSet();

public slots:
    void activate();
    virtual void deactivate();
    void findNext();
    void findPrevious();
    void findCurrentText();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void updateButtons();

protected:
    virtual void find(const QString &textToFind, bool skipCurrent, bool backward, bool *found, bool *wrapped) = 0;

    bool caseSensitive() const;
    bool wholeWords() const;

private:
    void findInternal(const QString &textToFind, bool skipCurrent, bool backward);

    QLineEdit *m_editFind;
    QLabel *m_labelWrapped;
    QToolButton *m_toolNext;
    QToolButton *m_toolClose;
    QToolButton *m_toolPrevious;
    QCheckBox *m_checkCase;
    QCheckBox *m_checkWholeWords;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFindWidget::FindFlags)

QT_END_NAMESPACE

#endif  // ABSTRACTFINDWIDGET_H
