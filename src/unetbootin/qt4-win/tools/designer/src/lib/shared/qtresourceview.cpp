/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "qtresourceview_p.h"
#include "qtresourcemodel_p.h"
#include "qtresourceeditordialog_p.h"
#include "iconloader_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QSplitter>
#include <QtGui/QTreeWidget>
#include <QtGui/QListWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtGui/QPainter>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtCore/QMimeData>
#include <QtXml/QDomDocument>

QT_BEGIN_NAMESPACE

static const char *elementResourceData = "resource";
static const char *typeAttribute = "type";
static const char *typeImage = "image";
static const char *typeStyleSheet = "stylesheet";
static const char *typeOther = "other";
static const char *fileAttribute = "file";

// ---------------- ResourceListWidget: A list widget that has drag enabled
class ResourceListWidget : public QListWidget {
public:
    ResourceListWidget(QWidget *parent = 0);

protected:
    virtual void startDrag(Qt::DropActions supportedActions);
};

ResourceListWidget::ResourceListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setDragEnabled(true);
}

void ResourceListWidget::startDrag(Qt::DropActions supportedActions)
{
    if (supportedActions == Qt::MoveAction)
        return;

    QListWidgetItem *item = currentItem();
    if (!item)
        return;

    const QString filePath = item->data(Qt::UserRole).toString();
    const QIcon icon = item->icon();

    QMimeData *mimeData = new QMimeData;
    const QtResourceView::ResourceType type = icon.isNull() ? QtResourceView::ResourceOther : QtResourceView::ResourceImage;
    mimeData->setText(QtResourceView::encodeMimeData(type , filePath));

    QDrag *drag = new QDrag(this);
    if (!icon.isNull()) {
        const QSize size = icon.actualSize(iconSize());
        drag->setPixmap(icon.pixmap(size));
        drag->setHotSpot(QPoint(size.width() / 2, size.height() / 2));
    }

    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
}

/* TODO

   1) load the icons in separate thread...Hmm..if Qt is configured with threads....
*/

// ---------------------------- QtResourceViewPrivate
class QtResourceViewPrivate
{
    QtResourceView *q_ptr;
    Q_DECLARE_PUBLIC(QtResourceView)
public:
    QtResourceViewPrivate(QDesignerFormEditorInterface *core);

    void slotResourceSetActivated(QtResourceSet *resourceSet);
    void slotCurrentPathChanged(QTreeWidgetItem *);
    void slotCurrentResourceChanged(QListWidgetItem *);
    void slotResourceActivated(QListWidgetItem *);
    void slotEditResources();
    void slotReloadResources();
    void slotCopyResourcePath();
    void slotListWidgetContextMenuRequested(const QPoint &pos);
    void createPaths();
    QTreeWidgetItem *createPath(const QString &path, QTreeWidgetItem *parent);
    void createResources(const QString &path);
    void storeExpansionState();
    void applyExpansionState();

    QPixmap makeThumbnail(const QPixmap &pix) const;

    QDesignerFormEditorInterface *m_core;
    QtResourceModel *m_resourceModel;
    QToolBar *m_toolBar;
    QTreeWidget *m_treeWidget;
    QListWidget *m_listWidget;
    QMap<QString, QStringList>       m_pathToContents; // full path to contents file names
    QMap<QString, QTreeWidgetItem *> m_pathToItem;
    QMap<QTreeWidgetItem *, QString> m_itemToPath;
    QMap<QString, QListWidgetItem *> m_resourceToItem;
    QMap<QListWidgetItem *, QString> m_itemToResource;
    QAction *m_editResourcesAction;
    QAction *m_reloadResourcesAction;
    QAction *m_copyResourcePathAction;

    QMap<QString, bool> m_expansionState;

    bool m_ignoreGuiSignals;
};

QtResourceViewPrivate::QtResourceViewPrivate(QDesignerFormEditorInterface *core) :
    q_ptr(0),
    m_core(core),
    m_resourceModel(0),
    m_toolBar(new QToolBar),
    m_treeWidget(new QTreeWidget),
    m_listWidget(new ResourceListWidget),
    m_editResourcesAction(0),
    m_reloadResourcesAction(0),
    m_copyResourcePathAction(0),
    m_ignoreGuiSignals(false)
{
}

