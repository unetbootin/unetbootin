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

#include "cppwriteincludes.h"
#include "driver.h"
#include "ui4.h"
#include "uic.h"
#include "databaseinfo.h"
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

namespace {
    enum { debugWriteIncludes = 0 };
    enum { warnHeaderGeneration = 0 };
}

namespace CPP {

struct ClassInfoEntry
{
    const char *klass;
    const char *module;
    const char *header;
};

static const ClassInfoEntry qclass_lib_map[] = {
#define QT_CLASS_LIB(klass, module, header) { #klass, #module, #header },
#include "qclass_lib_map.h"

#undef QT_CLASS_LIB
    { 0, 0, 0 }
};

WriteIncludes::WriteIncludes(Uic *uic)    :
    m_uic(uic),
    m_output(uic->output()),
    m_scriptsActivated(false)
{
    for(const ClassInfoEntry *it = &qclass_lib_map[0]; it->klass != 0;  ++it) {
        QString newHeader = QLatin1String(it->module);
        newHeader += QLatin1Char('/');
        newHeader += QLatin1String(it->klass);
        m_classToHeader.insert(QLatin1String(it->klass),         newHeader);
        m_oldHeaderToNewHeader.insert(QLatin1String(it->header), newHeader);
    }
}

void WriteIncludes::acceptUI(DomUI *node)
{
    m_scriptsActivated = false;
    m_localIncludes.clear();
    m_globalIncludes.clear();
    m_knownClasses.clear();
    m_includeBaseNames.clear();

    if (node->elementIncludes())
        acceptIncludes(node->elementIncludes());

    if (node->elementCustomWidgets())
        TreeWalker::acceptCustomWidgets(node->elementCustomWidgets());

    add(QLatin1String("QApplication"));
    add(QLatin1String("QVariant"));
    add(QLatin1String("QAction"));

    add(QLatin1String("QButtonGroup")); // ### only if it is really necessary

    if (m_uic->hasExternalPixmap() && m_uic->pixmapFunction() == QLatin1String("qPixmapFromMimeSource"))
        add(QLatin1String("Q3MimeSourceFactory"));

    if (m_uic->databaseInfo()->connections().size()) {
        add(QLatin1String("QSqlDatabase"));
        add(QLatin1String("Q3SqlCursor"));
        add(QLatin1String("QSqlRecord"));
        add(QLatin1String("Q3SqlForm"));
    }

    TreeWalker::acceptUI(node);

    writeHeaders(m_globalIncludes, true);
    writeHeaders(m_localIncludes, false);

    m_output << QLatin1Char('\n');
}

void WriteIncludes::acceptWidget(DomWidget *node)
{
    add(node->attributeClass());
    TreeWalker::acceptWidget(node);
}

void WriteIncludes::acceptLayout(DomLayout *node)
{
    add(node->attributeClass());
    TreeWalker::acceptLayout(node);
}

void WriteIncludes::acceptSpacer(DomSpacer *node)
{
    add(QLatin1String("QSpacerItem"));
    TreeWalker::acceptSpacer(node);
}

void WriteIncludes::acceptProperty(DomProperty *node)
{
    if (node->kind() == DomProperty::Date)
        add(QLatin1String("QDate"));
    if (node->kind() == DomProperty::Locale)
        add(QLatin1String("QLocale"));
    TreeWalker::acceptProperty(node);
}

void WriteIncludes::insertIncludeForClass(const QString &className, QString header, bool global)
{
    if (debugWriteIncludes)
        qDebug() << "WriteIncludes::insertIncludeForClass" << className << header  << global;

    do {
        if (!header.isEmpty())
            break;

        // Known class
        const StringMap::const_iterator it = m_classToHeader.constFind(className);
        if (it != m_classToHeader.constEnd()) {
            header = it.value();
            global =  true;
            break;
        }

        // Quick check by class name to detect includehints provided for custom widgets.
        // Remove namespaces
        QString lowerClassName = className.toLower();
        static const QString namespaceSeparator = QLatin1String("::");
        const int namespaceIndex = lowerClassName.lastIndexOf(namespaceSeparator);
        if (namespaceIndex != -1)
            lowerClassName.remove(0, namespaceIndex + namespaceSeparator.size());
        if (m_includeBaseNames.contains(lowerClassName)) {
            header.clear();
            break;
        }

        // Last resort: Create default header
        if (!m_uic->option().implicitIncludes)
            break;
        header = lowerClassName;
        header += QLatin1String(".h");
        if (warnHeaderGeneration) {
            const QString msg =  QString::fromUtf8("Warning: generated header '%1' for class '%2'.").arg(header).arg(className);
            qWarning(msg.toUtf8().constData());
        }

        global = true;
    } while (false);

    if (!header.isEmpty())
        insertInclude(header, global);
}

void WriteIncludes::add(const QString &className, bool determineHeader, const QString &header, bool global)
{
    if (debugWriteIncludes)
        qDebug() << "WriteIncludes::add" << className << header  << global;

    if (className.isEmpty() || m_knownClasses.contains(className))
        return;

    m_knownClasses.insert(className);

    if (className == QLatin1String("Line")) { // ### hmm, deprecate me!
        add(QLatin1String("QFrame"));
        return;
    }

    if (m_uic->customWidgetsInfo()->extends(className, QLatin1String("Q3ListView"))  ||
        m_uic->customWidgetsInfo()->extends(className, QLatin1String("Q3Table"))) {
        add(QLatin1String("Q3Header"));
    }
    if (determineHeader)
        insertIncludeForClass(className, header, global);
}

void WriteIncludes::acceptCustomWidget(DomCustomWidget *node)
{
    const QString className = node->elementClass();
    if (className.isEmpty())
        return;

    if (const DomScript *domScript = node->elementScript())
        if (!domScript->text().isEmpty())
            activateScripts();

    if (!node->elementHeader() || node->elementHeader()->text().isEmpty()) {
        add(className, false); // no header specified
    } else {
        // custom header unless it is a built-in qt class
        QString header;
        bool global = false;
        if (!m_classToHeader.contains(className)) {
            global = node->elementHeader()->attributeLocation().toLower() == QLatin1String("global");
            header = node->elementHeader()->text();
        }
        add(className, true, header, global);
    }
}

void WriteIncludes::acceptCustomWidgets(DomCustomWidgets *node)
{
    Q_UNUSED(node);
}

void WriteIncludes::acceptIncludes(DomIncludes *node)
{
    TreeWalker::acceptIncludes(node);
}

void WriteIncludes::acceptInclude(DomInclude *node)
{
    bool global = true;
    if (node->hasAttributeLocation())
        global = node->attributeLocation() == QLatin1String("global");
    insertInclude(node->text(), global);
}
void WriteIncludes::insertInclude(const QString &header, bool global)
{
    if (debugWriteIncludes)
        qDebug() << "WriteIncludes::insertInclude" <<  header  << global;

    OrderedSet &includes = global ?  m_globalIncludes : m_localIncludes;
    if (includes.contains(header))
        return;
    // Insert. Also remember base name for quick check of suspicious custom plugins
    includes.insert(header, false);
    const QString lowerBaseName = QFileInfo(header).completeBaseName ().toLower();
    m_includeBaseNames.insert(lowerBaseName);
}

void WriteIncludes::writeHeaders(const OrderedSet &headers, bool global)
{
    const QChar openingQuote = global ? QLatin1Char('<') : QLatin1Char('"');
    const QChar closingQuote = global ? QLatin1Char('>') : QLatin1Char('"');

    const OrderedSet::const_iterator cend = headers.constEnd();
    for ( OrderedSet::const_iterator sit = headers.constBegin(); sit != cend; ++sit) {
        const StringMap::const_iterator hit = m_oldHeaderToNewHeader.constFind(sit.key());
        const bool mapped =  hit != m_oldHeaderToNewHeader.constEnd();
        const  QString header =  mapped ? hit.value() : sit.key();
        if (!header.trimmed().isEmpty()) {
            m_output << "#include " << openingQuote << header << closingQuote << QLatin1Char('\n');
        }
    }
}

void WriteIncludes::acceptWidgetScripts(const DomScripts &scripts, DomWidget *, const  DomWidgets &)
{
    if (!scripts.empty()) {
        activateScripts();
    }
}

void WriteIncludes::activateScripts()
{
    if (!m_scriptsActivated) {
        add(QLatin1String("QScriptEngine"));
        add(QLatin1String("QDebug"));
        m_scriptsActivated = true;
    }
}
} // namespace CPP

QT_END_NAMESPACE
