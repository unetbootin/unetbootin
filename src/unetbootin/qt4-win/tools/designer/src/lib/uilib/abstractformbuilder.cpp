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
**sw
****************************************************************************/

#include "abstractformbuilder.h"
#include "formbuilderextra_p.h"
#include "resourcebuilder_p.h"
#include "ui4_p.h"
#include "properties_p.h"

#include <QtCore/QVariant>
#include <QtCore/QMetaProperty>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtCore/QHash>
#include <QtCore/qdebug.h>

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFontComboBox>
#include <QtGui/QGridLayout>
#ifndef QT_NO_FORMLAYOUT
#  include <QtGui/QFormLayout>
#endif
#include <QtGui/QIcon>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QPixmap>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QScrollArea>
#ifndef QFORMINTERNAL_NAMESPACE
#  include <private/qlayout_p.h> // Compiling within Designer
#endif

#include <QtXml/QDomDocument>

#include <QtGui/QDialog>

// containers
#include <QtGui/QToolBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QDockWidget>
#include <QtGui/QMdiArea>
#include <QtGui/QWorkspace>
#include <QtGui/QWizard>
#include <QtGui/QWizardPage>

#include <QtCore/qdebug.h>

#include <limits.h>

Q_DECLARE_METATYPE(QWidgetList)

QT_BEGIN_NAMESPACE

#ifdef QFORMINTERNAL_NAMESPACE
using namespace QFormInternal;
#endif

class QFriendlyLayout: public QLayout
{
public:
    inline QFriendlyLayout() { Q_ASSERT(0); }

#ifdef QFORMINTERNAL_NAMESPACE
    friend class QFormInternal::QAbstractFormBuilder;
#else
    friend class QAbstractFormBuilder;
#endif
};

/*!
    \class QAbstractFormBuilder

    \brief The QAbstractFormBuilder class provides a default
    implementation for classes that create user interfaces at
    run-time.

    \inmodule QtDesigner

    QAbstractFormBuilder provides a standard interface and a default
    implementation for constructing forms from user interface
    files. It is not intended to be instantiated directly. Use the
    QFormBuilder class to create user interfaces from \c{.ui} files at
    run-time. For example:

    \snippet doc/src/snippets/code/tools.designer.src.lib.uilib.abstractformbuilder.cpp 0

    To override certain aspects of the form builder's behavior,
    subclass QAbstractFormBuilder and reimplement the relevant virtual
    functions:

    \list
    \o load() handles reading of \c{.ui} format files from arbitrary
       QIODevices, and construction of widgets from the XML data
       that they contain.
    \o save() handles saving of widget details in \c{.ui} format to
       arbitrary QIODevices.
    \o workingDirectory() and setWorkingDirectory() control the
       directory in which forms are held. The form builder looks for
       other resources on paths relative to this directory.
    \endlist

    The QFormBuilder class is typically used by custom components and
    applications that embed \QD. Standalone applications that need to
    dynamically generate user interfaces at run-time use the
    QUiLoader, found in the QtUiTools module.

    \sa {QtUiTools Module}
*/

/*!
    Constructs a new form builder.*/
QAbstractFormBuilder::QAbstractFormBuilder() :
    m_defaultMargin(INT_MIN),
    m_defaultSpacing(INT_MIN)
{
    setResourceBuilder(new QResourceBuilder());
}

/*!
    Destroys the form builder.*/
QAbstractFormBuilder::~QAbstractFormBuilder()
{
    QFormBuilderExtra::removeInstance(this);
}


/*!
    \fn QWidget *QAbstractFormBuilder::load(QIODevice *device, QWidget *parent)

    Loads an XML representation of a widget from the given \a device,
    and constructs a new widget with the specified \a parent.

    \sa save()
*/
QWidget *QAbstractFormBuilder::load(QIODevice *dev, QWidget *parentWidget)
{
    QDomDocument doc;
    if (!doc.setContent(dev))
        return 0;

    QDomElement root = doc.firstChildElement();
    DomUI ui;
    ui.read(root); /// ### check the result

    return create(&ui, parentWidget);
}

/*!
    \internal
*/
QWidget *QAbstractFormBuilder::create(DomUI *ui, QWidget *parentWidget)
{
    QFormBuilderExtra *formBuilderPrivate = QFormBuilderExtra::instance(this);
    formBuilderPrivate->clear();
    if (const DomLayoutDefault *def = ui->elementLayoutDefault()) {
        m_defaultMargin = def->hasAttributeMargin() ? def->attributeMargin() : INT_MIN;
        m_defaultSpacing = def->hasAttributeSpacing() ? def->attributeSpacing() : INT_MIN;
    }

    DomWidget *ui_widget = ui->elementWidget();
    if (!ui_widget)
        return 0;

    initialize(ui);

    if (QWidget *widget = create(ui_widget, parentWidget)) {
        createConnections(ui->elementConnections(), widget);
        createResources(ui->elementResources()); // maybe this should go first, before create()...
        applyTabStops(widget, ui->elementTabStops());
        formBuilderPrivate->applyInternalProperties();
        reset();
        formBuilderPrivate->clear();
        return widget;
    }
    formBuilderPrivate->clear();
    return 0;
}

/*!
    \internal
    Retrieve relevant information from the custom widgets section.
    Called by create(DomUI *, QWidget *); call manually if you
    just use create(DomWidget *, QWidget *) on some child widget of DomUI.
 */

void QAbstractFormBuilder::initialize(const DomUI *ui)
{
    typedef QList<DomCustomWidget*> DomCustomWidgetList;

    DomCustomWidgets *domCustomWidgets  = ui->elementCustomWidgets();
    createCustomWidgets(domCustomWidgets);

    if (domCustomWidgets) {
        const DomCustomWidgetList customWidgets = domCustomWidgets->elementCustomWidget();
        if (!customWidgets.empty()) {
            QFormBuilderExtra *formBuilderPrivate = QFormBuilderExtra::instance(this);
            const DomCustomWidgetList::const_iterator cend = customWidgets.constEnd();
            for (DomCustomWidgetList::const_iterator it = customWidgets.constBegin(); it != cend; ++it) {
                const DomCustomWidget *cw = *it;
#ifndef QT_FORMBUILDER_NO_SCRIPT
                if (const DomScript *domScript = cw->elementScript()) {
                    const QString script = domScript->text();
                    if (!script.isEmpty())
                        formBuilderPrivate->storeCustomWidgetScript(cw->elementClass(), script);
                }
#endif
                const QString addPageMethod = cw->elementAddPageMethod();
                if (!addPageMethod.isEmpty())
                    formBuilderPrivate->storeCustomWidgetAddPageMethod(cw->elementClass(), addPageMethod);

                const QString extends = cw->elementExtends();
                if (!extends.isEmpty())
                    formBuilderPrivate->storeCustomWidgetBaseClass(cw->elementClass(), extends);
            }
        }
    }
}

/*!
    \internal
*/
QWidget *QAbstractFormBuilder::create(DomWidget *ui_widget, QWidget *parentWidget)
{
    QWidget *w = createWidget(ui_widget->attributeClass(), parentWidget, ui_widget->attributeName());
    if (!w)
        return 0;

    applyProperties(w, ui_widget->elementProperty());

    foreach (DomAction *ui_action, ui_widget->elementAction()) {
        QAction *child_action = create(ui_action, w);
        Q_UNUSED( child_action );
    }

    foreach (DomActionGroup *ui_action_group, ui_widget->elementActionGroup()) {
        QActionGroup *child_action_group = create(ui_action_group, w);
        Q_UNUSED( child_action_group );
    }

    QWidgetList children;
    foreach (DomWidget *ui_child, ui_widget->elementWidget()) {
        if (QWidget *child  = create(ui_child, w)) {
            children += child;
        } else {
            const QString className = ui_child->elementClass().empty() ? QString() : ui_child->elementClass().front();
            uiLibWarning(QObject::tr("The creation of a widget of the class '%1' failed.").arg(className));
        }
    }

    foreach (DomLayout *ui_lay, ui_widget->elementLayout()) {
        QLayout *child_lay = create(ui_lay, 0, w);
        Q_UNUSED( child_lay );
    }

    const QList<DomActionRef *> addActions = ui_widget->elementAddAction();
    if (!addActions.empty()) {
        const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
        foreach (DomActionRef *ui_action_ref, addActions) {
            const QString name = ui_action_ref->attributeName();
            if (name == strings.separator) {
                QAction *sep = new QAction(w);
                sep->setSeparator(true);
                w->addAction(sep);
                addMenuAction(sep);
            } else if (QAction *a = m_actions.value(name)) {
                w->addAction(a);
            } else if (QActionGroup *g = m_actionGroups.value(name)) {
                w->addActions(g->actions());
            } else if (QMenu *menu = qFindChild<QMenu*>(w, name)) {
                w->addAction(menu->menuAction());
                addMenuAction(menu->menuAction());
            }
        }
    }
    loadExtraInfo(ui_widget, w, parentWidget);
#ifndef QT_FORMBUILDER_NO_SCRIPT
    QString scriptErrorMessage;
    QFormBuilderExtra *extra = QFormBuilderExtra::instance(this);
    extra->formScriptRunner().run(ui_widget,
                                  extra->customWidgetScript(ui_widget->attributeClass()),
                                  w, children, &scriptErrorMessage);
#endif
    addItem(ui_widget, w, parentWidget);

    if (qobject_cast<QDialog *>(w) && parentWidget)
        w->setAttribute(Qt::WA_Moved, false); // So that QDialog::setVisible(true) will center it

    const QStringList zOrderNames = ui_widget->elementZOrder();
    if (!zOrderNames.isEmpty()) {
        QList<QWidget *> zOrder = qVariantValue<QWidgetList>(w->property("_q_zOrder"));
        foreach (QString widgetName, zOrderNames) {
            if (QWidget *child = qFindChild<QWidget*>(w, widgetName)) {
                if (child->parentWidget() == w) {
                    zOrder.removeAll(child);
                    zOrder.append(child);
                    child->raise();
                }
            }
        }
        w->setProperty("_q_zOrder", qVariantFromValue(zOrder));
    }

    return w;
}