void QtResourceViewPrivate::slotEditResources()
{
    const QString selectedResource = QtResourceEditorDialog::editResources(m_resourceModel, m_core->dialogGui(), q_ptr);
    if (!selectedResource.isEmpty())
        q_ptr->selectResource(selectedResource);
}

void QtResourceViewPrivate::slotReloadResources()
{
    if (m_resourceModel) {
        int errorCount;
        QString errorMessages;
        m_resourceModel->reload(&errorCount, &errorMessages);
        if (errorCount)
            QtResourceEditorDialog::displayResourceFailures(errorMessages, m_core->dialogGui(), q_ptr);
    }
}

void QtResourceViewPrivate::slotCopyResourcePath()
{
    const QString path = q_ptr->selectedResource();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(path);
}

void QtResourceViewPrivate::slotListWidgetContextMenuRequested(const QPoint &pos)
{
    QMenu menu(q_ptr);
    menu.addAction(m_copyResourcePathAction);
    menu.exec(m_listWidget->mapToGlobal(pos));
}

void QtResourceViewPrivate::storeExpansionState()
{
    QMapIterator<QString, QTreeWidgetItem *> it(m_pathToItem);
    while (it.hasNext()) {
        it.next();
        m_expansionState[it.key()] = it.value()->isExpanded();
    }
}

void QtResourceViewPrivate::applyExpansionState()
{
    QMapIterator<QString, QTreeWidgetItem *> it(m_pathToItem);
    while (it.hasNext()) {
        it.next();
        it.value()->setExpanded(m_expansionState.value(it.key(), true));
    }
}

