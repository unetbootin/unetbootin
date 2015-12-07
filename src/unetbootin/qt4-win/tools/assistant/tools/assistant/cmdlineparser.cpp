/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

#include "cmdlineparser.h"

QT_BEGIN_NAMESPACE

#define CHECK_NEXT_ARG \
    ++i < arguments.count() && !arguments.at(i).startsWith(QLatin1String("-"))

CmdLineParser::CmdLineParser()
    : m_enableRemoteControl(false),
    m_contents(Untouched),
    m_index(Untouched),
    m_bookmarks(Untouched),
    m_search(Untouched),
    m_register(None),
    m_copy(false),
    m_quiet(false)
{    
    m_helpMessage = QLatin1String(
        "Usage: assistant [Options]\n\n"
        "-collectionFile file   Uses the specified collection\n"
        "                       file instead of the default one\n"
        "-showUrl url           Shows the document with the\n"
        "                       url.\n"
        "-enableRemoteControl   Enables Assistant to be\n"
        "                       remotely controlled.\n"
        "-show widget           Shows the specified dockwidget\n"
        "                       which can be \"contents\", \"index\",\n"
        "                       \"bookmarks\" or \"search\".\n"
        "-activate widget       Activates the specified dockwidget\n"
        "                       which can be \"contents\", \"index\",\n"
        "                       \"bookmarks\" or \"search\".\n"
        "-hide widget           Hides the specified dockwidget\n"
        "                       which can be \"contents\", \"index\"\n"
        "                       \"bookmarks\" or \"search\".\n"
        "-register helpFile     Registers the specified help file\n"
        "                       (.qch) in the given collection\n"
        "                       file.\n"
        "-unregister helpFile   Unregisters the specified help file\n"
        "                       (.qch) from the give collection\n"
        "                       file.\n"
        "-quiet                 Does not display any error or\n"
        "                       status message.\n"
        "-help                  Displays this help.\n"
        );
}