/*!
    \internal
*/
QAction *QAbstractFormBuilder::create(DomAction *ui_action, QObject *parent)
{
    QAction *a = createAction(parent, ui_action->attributeName());
    if (!a)
        return 0;

    applyProperties(a, ui_action->elementProperty());
    return a;
}

/*!
    \internal
*/
QActionGroup *QAbstractFormBuilder::create(DomActionGroup *ui_action_group, QObject *parent)
{
    QActionGroup *a = createActionGroup(parent, ui_action_group->attributeName());
    if (!a)
        return 0;

    applyProperties(a, ui_action_group->elementProperty());

    foreach (DomAction *ui_action, ui_action_group->elementAction()) {
        QAction *child_action = create(ui_action, a);
        Q_UNUSED( child_action );
    }

    foreach (DomActionGroup *g, ui_action_group->elementActionGroup()) {
        QActionGroup *child_action_group = create(g, parent);
        Q_UNUSED( child_action_group );
    }

    return a;
}

// figure out the toolbar area of a DOM attrib list.
// By legacy, it is stored as an integer. As of 4.3.0, it is the enumeration value.
Qt::ToolBarArea QAbstractFormBuilder::toolbarAreaFromDOMAttributes(const DomPropertyHash &attributes) {
    const DomProperty *attr = attributes.value(QFormBuilderStrings::instance().toolBarAreaAttribute);
    if (!attr)
        return Qt::TopToolBarArea;
    switch(attr->kind()) {
    case DomProperty::Number:
        return static_cast<Qt::ToolBarArea>(attr->elementNumber());
    case DomProperty::Enum:
        return enumKeyOfObjectToValue<QAbstractFormBuilderGadget, Qt::ToolBarArea>("toolBarArea",  attr->elementEnum().toLatin1());
    default:
        break;
    }
    return Qt::TopToolBarArea;
}

/*!
    \internal
*/
bool QAbstractFormBuilder::addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const DomPropertyHash attributes = propertyMap(ui_widget->elementAttribute());

    if (parentWidget == 0)
        return true;
    // Check special cases. First: Custom container
    const QString className = QLatin1String(parentWidget->metaObject()->className());
    const QString addPageMethod = QFormBuilderExtra::instance(this)->customWidgetAddPageMethod(className);
    if (!addPageMethod.isEmpty()) {
        // If this fails ( non-existent or non-slot), use ContainerExtension in Designer, else it can't be helped
        return QMetaObject::invokeMethod(parentWidget, addPageMethod.toUtf8().constData(), Qt::DirectConnection, Q_ARG(QWidget*, widget));
    }

    if (QMainWindow *mw = qobject_cast<QMainWindow*>(parentWidget)) {

#ifndef QT_NO_MENUBAR
        // the menubar
        if (QMenuBar *menuBar = qobject_cast<QMenuBar*>(widget)) {
            mw->setMenuBar(menuBar);
            return true;
        }
#endif

#ifndef QT_NO_TOOLBAR
        // apply the toolbar's attributes
        else if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
            mw->addToolBar(toolbarAreaFromDOMAttributes(attributes), toolBar);
            // check break
            if (const DomProperty *attr = attributes.value(strings.toolBarBreakAttribute))
                if (attr->elementBool() == strings.trueValue)
                    mw->insertToolBarBreak (toolBar);

            return true;
        }
#endif

#ifndef QT_NO_STATUSBAR
        // statusBar
        else if (QStatusBar *statusBar = qobject_cast<QStatusBar*>(widget)) {
            mw->setStatusBar(statusBar);
            return true;
        }
#endif

#ifndef QT_NO_DOCKWIDGET
        // apply the dockwidget's attributes
        else if (QDockWidget *dockWidget = qobject_cast<QDockWidget*>(widget)) {
            if (const DomProperty *attr = attributes.value(strings.dockWidgetAreaAttribute)) {
                Qt::DockWidgetArea area = static_cast<Qt::DockWidgetArea>(attr->elementNumber());
                if (!dockWidget->isAreaAllowed(area)) {
                    if (dockWidget->isAreaAllowed(Qt::LeftDockWidgetArea))
                        area = Qt::LeftDockWidgetArea;
                    else if (dockWidget->isAreaAllowed(Qt::RightDockWidgetArea))
                        area = Qt::RightDockWidgetArea;
                    else if (dockWidget->isAreaAllowed(Qt::TopDockWidgetArea))
                        area = Qt::TopDockWidgetArea;
                    else if (dockWidget->isAreaAllowed(Qt::BottomDockWidgetArea))
                        area = Qt::BottomDockWidgetArea;
                }
                mw->addDockWidget(area, dockWidget);
            } else {
                mw->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
            }
            return true;
        }
#endif

        else if (! mw->centralWidget()) {
            mw->setCentralWidget(widget);
            return true;
        }
    }

#ifndef QT_NO_TABWIDGET
    else if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(parentWidget)) {
        widget->setParent(0);

        const int tabIndex = tabWidget->count();
        if (const DomProperty *titleP = attributes.value(strings.titleAttribute, 0))
            tabWidget->addTab(widget, toString(titleP->elementString()));
        else
            tabWidget->addTab(widget, strings.defaultTitle);

        if (DomProperty *picon = attributes.value(strings.iconAttribute)) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            tabWidget->setTabIcon(tabIndex, qvariant_cast<QIcon>(nativeValue));
        }

#ifndef QT_NO_TOOLTIP
        if (const DomProperty *ptoolTip = attributes.value(strings.toolTipAttribute)) {
            tabWidget->setTabToolTip(tabIndex, toString(ptoolTip->elementString()));
        }
#endif

        return true;
    }
#endif

#ifndef QT_NO_TOOLBOX
    else if (QToolBox *toolBox = qobject_cast<QToolBox*>(parentWidget)) {
        const int tabIndex = toolBox->count();
        if (const DomProperty *labelP =  attributes.value(strings.labelAttribute, 0))
            toolBox->addItem(widget, toString(labelP->elementString()));
        else
            toolBox->addItem(widget, strings.defaultTitle);

        if (DomProperty *picon = attributes.value(strings.iconAttribute)) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            toolBox->setItemIcon(tabIndex, qvariant_cast<QIcon>(nativeValue));
        }

#ifndef QT_NO_TOOLTIP
        if (const DomProperty *ptoolTip = attributes.value(strings.toolTipAttribute)) {
            toolBox->setItemToolTip(tabIndex, toString(ptoolTip->elementString()));
        }
#endif

        return true;
    }
#endif

