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

/*
TRANSLATOR qdesigner_internal::WidgetFactory
*/

#include "widgetfactory_p.h"
#include "widgetdatabase_p.h"
#include "metadatabase_p.h"
#include "qlayout_widget_p.h"
#include "qdesigner_widget_p.h"
#include "qdesigner_tabwidget_p.h"
#include "qdesigner_toolbox_p.h"
#include "qdesigner_stackedbox_p.h"
#include "qdesigner_toolbar_p.h"
#include "qdesigner_menubar_p.h"
#include "qdesigner_menu_p.h"
#include "qdesigner_dockwidget_p.h"
#include "qdesigner_utils_p.h"
#include "abstractformwindow.h"

// shared
#include "layoutinfo_p.h"
#include "spacer_widget_p.h"
#include "layout_p.h"
#include "abstractintrospection_p.h"

// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerLanguageExtension>

#include <QtGui/QtGui>
#include <QtGui/QScrollBar>
#include <QtGui/QFontComboBox>
#include <QtGui/QAbstractSpinBox>
#include <QtGui/QLineEdit>
#include <QtCore/qdebug.h>
#include <QtCore/QMetaObject>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// A friendly SpinBox that grants access to its QLineEdit
class FriendlySpinBox : public QAbstractSpinBox {
public:
    friend class WidgetFactory;
};

// An event filter for form-combo boxes that prevents
// the embedded line edit from getting edit focus (and drawing blue artifacts/lines).
// It catches the ChildPolished event when the "editable" property
// flips to true and the QLineEdit is created and turns off
// the LineEdit's focus policy.

class ComboEventFilter : public QObject {
public:
    explicit ComboEventFilter(QComboBox *parent) : QObject(parent) {}
    virtual bool eventFilter(QObject *watched, QEvent *event);
};

bool ComboEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ChildPolished) {
        QComboBox *cb = static_cast<QComboBox*>(watched);
        if (QLineEdit *le = cb->lineEdit())
            le->setFocusPolicy(Qt::NoFocus);
    }
    return QObject::eventFilter(watched, event);
}

// ---------------- WidgetFactory::Strings
WidgetFactory::Strings::Strings() :
    m_alignment(QLatin1String("alignment")),
    m_bottomMargin(QLatin1String("bottomMargin")),
    m_geometry(QLatin1String("geometry")),
    m_leftMargin(QLatin1String("leftMargin")),
    m_line(QLatin1String("Line")),
    m_objectName(QLatin1String("objectName")),
    m_spacerName(QLatin1String("spacerName")),
    m_orientation(QLatin1String("orientation")),
    m_q3WidgetStack(QLatin1String("Q3WidgetStack")),
    m_qAction(QLatin1String("QAction")),
    m_qAxWidget(QLatin1String("QAxWidget")),
    m_qDialog(QLatin1String("QDialog")),
    m_qDockWidget(QLatin1String("QDockWidget")),
    m_qLayoutWidget(QLatin1String("QLayoutWidget")),
    m_qMenu(QLatin1String("QMenu")),
    m_qMenuBar(QLatin1String("QMenuBar")),
    m_qWidget(QLatin1String("QWidget")),
    m_rightMargin(QLatin1String("rightMargin")),
    m_sizeHint(QLatin1String("sizeHint")),
    m_spacer(QLatin1String("Spacer")),
    m_text(QLatin1String("text")),
    m_title(QLatin1String("title")),
    m_topMargin(QLatin1String("topMargin")),
    m_windowIcon(QLatin1String("windowIcon")),
    m_windowTitle(QLatin1String("windowTitle"))
{
}
// ---------------- WidgetFactory
QPointer<QWidget> *WidgetFactory::m_lastPassiveInteractor = new QPointer<QWidget>();
bool WidgetFactory::m_lastWasAPassiveInteractor = false;


WidgetFactory::WidgetFactory(QDesignerFormEditorInterface *core, QObject *parent)
    : QDesignerWidgetFactoryInterface(parent),
      m_core(core),
      m_formWindow(0)
{
}

WidgetFactory::~WidgetFactory()
{
}

