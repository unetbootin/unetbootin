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

#ifndef QDESIGNER_TEMPLATEOPTIONS_H
#define QDESIGNER_TEMPLATEOPTIONS_H

#include <QtDesigner/private/abstractoptionspage_p.h>

#include <QtCore/QPointer>
#include <QtCore/QStringList>

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;

namespace qdesigner_internal {

namespace Ui {
    class TemplateOptionsWidget;
}

/* Present the user with a list of form template paths to save
 * form templates. */
class TemplateOptionsWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TemplateOptionsWidget)
public:
    explicit TemplateOptionsWidget(QDesignerFormEditorInterface *core,
                                              QWidget *parent = 0);
    ~TemplateOptionsWidget();


    QStringList templatePaths() const;
    void setTemplatePaths(const QStringList &l);

    static QString chooseTemplatePath(QDesignerFormEditorInterface *core, QWidget *parent);

private slots:
    void addTemplatePath();
    void removeTemplatePath();
    void templatePathSelectionChanged();

private:
    QDesignerFormEditorInterface *m_core;
    Ui::TemplateOptionsWidget *m_ui;
};

class TemplateOptionsPage : public QDesignerOptionsPageInterface
{
     Q_DISABLE_COPY(TemplateOptionsPage)
public:
    explicit TemplateOptionsPage(QDesignerFormEditorInterface *core);

    virtual QString name() const;
    virtual QWidget *createPage(QWidget *parent);
    virtual void apply();
    virtual void finish();

private:
    QDesignerFormEditorInterface *m_core;
    QStringList m_initialTemplatePaths;
    QPointer<TemplateOptionsWidget> m_widget;
};

}

QT_END_NAMESPACE

#endif // QDESIGNER_TEMPLATEOPTIONS_H