#ifndef QT_NO_STACKEDWIDGET
    else if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(parentWidget)) {
        stackedWidget->addWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_SPLITTER
    else if (QSplitter *splitter = qobject_cast<QSplitter*>(parentWidget)) {
        splitter->addWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_MDIAREA
    else if (QMdiArea *mdiArea = qobject_cast<QMdiArea*>(parentWidget)) {
        mdiArea->addSubWindow(widget);
        return true;
    }
#endif

#ifndef QT_NO_WORKSPACE
    else if (QWorkspace *ws = qobject_cast<QWorkspace*>(parentWidget)) {
        ws->addWindow(widget);
        return true;
    }
#endif

#ifndef QT_NO_DOCKWIDGET
    else if (QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget)) {
        dockWidget->setWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_SCROLLAREA
    else if (QScrollArea *scrollArea = qobject_cast<QScrollArea*>(parentWidget)) {
        scrollArea->setWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_WIZARD
     else if (QWizard *wizard = qobject_cast<QWizard *>(parentWidget)) {
         QWizardPage *page = qobject_cast<QWizardPage*>(widget);
         if (!page) {
             uiLibWarning(QObject::tr("Attempt to add child that is not of class QWizardPage to QWizard."));
             return false;
         }
         wizard->addPage(page);
         return true;
     }
#endif
    return false;
}

/*!
    \internal
*/
void QAbstractFormBuilder::layoutInfo(DomLayout *ui_layout, QObject *parent, int *margin, int *spacing)
{
    Q_UNUSED(parent)
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

    int mar = INT_MIN;
    int spac = INT_MIN;
    if (const DomProperty *p = properties.value(strings.marginProperty, 0))
        mar = p->elementNumber();

    if (const DomProperty *p = properties.value(strings.spacingProperty, 0))
        spac = p->elementNumber();

#ifdef Q_OS_MAC
    // here we recognize ui file < 4.3 (no we don't store margin property)
    if (mar != INT_MIN) {
        const int defaultMargin = parent->inherits("QLayoutWidget") ? 0 : 9;
        if (mar == defaultMargin)
            mar = INT_MIN;
        if (spac == 6)
            spac = INT_MIN;

        if (mar == INT_MIN || spac == INT_MIN) {
            QList<DomProperty *> properties = ui_layout->elementProperty();
            QMutableListIterator<DomProperty *> it(properties);
            while (it.hasNext()) {
                DomProperty *prop = it.next();
                if ((mar == INT_MIN && prop->attributeName() == strings.marginProperty) ||
                        (spac == INT_MIN && prop->attributeName() == strings.spacingProperty)) {
                    it.remove();
                    delete prop;
                }
            }
            ui_layout->setElementProperty(properties);
        }
    }
#endif
    if (margin)
        *margin = mar;
    if (spacing)
        *spacing = spac;
}

/*!
    \internal
*/
QLayout *QAbstractFormBuilder::create(DomLayout *ui_layout, QLayout *parentLayout, QWidget *parentWidget)
{
    QObject *p = parentLayout;

    if (p == 0)
        p = parentWidget;

    Q_ASSERT(p != 0);

    bool tracking = false;

    if (p == parentWidget && parentWidget->layout()) {
        tracking = true;
        p = parentWidget->layout();
    }

    QLayout *layout = createLayout(ui_layout->attributeClass(), p, ui_layout->hasAttributeName() ? ui_layout->attributeName() : QString());

    if (layout == 0)
        return 0;

    if (tracking && layout->parent() == 0) {
        QBoxLayout *box = qobject_cast<QBoxLayout*>(parentWidget->layout());
        Q_ASSERT(box != 0); // only QBoxLayout is supported
        box->addLayout(layout);
    }

    int margin = INT_MIN, spacing = INT_MIN;
    layoutInfo(ui_layout, p, &margin, &spacing);

    if (margin != INT_MIN) {
        layout->setMargin(margin);
    } else {
        const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
        int left, top, right, bottom;
        left = top = right = bottom = -1;
        layout->getContentsMargins(&left, &top, &right, &bottom);

        const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

        if (const DomProperty *p = properties.value(strings.leftMarginProperty, 0))
            left = p->elementNumber();

        if (const DomProperty *p = properties.value(strings.topMarginProperty, 0))
            top = p->elementNumber();

        if (const DomProperty *p = properties.value(strings.rightMarginProperty, 0))
            right = p->elementNumber();

        if (const DomProperty *p = properties.value(strings.bottomMarginProperty, 0))
            bottom = p->elementNumber();

        layout->setContentsMargins(left, top, right, bottom);
    }

    if (spacing != INT_MIN) {
        layout->setSpacing(spacing);
    } else {
        QGridLayout *grid = qobject_cast<QGridLayout *>(layout);
        if (grid) {
            const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
            const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

            if (const DomProperty *p = properties.value(strings.horizontalSpacingProperty, 0))
                grid->setHorizontalSpacing(p->elementNumber());
            if (const DomProperty *p = properties.value(strings.verticalSpacingProperty, 0))
                grid->setVerticalSpacing(p->elementNumber());
        }
    }

    applyProperties(layout, ui_layout->elementProperty());

    foreach (DomLayoutItem *ui_item, ui_layout->elementItem()) {
        if (QLayoutItem *item = create(ui_item, layout, parentWidget)) {
            addItem(ui_item, item, layout);
        }
    }

    return layout;
}

/*!
    \internal
*/
bool QAbstractFormBuilder::addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout)
{
    // Calling addChildWidget(), etc.  is required to maintain consistency of the layouts,
    // see documentation of addItem(), which should ideally not be used.
    if (item->widget()) {
        static_cast<QFriendlyLayout*>(layout)->addChildWidget(item->widget());
    } else if (item->layout()) {
        static_cast<QFriendlyLayout*>(layout)->addChildLayout(item->layout());
    } else if (item->spacerItem()) {
        // nothing to do
    } else {
        return false;
    }

    if (QGridLayout *grid = qobject_cast<QGridLayout*>(layout)) {
        const int rowSpan = ui_item->hasAttributeRowSpan() ? ui_item->attributeRowSpan() : 1;
        const int colSpan = ui_item->hasAttributeColSpan() ? ui_item->attributeColSpan() : 1;
        grid->addItem(item, ui_item->attributeRow(), ui_item->attributeColumn(),
                        rowSpan, colSpan, item->alignment());
        return true;
    }
#ifndef QT_NO_FORMLAYOUT
    if (QFormLayout *form = qobject_cast<QFormLayout *>(layout)) {
        const int row =  ui_item->attributeRow();
        form->setItem(row, ui_item->attributeColumn() == 0 ? QFormLayout::LabelRole : QFormLayout::FieldRole, item);
        return true;
    }

#endif
    layout->addItem(item);
    return true;
}

/*!
    \internal
*/
QLayoutItem *QAbstractFormBuilder::create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget)
{
    switch (ui_layoutItem->kind()) {
    case DomLayoutItem::Widget: {
        if (QWidget *w = create(ui_layoutItem->elementWidget(), parentWidget))
#ifdef QFORMINTERNAL_NAMESPACE // uilib
            return new QWidgetItemV2(w);
#else                         // Within Designer: Use factory method that returns special items that refuse to shrink to 0,0
            return QLayoutPrivate::createWidgetItem(layout, w);
#endif
        qWarning() << QObject::tr("Empty widget item in %1 '%2'.").arg(QString::fromUtf8(layout->metaObject()->className()), layout->objectName());
        return 0;
    }
    case DomLayoutItem::Spacer: {
        QSize size(0, 0);
        QSizePolicy::Policy sizeType = QSizePolicy::Expanding;
        bool isVspacer = false;

        const DomSpacer *ui_spacer = ui_layoutItem->elementSpacer();

        const QMetaEnum sizePolicy_enum  = metaEnum<QAbstractFormBuilderGadget>("sizeType");
        const QMetaEnum orientation_enum =  metaEnum<QAbstractFormBuilderGadget>("orientation");

        const QList<DomProperty *> spacerProperties =  ui_spacer->elementProperty();
        if (!spacerProperties.empty()) {
            const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
            foreach (DomProperty *p, spacerProperties) {
                const QVariant v = toVariant(&QAbstractFormBuilderGadget::staticMetaObject, p); // ### remove me
                if (v.isNull())
                    continue;
                if (p->attributeName() == strings.sizeHintProperty && p->kind() == DomProperty::Size) {
                    size = v.toSize();  // ###  remove me
                } else if (p->attributeName() == strings.sizeTypeProperty && p->kind() == DomProperty::Enum) {
                    sizeType = static_cast<QSizePolicy::Policy>(v.toInt());
                } else if (p->attributeName() == strings.orientationProperty && p->kind() == DomProperty::Enum) {
                    const Qt::Orientation o = static_cast<Qt::Orientation>(v.toInt());
                    isVspacer = (o == Qt::Vertical);
                }
            }
        }

        QSpacerItem *spacer = 0;
        if (isVspacer)
            spacer = new QSpacerItem(size.width(), size.height(), QSizePolicy::Minimum, sizeType);
        else
            spacer = new QSpacerItem(size.width(), size.height(), sizeType, QSizePolicy::Minimum);
        return spacer; }

    case DomLayoutItem::Layout:
        return create(ui_layoutItem->elementLayout(), layout, parentWidget);

    default:
        break;
    }

    return 0;
}

/*!
    \internal
*/
void QAbstractFormBuilder::applyProperties(QObject *o, const QList<DomProperty*> &properties)
{
    typedef QList<DomProperty*> DomPropertyList;

    if (properties.empty())
        return;

    QFormBuilderExtra *fb = QFormBuilderExtra::instance(this);

    const DomPropertyList::const_iterator cend = properties.constEnd();
    for (DomPropertyList::const_iterator it = properties.constBegin(); it != cend; ++it) {
        const QVariant v = toVariant(o->metaObject(), *it);
        if (!v.isNull()) {
            const  QString attributeName = (*it)->attributeName();
            if (!fb->applyPropertyInternally(o, attributeName, v))
                o->setProperty(attributeName.toUtf8(), v);
        }
    }
}


/*!
    \internal
    Check whether a property is applied internally by QAbstractFormBuilder. Call this
   from overwritten applyProperties().
*/

bool QAbstractFormBuilder::applyPropertyInternally(QObject *o, const QString &propertyName, const QVariant &value)
{
    return QFormBuilderExtra::instance(this)->applyPropertyInternally(o,propertyName, value);
}

/*!
    \internal
*/

QVariant QAbstractFormBuilder::toVariant(const QMetaObject *meta, DomProperty *p)
{
    return domPropertyToVariant(this, meta, p);
}

/*!
    \internal
*/
void QAbstractFormBuilder::setupColorGroup(QPalette &palette, QPalette::ColorGroup colorGroup,
            DomColorGroup *group)
{
    // old format
    const QList<DomColor*> colors = group->elementColor();
    for (int role = 0; role < colors.size(); ++role) {
        const DomColor *color = colors.at(role);
        const QColor c(color->elementRed(), color->elementGreen(), color->elementBlue());
        palette.setColor(colorGroup, QPalette::ColorRole(role), c);
    }

    // new format
    const QMetaEnum colorRole_enum = metaEnum<QAbstractFormBuilderGadget>("colorRole");

    const QList<DomColorRole*> colorRoles = group->elementColorRole();
    for (int role = 0; role < colorRoles.size(); ++role) {
        const DomColorRole *colorRole = colorRoles.at(role);

        if (colorRole->hasAttributeRole()) {
            const int r = colorRole_enum.keyToValue(colorRole->attributeRole().toLatin1());
            if (r != -1) {
                const QBrush br = setupBrush(colorRole->elementBrush());
                palette.setBrush(colorGroup, static_cast<QPalette::ColorRole>(r), br);
            }
        }
    }
}