QDesignerFormWindowInterface *WidgetFactory::currentFormWindow(QDesignerFormWindowInterface *fw)
{
    QDesignerFormWindowInterface *was = m_formWindow;
    m_formWindow = fw;
    return was;
}

void WidgetFactory::loadPlugins()
{
    m_customFactory.clear();

    QDesignerPluginManager *pluginManager = m_core->pluginManager();

    QList<QDesignerCustomWidgetInterface*> lst = pluginManager->registeredCustomWidgets();
    foreach (QDesignerCustomWidgetInterface *c, lst) {
        m_customFactory.insert(c->name(), c);
    }
}

// Convencience to create non-widget objects. Returns 0 if unknown
QObject* WidgetFactory::createObject(const QString &className, QObject* parent) const
{
    if (className == m_strings.m_qAction)
        return new QAction(parent);
    return 0;
}

QWidget*  WidgetFactory::createCustomWidget(const QString &className, QWidget *parentWidget, bool *creationError) const
{
    *creationError = false;
    CustomWidgetFactoryMap::const_iterator it = m_customFactory.constFind(className);
    if (it == m_customFactory.constEnd())
        return 0;

    QDesignerCustomWidgetInterface *factory = it.value();
    QWidget *rc = factory->createWidget(parentWidget);
    // shouldn't happen
    if (!rc) {
        *creationError = true;
        designerWarning(QObject::tr("The custom widget factory registered for widgets of class %1 returned 0.").arg(className));
        return 0;
    }
    // Figure out the base class unless it is known
    static QSet<QString> knownCustomClasses;
    if (!knownCustomClasses.contains(className)) {
        QDesignerWidgetDataBaseInterface *wdb = m_core->widgetDataBase();
        const int widgetInfoIndex = wdb->indexOfObject(rc, false);
        if (widgetInfoIndex != -1) {
            if (wdb->item(widgetInfoIndex)->extends().isEmpty()) {
                const QDesignerMetaObjectInterface *mo = core()->introspection()->metaObject(rc)->superClass();
                while (mo != 0) {
                    if (core()->widgetDataBase()->indexOfClassName(mo->className()) != -1) {
                        wdb->item(widgetInfoIndex)->setExtends(mo->className());
                        break;
                    }
                    mo = mo->superClass();
                }
            }
            knownCustomClasses.insert(className);
        }
    }
    // Since a language plugin may lie about its names, like Qt Jambi
    // does, return immediately here...
    QDesignerLanguageExtension *lang =
        qt_extension<QDesignerLanguageExtension *>(m_core->extensionManager(), m_core);
    if (lang)
        return rc;

    // Check for mismatched class names which is hard to track.
    // Perform literal comparison first for QAxWidget, for which a meta object hack is in effect.
    const char *createdClassNameC = rc->metaObject()->className();
    const QByteArray classNameB = className.toUtf8();
    const char *classNameC = classNameB.constData();

    if (qstrcmp(createdClassNameC, classNameC) && !rc->inherits(classNameC))
        designerWarning(QObject::tr("A class name mismatch occurred when creating a widget using the custom widget factory registered for widgets of class %1."
                                  " It returned a widget of class %2.").arg(className).arg(QString::fromUtf8(createdClassNameC)));
    return rc;
}