QPixmap QtResourceViewPrivate::makeThumbnail(const QPixmap &pix) const
{
    int w = qMax(48, pix.width());
    int h = qMax(48, pix.height());
    QRect imgRect(0, 0, w, h);
    QImage img(w, h, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    if (!pix.isNull()) {
        QRect r(0, 0, pix.width(), pix.height());
        r.moveCenter(imgRect.center());
        QPainter p(&img);
        p.drawPixmap(r.topLeft(), pix);
    }
    return QPixmap::fromImage(img);
}

void QtResourceViewPrivate::slotResourceSetActivated(QtResourceSet *resourceSet)
{
    const bool resourceActive = resourceSet;
    m_editResourcesAction->setEnabled(resourceActive);
    m_reloadResourcesAction->setEnabled(resourceActive);

    storeExpansionState();
    const QString currentPath = m_itemToPath.value(m_treeWidget->currentItem());
    const QString currentResource = m_itemToResource.value(m_listWidget->currentItem());
    m_treeWidget->clear();
    m_pathToContents.clear();
    m_pathToItem.clear();
    m_itemToPath.clear();
    m_listWidget->clear();
    m_resourceToItem.clear();
    m_itemToResource.clear();

    createPaths();
    applyExpansionState();

    if (!currentResource.isEmpty())
        q_ptr->selectResource(currentResource);
    else if (!currentPath.isEmpty())
        q_ptr->selectResource(currentPath);
}

void QtResourceViewPrivate::slotCurrentPathChanged(QTreeWidgetItem *item)
{
    if (m_ignoreGuiSignals)
        return;

    m_listWidget->clear();
    m_resourceToItem.clear();
    m_itemToResource.clear();

    if (!item)
        return;

    const QString currentPath = m_itemToPath.value(item);
    createResources(currentPath);
}

void QtResourceViewPrivate::slotCurrentResourceChanged(QListWidgetItem *item)
{
    m_copyResourcePathAction->setEnabled(item);
    if (m_ignoreGuiSignals)
        return;

    emit q_ptr->resourceSelected(m_itemToResource.value(item));
}

void QtResourceViewPrivate::slotResourceActivated(QListWidgetItem *item)
{
    if (m_ignoreGuiSignals)
        return;

    emit q_ptr->resourceActivated(m_itemToResource.value(item));
}

void QtResourceViewPrivate::createPaths()
{
    if (!m_resourceModel)
        return;

    const QString root(QLatin1Char(':'));

    QMap<QString, QString> pathToParentPath; // full path to full parent path
    QMap<QString, QStringList> pathToSubPaths; // full path to full sub paths

    QMap<QString, QString> contents = m_resourceModel->contents();
    QMapIterator<QString, QString> itContents(contents);
    while (itContents.hasNext()) {
        const QString filePath = itContents.next().key();
        const QFileInfo fi(filePath);
        QString dirPath = fi.absolutePath();
        m_pathToContents[dirPath].append(fi.fileName());
        while (!pathToParentPath.contains(dirPath) && dirPath != root) {
            const QFileInfo fd(dirPath);
            const QString parentDirPath = fd.absolutePath();
            pathToParentPath[dirPath] = parentDirPath;
            pathToSubPaths[parentDirPath].append(dirPath);
            dirPath = parentDirPath;
        }
    }

    QQueue<QPair<QString, QTreeWidgetItem *> > pathToParentItemQueue;
    pathToParentItemQueue.enqueue(qMakePair(root, static_cast<QTreeWidgetItem *>(0)));
    while (!pathToParentItemQueue.isEmpty()) {
        QPair<QString, QTreeWidgetItem *> pathToParentItem = pathToParentItemQueue.dequeue();
        const QString path = pathToParentItem.first;
        QTreeWidgetItem *item = createPath(path, pathToParentItem.second);
        QStringList subPaths = pathToSubPaths.value(path);
        QStringListIterator itSubPaths(subPaths);
        while (itSubPaths.hasNext())
            pathToParentItemQueue.enqueue(qMakePair(itSubPaths.next(), item));
    }
}

QTreeWidgetItem *QtResourceViewPrivate::createPath(const QString &path, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = 0;
    if (parent)
        item = new QTreeWidgetItem(parent);
    else
        item = new QTreeWidgetItem(m_treeWidget);
    m_pathToItem[path] = item;
    m_itemToPath[item] = path;
    QString substPath;
    if (parent) {
        QFileInfo di(path);
        substPath = di.fileName();
    } else {
        substPath = QLatin1String("<resource root>");
    }
    item->setText(0, substPath);
    item->setToolTip(0, path);
    return item;
}

void QtResourceViewPrivate::createResources(const QString &path)
{
    QDir dir(path);
    QStringList files = m_pathToContents.value(path);
    QStringListIterator it(files);
    while (it.hasNext()) {
        QString file = it.next();
        QString filePath = dir.absoluteFilePath(file);
        QFileInfo fi(filePath);
        if (fi.isFile()) {
            QListWidgetItem *item = new QListWidgetItem(fi.fileName(), m_listWidget);
            const QPixmap pix = QPixmap(filePath);
            if (pix.isNull()) {
                item->setToolTip(filePath);
            } else {
                item->setIcon(QIcon(makeThumbnail(pix)));
                const QSize size = pix.size();
                item->setToolTip(QtResourceView::tr("Size: %1 x %2\n%3").arg(size.width()).arg(size.height()).arg(filePath));
            }
            item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
            item->setData(Qt::UserRole, filePath);
            m_itemToResource[item] = filePath;
            m_resourceToItem[filePath] = item;
        }
    }
}

// -------------- QtResourceView

QtResourceView::QtResourceView(QDesignerFormEditorInterface *core, QWidget *parent) :
    QWidget(parent),
    d_ptr(new QtResourceViewPrivate(core))
{
    d_ptr->q_ptr = this;

    d_ptr->m_editResourcesAction = new QAction(qdesigner_internal::createIconSet(QLatin1String("edit.png")), tr("Edit Resources..."), this);
    d_ptr->m_toolBar->addAction(d_ptr->m_editResourcesAction);
    connect(d_ptr->m_editResourcesAction, SIGNAL(triggered()), this, SLOT(slotEditResources()));
    d_ptr->m_editResourcesAction->setEnabled(false);

    d_ptr->m_reloadResourcesAction = new QAction(qdesigner_internal::createIconSet(QLatin1String("reload.png")), tr("Reload"), this);
    d_ptr->m_toolBar->addAction(d_ptr->m_reloadResourcesAction);
    connect(d_ptr->m_reloadResourcesAction, SIGNAL(triggered()), this, SLOT(slotReloadResources()));
    d_ptr->m_reloadResourcesAction->setEnabled(false);

    d_ptr->m_copyResourcePathAction = new QAction(tr("Copy Path"), this);
    connect(d_ptr->m_copyResourcePathAction, SIGNAL(triggered()), this, SLOT(slotCopyResourcePath()));
    d_ptr->m_copyResourcePathAction->setEnabled(false);

    QSplitter *splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(d_ptr->m_treeWidget);
    splitter->addWidget(d_ptr->m_listWidget);

    QLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d_ptr->m_toolBar);
    layout->addWidget(splitter);

    d_ptr->m_treeWidget->setColumnCount(1);
    d_ptr->m_treeWidget->header()->hide();
    d_ptr->m_treeWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));

    d_ptr->m_listWidget->setViewMode(QListView::IconMode);
    d_ptr->m_listWidget->setResizeMode(QListView::Adjust);
    d_ptr->m_listWidget->setIconSize(QSize(48, 48));
    d_ptr->m_listWidget->setGridSize(QSize(64, 64));

    connect(d_ptr->m_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
                    this, SLOT(slotCurrentPathChanged(QTreeWidgetItem *)));
    connect(d_ptr->m_listWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
                    this, SLOT(slotCurrentResourceChanged(QListWidgetItem *)));
    connect(d_ptr->m_listWidget, SIGNAL(itemActivated(QListWidgetItem *)),
                    this, SLOT(slotResourceActivated(QListWidgetItem *)));
    d_ptr->m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d_ptr->m_listWidget, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(slotListWidgetContextMenuRequested(QPoint)));
}