CmdLineParser::Result CmdLineParser::parse(const QStringList &arguments)
{
    QString error;
    bool showHelp = false;

    for (int j=1; j<arguments.count(); ++j) {
        if (arguments.at(j).toLower() == QLatin1String("-quiet")) {
            m_quiet = true;
            break;
        }
    }
    
    for (int i=1; i<arguments.count(); ++i) {
        QString arg = arguments.at(i).toLower();
        if (arg == QLatin1String("-collectionfile")) {
            if (CHECK_NEXT_ARG) {
                m_collectionFile = getFileName(arguments.at(i));
                if (m_collectionFile.isEmpty()) {
                    error = QObject::tr("The specified collection file does not exist!");
                    break;
                }
            } else {
                error = QObject::tr("Missing collection file!");
                break;
            }
        } else if (arg == QLatin1String("-showurl")) {
            if (CHECK_NEXT_ARG) {
                QUrl url(arguments.at(i));
                if (url.isValid()) {
                    m_url = url;
                } else {
                    error = QObject::tr("Invalid URL!");
                    break;
                }
            } else {
                error = QObject::tr("Missing URL!");
                break;
            }
        } else if (arg == QLatin1String("-enableremotecontrol")) {
            m_enableRemoteControl = true;
        } else if (arg == QLatin1String("-show")) {
            if (CHECK_NEXT_ARG) {
                arg = arguments.at(i).toLower();
                if (arg == QLatin1String("contents")) {
                    m_contents = Show;
                } else if (arg == QLatin1String("index")) {
                    m_index = Show;
                } else if (arg == QLatin1String("bookmarks")) {
                    m_bookmarks = Show;
                } else if (arg == QLatin1String("search")) {
                    m_search = Show;
                } else {
                    error = QObject::tr("Unknown widget: %1").arg(arg);
                    break;
                }
            } else {
                error = QObject::tr("Missing widget!");
                break;
            }
        } else if (arg == QLatin1String("-hide")) {
            if (CHECK_NEXT_ARG) {
                arg = arguments.at(i).toLower();
                if (arg == QLatin1String("contents")) {
                    m_contents = Hide;
                } else if (arg == QLatin1String("index")) {
                    m_index = Hide;
                } else if (arg == QLatin1String("bookmarks")) {
                    m_bookmarks = Hide;
                } else if (arg == QLatin1String("search")) {
                    m_search = Hide;
                } else {
                    error = QObject::tr("Unknown widget: %1").arg(arg);
                    break;
                }
            } else {
                error = QObject::tr("Missing widget!");
                break;
            }
        } else if (arg == QLatin1String("-activate")) {
            if (CHECK_NEXT_ARG) {
                arg = arguments.at(i).toLower();
                if (arg == QLatin1String("contents")) {
                    m_contents = Activate;
                } else if (arg == QLatin1String("index")) {
                    m_index = Activate;
                } else if (arg == QLatin1String("bookmarks")) {
                    m_bookmarks = Activate;
                } else if (arg == QLatin1String("search")) {
                    m_search = Activate;
                } else {
                    error = QObject::tr("Unknown widget: %1").arg(arg);
                    break;
                }
            } else {
                error = QObject::tr("Missing widget!");
                break;
            }
        } else if (arg == QLatin1String("-register")) {
            if (CHECK_NEXT_ARG) {
                m_helpFile = getFileName(arguments.at(i));
                if (m_helpFile.isEmpty()) {
                    error = QObject::tr("The specified Qt help file does not exist!");
                    break;
                }
                m_register = Register;
            } else {
                error = QObject::tr("Missing help file!");
                break;
            }
        } else if (arg == QLatin1String("-unregister")) {
            if (CHECK_NEXT_ARG) {
                m_helpFile = getFileName(arguments.at(i));
                if (m_helpFile.isEmpty()) {
                    error = QObject::tr("The specified Qt help file does not exist!");
                    break;
                }
                m_register = Unregister;
            } else {
                error = QObject::tr("Missing help file!");
                break;
            }
        } else if (arg == QLatin1String("-quiet")) {
            continue;
        } else if (arg == QLatin1String("-help")) {
            showHelp = true;
        } else if (arg == QLatin1String("-copy")) {
            m_copy = true;
        } else {
            error = QObject::tr("Unknown option: %1").arg(arg);
            break;
        }
    }

    if (m_register != None && m_collectionFile.isEmpty())
        error = QObject::tr("Missing collection file!");

    if (!error.isEmpty()) {
        showMessage(error + QLatin1String("\n\n\n") + m_helpMessage, true);
        return Error;
    } else if (showHelp) {
        showMessage(m_helpMessage, false);
        return Help;
    }
    return Ok;
}

QString CmdLineParser::getFileName(const QString &fileName)
{
    QFileInfo fi(fileName);
    if (!fi.exists())
        return QString();
    return fi.absoluteFilePath();
}

void CmdLineParser::showMessage(const QString &msg, bool error)
{
    if (m_quiet)
        return;
#ifdef Q_OS_WIN
    QString s = QLatin1String("<pre>") + msg + QLatin1String("</pre>");
    if (error)
        QMessageBox::critical(0, QObject::tr("Qt Assistant"), s);
    else
        QMessageBox::information(0, QObject::tr("Qt Assistant"), s);
#else
    fprintf(error ? stderr : stdout, "%s\n", qPrintable(msg));    
#endif
}

void CmdLineParser::setCollectionFile(const QString &file)
{
    m_collectionFile = file;
}

QString CmdLineParser::collectionFile() const
{
    return m_collectionFile;
}

QUrl CmdLineParser::url() const
{
    return m_url;
}

bool CmdLineParser::enableRemoteControl() const
{
    return m_enableRemoteControl;
}

CmdLineParser::ShowState CmdLineParser::contents() const
{
    return m_contents;
}

CmdLineParser::ShowState CmdLineParser::index() const
{
    return m_index;
}

CmdLineParser::ShowState CmdLineParser::bookmarks() const
{
    return m_bookmarks;
}

CmdLineParser::ShowState CmdLineParser::search() const
{
    return m_search;
}

CmdLineParser::RegisterState CmdLineParser::registerRequest() const
{
    return m_register;
}

QString CmdLineParser::helpFile() const
{
    return m_helpFile;
}

QT_END_NAMESPACE