QWidget *WidgetFactory::createWidget(const QString &widgetName, QWidget *parentWidget) const
{
    // Preview or for form window?
    QDesignerFormWindowInterface *fw = m_formWindow;
    if (! fw)
        fw = QDesignerFormWindowInterface::findFormWindow(parentWidget);

    QWidget *w = 0;
    do {
        // 1) custom. If there is an explicit failure(factory wants to indicate something is wrong),
        //    return 0, do not try to find fallback, which might be worse in the case of Q3 widget.
        bool customWidgetCreationError;
        w = createCustomWidget(widgetName, parentWidget, &customWidgetCreationError);
        if (w) {
            break;
        } else {
            if (customWidgetCreationError)
                return 0;
        }

        // 2) Special widgets
        if (widgetName == m_strings.m_line) {
            w = new Line(parentWidget);
        } else if (widgetName == m_strings.m_qDockWidget) {
            w = new QDesignerDockWidget(parentWidget);
        } else if (widgetName == m_strings.m_qMenuBar) {
            w = new QDesignerMenuBar(parentWidget);
        } else if (widgetName == m_strings.m_qMenu) {
            w = new QDesignerMenu(parentWidget);
        } else if (widgetName == m_strings.m_spacer) {
            w = new Spacer(parentWidget);
        } else if (widgetName == m_strings.m_qDockWidget) {
            w = new QDesignerDockWidget(parentWidget);
        } else if (widgetName == m_strings.m_qLayoutWidget) {
            w = fw ? new QLayoutWidget(fw, parentWidget) : new QWidget(parentWidget);
        } else if (widgetName == m_strings.m_qDialog) {
            if (fw) {
                w = new QDesignerDialog(fw, parentWidget);
            } else {
                w = new QDialog(parentWidget);
            }
        } else if (widgetName == m_strings.m_qWidget) {
            if (fw && parentWidget &&
                (qobject_cast<QDesignerFormWindowInterface*>(parentWidget) || qt_extension<QDesignerContainerExtension*>(m_core->extensionManager(), parentWidget))) {
                w = new QDesignerWidget(fw, parentWidget);
            } else {
                w = new QWidget(parentWidget);
            }
        }
        if (w)
            break;

        // 3) table
        const QByteArray widgetNameBA = widgetName.toUtf8();
        const char *widgetNameC = widgetNameBA.constData();

        if (w) { // symmetry for macro
        }

#define DECLARE_LAYOUT(L, C)
#define DECLARE_COMPAT_WIDGET(W, C) /*DECLARE_WIDGET(W, C)*/
#define DECLARE_WIDGET(W, C) else if (!qstrcmp(widgetNameC, #W)) { Q_ASSERT(w == 0); w = new W(parentWidget); }
#define DECLARE_WIDGET_1(W, C) else if (!qstrcmp(widgetNameC, #W)) { Q_ASSERT(w == 0); w = new W(0, parentWidget); }

#include "widgets.table"

#undef DECLARE_COMPAT_WIDGET
#undef DECLARE_LAYOUT
#undef DECLARE_WIDGET
#undef DECLARE_WIDGET_1

        if (w)
            break;
        // 4) fallBack
        const QString fallBackBaseClass = m_strings.m_qWidget;
        QDesignerWidgetDataBaseInterface *db = core()->widgetDataBase();
        QDesignerWidgetDataBaseItemInterface *item = db->item(db->indexOfClassName(widgetName));
        if (item == 0) {
            // Emergency: Create, derived from QWidget
            QString includeFile = widgetName.toLower();
            includeFile +=  QLatin1String(".h");
            item = appendDerived(db,widgetName,tr("%1 Widget").arg(widgetName),fallBackBaseClass,
                                 includeFile, true, true);
            Q_ASSERT(item);
        }
        QString baseClass = item->extends();
        if (baseClass.isEmpty()) {
            // Currently happens in the case of Q3-Support widgets
            baseClass =fallBackBaseClass;
        }
        w = createWidget(baseClass, parentWidget);
        promoteWidget(core(),w,widgetName);
    } while (false);

    Q_ASSERT(w != 0);

    if (fw) { // form editor  initialization
        initialize(w);
    } else { // preview-only initialization
        if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(w))
            QStackedWidgetPreviewEventFilter::install(stackedWidget); // Add browse button only.
    }

    return w;
}

QString WidgetFactory::classNameOf(QDesignerFormEditorInterface *c, QObject* o)
{
    if (o == 0)
        return QString();

    // check promoted before designer special
    if (o->isWidgetType()) {
        const QString customClassName = promotedCustomClassName(c,qobject_cast<QWidget*>(o));
        if (!customClassName.isEmpty())
            return customClassName;
    }

    if (qobject_cast<QDesignerMenuBar*>(o))
        return QLatin1String("QMenuBar");
    else if (qobject_cast<QDesignerDockWidget*>(o))
        return QLatin1String("QDockWidget");
    else if (qobject_cast<QDesignerDialog*>(o))
        return QLatin1String("QDialog");
    else if (qobject_cast<QDesignerWidget*>(o))
        return QLatin1String("QWidget");
    else if (qstrcmp(o->metaObject()->className(), "QAxBase") == 0)
        return QLatin1String("QAxWidget");
    else if (qstrcmp(o->metaObject()->className(), "QDesignerQ3WidgetStack") == 0)
        return QLatin1String("Q3WidgetStack");

    return QLatin1String(o->metaObject()->className());
}

