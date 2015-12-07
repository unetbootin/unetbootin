/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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
    virtual void setPropertyComment(const QString &name, const QString &value);
    virtual void updatePropertySheet();

    virtual void setObject(QObject *object);

    void reloadResourceProperties();

    virtual QObject *object() const
    { return m_object; }

    virtual QString currentPropertyName() const;

private slots:
    void slotResetProperty(QtProperty *property);
    void slotValueChanged(QtProperty *property, const QVariant &value, bool enableSubPropertyHandling);
    void slotViewTriggered(QAction *action);
    void slotAddDynamicProperty(QAction *action);
    void slotRemoveDynamicProperty();
    void slotSorting(bool sort);
    void slotColoring(bool color);
    void slotCurrentItemChanged(QtBrowserItem*);

private:
    void updateBrowserValue(QtVariantProperty *property, const QVariant &value);
    void updateToolBarLabel();
    int toBrowserType(const QVariant &value, const QString &propertyName) const;
    QString removeScope(const QString &value) const;
    QDesignerMetaDataBaseItemInterface *metaDataBaseItem() const;
    void setupStringProperty(QtVariantProperty *property, const QString &pname, const QVariant &value, bool isMainContainer);
    void setupPaletteProperty(QtVariantProperty *property);
    QString realClassName(QObject *object) const;
    void storeExpansionState();
    void applyExpansionState();
    void storePropertiesExpansionState(const QList<QtBrowserItem *> &items);
    void applyPropertiesExpansionState(const QList<QtBrowserItem *> &items);
    void setExpanded(QtBrowserItem *item, bool expanded);
    bool isExpanded(QtBrowserItem *item);
    void collapseAll();
    void clearView();
    void fillView();
    bool isLayoutGroup(QtProperty *group) const;
    QColor propertyColor(QtProperty *property) const;
    void updateActionsState();
    QtBrowserItem *nonFakePropertyBrowserItem(QtBrowserItem *item) const;
    void saveSettings() const;
    void addCommentProperty(QtVariantProperty *property, const QString &propertyName);
    void editProperty(const QString &name);
    bool isDynamicProperty(const QtBrowserItem* item) const;

    struct Strings {
        Strings();
        const QString m_commentName;
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
    QMap<QtVariantProperty *, QtVariantProperty *> m_propertyToComment;
    QMap<QtVariantProperty *, QtVariantProperty *> m_commentToProperty;
    QList<QtProperty *> m_groups;
    QtProperty *m_dynamicGroup;
    QString m_recentlyAddedDynamicProperty;
    bool m_updatingBrowser;

    QStackedWidget *m_stackedWidget;
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
    QVector<QColor> m_colors;
    const QColor m_dynamicColor;
    const QColor m_layoutColor;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // PROPERTYEDITOR_H
