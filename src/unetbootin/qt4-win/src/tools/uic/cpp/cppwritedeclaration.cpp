/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "cppwritedeclaration.h"
#include "cppwriteicondeclaration.h"
#include "cppwriteinitialization.h"
#include "cppwriteiconinitialization.h"
#include "cppextractimages.h"
#include "driver.h"
#include "ui4.h"
#include "uic.h"
#include "databaseinfo.h"
#include "customwidgetsinfo.h"

#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

namespace {
    void openNameSpaces(const QStringList &namespaceList, QTextStream &output) {
        if (namespaceList.empty())
            return;
        const QStringList::const_iterator cend = namespaceList.constEnd();
        for (QStringList::const_iterator it = namespaceList.constBegin(); it != cend; ++it) {
            if (!it->isEmpty()) {
                output << "namespace " << *it << " {\n";
            }
        }
    }

    void closeNameSpaces(const QStringList &namespaceList, QTextStream &output) {
        if (namespaceList.empty())
            return;

        QListIterator<QString> it(namespaceList);
        it.toBack();
        while (it.hasPrevious()) {
            const QString ns = it.previous();
            if (!ns.isEmpty()) {
                output << "} // namespace " << ns << "\n";
            }
        }
    }

    void writeScriptContextClass(const QString &indent, QTextStream &str) {
         str << indent << "class ScriptContext\n"
             << indent << "{\n"
             << indent << "public:\n"
             << indent << "    void run(const QString &script, QWidget *widget, const QWidgetList &childWidgets)\n"
             << indent << "    {\n"
             << indent << "        QScriptValue widgetObject =  scriptEngine.newQObject(widget);\n"
             << indent << "        QScriptValue childWidgetArray = scriptEngine.newArray (childWidgets.size());\n"
             << indent << "        for (int i = 0; i < childWidgets.size(); i++)\n"
             << indent << "               childWidgetArray.setProperty(i, scriptEngine.newQObject(childWidgets[i]));\n"
             << indent << "        QScriptContext *ctx = scriptEngine.pushContext();\n"
             << indent << "        ctx ->activationObject().setProperty(QLatin1String(\"widget\"), widgetObject);\n"
             << indent << "        ctx ->activationObject().setProperty(QLatin1String(\"childWidgets\"), childWidgetArray);\n\n"
             << indent << "        scriptEngine.evaluate(script);\n"
             << indent << "        if (scriptEngine.hasUncaughtException ()) {\n"
             << indent << "            qWarning() << \"An exception occurred at line \" << scriptEngine.uncaughtExceptionLineNumber()\n"
             << indent << "                       << \" of the script for \" << widget->objectName() << \": \" << engineError() << '\\n'\n"
             << indent << "                       << script;\n"
             << indent << "        }\n\n"
             << indent << "        scriptEngine.popContext();\n"
             << indent << "    }\n\n"
             << indent << "private:\n"
             << indent << "    QString engineError()\n"
             << indent << "    {\n"
             << indent << "        QScriptValue error = scriptEngine.evaluate(\"Error\");\n"
             << indent << "        return error.toString();\n"
             << indent << "    }\n\n"
             << indent << "    QScriptEngine scriptEngine;\n"
             << indent << "};\n\n";
    }
}

