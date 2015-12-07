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

#include "qdesigner_tabwidget_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertycommand_p.h"
#include "promotiontaskmenu_p.h"
#include "formwindowbase_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QApplication>
#include <QtGui/QTabBar>
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
// Store tab widget as drag source
class MyMimeData : public QMimeData
{
    Q_OBJECT
public:
    MyMimeData(const QTabWidget *tab) : m_tab(tab) {}
    static bool fromMyTab(const QMimeData *mimeData, const QTabWidget *tab) {
        if (!mimeData)
            return false;
        const MyMimeData *m = qobject_cast<const MyMimeData *>(mimeData);
        return m &&  m->m_tab ==  tab;
    }
private:
    const QTabWidget *m_tab;
};

} // namespace qdesigner_internal

// ------------- QTabWidgetEventFilter

QTabWidgetEventFilter::QTabWidgetEventFilter(QTabWidget *parent) :
    QObject(parent),
    m_tabWidget(parent),
    m_dropIndicator(0),
    m_dragPage(0),
    m_mousePressed(false),
    m_actionDeletePage(new QAction(tr("Delete"),  this)),
    m_actionInsertPage(new QAction(tr("Before Current Page"), this)),
    m_actionInsertPageAfter(new QAction(tr("After Current Page"), this)),
    m_pagePromotionTaskMenu(new qdesigner_internal::PromotionTaskMenu(0, qdesigner_internal::PromotionTaskMenu::ModeSingleWidget, this))
{
    tabBar()->setAcceptDrops(true);
    tabBar()->installEventFilter(this);

    connect(m_actionInsertPage, SIGNAL(triggered()), this, SLOT(addPage()));
    connect(m_actionInsertPageAfter, SIGNAL(triggered()), this, SLOT(addPageAfter()));
    connect(m_actionDeletePage, SIGNAL(triggered()), this, SLOT(removeCurrentPage()));
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));

}

QTabWidgetEventFilter::~QTabWidgetEventFilter()
{
}

void QTabWidgetEventFilter::install(QTabWidget *tabWidget)
{
    new QTabWidgetEventFilter(tabWidget);
}

QTabWidgetEventFilter *QTabWidgetEventFilter::eventFilterOf(const QTabWidget *tabWidget)
{
    QList<QTabWidgetEventFilter*> filters = qFindChildren<QTabWidgetEventFilter*>(tabWidget);
    if (filters.empty())
        return 0;
    return filters.front();
}

QMenu *QTabWidgetEventFilter::addTabWidgetContextMenuActions(const QTabWidget *tabWidget, QMenu *popup)
{
    QTabWidgetEventFilter *filter = eventFilterOf(tabWidget);
    if (!filter)
        return 0;
    return filter->addContextMenuActions(popup);
}

QTabBar *QTabWidgetEventFilter::tabBar() const
{
    // QTabWidget::tabBar() accessor is protected, grmbl...
    if (!m_cachedTabBar) {
        const QList<QTabBar *> tabBars = qFindChildren<QTabBar *>(m_tabWidget);
        Q_ASSERT(tabBars.size() == 1);
        m_cachedTabBar = tabBars.front();
    }
    return m_cachedTabBar;

}

static bool canMove(const QPoint &pressPoint, const QMouseEvent *e)
{
    const QPoint pt = pressPoint - e->pos();
    return pt.manhattanLength() > QApplication::startDragDistance();
}

