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

#ifndef ABSTRACTFORMBUILDER_H
#define ABSTRACTFORMBUILDER_H

#include <QtDesigner/uilib_global.h>

#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QDir>

#include <QtGui/QSizePolicy>
#include <QtGui/QPalette>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
#if 0
// pragma for syncqt, don't remove.

#pragma qt_class(QAbstractFormBuilder)
#endif

class QAction;
class QActionGroup;
class QComboBox;
class QIODevice;
class QIcon;
class QLayout;
class QLayoutItem;
class QListWidget;
class QObject;
class QSpacerItem;
class QTreeWidget;
class QTableWidget;
class QVariant;
class QWidget;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif

class DomAction;
class DomActionGroup;
class DomActionRef;
class DomBrush;
class DomColorGroup;
class DomConnections;
class DomCustomWidgets;
class DomLayout;
class DomLayoutItem;
class DomProperty;
class DomResources;
class DomSpacer;
class DomString;
class DomTabStops;
class DomUI;
class DomWidget;
class DomResourcePixmap;

class QResourceBuilder;

#ifndef QT_FORMBUILDER_NO_SCRIPT
class QFormScriptRunner;
#endif 

class QDESIGNER_UILIB_EXPORT QAbstractFormBuilder
{
public:
    QAbstractFormBuilder();
    virtual ~QAbstractFormBuilder();

    QDir workingDirectory() const;
    void setWorkingDirectory(const QDir &directory);

    virtual QWidget *load(QIODevice *dev, QWidget *parentWidget=0);
    virtual void save(QIODevice *dev, QWidget *widget);

    void setScriptingEnabled(bool enabled);
    bool isScriptingEnabled() const;

    static int resourceRole();

protected:
//
// load
//
    virtual void loadExtraInfo(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);

    virtual QWidget *create(DomUI *ui, QWidget *parentWidget);
    virtual QWidget *create(DomWidget *ui_widget, QWidget *parentWidget);
    virtual QLayout *create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget);
    virtual QLayoutItem *create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget);

    virtual QAction *create(DomAction *ui_action, QObject *parent);
    virtual QActionGroup *create(DomActionGroup *ui_action_group, QObject *parent);
    virtual void addMenuAction(QAction *action);

    virtual void applyProperties(QObject *o, const QList<DomProperty*> &properties);
    bool applyPropertyInternally(QObject *o, const QString &propertyName, const QVariant &value);

    virtual void applyTabStops(QWidget *widget, DomTabStops *tabStops);

    virtual QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name);
    virtual QLayout *createLayout(const QString &layoutName, QObject *parent, const QString &name);
    virtual QAction *createAction(QObject *parent, const QString &name);
    virtual QActionGroup *createActionGroup(QObject *parent, const QString &name);

    virtual void createCustomWidgets(DomCustomWidgets *) {}
    virtual void createConnections(DomConnections *, QWidget *) {}
    virtual void createResources(DomResources*) {}

    virtual bool addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout);
    virtual bool addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);

