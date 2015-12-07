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

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include "propertyeditor_global.h"
#include <qdesigner_propertyeditor_p.h>

#include <QtCore/QPointer>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QSet>

QT_BEGIN_NAMESPACE

class DomProperty;
class QDesignerMetaDataBaseItemInterface;
class QDesignerPropertySheetExtension;

class QtAbstractPropertyBrowser;
class QtButtonPropertyBrowser;
class QtTreePropertyBrowser;
class QtProperty;
class QtVariantProperty;
class QtBrowserItem;

class QStackedWidget;
class QLabel;

namespace qdesigner_internal {

class StringProperty;
class DesignerPropertyManager;
class DesignerEditorFactory;
class FilterWidget;

class QT_PROPERTYEDITOR_EXPORT PropertyEditor: public QDesignerPropertyEditor
{
    Q_OBJECT
public:
    explicit PropertyEditor(QDesignerFormEditorInterface *core, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~PropertyEditor();

    virtual QDesignerFormEditorInterface *core() const;

    virtual bool isReadOnly() const;
    virtual void setReadOnly(bool readOnly);
    virtual void setPropertyValue(const QString &name, const QVariant &value, bool changed = true);
    virtual void updatePropertySheet();

    virtual void setObject(QObject *object);

    void reloadResourceProperties();

    virtual QObject *object() const
    { return m_object; }

    virtual QString currentPropertyName() const;

protected:

    bool event(QEvent *event);

private slots:
    void slotResetProperty(QtProperty *property);
    void slotValueChanged(QtProperty *property, const QVariant &value, bool enableSubPropertyHandling);
    void slotViewTriggered(QAction *action);
    void slotAddDynamicProperty(QAction *action);
    void slotRemoveDynamicProperty();
    void slotSorting(bool sort);
    void slotColoring(bool color);
    void slotCurrentItemChanged(QtBrowserItem*);
    void setFilter(const QString &pattern);

private:
    void updateBrowserValue(QtVariantProperty *property, const QVariant &value);
    void updateToolBarLabel();
    int toBrowserType(const QVariant &value, const QString &propertyName) const;
    QString removeScope(const QString &value) const;
    QDesignerMetaDataBaseItemInterface *metaDataBaseItem() const;
    void setupStringProperty(QtVariantProperty *property, bool isMainContainer);
    void setupPaletteProperty(QtVariantProperty *property);
    QString realClassName(QObject *object) const;
    void storeExpansionState();
    void applyExpansionState();
    void storePropertiesExpansionState(const QList<QtBrowserItem *> &items);
    void applyPropertiesExpansionState(const QList<QtBrowserItem *> &items);
    void applyFilter();
    int applyPropertiesFilter(const QList<QtBrowserItem *> &items);
    void setExpanded(QtBrowserItem *item, bool expanded);
    bool isExpanded(QtBrowserItem *item) const;
    void setItemVisible(QtBrowserItem *item, bool visible);
    bool isItemVisible(QtBrowserItem *item) const;
    void collapseAll();
    void clearView();
    void fillView();
    bool isLayoutGroup(QtProperty *group) const;
    void updateColors();
    void updateForegroundBrightness();
    QColor propertyColor(QtProperty *property) const;
    void updateActionsState();
    QtBrowserItem *nonFakePropertyBrowserItem(QtBrowserItem *item) const;
    void saveSettings() const;
    void editProperty(const QString &name);
    bool isDynamicProperty(const QtBrowserItem* item) const;

    struct Strings {
        Strings();
        QSet<QString> m_alignmentProperties;
        const QString m_fontProperty;
        const QString m_qLayoutWidget;
        const QString m_designerPrefix;
        const QString m_layout;
        const QString m_validationModeAttribute;
        const QString m_fontAttribute;
        const QString m_superPaletteAttribute;
        const QString m_enumNamesAttribute;
        const QString m_resettableAttribute;
        const QString m_flagsAttribute;
    };

    const Strings m_strings;
    QDesignerFormEditorInterface *m_core;
    QDesignerPropertySheetExtension *m_propertySheet;
    QtAbstractPropertyBrowser *m_currentBrowser;
    QtButtonPropertyBrowser *m_buttonBrowser;
    QtTreePropertyBrowser *m_treeBrowser;
    DesignerPropertyManager *m_propertyManager;
    DesignerEditorFactory *m_treeFactory;
    DesignerEditorFactory *m_groupFactory;
    QPointer<QObject> m_object;
    QMap<QString, QtVariantProperty*> m_nameToProperty;
    QMap<QtProperty*, QString> m_propertyToGroup;
    QMap<QString, QtVariantProperty*> m_nameToGroup;
    QList<QtProperty *> m_groups;
    QtProperty *m_dynamicGroup;
    QString m_recentlyAddedDynamicProperty;
    bool m_updatingBrowser;

    QStackedWidget *m_stackedWidget;
    FilterWidget *m_filterWidget;
    int m_buttonIndex;
    int m_treeIndex;
    QAction *m_addDynamicAction;
    QAction *m_removeDynamicAction;
    QAction *m_sortingAction;
    QAction *m_coloringAction;
    QAction *m_treeAction;
    QAction *m_buttonAction;
    QLabel *m_classLabel;

    bool m_sorting;
    bool m_coloring;

    QMap<QString, bool> m_expansionState;

    QString m_filterPattern;
    QVector<QPair<QColor, QColor> > m_colors;
    QPair<QColor, QColor> m_dynamicColor;
    QPair<QColor, QColor> m_layoutColor;

    bool m_brightness;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // PROPERTYEDITOR_H