QtResourceView::~QtResourceView()
{
    delete d_ptr;
}

bool QtResourceView::event(QEvent *event)
{
    if (event->type() == QEvent::Show) {
        d_ptr->m_listWidget->scrollToItem(d_ptr->m_listWidget->currentItem());
        d_ptr->m_treeWidget->scrollToItem(d_ptr->m_treeWidget->currentItem());
    }
    return QWidget::event(event);
}

QtResourceModel *QtResourceView::model() const
{
    return d_ptr->m_resourceModel;
}

QString QtResourceView::selectedResource() const
{
    QListWidgetItem *item = d_ptr->m_listWidget->currentItem();
    return d_ptr->m_itemToResource.value(item);
}

void QtResourceView::selectResource(const QString &resource)
{
    QFileInfo fi(resource);
    QDir dir = fi.absoluteDir();
    if (fi.isDir())
        dir = QDir(resource);
    QString dirPath = dir.absolutePath();
    QMap<QString, QTreeWidgetItem *>::const_iterator it;
    while ((it = d_ptr->m_pathToItem.find(dirPath)) == d_ptr->m_pathToItem.constEnd()) {
        if (!dir.cdUp())
            break;
        dirPath = dir.absolutePath();
    }
    if (it != d_ptr->m_pathToItem.constEnd()) {
        QTreeWidgetItem *treeItem = it.value();
        d_ptr->m_treeWidget->setCurrentItem(treeItem);
        d_ptr->m_treeWidget->scrollToItem(treeItem);
        // expand all up to current one is done by qt
        // list widget is already propagated (currrent changed was sent by qt)
        QListWidgetItem *item = d_ptr->m_resourceToItem.value(resource);
        if (item) {
            d_ptr->m_listWidget->setCurrentItem(item);
            d_ptr->m_listWidget->scrollToItem(item);
        }
    }
}

void QtResourceView::setResourceModel(QtResourceModel *model)
{
    if (d_ptr->m_resourceModel) {
        disconnect(d_ptr->m_resourceModel, SIGNAL(resourceSetActivated(QtResourceSet *, bool)),
                    this, SLOT(slotResourceSetActivated(QtResourceSet *)));
    }

    // clear here
    d_ptr->m_treeWidget->clear();
    d_ptr->m_listWidget->clear();

    d_ptr->m_resourceModel = model;

    if (!d_ptr->m_resourceModel)
        return;

    connect(d_ptr->m_resourceModel, SIGNAL(resourceSetActivated(QtResourceSet *, bool)),
            this, SLOT(slotResourceSetActivated(QtResourceSet *)));

    // fill new here
    d_ptr->slotResourceSetActivated(d_ptr->m_resourceModel->currentResourceSet());
}

void QtResourceView::setDragEnabled(bool dragEnabled)
{
    d_ptr->m_listWidget->setDragEnabled(dragEnabled);
}

bool QtResourceView::dragEnabled() const
{
    return d_ptr->m_listWidget->dragEnabled();
}