bool QTabWidgetEventFilter::eventFilter(QObject *o, QEvent *e)
{
    const QEvent::Type type = e->type();
    // Do not try to locate tab bar and form window, etc. for uninteresting events and
    // avoid asserts about missing tab bars when being destroyed
    switch (type) {
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::DragLeave:
    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::Drop:
        break;
    default:
        return false;
    }

    if (o != tabBar())
        return false;

    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return false;

    switch (type) {
    case QEvent::MouseButtonDblClick:
        break;
    case QEvent::MouseButtonPress: {
        QMouseEvent *mev = static_cast<QMouseEvent*>(e);
        if (mev->button() & Qt::LeftButton) {
            m_mousePressed = true;
            m_pressPoint = mev->pos();

            QTabBar *tabbar = tabBar();
            const int count = tabbar->count();
            for (int i = 0; i < count; ++i) {
                if (tabbar->tabRect(i).contains(m_pressPoint)) {
                    if (i != tabbar->currentIndex()) {
                        qdesigner_internal::SetPropertyCommand *cmd = new qdesigner_internal::SetPropertyCommand(fw);
                        cmd->init(m_tabWidget, QLatin1String("currentIndex"), i);
                        fw->commandHistory()->push(cmd);
                    }
                    break;
                }
            }
        }
    } break;

    case QEvent::MouseButtonRelease:
        m_mousePressed = false;
        break;

    case QEvent::MouseMove: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
        if (m_mousePressed && canMove(m_pressPoint, mouseEvent)) {
            const int index = m_tabWidget->currentIndex();
            if (index == -1)
                break;

            m_mousePressed = false;
            QDrag *drg = new QDrag(m_tabWidget);
            drg->setMimeData(new qdesigner_internal::MyMimeData(m_tabWidget));

            m_dragIndex = index;
            m_dragPage  = m_tabWidget->currentWidget();
            m_dragLabel = m_tabWidget->tabText(index);
            m_dragIcon  = m_tabWidget->tabIcon(index);
            if (m_dragIcon.isNull()) {
                QLabel *label = new QLabel(m_dragLabel);
                label->adjustSize();
                drg->setPixmap(QPixmap::grabWidget(label));
                label->deleteLater();
            } else {
                drg->setPixmap(m_dragIcon.pixmap(22, 22));
            }

            m_tabWidget->removeTab(m_dragIndex);

            const Qt::DropActions dropAction = drg->start(Qt::MoveAction);

            if (dropAction == Qt::IgnoreAction) {
                // abort
                m_tabWidget->insertTab(m_dragIndex, m_dragPage, m_dragIcon, m_dragLabel);
                m_tabWidget->setCurrentIndex(m_dragIndex);
            }

            if (m_dropIndicator)
                m_dropIndicator->hide();
        }
    } break;

    case QEvent::DragLeave: {
        if (m_dropIndicator)
            m_dropIndicator->hide();
    } break;

    case QEvent::DragEnter:
    case QEvent::DragMove: {
        QDragMoveEvent *de = static_cast<QDragMoveEvent*>(e);
        if (!qdesigner_internal::MyMimeData::fromMyTab(de->mimeData(), m_tabWidget))
            return false;

        if (de->proposedAction() == Qt::MoveAction)
            de->acceptProposedAction();
        else {
            de->setDropAction(Qt::MoveAction);
            de->accept();
        }

        QRect rect;
        const int index = pageFromPosition(de->pos(), rect);

        if (!m_dropIndicator) {
            m_dropIndicator = new QWidget(m_tabWidget);
            QPalette p = m_dropIndicator->palette();
            p.setColor(m_tabWidget->backgroundRole(), Qt::red);
            m_dropIndicator->setPalette(p);
        }

        QPoint pos;
        if (index == m_tabWidget->count())
            pos = tabBar()->mapToParent(QPoint(rect.x() + rect.width(), rect.y()));
        else
            pos = tabBar()->mapToParent(QPoint(rect.x(), rect.y()));

        m_dropIndicator->setGeometry(pos.x(), pos.y() , 3, rect.height());
        m_dropIndicator->show();
    } break;

    case QEvent::Drop: {
        QDropEvent *de = static_cast<QDropEvent*>(e);
        if (!qdesigner_internal::MyMimeData::fromMyTab(de->mimeData(), m_tabWidget))
            return false;
        de->acceptProposedAction();
        de->accept();

        QRect rect;
        const int newIndex = pageFromPosition(de->pos(), rect);

        qdesigner_internal::MoveTabPageCommand *cmd = new qdesigner_internal::MoveTabPageCommand(fw);
        m_tabWidget->insertTab(m_dragIndex, m_dragPage, m_dragIcon, m_dragLabel);
        cmd->init(m_tabWidget, m_dragPage, m_dragIcon, m_dragLabel, m_dragIndex, newIndex);
        fw->commandHistory()->push(cmd);
    } break;

    default:
        break;
    }

    return false;
}

void QTabWidgetEventFilter::removeCurrentPage()
{
    if (!m_tabWidget->currentWidget())
        return;

    if (QDesignerFormWindowInterface *fw = formWindow()) {
        qdesigner_internal::DeleteTabPageCommand *cmd = new qdesigner_internal::DeleteTabPageCommand(fw);
        cmd->init(m_tabWidget);
        fw->commandHistory()->push(cmd);
    }
}