QLayout *WidgetFactory::createUnmanagedLayout(QWidget *parentWidget, int type)
{
    switch (type) {
    case LayoutInfo::HBox:
        return new QHBoxLayout(parentWidget);
    case LayoutInfo::VBox:
        return new QVBoxLayout(parentWidget);
    case LayoutInfo::Grid:
        return new QGridLayout(parentWidget);
    case LayoutInfo::Form:
        return new QFormLayout(parentWidget);
    default:
        Q_ASSERT(0);
        break;
    }
    return 0;
}


/*!  Creates a layout on the widget \a widget of the type \a type
  which can be \c HBox, \c VBox or \c Grid.
*/

QLayout *WidgetFactory::createLayout(QWidget *widget, QLayout *parentLayout, int type) const // ### (sizepolicy)
{
    QDesignerMetaDataBaseInterface *metaDataBase = core()->metaDataBase();

    if (parentLayout == 0) {
        QWidget *page = containerOfWidget(widget);
        if (page) {
            widget = page;
        } else {
            const QString msg = QObject::tr("The current page of the container '%1' (%2) could not be determined while creating a layout."
"This indicates an inconsistency in the ui-file, probably a layout being constructed on a container widget.").arg(widget->objectName()).arg(classNameOf(core(), widget));
            designerWarning(msg);
        }
    }

    Q_ASSERT(metaDataBase->item(widget) != 0); // ensure the widget is managed

    if (parentLayout == 0 && metaDataBase->item(widget->layout()) == 0) {
        parentLayout = widget->layout();
    }

    QWidget *parentWidget = parentLayout != 0 ? 0 : widget;

    QLayout *layout = createUnmanagedLayout(parentWidget, type);
    metaDataBase->add(layout); // add the layout in the MetaDataBase

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), layout);

    sheet->setChanged(sheet->indexOf(m_strings.m_objectName), true);
    if (widget->inherits("Q3GroupBox")) {
        layout->setContentsMargins(widget->style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                    widget->style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                    widget->style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                    widget->style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
        QGridLayout *grid = qobject_cast<QGridLayout *>(layout);
        if (grid) {
            grid->setHorizontalSpacing(-1);
            grid->setVerticalSpacing(-1);
        } else {
            layout->setSpacing(-1);
        }
        layout->setAlignment(Qt::AlignTop);
        // Just to ensure; before 4.3 orientation property was always set (now only for QSplitter class).
        // Calling Q3GroupBox::setOrientation() invoked in turn setSpacing(0). Below fixes that
        widget->layout()->setSpacing(-1);
    } else if (widget->inherits("QLayoutWidget")) {
        sheet->setProperty(sheet->indexOf(m_strings.m_leftMargin), 0);
        sheet->setProperty(sheet->indexOf(m_strings.m_topMargin), 0);
        sheet->setProperty(sheet->indexOf(m_strings.m_rightMargin), 0);
        sheet->setProperty(sheet->indexOf(m_strings.m_bottomMargin), 0);
    }

    if (sheet) {
        const int index = sheet->indexOf(m_strings.m_alignment);
        if (index != -1)
            sheet->setChanged(index, true);
    }

    if (metaDataBase->item(widget->layout()) == 0) {
        Q_ASSERT(layout->parent() == 0);
        QBoxLayout *box = qobject_cast<QBoxLayout*>(widget->layout());
        Q_ASSERT(box != 0); // we support only unmanaged box layouts
        box->addLayout(layout);
    }

    return layout;
}

