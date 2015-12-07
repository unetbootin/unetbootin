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

#ifndef TEXTEDIT_TASKMENU_H
#define TEXTEDIT_TASKMENU_H

#include <QtGui/QTextEdit>
#include <QtGui/QPlainTextEdit>

#include <qdesigner_taskmenu_p.h>
#include <extensionfactory_p.h>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class TextEditTaskMenu: public QDesignerTaskMenu
{
    Q_OBJECT
public:
    explicit TextEditTaskMenu(QTextEdit *button, QObject *parent = 0);
    explicit TextEditTaskMenu(QPlainTextEdit *button, QObject *parent = 0);

    virtual ~TextEditTaskMenu();

    virtual QAction *preferredEditAction() const;
    virtual QList<QAction*> taskActions() const;

private slots:
    void editText();

private:
    void initialize();

    const Qt::TextFormat m_format;
    const QString m_property;
    const QString m_windowTitle;

    mutable QList<QAction*> m_taskActions;
    QAction *m_editTextAction;
};

typedef ExtensionFactory<QDesignerTaskMenuExtension, QTextEdit, TextEditTaskMenu> TextEditTaskMenuFactory;
typedef ExtensionFactory<QDesignerTaskMenuExtension, QPlainTextEdit, TextEditTaskMenu> PlainTextEditTaskMenuFactory;
}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TEXTEDIT_TASKMENU_H