/*!
    \internal
*/
DomColorGroup *QAbstractFormBuilder::saveColorGroup(const QPalette &palette)
{

    const QMetaEnum colorRole_enum = metaEnum<QAbstractFormBuilderGadget>("colorRole");

    DomColorGroup *group = new DomColorGroup();
    QList<DomColorRole*> colorRoles;

    const uint mask = palette.resolve();
    for (int role = QPalette::WindowText; role < QPalette::NColorRoles; ++role) {
        if (mask & (1 << role)) {
            QBrush br = palette.brush(QPalette::ColorRole(role));

            DomColorRole *colorRole = new DomColorRole();
            colorRole->setElementBrush(saveBrush(br));
            colorRole->setAttributeRole(QLatin1String(colorRole_enum.valueToKey(role)));
            colorRoles.append(colorRole);
        }
    }

    group->setElementColorRole(colorRoles);
    return group;
}

/*!
    \internal
*/
QBrush QAbstractFormBuilder::setupBrush(DomBrush *brush)
{
    QBrush br;
    if (!brush->hasAttributeBrushStyle())
        return br;

    const Qt::BrushStyle style = enumKeyOfObjectToValue<QAbstractFormBuilderGadget, Qt::BrushStyle>("brushStyle", brush->attributeBrushStyle().toLatin1());

    if (style == Qt::LinearGradientPattern ||
            style == Qt::RadialGradientPattern ||
            style == Qt::ConicalGradientPattern) {
        const QMetaEnum gradientType_enum = metaEnum<QAbstractFormBuilderGadget>("gradientType");
        const QMetaEnum gradientSpread_enum = metaEnum<QAbstractFormBuilderGadget>("gradientSpread");
        const QMetaEnum gradientCoordinate_enum = metaEnum<QAbstractFormBuilderGadget>("gradientCoordinate");

        const DomGradient *gradient = brush->elementGradient();
        const QGradient::Type type = enumKeyToValue<QGradient::Type>(gradientType_enum, gradient->attributeType().toLatin1());


        QGradient *gr = 0;

        if (type == QGradient::LinearGradient) {
            gr = new QLinearGradient(QPointF(gradient->attributeStartX(), gradient->attributeStartY()),
                            QPointF(gradient->attributeEndX(), gradient->attributeEndY()));
        } else if (type == QGradient::RadialGradient) {
            gr = new QRadialGradient(QPointF(gradient->attributeCentralX(), gradient->attributeCentralY()),
                            gradient->attributeRadius(),
                            QPointF(gradient->attributeFocalX(), gradient->attributeFocalY()));
        } else if (type == QGradient::ConicalGradient) {
            gr = new QConicalGradient(QPointF(gradient->attributeCentralX(), gradient->attributeCentralY()),
                            gradient->attributeAngle());
        }
        if (!gr)
            return br;

        const QGradient::Spread spread = enumKeyToValue<QGradient::Spread>(gradientSpread_enum, gradient->attributeSpread().toLatin1());
        gr->setSpread(spread);

        const QGradient::CoordinateMode coord = enumKeyToValue<QGradient::CoordinateMode>(gradientCoordinate_enum, gradient->attributeCoordinateMode().toLatin1());
        gr->setCoordinateMode(coord);

        const QList<DomGradientStop *> stops = gradient->elementGradientStop();
        QListIterator<DomGradientStop *> it(stops);
        while (it.hasNext()) {
            const DomGradientStop *stop = it.next();
            const DomColor *color = stop->elementColor();
            gr->setColorAt(stop->attributePosition(), QColor::fromRgb(color->elementRed(),
                            color->elementGreen(), color->elementBlue(), color->attributeAlpha()));
        }
        br = QBrush(*gr);
        delete gr;
    } else if (style == Qt::TexturePattern) {
        const DomProperty *texture = brush->elementTexture();
        if (texture && texture->kind() == DomProperty::Pixmap) {
            br.setTexture(domPropertyToPixmap(texture));
        }
    } else {
        const DomColor *color = brush->elementColor();
        br.setColor(QColor::fromRgb(color->elementRed(),
                            color->elementGreen(), color->elementBlue(), color->attributeAlpha()));
        br.setStyle((Qt::BrushStyle)style);
    }
    return br;
}

/*!
    \internal
*/
DomBrush *QAbstractFormBuilder::saveBrush(const QBrush &br)
{
    const QMetaEnum brushStyle_enum = metaEnum<QAbstractFormBuilderGadget>("brushStyle");

    DomBrush *brush = new DomBrush();
    const Qt::BrushStyle style = br.style();
    brush->setAttributeBrushStyle(QLatin1String(brushStyle_enum.valueToKey(style)));
    if (style == Qt::LinearGradientPattern ||
                style == Qt::RadialGradientPattern ||
                style == Qt::ConicalGradientPattern) {
        const QMetaEnum gradientType_enum = metaEnum<QAbstractFormBuilderGadget>("gradientType");
        const QMetaEnum gradientSpread_enum = metaEnum<QAbstractFormBuilderGadget>("gradientSpread");
        const QMetaEnum gradientCoordinate_enum = metaEnum<QAbstractFormBuilderGadget>("gradientCoordinate");

        DomGradient *gradient = new DomGradient();
        const QGradient *gr = br.gradient();
        const QGradient::Type type = gr->type();
        gradient->setAttributeType(QLatin1String(gradientType_enum.valueToKey(type)));
        gradient->setAttributeSpread(QLatin1String(gradientSpread_enum.valueToKey(gr->spread())));
        gradient->setAttributeCoordinateMode(QLatin1String(gradientCoordinate_enum.valueToKey(gr->coordinateMode())));
        QList<DomGradientStop *> stops;
        QGradientStops st = gr->stops();
        QVectorIterator<QPair<qreal, QColor> > it(st);
        while (it.hasNext()) {
            const QPair<qreal, QColor> pair = it.next();
            DomGradientStop *stop = new DomGradientStop();
            stop->setAttributePosition(pair.first);
            DomColor *color = new DomColor();
            color->setElementRed(pair.second.red());
            color->setElementGreen(pair.second.green());
            color->setElementBlue(pair.second.blue());
            color->setAttributeAlpha(pair.second.alpha());
            stop->setElementColor(color);
            stops.append(stop);
        }
        gradient->setElementGradientStop(stops);
        if (type == QGradient::LinearGradient) {
            QLinearGradient *lgr = (QLinearGradient *)(gr);
            gradient->setAttributeStartX(lgr->start().x());
            gradient->setAttributeStartY(lgr->start().y());
            gradient->setAttributeEndX(lgr->finalStop().x());
            gradient->setAttributeEndY(lgr->finalStop().y());
        } else if (type == QGradient::RadialGradient) {
            QRadialGradient *rgr = (QRadialGradient *)(gr);
            gradient->setAttributeCentralX(rgr->center().x());
            gradient->setAttributeCentralY(rgr->center().y());
            gradient->setAttributeFocalX(rgr->focalPoint().x());
            gradient->setAttributeFocalY(rgr->focalPoint().y());
            gradient->setAttributeRadius(rgr->radius());
        } else if (type == QGradient::ConicalGradient) {
            QConicalGradient *cgr = (QConicalGradient *)(gr);
            gradient->setAttributeCentralX(cgr->center().x());
            gradient->setAttributeCentralY(cgr->center().y());
            gradient->setAttributeAngle(cgr->angle());
        }

        brush->setElementGradient(gradient);
    } else if (style == Qt::TexturePattern) {
        const QPixmap pixmap = br.texture();
        if (!pixmap.isNull()) {
            DomProperty *p = new DomProperty;
            setPixmapProperty(*p,  pixmapPaths(pixmap));
            brush->setElementTexture(p);
        }
    } else {
        QColor c = br.color();
        DomColor *color = new DomColor();
        color->setElementRed(c.red());
        color->setElementGreen(c.green());
        color->setElementBlue(c.blue());
        color->setAttributeAlpha(c.alpha());
        brush->setElementColor(color);
    }
    return brush;
}

/*!
    \internal
*/
QWidget *QAbstractFormBuilder::createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name)
{
    Q_UNUSED(widgetName);
    Q_UNUSED(parentWidget);
    Q_UNUSED(name);
    return 0;
}

/*!
    \internal
*/
QLayout *QAbstractFormBuilder::createLayout(const QString &layoutName, QObject *parent, const QString &name)
{
    Q_UNUSED(layoutName);
    Q_UNUSED(parent);
    Q_UNUSED(name);
    return 0;
}

/*!
    \internal
*/
QAction *QAbstractFormBuilder::createAction(QObject *parent, const QString &name)
{
    QAction *action = new QAction(parent);
    action->setObjectName(name);
    m_actions.insert(name, action);

    return action;
}

/*!
    \internal
*/
QActionGroup *QAbstractFormBuilder::createActionGroup(QObject *parent, const QString &name)
{
    QActionGroup *g = new QActionGroup(parent);
    g->setObjectName(name);
    m_actionGroups.insert(name, g);

    return g;
}

/*!
    \fn void QAbstractFormBuilder::save(QIODevice *device, QWidget *widget)

    Saves an XML representation of the given \a widget to the
    specified \a device in the standard \c{.ui} file format.

    \sa load()*/
