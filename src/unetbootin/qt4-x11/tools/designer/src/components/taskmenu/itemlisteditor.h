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

#ifndef ITEMLISTEDITOR_H
#define ITEMLISTEDITOR_H

#include "ui_itemlisteditor.h"

#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QtProperty;
class QtVariantProperty;
class QtTreePropertyBrowser;
class QSplitter;

namespace qdesigner_internal {

class DesignerIconCache;
class DesignerPropertyManager;
class DesignerEditorFactory;

// Utility class that ensures a bool is true while in scope.
// Courtesy of QBoolBlocker in qobject_p.h
class BoolBlocker
{
public:
    inline BoolBlocker(bool &b):block(b), reset(b){block = true;}
    inline ~BoolBlocker(){block = reset; }
private:
    bool &block;
    bool reset;
};

class AbstractItemEditor: public QDialog
{
    Q_OBJECT

public:
    AbstractItemEditor(QDesignerFormWindowInterface *form, QWidget *parent);
    ~AbstractItemEditor();

    DesignerIconCache *iconCache() const { return m_iconCache; }

    struct PropertyDefinition {
        int role;
        int type;
        int (*typeFunc)();
        const char *name;
    };

private slots:
    void propertyChanged(QtProperty *property);
    void resetProperty(QtProperty *property);
    void cacheReloaded();

protected:
    void keyPressEvent(QKeyEvent *e);
    void setupProperties(PropertyDefinition *propDefs);
    void setupObject(QWidget *object);
    void setupEditor(QWidget *object, PropertyDefinition *propDefs);
    void injectPropertyBrowser(QWidget *parent, QWidget *widget);
    void updateBrowser();
    virtual void setItemData(int role, const QVariant &v) = 0;
    virtual QVariant getItemData(int role) const = 0;

    DesignerIconCache *m_iconCache;
    DesignerPropertyManager *m_propertyManager;
    DesignerEditorFactory *m_editorFactory;
    QSplitter *m_propertySplitter;
    QtTreePropertyBrowser *m_propertyBrowser;
    QList<QtVariantProperty*> m_properties;
    QList<QtVariantProperty*> m_rootProperties;
    QHash<QtVariantProperty*, int> m_propertyToRole;
    bool m_updatingBrowser;
};

class ItemListEditor: public AbstractItemEditor
{
    Q_OBJECT

public:
    ItemListEditor(QDesignerFormWindowInterface *form, QWidget *parent);

    void setupEditor(QWidget *object, PropertyDefinition *propDefs);
    QListWidget *listWidget() const { return ui.listWidget; }
    void setNewItemText(const QString &tpl) { m_newItemText = tpl; }
    QString newItemText() const { return m_newItemText; }
    void setCurrentIndex(int idx);

signals:
    void indexChanged(int idx);
    void itemChanged(int idx, int role, const QVariant &v);
    void itemInserted(int idx);
    void itemDeleted(int idx);
    void itemMovedUp(int idx);
    void itemMovedDown(int idx);

private slots:
    void on_newListItemButton_clicked();
    void on_deleteListItemButton_clicked();
    void on_moveListItemUpButton_clicked();
    void on_moveListItemDownButton_clicked();
    void on_listWidget_currentRowChanged();
    void on_listWidget_itemChanged(QListWidgetItem * item);
    void togglePropertyBrowser();
    void cacheReloaded();

protected:
    virtual void setItemData(int role, const QVariant &v);
    virtual QVariant getItemData(int role) const;

    void updateEditor();
    Ui::ItemListEditor ui;
    bool m_updating;
    QString m_newItemText;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // ITEMLISTEDITOR_H
