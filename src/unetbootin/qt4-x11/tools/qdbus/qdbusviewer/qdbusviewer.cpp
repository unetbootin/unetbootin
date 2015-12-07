/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "qdbusviewer.h"
#include "qdbusmodel.h"
#include "propertydialog.h"

#include <QtXml/QtXml>
#include <QtDBus/private/qdbusutil_p.h>

class QDBusViewModel: public QDBusModel
{
public:
    inline QDBusViewModel(const QString &service, const QDBusConnection &connection)
        : QDBusModel(service, connection)
    {}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::FontRole && itemType(index) == InterfaceItem) {
            QFont f;
            f.setItalic(true);
            return f;
        }
        return QDBusModel::data(index, role);
    }
};

QDBusViewer::QDBusViewer(const QDBusConnection &connection, QWidget *parent)
    : QWidget(parent), c(connection), objectPathRegExp("\\[ObjectPath: (.*)\\]")
{
    services = new QTreeWidget;
    services->setRootIsDecorated(false);
    services->setHeaderLabels(QStringList("Services"));

    tree = new QTreeView;
    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree, SIGNAL(activated(const QModelIndex&)), this, SLOT(activate(const QModelIndex&)));

    refreshAction = new QAction(tr("&Refresh"), tree);
    refreshAction->setData(42); // increase the amount of 42 used as magic number by one
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(refreshChildren()));

    QShortcut *refreshShortcut = new QShortcut(QKeySequence::Refresh, tree);
    connect(refreshShortcut, SIGNAL(activated()), this, SLOT(refreshChildren()));

    QVBoxLayout *topLayout = new QVBoxLayout(this);
    log = new QTextBrowser;
    connect(log, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(services, 1);
    layout->addWidget(tree, 2);

    topLayout->addLayout(layout);
    topLayout->addWidget(log);

    connect(services, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(serviceChanged(QTreeWidgetItem*)));
    connect(tree, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));

    QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);

    if (c.isConnected()) {
        logMessage("Connected to D-Bus.");
        QDBusConnectionInterface *iface = c.interface();
        connect(iface, SIGNAL(serviceRegistered(QString)),
                this, SLOT(serviceRegistered(QString)));
        connect(iface, SIGNAL(serviceUnregistered(QString)),
                this, SLOT(serviceUnregistered(QString)));
        connect(iface, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                this, SLOT(serviceOwnerChanged(QString,QString,QString)));
    } else {
        logError("Cannot connect to D-Bus: " + c.lastError().message());
    }

    objectPathRegExp.setMinimal(true);

}

void QDBusViewer::logMessage(const QString &msg)
{
    log->append(msg + "\n");
}

void QDBusViewer::logError(const QString &msg)
{
    log->append("<font color=\"red\">Error: </font>" + Qt::escape(msg) + "<br>");
}

void QDBusViewer::refresh()
{
    services->clear();

    if (c.isConnected()) {
        const QStringList serviceNames = c.interface()->registeredServiceNames();
        foreach (QString service, serviceNames)
            new QTreeWidgetItem(services, QStringList(service));
    }
}

void QDBusViewer::activate(const QModelIndex &item)
{
    if (!item.isValid())
        return;

    const QDBusModel *model = static_cast<const QDBusModel *>(item.model());

    BusSignature sig;
    sig.mService = currentService;
    sig.mPath = model->dBusPath(item);
    sig.mInterface = model->dBusInterface(item);
    sig.mName = model->dBusMethodName(item);

    switch (model->itemType(item)) {
    case QDBusModel::SignalItem:
        connectionRequested(sig);
        break;
    case QDBusModel::MethodItem:
        callMethod(sig);
        break;
    case QDBusModel::PropertyItem:
        getProperty(sig);
        break;
    default:
        break;
    }
}

void QDBusViewer::getProperty(const BusSignature &sig)
{
    QDBusMessage message = QDBusMessage::createMethodCall(sig.mService, sig.mPath, "org.freedesktop.DBus.Properties", "Get");
    QList<QVariant> arguments;
    arguments << sig.mInterface << sig.mName;
    message.setArguments(arguments);
    c.callWithCallback(message, this, SLOT(dumpMessage(QDBusMessage)));
}