namespace CPP {

WriteDeclaration::WriteDeclaration(Uic *uic, bool activateScripts)  :
    m_uic(uic),
    m_driver(uic->driver()),
    m_output(uic->output()),
    m_option(uic->option()),
    m_activateScripts(activateScripts)
{
}

void WriteDeclaration::acceptUI(DomUI *node)
{
    QString qualifiedClassName = node->elementClass() + m_option.postfix;
    QString className = qualifiedClassName;

    QString varName = m_driver->findOrInsertWidget(node->elementWidget());
    QString widgetClassName = node->elementWidget()->attributeClass();

    QString exportMacro = node->elementExportMacro();
    if (!exportMacro.isEmpty())
        exportMacro.append(QLatin1Char(' '));

    QStringList namespaceList = qualifiedClassName.split(QLatin1String("::"));
    if (namespaceList.count()) {
        className = namespaceList.last();
        namespaceList.removeLast();
    }

    // This is a bit of the hack but covers the cases Qt in/without namespaces
    // and User defined classes in/without namespaces. The "strange" case
    // is a User using Qt-in-namespace having his own classes not in a namespace.
    // In this case the generated Ui helper classes will also end up in
    // the Qt namespace (which is harmless, but not "pretty")
    const bool needsMacro = namespaceList.count() == 0
        || namespaceList[0] == QLatin1String("qdesigner_internal");

    if (needsMacro)
        m_output << "QT_BEGIN_NAMESPACE\n\n";

    openNameSpaces(namespaceList, m_output);

    if (namespaceList.count())
        m_output << "\n";

    m_output << "class " << exportMacro << m_option.prefix << className << "\n"
           << "{\n"
           << "public:\n";

    const QStringList connections = m_uic->databaseInfo()->connections();
    for (int i=0; i<connections.size(); ++i) {
        const QString connection = connections.at(i);

        if (connection == QLatin1String("(default)"))
            continue;

        m_output << m_option.indent << "QSqlDatabase " << connection << "Connection;\n";
    }

    TreeWalker::acceptWidget(node->elementWidget());

    m_output << "\n";

    WriteInitialization(m_uic, m_activateScripts).acceptUI(node);

    if (node->elementImages()) {
        if (m_option.extractImages) {
            ExtractImages(m_uic->option()).acceptUI(node);
        } else {
            m_output << "\n"
                << "protected:\n"
                << m_option.indent << "enum IconID\n"
                << m_option.indent << "{\n";
            WriteIconDeclaration(m_uic).acceptUI(node);

            m_output << m_option.indent << m_option.indent << "unknown_ID\n"
                << m_option.indent << "};\n";

            WriteIconInitialization(m_uic).acceptUI(node);
        }
    }

    if (m_activateScripts) {
        m_output << "\nprivate:\n\n";
        writeScriptContextClass(m_option.indent, m_output);
    }

    m_output << "};\n\n";

    closeNameSpaces(namespaceList, m_output);

    if (namespaceList.count())
        m_output << "\n";

    if (m_option.generateNamespace && !m_option.prefix.isEmpty()) {
        namespaceList.append(QLatin1String("Ui"));

        openNameSpaces(namespaceList, m_output);

        m_output << m_option.indent << "class " << exportMacro << className << ": public " << m_option.prefix << className << " {};\n";

        closeNameSpaces(namespaceList, m_output);

        if (namespaceList.count())
            m_output << "\n";
    }

    if (needsMacro)
        m_output << "QT_END_NAMESPACE\n\n";
}

void WriteDeclaration::acceptWidget(DomWidget *node)
{
    QString className = QLatin1String("QWidget");
    if (node->hasAttributeClass())
        className = node->attributeClass();

    m_output << m_option.indent << m_uic->customWidgetsInfo()->realClassName(className) << " *" << m_driver->findOrInsertWidget(node) << ";\n";

    TreeWalker::acceptWidget(node);
}

void WriteDeclaration::acceptSpacer(DomSpacer *node)
{
     m_output << m_option.indent << "QSpacerItem *" << m_driver->findOrInsertSpacer(node) << ";\n";
     TreeWalker::acceptSpacer(node);
}

void WriteDeclaration::acceptLayout(DomLayout *node)
{
    QString className = QLatin1String("QLayout");
    if (node->hasAttributeClass())
        className = node->attributeClass();

    m_output << m_option.indent << className << " *" << m_driver->findOrInsertLayout(node) << ";\n";

    TreeWalker::acceptLayout(node);
}

void WriteDeclaration::acceptActionGroup(DomActionGroup *node)
{
    m_output << m_option.indent << "QActionGroup *" << m_driver->findOrInsertActionGroup(node) << ";\n";

    TreeWalker::acceptActionGroup(node);
}

void WriteDeclaration::acceptAction(DomAction *node)
{
    m_output << m_option.indent << "QAction *" << m_driver->findOrInsertAction(node) << ";\n";

    TreeWalker::acceptAction(node);
}

} // namespace CPP

QT_END_NAMESPACE