void QAbstractFormBuilder::save(QIODevice *dev, QWidget *widget)
{
    DomWidget *ui_widget = createDom(widget, 0);
    Q_ASSERT( ui_widget != 0 );

    DomUI *ui = new DomUI();
    ui->setAttributeVersion(QLatin1String("4.0"));
    ui->setElementWidget(ui_widget);

    saveDom(ui, widget);

    QDomDocument doc;
    doc.appendChild(ui->write(doc));
    QByteArray bytes = doc.toString().toUtf8();
    dev->write(bytes, bytes.size());

    m_laidout.clear();

    delete ui;
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveDom(DomUI *ui, QWidget *widget)
{
    ui->setElementClass(widget->objectName());

    if (DomConnections *ui_connections = saveConnections()) {
        ui->setElementConnections(ui_connections);
    }

    if (DomCustomWidgets *ui_customWidgets = saveCustomWidgets()) {
        ui->setElementCustomWidgets(ui_customWidgets);
    }

    if (DomTabStops *ui_tabStops = saveTabStops()) {
        ui->setElementTabStops(ui_tabStops);
    }

    if (DomResources *ui_resources = saveResources()) {
        ui->setElementResources(ui_resources);
    }
}

/*!
    \internal
*/
DomConnections *QAbstractFormBuilder::saveConnections()
{
    return new DomConnections;
}

/*!
    \internal
*/

DomWidget *QAbstractFormBuilder::createDom(QWidget *widget, DomWidget *ui_parentWidget, bool recursive)
{
    DomWidget *ui_widget = new DomWidget();
    ui_widget->setAttributeClass(QLatin1String(widget->metaObject()->className()));
    ui_widget->setElementProperty(computeProperties(widget));

    if (recursive) {
        if (QLayout *layout = widget->layout()) {
            if (DomLayout *ui_layout = createDom(layout, 0, ui_parentWidget)) {
                QList<DomLayout*> ui_layouts;
                ui_layouts.append(ui_layout);

                ui_widget->setElementLayout(ui_layouts);
            }
        }
    }

    // widgets, actions and action groups
    QList<DomWidget*> ui_widgets;
    QList<DomAction*> ui_actions;
    QList<DomActionGroup*> ui_action_groups;

    QList<QObject*> children;

    // splitters need to store their children in the order specified by child indexes,
    // not the order of the child list.
#ifndef QT_NO_SPLITTER
    if (const QSplitter *splitter = qobject_cast<const QSplitter*>(widget)) {
        const int count = splitter->count();
        for (int i = 0; i < count; ++i)
            children.append(splitter->widget(i));
    } else
#endif
    {
        QList<QObject *> childObjects = widget->children();

        const QList<QWidget *> list = qVariantValue<QWidgetList>(widget->property("_q_widgetOrder"));
        foreach (QWidget *w, list) {
            if (childObjects.contains(w)) {
                children.append(w);
                childObjects.removeAll(w);
            }
        }
        children += childObjects;

        const QList<QWidget *> zOrder = qVariantValue<QWidgetList>(widget->property("_q_zOrder"));
        if (list != zOrder) {
            QStringList zOrderList;
            QListIterator<QWidget* > itZOrder(zOrder);
            while (itZOrder.hasNext())
                zOrderList.append(itZOrder.next()->objectName());
            ui_widget->setElementZOrder(zOrderList);
        }
    }

    foreach (QObject *obj, children) {
        if (QWidget *childWidget = qobject_cast<QWidget*>(obj)) {
            if (m_laidout.contains(childWidget) || recursive == false)
                continue;

            if (QMenu *menu = qobject_cast<QMenu *>(childWidget)) {
                QList<QAction *> actions = menu->parentWidget()->actions();
                QListIterator<QAction *> it(actions);
                bool found = false;
                while (it.hasNext()) {
                    if (it.next()->menu() == menu)
                        found = true;
                }
                if (!found)
                    continue;
            }

            if (DomWidget *ui_child = createDom(childWidget, ui_widget)) {
                ui_widgets.append(ui_child);
            }
        } else if (QAction *childAction = qobject_cast<QAction*>(obj)) {
            if (childAction->actionGroup() != 0) {
                // it will be added later.
                continue;
            }

            if (DomAction *ui_action = createDom(childAction)) {
                ui_actions.append(ui_action);
            }
        } else if (QActionGroup *childActionGroup = qobject_cast<QActionGroup*>(obj)) {
            if (DomActionGroup *ui_action_group = createDom(childActionGroup)) {
                ui_action_groups.append(ui_action_group);
            }
        }
    }

    // add-action
    QList<DomActionRef*> ui_action_refs;
    foreach (QAction *action, widget->actions()) {
        if (DomActionRef *ui_action_ref = createActionRefDom(action)) {
            ui_action_refs.append(ui_action_ref);
        }
    }

    if (recursive)
        ui_widget->setElementWidget(ui_widgets);

    ui_widget->setElementAction(ui_actions);
    ui_widget->setElementActionGroup(ui_action_groups);
    ui_widget->setElementAddAction(ui_action_refs);

    saveExtraInfo(widget, ui_widget, ui_parentWidget);

    return ui_widget;
}

/*!
    \internal
*/
DomActionRef *QAbstractFormBuilder::createActionRefDom(QAction *action)
{
    QString name = action->objectName();

    if (action->menu() != 0)
        name = action->menu()->objectName();

    DomActionRef *ui_action_ref = new DomActionRef();
    if (action->isSeparator())
        ui_action_ref->setAttributeName(QFormBuilderStrings::instance().separator);
    else
        ui_action_ref->setAttributeName(name);

    return ui_action_ref;
}

/*!
    \internal
*/
DomLayout *QAbstractFormBuilder::createDom(QLayout *layout, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_layout)
    DomLayout *lay = new DomLayout();
    lay->setAttributeClass(QLatin1String(layout->metaObject()->className()));
    const QString objectName = layout->objectName();
    if (!objectName.isEmpty())
        lay->setAttributeName(objectName);
    lay->setElementProperty(computeProperties(layout));

    QList<DomLayoutItem*> ui_items;

    QMap<QObject *, QLayoutItem *> objectToItem;
    QList<QLayoutItem *> spacerItems;
    QList<QLayoutItem *> newList;

    for (int idx=0; layout->itemAt(idx); ++idx) {
        QLayoutItem *item = layout->itemAt(idx);
        if (item->widget())
            objectToItem[item->widget()] = item;
        else if (item->layout())
            objectToItem[item->layout()] = item;
        else if (item->spacerItem())
            spacerItems.append(item);
        newList.append(item);
    }

    if (qobject_cast<QGridLayout *>(layout)) {
        newList.clear();
        QList<QObject *> childrenList = layout->parentWidget()->children();
        foreach (QObject *o, childrenList) {
            if (objectToItem.contains(o))
                newList.append(objectToItem[o]);
        }
        newList += spacerItems;
    }

    foreach (QLayoutItem *item, newList) {
        DomLayoutItem *ui_item = createDom(item, lay, ui_parentWidget);
        if (ui_item)
            ui_items.append(ui_item);
    }

    lay->setElementItem(ui_items);

    return lay;
}

/*!
    \internal
*/
DomLayoutItem *QAbstractFormBuilder::createDom(QLayoutItem *item, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    DomLayoutItem *ui_item = new DomLayoutItem();

    if (item->widget())  {
        ui_item->setElementWidget(createDom(item->widget(), ui_parentWidget));
        m_laidout.insert(item->widget(), true);
    } else if (item->layout()) {
        ui_item->setElementLayout(createDom(item->layout(), ui_layout, ui_parentWidget));
    } else if (item->spacerItem()) {
        ui_item->setElementSpacer(createDom(item->spacerItem(), ui_layout, ui_parentWidget));
    }

    return ui_item;
}

/*!
    \internal
*/
DomSpacer *QAbstractFormBuilder::createDom(QSpacerItem *spacer, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_layout);
    Q_UNUSED(ui_parentWidget);

    DomSpacer *ui_spacer = new DomSpacer();
    QList<DomProperty*> properties;

    DomProperty *prop = 0;
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    // sizeHint property
    prop = new DomProperty();
    prop->setAttributeName(strings.sizeHintProperty);
    prop->setElementSize(new DomSize());
    prop->elementSize()->setElementWidth(spacer->sizeHint().width());
    prop->elementSize()->setElementHeight(spacer->sizeHint().height());
    properties.append(prop);

    // orientation property
    prop = new DomProperty(); // ### we don't implemented the case where expandingDirections() is both Vertical and Horizontal
    prop->setAttributeName(strings.orientationProperty);
    prop->setElementEnum((spacer->expandingDirections() & Qt::Horizontal) ? strings.qtHorizontal : strings.qtVertical);
    properties.append(prop);

    ui_spacer->setElementProperty(properties);
    return ui_spacer;
}

/*!
    \internal
*/
DomProperty *QAbstractFormBuilder::createProperty(QObject *obj, const QString &pname, const QVariant &v)
{
    if (!checkProperty(obj, pname)) {
        return 0;
    }
    return variantToDomProperty(this, obj, pname, v);
}