/*!  Returns the widget into which children should be inserted when \a
  w is a container known to designer.

  Usually, it is \a w itself, but there are exceptions (for example, a
  tabwidget is known to designer as a container, but the child
  widgets should be inserted into the current page of the
  tabwidget. In this case, the current page of
  the tabwidget  would be returned.)
 */
QWidget* WidgetFactory::containerOfWidget(QWidget *w) const
{
    if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), w))
        return container->widget(container->currentIndex());

    return w;
}

/*!  Returns the actual designer widget of the container \a w. This is
  normally \a w itself, but it might be a parent or grand parent of \a w
  (for example, when working with a tabwidget and \a w is the container which
  contains and layouts children, but the actual widget known to
  designer is the tabwidget which is the parent of \a w. In this case,
  the tabwidget would be returned.)
*/

QWidget* WidgetFactory::widgetOfContainer(QWidget *w) const
{
    // ### cleanup
    if (!w)
        return 0;
    if (w->parentWidget() && w->parentWidget()->parentWidget() &&
         w->parentWidget()->parentWidget()->parentWidget() &&
         qobject_cast<QToolBox*>(w->parentWidget()->parentWidget()->parentWidget()))
        return w->parentWidget()->parentWidget()->parentWidget();

    while (w != 0) {
        if (core()->widgetDataBase()->isContainer(w) ||
             w && qobject_cast<QDesignerFormWindowInterface*>(w->parentWidget()))
            return w;

        w = w->parentWidget();
    }

    return w;
}

QDesignerFormEditorInterface *WidgetFactory::core() const
{
    return m_core;
}

void WidgetFactory::initialize(QObject *object) const
{
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(m_core->extensionManager(), object);
    if (!sheet)
        return;

    sheet->setChanged(sheet->indexOf(m_strings.m_objectName), true);

    if (!object->isWidgetType()) {
        if (qobject_cast<QAction*>(object))
            sheet->setChanged(sheet->indexOf(m_strings.m_text), true);
        return;
    }

    QWidget *widget = static_cast<QWidget*>(object);
    const bool isMenu = qobject_cast<QMenu*>(widget);
    const bool isMenuBar = !isMenu && qobject_cast<QMenuBar*>(widget);

    widget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    widget->setFocusPolicy((isMenu || isMenuBar) ? Qt::StrongFocus : Qt::NoFocus);

    if (!isMenu)
        sheet->setChanged(sheet->indexOf(m_strings.m_geometry), true);

    if (qobject_cast<Spacer*>(widget)) {
        sheet->setChanged(sheet->indexOf(m_strings.m_spacerName), true);
        return;
    }

    const int o = sheet->indexOf(m_strings.m_orientation);
    if (o != -1 && widget->inherits("QSplitter"))
        sheet->setChanged(o, true);

    if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
        ToolBarEventFilter::install(toolBar);
        sheet->setVisible(sheet->indexOf(m_strings.m_windowTitle), true);
        toolBar->setFloatable(false);  // prevent toolbars from being dragged off
        return;
    }

    if (qobject_cast<QDockWidget*>(widget)) {
        sheet->setVisible(sheet->indexOf(m_strings.m_windowTitle), true);
        sheet->setVisible(sheet->indexOf(m_strings.m_windowIcon), true);
        return;
    }

    if (isMenu) {
        sheet->setChanged(sheet->indexOf(m_strings.m_title), true);
        return;
    }
    // helpers
    if (QToolBox *toolBox = qobject_cast<QToolBox*>(widget)) {
        QToolBoxHelper::install(toolBox);
        return;
    }
    if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(widget)) {
        QStackedWidgetEventFilter::install(stackedWidget);
        return;
    }
    if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget)) {
        QTabWidgetEventFilter::install(tabWidget);
        return;
    }
    // Prevent embedded line edits from getting focus
    if (QAbstractSpinBox *asb = qobject_cast<QAbstractSpinBox *>(widget)) {
        if (QLineEdit *lineEdit = static_cast<FriendlySpinBox*>(asb)->lineEdit())
            lineEdit->setFocusPolicy(Qt::NoFocus);
        return;
    }
    if (QComboBox *cb =  qobject_cast<QComboBox *>(widget)) {
        if (QFontComboBox *fcb =  qobject_cast<QFontComboBox *>(widget)) {
            fcb->lineEdit()->setFocusPolicy(Qt::NoFocus); // Always present
            return;
        }
        cb->installEventFilter(new ComboEventFilter(cb));
        return;
    }
}