void QTabWidgetEventFilter::addPage()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        qdesigner_internal::AddTabPageCommand *cmd = new qdesigner_internal::AddTabPageCommand(fw);
        cmd->init(m_tabWidget, qdesigner_internal::AddTabPageCommand::InsertBefore);
        fw->commandHistory()->push(cmd);
    }
}

void QTabWidgetEventFilter::addPageAfter()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        qdesigner_internal::AddTabPageCommand *cmd = new qdesigner_internal::AddTabPageCommand(fw);
        cmd->init(m_tabWidget, qdesigner_internal::AddTabPageCommand::InsertAfter);
        fw->commandHistory()->push(cmd);
    }
}

void QTabWidgetEventFilter::slotCurrentChanged(int index)
{
    if (m_tabWidget->widget(index)) {
        if (QDesignerFormWindowInterface *fw = formWindow()) {
            fw->clearSelection();
            fw->selectWidget(m_tabWidget, true);
        }
    }
}

QDesignerFormWindowInterface *QTabWidgetEventFilter::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(const_cast<QTabWidget*>(m_tabWidget));
}

// Get page from mouse position. Default to new page if in right half of last page?
int QTabWidgetEventFilter::pageFromPosition(const QPoint &pos, QRect &rect) const
{
    int index = 0;
    const QTabBar *tabbar = tabBar();
    const int count = m_tabWidget->count();
    for (; index < count; index++) {
        const QRect rc = tabbar->tabRect(index);
        if (rc.contains(pos)) {
            rect = rc;
            break;
        }
    }

    if (index == count -1) {
        QRect rect2 = rect;
        rect2.setLeft(rect2.left() + rect2.width() / 2);
        if (rect2.contains(pos))
            index++;
    }
    return index;
}

QMenu *QTabWidgetEventFilter::addContextMenuActions(QMenu *popup)
{
    QMenu *pageMenu = 0;
    const int count = m_tabWidget->count();
    m_actionDeletePage->setEnabled(count > 1);
    if (count) {
        const int currentIndex = m_tabWidget->currentIndex();
        const QString pageSubMenuLabel = tr("Page %1 of %2").arg(currentIndex + 1).arg(count);
        pageMenu = popup->addMenu(pageSubMenuLabel);
        pageMenu->addAction(m_actionDeletePage);
        // Set up promotion menu for current widget.
        if (QWidget *page =  m_tabWidget->currentWidget ()) {
            m_pagePromotionTaskMenu->setWidget(page);
            m_pagePromotionTaskMenu->addActions(QDesignerFormWindowInterface::findFormWindow(m_tabWidget),
                                                qdesigner_internal::PromotionTaskMenu::SuppressGlobalEdit,
                                                pageMenu);
        }
    }

    QMenu *insertPageMenu = popup->addMenu(tr("Insert Page"));
    insertPageMenu->addAction(m_actionInsertPageAfter);
    insertPageMenu->addAction(m_actionInsertPage);
    popup->addSeparator();
    return pageMenu;
}

// ----------- QTabWidgetPropertySheet

static const char *currentTabTextKey = "currentTabText";
static const char *currentTabNameKey = "currentTabName";
static const char *currentTabIconKey = "currentTabIcon";
static const char *currentTabToolTipKey = "currentTabToolTip";

QTabWidgetPropertySheet::QTabWidgetPropertySheet(QTabWidget *object, QObject *parent) :
    QDesignerPropertySheet(object, parent),
    m_tabWidget(object)
{
    createFakeProperty(QLatin1String(currentTabTextKey), QString());
    createFakeProperty(QLatin1String(currentTabNameKey), QString());
    createFakeProperty(QLatin1String(currentTabIconKey), qVariantFromValue(qdesigner_internal::PropertySheetIconValue()));
    if (formWindowBase())
        formWindowBase()->addReloadableProperty(this, indexOf(QLatin1String(currentTabIconKey)));
    createFakeProperty(QLatin1String(currentTabToolTipKey), QString());
}