/*!
    \internal
*/
QList<DomProperty*> QAbstractFormBuilder::computeProperties(QObject *obj)
{
    QList<DomProperty*> lst;

    const QMetaObject *meta = obj->metaObject();

    QHash<QByteArray, bool> properties;
    const int propertyCount = meta->propertyCount();
    for(int i=0; i < propertyCount; ++i)
        properties.insert(meta->property(i).name(), true);

    const QList<QByteArray> propertyNames = properties.keys();

    const int propertyNamesCount = propertyNames.size();
    for(int i=0; i<propertyNamesCount ; ++i) {
        const QString pname = QString::fromUtf8(propertyNames.at(i));
        const QMetaProperty prop = meta->property(meta->indexOfProperty(pname.toUtf8()));

        if (!prop.isWritable() || !checkProperty(obj, QLatin1String(prop.name())))
            continue;

        const QVariant v = prop.read(obj);

        DomProperty *dom_prop = 0;
        if (v.type() == QVariant::Int) {
            dom_prop = new DomProperty();

            if (prop.isFlagType())
                uiLibWarning(QObject::tr("Flags property are not supported yet."));

            if (prop.isEnumType()) {
                QString scope = QString::fromUtf8(prop.enumerator().scope());
                if (scope.size())
                    scope += QString::fromUtf8("::");
                const QString e = QString::fromUtf8(prop.enumerator().valueToKey(v.toInt()));
                if (e.size())
                    dom_prop->setElementEnum(scope + e);
            } else
                dom_prop->setElementNumber(v.toInt());
            dom_prop->setAttributeName(pname);
        } else {
            dom_prop = createProperty(obj, pname, v);
        }

        if (!dom_prop || dom_prop->kind() == DomProperty::Unknown)
            delete dom_prop;
        else
            lst.append(dom_prop);
    }

    return lst;
}


/*!
   \internal
   \typedef QAbstractFormBuilder::DomPropertyHash
   \typedef QAbstractFormBuilder::IconPaths
*/


/*!
    \internal
*/
QAbstractFormBuilder::DomPropertyHash QAbstractFormBuilder::propertyMap(const QList<DomProperty*> &properties)
{
    DomPropertyHash map;

    foreach (DomProperty *p, properties)
        map.insert(p->attributeName(), p);

    return map;
}

/*!
    \internal
*/
bool QAbstractFormBuilder::checkProperty(QObject *obj, const QString &prop) const
{
    Q_UNUSED(obj);
    Q_UNUSED(prop);

    return true;
}

/*!
    \internal
*/
QString QAbstractFormBuilder::toString(const DomString *str)
{
    return str ? str->text() : QString();
}

/*!
    \internal
*/
void QAbstractFormBuilder::applyTabStops(QWidget *widget, DomTabStops *tabStops)
{
    if (!tabStops)
        return;

    QWidget *lastWidget = 0;

    const QStringList l = tabStops->elementTabStop();
    for (int i=0; i<l.size(); ++i) {
        const QString name = l.at(i);

        QWidget *child = qFindChild<QWidget*>(widget, name);
        if (!child) {
            uiLibWarning(QObject::tr("While applying tab stops: The widget '%1' could not be found.").arg(name));
            continue;
        }

        if (i == 0) {
            lastWidget = qFindChild<QWidget*>(widget, name);
            continue;
        } else if (!child || !lastWidget) {
            continue;
        }

        QWidget::setTabOrder(lastWidget, child);

        lastWidget = qFindChild<QWidget*>(widget, name);
    }
}

/*!
    \internal
*/
DomCustomWidgets *QAbstractFormBuilder::saveCustomWidgets()
{
    return 0;
}

/*!
    \internal
*/
DomTabStops *QAbstractFormBuilder::saveTabStops()
{
    return 0;
}