void QDBusViewer::setProperty(const BusSignature &sig)
{
    QDBusInterface iface(sig.mService, sig.mPath, sig.mInterface, c);
    QMetaProperty prop = iface.metaObject()->property(iface.metaObject()->indexOfProperty(sig.mName.toLatin1()));

    bool ok;
    QString input = QInputDialog::getText(this, "Arguments",
                    QString("Please enter the value of the property %1 (type %2)").arg(
                        sig.mName).arg(prop.typeName()),
                    QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;

    QVariant value = input;
    if (!value.convert(prop.type())) {
        QMessageBox::warning(this, "Unable to marshall",
                "Value conversion failed, unable to set property");
        return;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(sig.mService, sig.mPath, "org.freedesktop.DBus.Properties", "Set");
    QList<QVariant> arguments;
    arguments << sig.mInterface << sig.mName << qVariantFromValue(QDBusVariant(value));
    message.setArguments(arguments);
    c.callWithCallback(message, this, SLOT(dumpMessage(QDBusMessage)));

}

void QDBusViewer::callMethod(const BusSignature &sig)
{
    QDBusInterface iface(sig.mService, sig.mPath, sig.mInterface, c);
    const QMetaObject *mo = iface.metaObject();

    // find the method
    QMetaMethod method;
    for (int i = 0; i < mo->methodCount(); ++i) {
        const QString signature = QString::fromLatin1(mo->method(i).signature());
        if (signature.startsWith(sig.mName) && signature.at(sig.mName.length()) == '(')
            method = mo->method(i);
    }
    if (!method.signature()) {
        QMessageBox::warning(this, "Unable to find method",
                QString("Unable to find method %1 on path %2 in interface %3").arg(
                    sig.mName).arg(sig.mPath).arg(sig.mInterface));
        return;
    }

    PropertyDialog dialog;
    QList<QVariant> args;

    const QList<QByteArray> paramTypes = method.parameterTypes();
    const QList<QByteArray> paramNames = method.parameterNames();
    QList<int> types; // remember the low-level D-Bus type
    for (int i = 0; i < paramTypes.count(); ++i) {
        const QByteArray paramType = paramTypes.at(i);
        if (paramType.endsWith('&'))
            continue; // ignore OUT parameters

        QVariant::Type type = QVariant::nameToType(paramType);
        dialog.addProperty(QString::fromLatin1(paramNames.value(i)), type);
        types.append(QMetaType::type(paramType));
    }

    if (!types.isEmpty()) {
        dialog.setInfo("Please enter parameters for the method \"" + sig.mName + "\"");

        if (dialog.exec() != QDialog::Accepted)
            return;

        args = dialog.values();
    }

    // Special case - convert a value to a QDBusVariant if the
    // interface wants a variant
    for (int i = 0; i < args.count(); ++i) {
        if (types.at(i) == qMetaTypeId<QDBusVariant>())
            args[i] = qVariantFromValue(QDBusVariant(args.at(i)));
    }

    QDBusMessage message = QDBusMessage::createMethodCall(sig.mService, sig.mPath, sig.mInterface,
            sig.mName);
    message.setArguments(args);
    c.callWithCallback(message, this, SLOT(dumpMessage(QDBusMessage)));
}

void QDBusViewer::showContextMenu(const QPoint &point)
{
    QModelIndex item = tree->indexAt(point);
    if (!item.isValid())
        return;

    const QDBusModel *model = static_cast<const QDBusModel *>(item.model());

    BusSignature sig;
    sig.mService = currentService;
    sig.mPath = model->dBusPath(item);
    sig.mInterface = model->dBusInterface(item);
    sig.mName = model->dBusMethodName(item);

    QMenu menu;
    menu.addAction(refreshAction);

    switch (model->itemType(item)) {
    case QDBusModel::SignalItem: {
        QAction *action = new QAction("&Connect", &menu);
        action->setData(1);
        menu.addAction(action);
        break; }
    case QDBusModel::MethodItem: {
        QAction *action = new QAction("&Call", &menu);
        action->setData(2);
        menu.addAction(action);
        break; }
    case QDBusModel::PropertyItem: {
        QAction *actionSet = new QAction("&Set value", &menu);
        actionSet->setData(3);
        QAction *actionGet = new QAction("&Get value", &menu);
        actionGet->setData(4);
        menu.addAction(actionSet);
        menu.addAction(actionGet);
        break; }
    default:
        break;
    }

    QAction *selectedAction = menu.exec(tree->viewport()->mapToGlobal(point));
    if (!selectedAction)
        return;

    switch (selectedAction->data().toInt()) {
    case 1:
        connectionRequested(sig);
        break;
    case 2:
        callMethod(sig);
        break;
    case 3:
        setProperty(sig);
        break;
    case 4:
        getProperty(sig);
        break;
    }
}

void QDBusViewer::connectionRequested(const BusSignature &sig)
{
    if (!c.connect(sig.mService, QString(), sig.mInterface, sig.mName, this,
              SLOT(dumpMessage(QDBusMessage)))) {
        logError(QString("Unable to connect to service %1, path %2, interface %3, signal %4").arg(
                    sig.mService).arg(sig.mPath).arg(sig.mInterface).arg(sig.mName));
    }
}

void QDBusViewer::dumpMessage(const QDBusMessage &message)
{
    QList<QVariant> args = message.arguments();
    QString out = "Received ";

    switch (message.type()) {
    case QDBusMessage::SignalMessage:
        out += "signal ";
        break;
    case QDBusMessage::ErrorMessage:
        out += "error message ";
        break;
    case QDBusMessage::ReplyMessage:
        out += "reply ";
        break;
    default:
        out += "message ";
        break;
    }

    out += "from ";
    out += message.service();
    if (!message.path().isEmpty())
        out += ", path " + message.path();
    if (!message.interface().isEmpty())
        out += ", interface <i>" + message.interface() + "</i>";
    if (!message.member().isEmpty())
        out += ", member " + message.member();
    out += "<br>";
    if (args.isEmpty()) {
        out += "&nbsp;&nbsp;(no arguments)";
    } else {
        out += "&nbsp;&nbsp;Arguments: ";
        foreach (QVariant arg, args) {
            QString str = Qt::escape(QDBusUtil::argumentToString(arg));
            // turn object paths into clickable links
            str.replace(objectPathRegExp, "[ObjectPath: <a href=\"qdbus://bus\\1\">\\1</a>]");
            out += str;
            out += ", ";
        }
        out.chop(2);
    }

    log->append(out);
}

void QDBusViewer::serviceChanged(QTreeWidgetItem *item)
{
    delete tree->model();

    currentService.clear();
    if (!item)
        return;
    currentService = item->text(0);

    tree->setModel(new QDBusViewModel(currentService, c));
    connect(tree->model(), SIGNAL(busError(QString)), this, SLOT(logError(QString)));
}

void QDBusViewer::serviceRegistered(const QString &service)
{
    if (service == c.baseService())
        return;

    new QTreeWidgetItem(services, QStringList(service));
}

static QTreeWidgetItem *findItem(const QTreeWidget *services, const QString &name)
{
    for (int i = 0; i < services->topLevelItemCount(); ++i) {
        if (services->topLevelItem(i)->text(0) == name)
            return services->topLevelItem(i);
    }
    return 0;
}

void QDBusViewer::serviceUnregistered(const QString &name)
{
    delete findItem(services, name);
}

void QDBusViewer::serviceOwnerChanged(const QString &name, const QString &oldOwner,
                                      const QString &newOwner)
{
    QTreeWidgetItem *item = findItem(services, name);

    if (!item && oldOwner.isEmpty() && !newOwner.isEmpty())
        serviceRegistered(name);
    else if (item && !oldOwner.isEmpty() && newOwner.isEmpty())
        delete item;
    else if (item && !oldOwner.isEmpty() && !newOwner.isEmpty()) {
        delete item;
        serviceRegistered(name);
    }
}

void QDBusViewer::refreshChildren()
{
    QDBusModel *model = qobject_cast<QDBusModel *>(tree->model());
    if (!model)
        return;
    model->refresh(tree->currentIndex());
}

void QDBusViewer::about()
{
    QMessageBox box(this);
#if QT_EDITION == QT_EDITION_OPENSOURCE
    QString edition = tr("Open Source Edition");
    QString info = tr("This version of Qt's D-Bus Viewer is part of the Qt Open Source Edition. "
            "Qt is a comprehensive C++ framework for cross-platform application "
            "development.");
    QString moreInfo = tr("You need a commercial Qt license for development of proprietary (closed "
            "source) applications. Please see <a href=\"http://qtsoftware.com/company/model"
            ".html\">qtsoftware.com/company/model.html</a> for an overview of Qt licensing.");
#else
    QString edition;
    QString info;
    QString moreInfo(tr("This program is licensed to you under the terms of the "
                "Qt Commercial License Agreement. For details, see the file LICENSE "
                "that came with this software distribution."));

#endif

    box.setText(QString::fromLatin1("<center><img src=\":/trolltech/qdbusviewer/images/qdbusviewer-128.png\">"
                "<h3>%1</h3>"
                "<p>Version %2 %3</p></center>"
                "<p>%4</p>"
                "<p>%5</p>"
                "<p>Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).</p>"
                "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                " INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A"
                " PARTICULAR PURPOSE.<p/>")
            .arg(tr("D-Bus Viewer")).arg(QLatin1String(QT_VERSION_STR)).arg(edition).arg(info).arg(moreInfo));
    box.setWindowTitle(tr("D-Bus Viewer"));
    box.exec();
}

void QDBusViewer::anchorClicked(const QUrl &url)
{
    if (url.scheme() != "qdbus")
        // not ours
        return;

    // swallow the click without setting a new document
    log->setSource(QUrl());

    QDBusModel *model = qobject_cast<QDBusModel *>(tree->model());
    if (!model)
        return;

    QModelIndex idx = model->findObject(QDBusObjectPath(url.path()));
    if (!idx.isValid())
        return;

    tree->scrollTo(idx);
    tree->setCurrentIndex(idx);
}

/*!
  \page qdbusviewer.html
  \title D-Bus Viewer
  \keyword qdbusviewer

  The Qt D-Bus Viewer is a tool that lets you introspect D-Bus objects and messages. You can
  choose between the system bus and the session bus. Click on any service on the list
  on the left side to see all the exported objects.

  You can invoke methods by double-clicking on them. If a method takes one or more IN parameters,
  a property editor opens.

  Right-click on a signal to connect to it. All emitted signals including their parameters
  are output in the message view on the lower side of the window.
*/