//
// save
//
    virtual void saveExtraInfo(QWidget *widget, DomWidget *ui_widget, DomWidget *ui_parentWidget);

    virtual void saveDom(DomUI *ui, QWidget *widget);

    virtual DomActionRef *createActionRefDom(QAction *action);

    virtual DomWidget *createDom(QWidget *widget, DomWidget *ui_parentWidget, bool recursive = true);
    virtual DomLayout *createDom(QLayout *layout, DomLayout *ui_layout, DomWidget *ui_parentWidget);
    virtual DomLayoutItem *createDom(QLayoutItem *item, DomLayout *ui_parentLayout, DomWidget *ui_parentWidget);
    virtual DomSpacer *createDom(QSpacerItem *spacer, DomLayout *ui_parentLayout, DomWidget *ui_parentWidget);

    virtual DomAction *createDom(QAction *action);
    virtual DomActionGroup *createDom(QActionGroup *actionGroup);

    virtual DomConnections *saveConnections();
    virtual DomCustomWidgets *saveCustomWidgets();
    virtual DomTabStops *saveTabStops();
    virtual DomResources *saveResources();
    virtual QList<DomProperty*> computeProperties(QObject *obj);
    virtual bool checkProperty(QObject *obj, const QString &prop) const;
    virtual DomProperty *createProperty(QObject *object, const QString &propertyName, const QVariant &value);

    virtual void layoutInfo(DomLayout *layout, QObject *parent, int *margin, int *spacing);

    virtual QIcon nameToIcon(const QString &filePath, const QString &qrcPath);
    virtual QString iconToFilePath(const QIcon &pm) const;
    virtual QString iconToQrcPath(const QIcon &pm) const;
    virtual QPixmap nameToPixmap(const QString &filePath, const QString &qrcPath);
    virtual QString pixmapToFilePath(const QPixmap &pm) const;
    virtual QString pixmapToQrcPath(const QPixmap &pm) const;

    void loadListWidgetExtraInfo(DomWidget *ui_widget, QListWidget *listWidget, QWidget *parentWidget);
    void loadTreeWidgetExtraInfo(DomWidget *ui_widget, QTreeWidget *treeWidget, QWidget *parentWidget);
    void loadTableWidgetExtraInfo(DomWidget *ui_widget, QTableWidget *tableWidget, QWidget *parentWidget);
    void loadComboBoxExtraInfo(DomWidget *ui_widget, QComboBox *comboBox, QWidget *parentWidget);

    void saveListWidgetExtraInfo(QListWidget *widget, DomWidget *ui_widget, DomWidget *ui_parentWidget);
    void saveTreeWidgetExtraInfo(QTreeWidget *treeWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget);
    void saveTableWidgetExtraInfo(QTableWidget *tablWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget);
    void saveComboBoxExtraInfo(QComboBox *widget, DomWidget *ui_widget, DomWidget *ui_parentWidget);

    void setResourceBuilder(QResourceBuilder *builder);
    QResourceBuilder *resourceBuilder() const;
    DomProperty *saveResource(const QVariant &v) const;
//
// utils
//

    QVariant toVariant(const QMetaObject *meta, DomProperty *property);
    static QString toString(const DomString *str);

    typedef QHash<QString, DomProperty*> DomPropertyHash;
    static DomPropertyHash propertyMap(const QList<DomProperty*> &properties);

    void setupColorGroup(QPalette &palette, QPalette::ColorGroup colorGroup, DomColorGroup *group);
    DomColorGroup *saveColorGroup(const QPalette &palette);
    QBrush setupBrush(DomBrush *brush);
    DomBrush *saveBrush(const QBrush &brush);

    void reset();
    void initialize(const DomUI *ui);

#ifndef QT_FORMBUILDER_NO_SCRIPT
    QFormScriptRunner *formScriptRunner() const;
#endif
//
//  utils
//

    static QMetaEnum toolBarAreaMetaEnum();

//
//  Icon/pixmap stuff
//
    // A Pair of icon path/qrc path.
    typedef QPair<QString, QString> IconPaths;
 
    IconPaths iconPaths(const QIcon &) const;
    IconPaths pixmapPaths(const QPixmap &) const;
    void setIconProperty(DomProperty &, const IconPaths &) const;
    void setPixmapProperty(DomProperty &, const IconPaths &) const;
    DomProperty* iconToDomProperty(const QIcon &) const;

    static const DomResourcePixmap *domPixmap(const DomProperty* p);
    QIcon domPropertyToIcon(const DomResourcePixmap *);
    QIcon domPropertyToIcon(const DomProperty* p);
    QPixmap domPropertyToPixmap(const DomResourcePixmap* p);
    QPixmap domPropertyToPixmap(const DomProperty* p);

    QHash<QObject*, bool> m_laidout;
    QHash<QString, QAction*> m_actions;
    QHash<QString, QActionGroup*> m_actionGroups;
    int m_defaultMargin;
    int m_defaultSpacing;
    QDir m_workingDirectory;

private:
//
//  utils
//
    static Qt::ToolBarArea toolbarAreaFromDOMAttributes(const DomPropertyHash &attributeMap);

    QAbstractFormBuilder(const QAbstractFormBuilder &other);
    void operator = (const QAbstractFormBuilder &other);

    friend QDESIGNER_UILIB_EXPORT DomProperty *variantToDomProperty(QAbstractFormBuilder *abstractFormBuilder, QObject *object, const QString &propertyName, const QVariant &value);
    friend QDESIGNER_UILIB_EXPORT QVariant domPropertyToVariant(QAbstractFormBuilder *abstractFormBuilder,const QMetaObject *meta, const DomProperty *property);
};

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTFORMBUILDER_H