/*!
    \internal
*/
DomResources *QAbstractFormBuilder::saveResources()
{
    return 0;
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveTreeWidgetExtraInfo(QTreeWidget *treeWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);

    QList<DomColumn*> columns;
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    // save the header
    for (int c = 0; c<treeWidget->columnCount(); ++c) {
        DomColumn *column = new DomColumn;

        QList<DomProperty*> properties;

        // property text
        DomProperty *ptext = new DomProperty;
        DomString *str = new DomString;
        str->setText(treeWidget->headerItem()->text(c));
        ptext->setAttributeName(strings.textAttribute);
        ptext->setElementString(str);
        properties.append(ptext);

        if (DomProperty *p = saveResource(treeWidget->headerItem()->data(c, resourceRole())))
            properties.append(p);

        column->setElementProperty(properties);
        columns.append(column);
    }

    ui_widget->setElementColumn(columns);

    QList<DomItem *> items = ui_widget->elementItem();

    QQueue<QPair<QTreeWidgetItem *, DomItem *> > pendingQueue;
    for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
        pendingQueue.enqueue(qMakePair(treeWidget->topLevelItem(i), (DomItem *)0));

    while (!pendingQueue.isEmpty()) {
        const QPair<QTreeWidgetItem *, DomItem *> pair = pendingQueue.dequeue();
        QTreeWidgetItem *item = pair.first;
        DomItem *parentDomItem = pair.second;

        DomItem *currentDomItem = new DomItem;

        QList<DomProperty*> properties;
        for (int c = 0; c < treeWidget->columnCount(); c++) {
            DomProperty *ptext = new DomProperty;
            DomString *str = new DomString;
            str->setText(item->text(c));
            ptext->setAttributeName(strings.textAttribute);
            ptext->setElementString(str);
            properties.append(ptext);

            if (DomProperty *p = saveResource(item->data(c, resourceRole())))
                properties.append(p);
        }
        currentDomItem->setElementProperty(properties);

        if (parentDomItem) {
            QList<DomItem *> childrenItems = parentDomItem->elementItem();
            childrenItems.append(currentDomItem);
            parentDomItem->setElementItem(childrenItems);
        } else
            items.append(currentDomItem);

        for (int i = 0; i < item->childCount(); i++)
            pendingQueue.enqueue(qMakePair(item->child(i), currentDomItem));
    }

    ui_widget->setElementItem(items);
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveTableWidgetExtraInfo(QTableWidget *tableWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    // save the horizontal header
    QList<DomColumn*> columns;
    for (int c = 0; c < tableWidget->columnCount(); c++) {
        DomColumn *column = new DomColumn;
        QList<DomProperty*> properties;
        QTableWidgetItem *item = tableWidget->horizontalHeaderItem(c);
        if (item) {
            // property text
            DomProperty *ptext = new DomProperty;
            DomString *str = new DomString;
            str->setText(item->text());
            ptext->setAttributeName(strings.textAttribute);
            ptext->setElementString(str);
            properties.append(ptext);

            if (DomProperty *p = saveResource(item->data(resourceRole())))
                properties.append(p);
        }

        column->setElementProperty(properties);
        columns.append(column);
    }
    ui_widget->setElementColumn(columns);

    // save the vertical header
    QList<DomRow*> rows;
    for (int r = 0; r < tableWidget->rowCount(); r++) {
        DomRow *row = new DomRow;
        QList<DomProperty*> properties;
        QTableWidgetItem *item = tableWidget->verticalHeaderItem(r);
        if (item) {
            // property text
            DomProperty *ptext = new DomProperty;
            DomString *str = new DomString;
            str->setText(item->text());
            ptext->setAttributeName(strings.textAttribute);
            ptext->setElementString(str);
            properties.append(ptext);

            if (DomProperty *p = saveResource(item->data(resourceRole())))
                properties.append(p);
        }

        row->setElementProperty(properties);
        rows.append(row);
    }
    ui_widget->setElementRow(rows);

    QList<DomItem *> items = ui_widget->elementItem();

    for (int r = 0; r < tableWidget->rowCount(); r++)
        for (int c = 0; c < tableWidget->columnCount(); c++) {
            QTableWidgetItem *item = tableWidget->item(r, c);
            if (item) {
                DomItem *domItem = new DomItem;
                domItem->setAttributeRow(r);
                domItem->setAttributeColumn(c);
                QList<DomProperty*> properties;

                DomProperty *ptext = new DomProperty;

                DomString *str = new DomString;
                str->setText(item->text());
                ptext->setAttributeName(strings.textAttribute);
                ptext->setElementString(str);
                properties.append(ptext);

                if (DomProperty *p = saveResource(item->data(resourceRole())))
                    properties.append(p);

                domItem->setElementProperty(properties);
                items.append(domItem);
            }
        }

    ui_widget->setElementItem(items);
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveListWidgetExtraInfo(QListWidget *listWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);

    QList<DomItem*> ui_items = ui_widget->elementItem();
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    for (int i=0; i<listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        DomItem *ui_item = new DomItem();

        QList<DomProperty*> properties;

        // text
        DomString *str = new DomString;
        str->setText(item->text());

        DomProperty *p = 0;

        p = new DomProperty;
        p->setAttributeName(strings.textAttribute);
        p->setElementString(str);
        properties.append(p);

        if (DomProperty *p = saveResource(item->data(resourceRole())))
            properties.append(p);

        ui_item->setElementProperty(properties);
        ui_items.append(ui_item);
    }

    ui_widget->setElementItem(ui_items);
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveComboBoxExtraInfo(QComboBox *comboBox, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    QList<DomItem*> ui_items = ui_widget->elementItem();

    for (int i=0; i<comboBox->count(); ++i) {
        DomItem *ui_item = new DomItem();

        QList<DomProperty*> properties;

        // text
        DomString *str = new DomString;
        str->setText(comboBox->itemText(i));

        DomProperty *p = 0;

        p = new DomProperty;
        p->setAttributeName(strings.textAttribute);
        p->setElementString(str);
        properties.append(p);

        if (DomProperty *p = saveResource(comboBox->itemData(i, resourceRole())))
                properties.append(p);

        ui_item->setElementProperty(properties);
        ui_items.append(ui_item);
    }

    ui_widget->setElementItem(ui_items);
}

/*!
    \internal
    \since 4.4
*/

void QAbstractFormBuilder::setResourceBuilder(QResourceBuilder *builder)
{
    QFormBuilderExtra::instance(this)->setResourceBuilder(builder);
}

/*!
    \internal
    \since 4.4
*/

QResourceBuilder *QAbstractFormBuilder::resourceBuilder() const
{
    return QFormBuilderExtra::instance(this)->resourceBuilder();
}

/*!
    \internal
    \since 4.4
*/

int QAbstractFormBuilder::resourceRole()
{
    return Qt::UserRole + 987;
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveExtraInfo(QWidget *widget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    if (QListWidget *listWidget = qobject_cast<QListWidget*>(widget)) {
        saveListWidgetExtraInfo(listWidget, ui_widget, ui_parentWidget);
    } else if (QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(widget)) {
        saveTreeWidgetExtraInfo(treeWidget, ui_widget, ui_parentWidget);
    } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget*>(widget)) {
        saveTableWidgetExtraInfo(tableWidget, ui_widget, ui_parentWidget);
    } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(widget)) {
        if (!qobject_cast<QFontComboBox*>(widget))
            saveComboBoxExtraInfo(comboBox, ui_widget, ui_parentWidget);
    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadListWidgetExtraInfo(DomWidget *ui_widget, QListWidget *listWidget, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    foreach (DomItem *ui_item, ui_widget->elementItem()) {
        const DomPropertyHash properties = propertyMap(ui_item->elementProperty());
        QListWidgetItem *item = new QListWidgetItem(listWidget);

        DomProperty *p = properties.value(strings.textAttribute);
        if (p && p->kind() == DomProperty::String) {
            item->setText(p->elementString()->text());
        }

        p = properties.value(strings.iconAttribute);
        if (p) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), p);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            item->setIcon(qVariantValue<QIcon>(nativeValue));
            item->setData(resourceRole(), v);
        }
    }

    DomProperty *currentRow = propertyMap(ui_widget->elementProperty()).value(strings.currentRowProperty);
    if (currentRow)
        listWidget->setCurrentRow(currentRow->elementNumber());
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadTreeWidgetExtraInfo(DomWidget *ui_widget, QTreeWidget *treeWidget, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const QList<DomColumn*> columns = ui_widget->elementColumn();

    if (columns.count() > 0)
        treeWidget->setColumnCount(columns.count());

    for (int i = 0; i<columns.count(); ++i) {
        const DomColumn *c = columns.at(i);
        const DomPropertyHash properties = propertyMap(c->elementProperty());

        DomProperty *ptext = properties.value(strings.textAttribute);
        DomProperty *picon = properties.value(strings.iconAttribute);

        if (ptext != 0 && ptext->elementString())
            treeWidget->headerItem()->setText(i, ptext->elementString()->text());

        if (picon) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            treeWidget->headerItem()->setIcon(i, qVariantValue<QIcon>(nativeValue));
            treeWidget->headerItem()->setData(i, resourceRole(), v);
        }
    }

    QQueue<QPair<DomItem *, QTreeWidgetItem *> > pendingQueue;
    foreach (DomItem *ui_item, ui_widget->elementItem())
        pendingQueue.enqueue(qMakePair(ui_item, (QTreeWidgetItem *)0));

    while (!pendingQueue.isEmpty()) {
        const QPair<DomItem *, QTreeWidgetItem *> pair = pendingQueue.dequeue();
        const DomItem *domItem = pair.first;
        QTreeWidgetItem *parentItem = pair.second;

        QTreeWidgetItem *currentItem = 0;

        if (parentItem)
            currentItem = new QTreeWidgetItem(parentItem);
        else
            currentItem = new QTreeWidgetItem(treeWidget);

        const QList<DomProperty *> properties = domItem->elementProperty();
        int col = 0;
        foreach (DomProperty *property, properties) {
            if (property->attributeName() == strings.textAttribute &&
                        property->elementString()) {
                currentItem->setText(col, property->elementString()->text());
                col++;
            } else if (property->attributeName() == strings.iconAttribute && col > 0) {
                QVariant v = resourceBuilder()->loadResource(workingDirectory(), property);
                QVariant nativeValue = resourceBuilder()->toNativeValue(v);
                currentItem->setIcon(col - 1, qVariantValue<QIcon>(nativeValue));
                currentItem->setData(col - 1, resourceRole(), v);
            }
        }


        foreach (DomItem *childItem, domItem->elementItem())
            pendingQueue.enqueue(qMakePair(childItem, currentItem));

    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadTableWidgetExtraInfo(DomWidget *ui_widget, QTableWidget *tableWidget, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const QList<DomColumn*> columns = ui_widget->elementColumn();
    if (columns.count() > 0)
        tableWidget->setColumnCount(columns.count());
    for (int i = 0; i< columns.count(); i++) {
        DomColumn *c = columns.at(i);
        const DomPropertyHash properties = propertyMap(c->elementProperty());

        const DomProperty *ptext = properties.value(strings.textAttribute);
        const DomProperty *picon = properties.value(strings.iconAttribute);

        if (ptext || picon) {
            QTableWidgetItem *item = new QTableWidgetItem;
            if (ptext != 0 && ptext->elementString()) {
                item->setText(ptext->elementString()->text());
            }

            if (picon) {
                QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
                QVariant nativeValue = resourceBuilder()->toNativeValue(v);
                item->setIcon(qVariantValue<QIcon>(nativeValue));
                item->setData(resourceRole(), v);
            }
            tableWidget->setHorizontalHeaderItem(i, item);
        }
    }

    const QList<DomRow*> rows = ui_widget->elementRow();
    if (rows.count() > 0)
        tableWidget->setRowCount(rows.count());
    for (int i = 0; i< rows.count(); i++) {
        const DomRow *r = rows.at(i);
        const DomPropertyHash properties = propertyMap(r->elementProperty());

        const DomProperty *ptext = properties.value(strings.textAttribute);
        const DomProperty *picon = properties.value(strings.iconAttribute);

        if (ptext || picon) {
            QTableWidgetItem *item = new QTableWidgetItem;
            if (ptext != 0 && ptext->elementString()) {
                item->setText(ptext->elementString()->text());
            }

            if (picon) {
                QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
                QVariant nativeValue = resourceBuilder()->toNativeValue(v);
                item->setIcon(qVariantValue<QIcon>(nativeValue));
                item->setData(resourceRole(), v);
            }
            tableWidget->setVerticalHeaderItem(i, item);
        }
    }

    foreach (DomItem *ui_item, ui_widget->elementItem()) {
        if (ui_item->hasAttributeRow() && ui_item->hasAttributeColumn()) {
            QTableWidgetItem *item = new QTableWidgetItem;
            foreach (DomProperty *property, ui_item->elementProperty()) {
                if (property->attributeName() == strings.textAttribute &&
                        property->elementString()) {
                    item->setText(property->elementString()->text());
                } else if (property->attributeName() == strings.iconAttribute) {
                    QVariant v = resourceBuilder()->loadResource(workingDirectory(), property);
                    QVariant nativeValue = resourceBuilder()->toNativeValue(v);
                    item->setIcon(qVariantValue<QIcon>(nativeValue));
                    item->setData(resourceRole(), v);
                }

            }
            tableWidget->setItem(ui_item->attributeRow(), ui_item->attributeColumn(), item);
        }
    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadComboBoxExtraInfo(DomWidget *ui_widget, QComboBox *comboBox, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    foreach (DomItem *ui_item, ui_widget->elementItem()) {
        const DomPropertyHash properties = propertyMap(ui_item->elementProperty());
        QString text;
        QIcon icon;
        QVariant data;

        DomProperty *p = 0;

        p = properties.value(strings.textAttribute);
        if (p && p->elementString()) {
            text = p->elementString()->text();
        }

        p = properties.value(strings.iconAttribute);
        if (p) {
             data = resourceBuilder()->loadResource(workingDirectory(), p);
             icon = qVariantValue<QIcon>(resourceBuilder()->toNativeValue(data));
        }

        comboBox->addItem(icon, text);
        comboBox->setItemData((comboBox->count()-1), icon);
        comboBox->setItemData((comboBox->count()-1), data, resourceRole());
    }

    DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
    if (currentIndex)
        comboBox->setCurrentIndex(currentIndex->elementNumber());
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadExtraInfo(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    if (0) {
#ifndef QT_NO_LISTWIDGET
    } else if (QListWidget *listWidget = qobject_cast<QListWidget*>(widget)) {
        loadListWidgetExtraInfo(ui_widget, listWidget, parentWidget);
#endif
#ifndef QT_NO_TREEWIDGET
    } else if (QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(widget)) {
        loadTreeWidgetExtraInfo(ui_widget, treeWidget, parentWidget);
#endif
#ifndef QT_NO_TABLEWIDGET
    } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget*>(widget)) {
        loadTableWidgetExtraInfo(ui_widget, tableWidget, parentWidget);
#endif
#ifndef QT_NO_COMBOBOX
    } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(widget)) {
        if (!qobject_cast<QFontComboBox *>(widget))
            loadComboBoxExtraInfo(ui_widget, comboBox, parentWidget);
#endif
#ifndef QT_NO_TABWIDGET
    } else if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget)) {
        const DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
        if (currentIndex)
            tabWidget->setCurrentIndex(currentIndex->elementNumber());
#endif
#ifndef QT_NO_STACKEDWIDGET
    } else if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(widget)) {
        const DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
        if (currentIndex)
            stackedWidget->setCurrentIndex(currentIndex->elementNumber());
#endif
#ifndef QT_NO_TOOLBOX
    } else if (QToolBox *toolBox = qobject_cast<QToolBox*>(widget)) {
        const DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
        if (currentIndex)
            toolBox->setCurrentIndex(currentIndex->elementNumber());
        const DomProperty *tabSpacing = propertyMap(ui_widget->elementProperty()).value(strings.tabSpacingProperty);
        if (tabSpacing)
            toolBox->layout()->setSpacing(tabSpacing->elementNumber());
#endif
    }
}