QString QtResourceView::encodeMimeData(ResourceType resourceType, const QString &path)
{
    QDomDocument doc;
    QDomElement elem = doc.createElement(QLatin1String(elementResourceData));
    switch (resourceType) {
    case ResourceImage:
        elem.setAttribute(QLatin1String(typeAttribute), QLatin1String(typeImage));
        break;
    case ResourceStyleSheet:
        elem.setAttribute(QLatin1String(typeAttribute), QLatin1String(typeStyleSheet));
        break;
    case ResourceOther:
        elem.setAttribute(QLatin1String(typeAttribute), QLatin1String(typeOther));
        break;
    }
    elem.setAttribute(QLatin1String(fileAttribute), path);
    doc.appendChild(elem);
    return doc.toString();
}

bool QtResourceView::decodeMimeData(const QMimeData *md, ResourceType *t, QString *file)
{
    return md->hasText() ? decodeMimeData(md->text(), t, file) : false;
}

bool QtResourceView::decodeMimeData(const QString &text, ResourceType *t, QString *file)
{

    const QString docElementName = QLatin1String(elementResourceData);
    static const QString docElementString = QLatin1Char('<') + docElementName;

    if (text.isEmpty() || text.indexOf(docElementString) == -1)
        return false;

    QDomDocument doc;
    if (!doc.setContent(text))
        return false;

    const QDomElement domElement = doc.documentElement();
    if (domElement.tagName() != docElementName)
        return false;

    if (t) {
        const QString typeAttr = QLatin1String(typeAttribute);
        if (domElement.hasAttribute (typeAttr)) {
            const QString typeValue = domElement.attribute(typeAttr, QLatin1String(typeOther));
            if (typeValue == QLatin1String(typeImage)) {
                *t = ResourceImage;
            } else {
                *t = typeValue == QLatin1String(typeStyleSheet) ? ResourceStyleSheet : ResourceOther;
            }
        }
    }
    if (file) {
        const QString fileAttr = QLatin1String(fileAttribute);
        if (domElement.hasAttribute(fileAttr)) {
            *file = domElement.attribute(fileAttr, QString());
        } else {
            file->clear();
        }
    }
    return true;
}

// ---------------------------- QtResourceViewDialogPrivate

class QtResourceViewDialogPrivate
{
    QtResourceViewDialog *q_ptr;
    Q_DECLARE_PUBLIC(QtResourceViewDialog)
public:
    QtResourceViewDialogPrivate(QDesignerFormEditorInterface *core);

    void slotResourceSelected(const QString &resource) { setOkButtonEnabled(!resource.isEmpty()); }
    void setOkButtonEnabled(bool v)                    { m_box->button(QDialogButtonBox::Ok)->setEnabled(v); }

    QtResourceView *m_view;
    QDialogButtonBox *m_box;
};

QtResourceViewDialogPrivate::QtResourceViewDialogPrivate(QDesignerFormEditorInterface *core) :
    q_ptr(0),
    m_view(new QtResourceView(core)),
    m_box(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel))
{
}

// ------------ QtResourceViewDialog
QtResourceViewDialog::QtResourceViewDialog(QDesignerFormEditorInterface *core, QWidget *parent) :
    QDialog(parent),
    d_ptr(new QtResourceViewDialogPrivate(core))
{
    setWindowTitle(tr("Select Resource"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    d_ptr->q_ptr = this;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d_ptr->m_view);
    layout->addWidget(d_ptr->m_box);
    connect(d_ptr->m_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d_ptr->m_box, SIGNAL(rejected()), this, SLOT(reject()));
    connect(d_ptr->m_view, SIGNAL(resourceActivated(QString)), this, SLOT(accept()));
    connect(d_ptr->m_view, SIGNAL(resourceSelected(QString)), this, SLOT(slotResourceSelected(QString)));
    d_ptr->setOkButtonEnabled(false);
    d_ptr->m_view->setResourceModel(core->resourceModel());
}

QtResourceViewDialog::~QtResourceViewDialog()
{
    delete d_ptr;
}

QString QtResourceViewDialog::selectedResource() const
{
    return d_ptr->m_view->selectedResource();
}

void QtResourceViewDialog::selectResource(const QString &path)
{
    d_ptr->m_view->selectResource(path);
}

QT_END_NAMESPACE

#include "moc_qtresourceview_p.cpp"