// Check for 'interactor' click on a tab bar,
// which can appear within a QTabWidget or as a standalone widget.

static bool isTabBarInteractor(const QTabBar *tabBar)
{
    // Tabbar embedded in Q(Designer)TabWidget, ie, normal tab widget case
    if (qobject_cast<const QTabWidget*>(tabBar->parentWidget()))
        return true;

    // Standalone tab bar on the form. Return true for tab rect areas
    // only to allow the user to select the tab bar by clicking outside the actual tabs.
    const int count = tabBar->count();
    if (count == 0)
        return false;

    // click into current tab: No Interaction
    const int currentIndex = tabBar->currentIndex();
    const QPoint pos = tabBar->mapFromGlobal(QCursor::pos());
    if (tabBar->tabRect(currentIndex).contains(pos))
        return false;

    // click outside: No Interaction
    const QRect geometry = QRect(QPoint(0, 0), tabBar->size());
    if (!geometry.contains(pos))
        return false;
    // click into another tab: Let's interact, switch tabs.
    for (int i = 0; i < count; i++)
        if (tabBar->tabRect(i).contains(pos))
            return true;
    return false;
}

bool WidgetFactory::isPassiveInteractor(QWidget *widget)
{
    static const QString qtPassive = QLatin1String("__qt__passive_");
    if (m_lastPassiveInteractor != 0 && (QWidget*)(*m_lastPassiveInteractor) == widget)
        return m_lastWasAPassiveInteractor;

    if (QApplication::activePopupWidget() || widget == 0) // if a popup is open, we have to make sure that this one is closed, else X might do funny things
        return true;

    m_lastWasAPassiveInteractor = false;
    (*m_lastPassiveInteractor) = widget;

    if (const QTabBar *tabBar = qobject_cast<const QTabBar*>(widget)) {
        if (isTabBarInteractor(tabBar))
            m_lastWasAPassiveInteractor = true;
        return m_lastWasAPassiveInteractor;
    }  else if (qobject_cast<QSizeGrip*>(widget))
        return (m_lastWasAPassiveInteractor = true);
     else if (qobject_cast<QMdiSubWindow*>(widget))
        return (m_lastWasAPassiveInteractor = true);
    else if (qobject_cast<QAbstractButton*>(widget) && (qobject_cast<QTabBar*>(widget->parent()) || qobject_cast<QToolBox*>(widget->parent())))
        return (m_lastWasAPassiveInteractor = true);
    else if (qobject_cast<QMenuBar*>(widget))
        return (m_lastWasAPassiveInteractor = true);
    else if (qobject_cast<QToolBar*>(widget))
        return (m_lastWasAPassiveInteractor = true);
    else if (qobject_cast<QScrollBar*>(widget)) {
        // A scroll bar is an interactor on a QAbstractScrollArea only.
        if (const QWidget *parent = widget->parentWidget()) {
            const QString objectName = parent->objectName();
            static const QString scrollAreaVContainer = QLatin1String("qt_scrollarea_vcontainer");
            static const QString scrollAreaHContainer = QLatin1String("qt_scrollarea_hcontainer");
            if (objectName == scrollAreaVContainer || objectName == scrollAreaHContainer) {
                m_lastWasAPassiveInteractor = true;
                return m_lastWasAPassiveInteractor;
            }
        }
    } else if (qstrcmp(widget->metaObject()->className(), "QDockWidgetTitle") == 0)
        return (m_lastWasAPassiveInteractor = true);
    else if (qstrcmp(widget->metaObject()->className(), "QWorkspaceTitleBar") == 0)
        return (m_lastWasAPassiveInteractor = true);
    else if (widget->objectName().startsWith(qtPassive))
        return (m_lastWasAPassiveInteractor = true);
    return m_lastWasAPassiveInteractor;
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