/*!
    \internal
*/
QIcon QAbstractFormBuilder::nameToIcon(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QAbstractFormBuilder::nameToIcon() is obsoleted";
    return QIcon();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::iconToFilePath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::iconToFilePath() is obsoleted";
    return QString();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::iconToQrcPath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::iconToQrcPath() is obsoleted";
    return QString();
}

/*!
    \internal
*/
QPixmap QAbstractFormBuilder::nameToPixmap(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QAbstractFormBuilder::nameToPixmap() is obsoleted";
    return QPixmap();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::pixmapToFilePath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::pixmapToFilePath() is obsoleted";
    return QString();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::pixmapToQrcPath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::pixmapToQrcPath() is obsoleted";
    return QString();
}

/*!
    Returns the current working directory of the form builder.

    \sa setWorkingDirectory() */
QDir QAbstractFormBuilder::workingDirectory() const
{
    return m_workingDirectory;
}

/*!
    Sets the current working directory of the form builder to the
    specified \a directory.

    \sa workingDirectory()*/
void QAbstractFormBuilder::setWorkingDirectory(const QDir &directory)
{
    m_workingDirectory = directory;
}

/*!
    \internal
*/
DomAction *QAbstractFormBuilder::createDom(QAction *action)
{
    if (action->parentWidget() == action->menu() || action->isSeparator())
        return 0;

    DomAction *ui_action = new DomAction;
    ui_action->setAttributeName(action->objectName());

    const QList<DomProperty*> properties = computeProperties(action);
    ui_action->setElementProperty(properties);

    return ui_action;
}

/*!
    \internal
*/
DomActionGroup *QAbstractFormBuilder::createDom(QActionGroup *actionGroup)
{
    DomActionGroup *ui_action_group = new DomActionGroup;
    ui_action_group->setAttributeName(actionGroup->objectName());

    QList<DomProperty*> properties = computeProperties(actionGroup);
    ui_action_group->setElementProperty(properties);

    QList<DomAction*> ui_actions;

    foreach (QAction *action, actionGroup->actions()) {
        if (DomAction *ui_action = createDom(action)) {
            ui_actions.append(ui_action);
        }
    }

    ui_action_group->setElementAction(ui_actions);

    return ui_action_group;
}

/*!
    \internal
*/
void QAbstractFormBuilder::addMenuAction(QAction *action)
{
    Q_UNUSED(action);
}

/*!
    \internal
*/
void QAbstractFormBuilder::reset()
{
    m_laidout.clear();
    m_actions.clear();
    m_actionGroups.clear();
    m_defaultMargin = INT_MIN;
    m_defaultSpacing = INT_MIN;
}

/*!
    \internal
    Access meta enumeration for Qt::ToolBarArea
*/

QMetaEnum QAbstractFormBuilder::toolBarAreaMetaEnum()
{
    return metaEnum<QAbstractFormBuilderGadget>("toolBarArea");
}

namespace {
    // set forward slashes in image path.
    inline void fixImagePath(QString &p)    {
        p.replace(QLatin1Char('\\'), QLatin1Char('/'));
    }
}

/*!
    \internal
    Return paths of an icon.
*/

QAbstractFormBuilder::IconPaths QAbstractFormBuilder::iconPaths(const QIcon &icon) const
{
    Q_UNUSED(icon);
    qWarning() << "QAbstractFormBuilder::iconPaths() is obsoleted";
    return IconPaths();
}

/*!
    \internal
    Return paths of a pixmap.
*/

QAbstractFormBuilder::IconPaths QAbstractFormBuilder::pixmapPaths(const QPixmap &pixmap) const
{
    Q_UNUSED(pixmap);
    qWarning() << "QAbstractFormBuilder::pixmapPaths() is obsoleted";
    return IconPaths();
}

/*!
    \internal
    Set up a DOM property with icon.
*/

void QAbstractFormBuilder::setIconProperty(DomProperty &p, const IconPaths &ip) const
{
    DomResourceIcon *dpi = new DomResourceIcon;

 /* TODO
    if (!ip.second.isEmpty())
        pix->setAttributeResource(ip.second);
*/
    dpi->setText(ip.first);

    p.setAttributeName(QFormBuilderStrings::instance().iconAttribute);
    p.setElementIconSet(dpi);
}

/*!
    \internal
    Set up a DOM property with pixmap.
*/

void QAbstractFormBuilder::setPixmapProperty(DomProperty &p, const IconPaths &ip) const
{
    DomResourcePixmap *pix = new DomResourcePixmap;
    if (!ip.second.isEmpty())
        pix->setAttributeResource(ip.second);

    pix->setText(ip.first);

    p.setAttributeName(QFormBuilderStrings::instance().pixmapAttribute);
    p.setElementPixmap(pix);
}

/*!
    \internal
    Convenience. Return DOM property for icon; 0 if icon.isNull().
*/

DomProperty* QAbstractFormBuilder::iconToDomProperty(const QIcon &icon) const
{
    Q_UNUSED(icon);
    qWarning() << "QAbstractFormBuilder::iconToDomProperty() is obsoleted";
    return 0;
}

/*!
    \internal
    \since 4.4
*/

DomProperty *QAbstractFormBuilder::saveResource(const QVariant &v) const
{
    if (v.isNull())
        return 0;

    DomProperty *p = resourceBuilder()->saveResource(workingDirectory(), v);
    if (p)
        p->setAttributeName(QFormBuilderStrings::instance().iconAttribute);
    return p;
}
/*!
    \internal
    Return the appropriate DOM pixmap for an image dom property.
    From 4.4 - unused
*/

const DomResourcePixmap *QAbstractFormBuilder::domPixmap(const DomProperty* p) {
    switch (p->kind()) {
    case DomProperty::IconSet:
        qDebug() << "** WARNING QAbstractFormBuilder::domPixmap() called for icon set!";
        break;
    case DomProperty::Pixmap:
        return p->elementPixmap();
    default:
        break;
    }
    return 0;
}

/*!
    \internal
    Create icon from DOM.
    From 4.4 - unused
*/

QIcon QAbstractFormBuilder::domPropertyToIcon(const DomResourcePixmap *icon)
{
    Q_UNUSED(icon);
    qWarning() << "QAbstractFormBuilder::domPropertyToIcon() is obsoleted";
    return QIcon();
}

/*!
    \internal
    Create icon from DOM. Assert if !domPixmap
    From 4.4 - unused
*/

QIcon QAbstractFormBuilder::domPropertyToIcon(const DomProperty* p)
{
    Q_UNUSED(p);
    qWarning() << "QAbstractFormBuilder::domPropertyToIcon() is obsoleted";
    return QIcon();
}


/*!
    \internal
    Create pixmap from DOM.
    From 4.4 - unused
*/

QPixmap QAbstractFormBuilder::domPropertyToPixmap(const DomResourcePixmap* pixmap)
{
    Q_UNUSED(pixmap);
    qWarning() << "QAbstractFormBuilder::domPropertyToPixmap() is obsoleted";
    return QPixmap();
}


/*!
    \internal
    Create pixmap from DOM. Assert if !domPixmap
    From 4.4 - unused
*/

QPixmap QAbstractFormBuilder::domPropertyToPixmap(const DomProperty* p)
{
    Q_UNUSED(p);
    qWarning() << "QAbstractFormBuilder::domPropertyToPixmap() is obsoleted";
    return QPixmap();
}

/*!
    \fn void QAbstractFormBuilder::createConnections ( DomConnections *, QWidget * )
    \internal
*/

/*!
    \fn void QAbstractFormBuilder::createCustomWidgets ( DomCustomWidgets * )
    \internal
*/

/*!
    \fn void QAbstractFormBuilder::createResources ( DomResources * )
    \internal
*/

/*!
     \fn QFormScriptRunner *QAbstractFormBuilder::formScriptRunner() const
     \internal
     \since 4.3
*/
#ifndef QT_FORMBUILDER_NO_SCRIPT
QFormScriptRunner *QAbstractFormBuilder::formScriptRunner() const
{
    return &(QFormBuilderExtra::instance(this)->formScriptRunner());
}
#endif

/*!
    Sets whether the execution of scripts is enabled to \a enabled.
    \since 4.3
    \internal
*/

void QAbstractFormBuilder::setScriptingEnabled(bool enabled)
{
#ifdef QT_FORMBUILDER_NO_SCRIPT
    if (enabled)
        uiLibWarning(QObject::tr("This version of the uitools library is linked without script support."));
#else
    QFormScriptRunner::Options options = formScriptRunner()->options();
    if (enabled)
        options &= ~QFormScriptRunner::DisableScripts;
    else
        options |= QFormScriptRunner::DisableScripts;
    formScriptRunner()->setOptions(options);
#endif
}

/*!
    Returns whether the execution of scripts is enabled.
    \sa setScriptingEnabled()
    \since 4.3
    \internal
*/

bool QAbstractFormBuilder::isScriptingEnabled() const
{
#ifdef QT_FORMBUILDER_NO_SCRIPT
    return false;
#else
    return !(formScriptRunner()->options() & QFormScriptRunner::DisableScripts);
#endif
}

QT_END_NAMESPACE