QTabWidgetPropertySheet::TabWidgetProperty QTabWidgetPropertySheet::tabWidgetPropertyFromName(const QString &name)
{
    typedef QHash<QString, TabWidgetProperty> TabWidgetPropertyHash;
    static TabWidgetPropertyHash tabWidgetPropertyHash;
    if (tabWidgetPropertyHash.empty()) {
        tabWidgetPropertyHash.insert(QLatin1String(currentTabTextKey),    PropertyCurrentTabText);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabNameKey),    PropertyCurrentTabName);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabIconKey),    PropertyCurrentTabIcon);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabToolTipKey), PropertyCurrentTabToolTip);
    }
    return tabWidgetPropertyHash.value(name, PropertyTabWidgetNone);
}

void QTabWidgetPropertySheet::setProperty(int index, const QVariant &value)
{
    const TabWidgetProperty tabWidgetProperty = tabWidgetPropertyFromName(propertyName(index));
    if (tabWidgetProperty == PropertyTabWidgetNone) {
        QDesignerPropertySheet::setProperty(index, value);
        return;
    }

    // index-dependent
    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex == -1)
        return;

    switch (tabWidgetProperty) {
    case PropertyCurrentTabText:
        m_tabWidget->setTabText(currentIndex, value.toString());
        break;
    case PropertyCurrentTabName:
        m_tabWidget->widget(currentIndex)->setObjectName(value.toString());
        break;
    case PropertyCurrentTabIcon:
        m_tabWidget->setTabIcon(currentIndex, qvariant_cast<QIcon>(resolvePropertyValue(index, value)));
        m_pageToIcon[currentIndex] = qVariantValue<qdesigner_internal::PropertySheetIconValue>(value);
        break;
    case PropertyCurrentTabToolTip:
        m_tabWidget->setTabToolTip(currentIndex, value.toString());
        break;
    case PropertyTabWidgetNone:
        break;
    }
}

bool QTabWidgetPropertySheet::isEnabled(int index) const
{
    if (tabWidgetPropertyFromName(propertyName(index)) == PropertyTabWidgetNone)
        return QDesignerPropertySheet::isEnabled(index);
    return m_tabWidget->currentIndex() != -1;
}

QVariant QTabWidgetPropertySheet::property(int index) const
{
    const TabWidgetProperty tabWidgetProperty = tabWidgetPropertyFromName(propertyName(index));
    if (tabWidgetProperty == PropertyTabWidgetNone)
        return  QDesignerPropertySheet::property(index);

    // index-dependent
    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex == -1) {
        if (tabWidgetProperty == PropertyCurrentTabIcon)
            return qVariantFromValue(qdesigner_internal::PropertySheetIconValue());
        return QVariant(QString());
    }

    // index-dependent
    switch (tabWidgetProperty) {
    case PropertyCurrentTabText:
        return m_tabWidget->tabText(currentIndex);
    case PropertyCurrentTabName:
        return m_tabWidget->widget(currentIndex)->objectName();
    case PropertyCurrentTabIcon:
        return qVariantFromValue(m_pageToIcon.value(currentIndex));
    case PropertyCurrentTabToolTip:
        return m_tabWidget->tabToolTip(currentIndex);
    case PropertyTabWidgetNone:
        break;
    }
    return QVariant();
}

bool QTabWidgetPropertySheet::reset(int index)
{
    const TabWidgetProperty tabWidgetProperty = tabWidgetPropertyFromName(propertyName(index));
    if (tabWidgetProperty == PropertyTabWidgetNone)
        return QDesignerPropertySheet::reset(index);

    // index-dependent
    const int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex == -1)
        return false;

    // index-dependent
    switch (tabWidgetProperty) {
    case PropertyCurrentTabText:
    case PropertyCurrentTabName:
    case PropertyCurrentTabToolTip:
        setProperty(index, QString());
        break;
    case PropertyCurrentTabIcon:
        m_pageToIcon.remove(index);
        setProperty(index, QIcon());
        break;
    case PropertyTabWidgetNone:
        break;
    }
    return true;
}

bool QTabWidgetPropertySheet::checkProperty(const QString &propertyName)
{
    switch (tabWidgetPropertyFromName(propertyName)) {
    case PropertyCurrentTabText:
    case PropertyCurrentTabName:
    case PropertyCurrentTabToolTip:
    case PropertyCurrentTabIcon:
        return false;
    default:
        break;
    }
    return true;
}

QT_END_NAMESPACE

#include "qdesigner_tabwidget.moc" // required for MyMimeData
