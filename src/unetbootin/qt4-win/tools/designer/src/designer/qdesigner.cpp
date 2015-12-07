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

// designer
#include "qdesigner.h"
#include "qdesigner_actions.h"
#include "qdesigner_server.h"
#include "qdesigner_settings.h"
#include "qdesigner_workbench.h"
#include "qdesigner_toolwindow.h"

#include <QtGui/QFileOpenEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QIcon>
#include <QtGui/QErrorMessage>
#include <QtCore/QMetaObject>
#include <QtCore/QFile>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtCore/qdebug.h>

#include <QtDesigner/QDesignerComponents>

QT_BEGIN_NAMESPACE

static const char *designerApplicationName = "Designer";
static const char *designerWarningPrefix = "Designer: ";

static void designerMessageHandler(QtMsgType type, const char *msg)
{
    // Only Designer warnings are displayed as box
    QDesigner *designerApp = qDesigner;
    if (type != QtWarningMsg || !designerApp || qstrncmp(designerWarningPrefix, msg, qstrlen(designerWarningPrefix))) {
        qInstallMsgHandler(0);
        qt_message_output(type, msg);
        qInstallMsgHandler (designerMessageHandler);
        return;
    }
    designerApp->showErrorMessage(msg);
}

QDesigner::QDesigner(int &argc, char **argv)
    : QApplication(argc, argv),
      m_server(0),
      m_client(0),
      m_workbench(0), m_suppressNewFormShow(false)
{
    setOrganizationName(QLatin1String("Trolltech"));
    setApplicationName(QLatin1String(designerApplicationName));
    QDesignerComponents::initializeResources();

#ifndef Q_WS_MAC
    setWindowIcon(QIcon(QLatin1String(":/trolltech/designer/images/designer.png")));
#endif
    initialize();
}

QDesigner::~QDesigner()
{
    if (m_workbench)
        delete m_workbench;
    if (m_server)
        delete m_server;
    if (m_client)
        delete m_client;
}

void QDesigner::showErrorMessage(const char *message)
{
    // strip the prefix
    const QString qMessage = QString::fromUtf8(message + qstrlen(designerWarningPrefix));
    // If there is no main window yet, just store the message.
    // The QErrorMessage would otherwise be hidden by the main window.
    if (m_mainWindow) {
        showErrorMessageBox(qMessage);
    } else {
        qInstallMsgHandler(0);
        qt_message_output(QtWarningMsg, message); // just in case we crash
        qInstallMsgHandler (designerMessageHandler);
        m_initializationErrors += qMessage;
        m_initializationErrors += QLatin1Char('\n');
    }
}

void QDesigner::showErrorMessageBox(const QString &msg)
{
    // Manually suppress consecutive messages.
    // This happens if for example sth is wrong with custom widget creation.
    // The same warning will be displayed by Widget box D&D and form Drop
    // while trying to create instance.
    if (m_errorMessageDialog && m_lastErrorMessage == msg)
        return;

    if (!m_errorMessageDialog) {
        m_lastErrorMessage.clear();
        m_errorMessageDialog = new QErrorMessage(m_mainWindow);
        const QString title = QObject::tr("%1 - warning").arg(QLatin1String(designerApplicationName));
        m_errorMessageDialog->setWindowTitle(title);
        m_errorMessageDialog->setMinimumSize(QSize(600, 250));
        m_errorMessageDialog->setWindowFlags(m_errorMessageDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }
    m_errorMessageDialog->showMessage(msg);
    m_lastErrorMessage = msg;
}

QDesignerWorkbench *QDesigner::workbench() const
{
    return m_workbench;
}

QDesignerServer *QDesigner::server() const
{
    return m_server;
}

void QDesigner::initialize()
{
    // initialize the sub components
    QStringList files;

    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

    const QStringList args = arguments();

    for (int i = 1; i < args.count(); ++i)
    {
        const QString argument = args.at(i);
        if (argument == QLatin1String("-server")) {
            m_server = new QDesignerServer();
            printf("%d\n", m_server->serverPort());
            fflush(stdout);
        } else if (argument == QLatin1String("-client")) {
            bool ok = true;
            if (i + 1 < args.count()) {
                const quint16 port = args.at(++i).toUShort(&ok);
                if (ok)
                    m_client = new QDesignerClient(port, this);
            }
        } else if (argument == QLatin1String("-resourcedir")) {
            if (i + 1 < args.count()) {
                resourceDir = QFile::decodeName(args.at(++i).toLocal8Bit());
            } else {
                // issue a warning
            }
        } else if (!files.contains(argument)) {
            files.append(argument);
        }
    }

    QTranslator *translator = new QTranslator(this);
    QTranslator *qtTranslator = new QTranslator(this);

    const QString localSysName = QLocale::system().name();
    QString  translatorFileName = QLatin1String("designer_");
    translatorFileName += localSysName;
    translator->load(translatorFileName, resourceDir);

    translatorFileName = QLatin1String("qt_");
    translatorFileName += localSysName;
    qtTranslator->load(translatorFileName, resourceDir);
    installTranslator(translator);
    installTranslator(qtTranslator);

    if (QLibraryInfo::licensedProducts() == QLatin1String("Console")) {
        QMessageBox::information(0, tr("Qt Designer"),
                tr("This application cannot be used for the Console edition of Qt"));
        QMetaObject::invokeMethod(this, "quit", Qt::QueuedConnection);
        return;
    }

    m_workbench = new QDesignerWorkbench();

    emit initialized();
    qInstallMsgHandler(designerMessageHandler); // Warn when loading faulty forms

    m_suppressNewFormShow = m_workbench->readInBackup();

    foreach (QString file, files) {
        m_workbench->readInForm(file);
    }
    if ( m_workbench->formWindowCount())
        m_suppressNewFormShow = true;

    // Show up error box with parent now if something went wrong
    if (m_initializationErrors.isEmpty()) {
        if (!m_suppressNewFormShow && QDesignerSettings().showNewFormOnStartup())
            QTimer::singleShot(100, this, SLOT(callCreateForm())); // won't show anything if suppressed
    } else {
        showErrorMessageBox(m_initializationErrors);
        m_initializationErrors.clear();
    }
}

bool QDesigner::event(QEvent *ev)
{
    bool eaten;
    switch (ev->type()) {
    case QEvent::FileOpen:
        // Set it true first since, if it's a Qt 3 form, the messagebox from convert will fire the timer.
        m_suppressNewFormShow = true;
        if (!m_workbench->readInForm(static_cast<QFileOpenEvent *>(ev)->file()))
            m_suppressNewFormShow = false;
        eaten = true;
        break;
    case QEvent::Close: {
        QCloseEvent *closeEvent = static_cast<QCloseEvent *>(ev);
        closeEvent->setAccepted(m_workbench->handleClose());
        if (closeEvent->isAccepted()) {
            // We're going down, make sure that we don't get our settings saved twice.
            if (m_mainWindow)
                m_mainWindow->setSaveSettingsOnClose(false);
            eaten = QApplication::event(ev);
        }
        eaten = true;
        break;
    }
    default:
        eaten = QApplication::event(ev);
        break;
    }
    return eaten;
}

void QDesigner::setMainWindow(QDesignerToolWindow *tw)
{
    m_mainWindow = tw;
}

QDesignerToolWindow *QDesigner::mainWindow() const
{
    return m_mainWindow;
}

void QDesigner::callCreateForm()
{
    if (!m_suppressNewFormShow)
        m_workbench->actionManager()->createForm();
}

QT_END_NAMESPACE
