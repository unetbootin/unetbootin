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

/*
  htmlgenerator.cpp
*/

#include "codemarker.h"
#include "helpprojectwriter.h"
#include "htmlgenerator.h"
#include "node.h"
#include "separator.h"
#include "tree.h"
#include <ctype.h>

#include <qdebug.h>
#include <qlist.h>
#include <qiterator.h>

QT_BEGIN_NAMESPACE

#define COMMAND_VERSION                 Doc::alias("version")

static bool showBrokenLinks = false;

HtmlGenerator::HtmlGenerator()
    : helpProjectWriter(0), inLink(false), inContents(false),
      inSectionHeading(false), inTableHeader(false), numTableRows(0),
      threeColumnEnumValueTable(true), funcLeftParen("\\S(\\()"),
      tre(0), slow(false)
{
}

HtmlGenerator::~HtmlGenerator()
{
    if (helpProjectWriter)
        delete helpProjectWriter;
}

void HtmlGenerator::initializeGenerator(const Config &config)
{
    static const struct {
        const char *key;
        const char *left;
        const char *right;
    } defaults[] = {
        { ATOM_FORMATTING_BOLD, "<b>", "</b>" },
        { ATOM_FORMATTING_INDEX, "<!--", "-->" },
        { ATOM_FORMATTING_ITALIC, "<i>", "</i>" },
        { ATOM_FORMATTING_PARAMETER, "<i>", "</i>" },
        { ATOM_FORMATTING_SUBSCRIPT, "<sub>", "</sub>" },
        { ATOM_FORMATTING_SUPERSCRIPT, "<sup>", "</sup>" },
        { ATOM_FORMATTING_TELETYPE, "<tt>", "</tt>" },
        { ATOM_FORMATTING_UNDERLINE, "<u>", "</u>" },
        { 0, 0, 0 }
    };

    Generator::initializeGenerator( config );
    setImageFileExtensions(QStringList() << "png" << "jpg" << "jpeg" << "gif");
    int i = 0;
    while (defaults[i].key) {
        formattingLeftMap().insert(defaults[i].key, defaults[i].left);
        formattingRightMap().insert(defaults[i].key, defaults[i].right);
        i++;
    }

    style = config.getString(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_STYLE);
    postHeader = config.getString(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_POSTHEADER);
    footer = config.getString(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_FOOTER);
    address = config.getString(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_ADDRESS);
    pleaseGenerateMacRef = config.getBool(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_GENERATEMACREFS);

    project = config.getString(CONFIG_PROJECT);

    projectDescription = config.getString(CONFIG_DESCRIPTION);
    if (projectDescription.isEmpty() && !project.isEmpty())
        projectDescription = project + " Reference Documentation";

    projectUrl = config.getString(CONFIG_URL);

    QSet<QString> editionNames = config.subVars(CONFIG_EDITION);
    QSet<QString>::ConstIterator edition = editionNames.begin();
    while (edition != editionNames.end()) {
        QString editionName = *edition;
        QStringList editionModules = config.getStringList(
            CONFIG_EDITION + Config::dot + editionName + Config::dot + "modules");
        QStringList editionGroups = config.getStringList(
            CONFIG_EDITION + Config::dot + editionName + Config::dot + "groups");

        if (!editionModules.isEmpty())
            editionModuleMap[editionName] = editionModules;
        if (!editionGroups.isEmpty())
            editionGroupMap[editionName] = editionGroups;

        ++edition;
    }

    slow = config.getBool(CONFIG_SLOW);

    stylesheets = config.getStringList(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_STYLESHEETS);
    customHeadElements = config.getStringList(HtmlGenerator::format() + Config::dot + HTMLGENERATOR_CUSTOMHEADELEMENTS);
    codeIndent = config.getInt(CONFIG_CODEINDENT);

    helpProjectWriter = new HelpProjectWriter(config, project.toLower() + ".qhp");
}

void HtmlGenerator::terminateGenerator()
{
    Generator::terminateGenerator();
}

QString HtmlGenerator::format()
{
    return "HTML";
}

void HtmlGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    // Copy the stylesheets from the directory containing the qdocconf file.
    // ### This should be changed to use a special directory in doc/src.
    QStringList::ConstIterator styleIter = stylesheets.begin();
    QDir configPath = QDir::current();
    while (styleIter != stylesheets.end()) {
        QString filePath = configPath.absoluteFilePath(*styleIter);
        Config::copyFile(Location(), filePath, filePath, outputDir());
        ++styleIter;
    }

    tre = tree;
    nonCompatClasses.clear();
    mainClasses.clear();
    compatClasses.clear();
    moduleClassMap.clear();
    moduleNamespaceMap.clear();
    funcIndex.clear();
    legaleseTexts.clear();
    serviceClasses.clear();
    findAllClasses(tree->root());
    findAllFunctions(tree->root());
    findAllLegaleseTexts(tree->root());
    findAllNamespaces(tree->root());

    PageGenerator::generateTree(tree, marker);

    dcfClassesRoot.ref = "classes.html";
    dcfClassesRoot.title = "Classes";
    qSort(dcfClassesRoot.subsections);

    dcfOverviewsRoot.ref = "overviews.html";
    dcfOverviewsRoot.title = "Overviews";
    qSort(dcfOverviewsRoot.subsections);

    dcfExamplesRoot.ref = "examples.html";
    dcfExamplesRoot.title = "Tutorial & Examples";
    qSort(dcfExamplesRoot.subsections);

    DcfSection qtRoot;
    appendDcfSubSection(&qtRoot, dcfClassesRoot);
    appendDcfSubSection(&qtRoot, dcfOverviewsRoot);
    appendDcfSubSection(&qtRoot, dcfExamplesRoot);

    generateDcf(project.toLower().simplified().replace(" ", "-"), "index.html", projectDescription, qtRoot);
    generateDcf("designer", "designer-manual.html", "Qt Designer Manual", dcfDesignerRoot);
    generateDcf("linguist", "linguist-manual.html", "Qt Linguist Manual", dcfLinguistRoot);
    generateDcf("assistant", "assistant-manual.html", "Qt Assistant Manual", dcfAssistantRoot);
    generateDcf("qmake", "qmake-manual.html", "qmake Manual", dcfQmakeRoot);

    generateIndex(project.toLower().simplified().replace(" ", "-"), projectUrl, projectDescription);

    helpProjectWriter->generate(tre);
}

void HtmlGenerator::startText(const Node * /* relative */, CodeMarker * /* marker */)
{
    inLink = false;
    inContents = false;
    inSectionHeading = false;
    inTableHeader = false;
    numTableRows = 0;
    threeColumnEnumValueTable = true;
    link.clear();
    sectionNumber.clear();
}

int HtmlGenerator::generateAtom(const Atom *atom, const Node *relative, CodeMarker *marker)
{
    int skipAhead = 0;
    static bool in_para = false;

    switch (atom->type()) {
    case Atom::AbstractLeft:
        break;
    case Atom::AbstractRight:
        break;
    case Atom::AutoLink:
        if (!inLink && !inContents && !inSectionHeading) {
            const Node *node = 0;
            QString link = getLink(atom, relative, marker, node);
            if (!link.isEmpty()) {
                beginLink(link, node, relative, marker);
                generateLink(atom, relative, marker);
                endLink();
            } else {
                out() << protect(atom->string());
            }
        } else {
            out() << protect(atom->string());
        }
        break;
    case Atom::BaseName:
        break;
    case Atom::BriefLeft:
        if (relative->type() == Node::Fake) {
            skipAhead = skipAtoms(atom, Atom::BriefRight);
            break;
        }

        out() << "<p>";
        if ( relative->type() == Node::Property || relative->type() == Node::Variable ) {
            QString str;
            atom = atom->next();
            while ( atom != 0 && atom->type() != Atom::BriefRight ) {
                if ( atom->type() == Atom::String || atom->type() == Atom::AutoLink )
                    str += atom->string();
                skipAhead++;
                atom = atom->next();
            }
            str[0] = str[0].toLower();
            if ( str.right(1) == "." )
                str.truncate( str.length() - 1 );
            out() << "This ";
            if (relative->type() == Node::Property)
                out() << "property";
            else
                out() << "variable";
            QStringList words = str.split(" ");
            if (!(words.first() == "contains" || words.first() == "specifies"
                || words.first() == "describes" || words.first() == "defines"
                || words.first() == "holds" || words.first() == "determines"))
                out() << " holds ";
            else
                out() << " ";
            out() << str << ".";
        }
        break;
    case Atom::BriefRight:
        if (relative->type() != Node::Fake)
            out() << "</p>\n";
        break;
    case Atom::C:
        out() << formattingLeftMap()[ATOM_FORMATTING_TELETYPE];
        if ( inLink ) {
            out() << protect(plainCode(atom->string()));
        } else {
            out() << highlightedCode(atom->string(), marker, relative);
        }
        out() << formattingRightMap()[ATOM_FORMATTING_TELETYPE];
        break;
    case Atom::Code:
	out() << "<pre>" << trimmedTrailing(highlightedCode(indent(codeIndent, atom->string()),
                                                            marker, relative))
              << "</pre>\n";
	break;
    case Atom::CodeNew:
        out() << "<p>you can rewrite it as</p>\n"
              << "<pre>" << trimmedTrailing(highlightedCode(indent(codeIndent, atom->string()),
                                                            marker, relative))
              << "</pre>\n";
        break;
    case Atom::CodeOld:
        out() << "<p>For example, if you have code like</p>\n";
        // fallthrough
    case Atom::CodeBad:
        out() << "<pre><font color=\"#404040\">"
              << trimmedTrailing(protect(plainCode(indent(codeIndent, atom->string()))))
              << "</font></pre>\n";
	break;
    case Atom::FootnoteLeft:
        // ### For now
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        out() << "<!-- ";
        break;
    case Atom::FootnoteRight:
        // ### For now
        out() << "-->";
        break;
    case Atom::FormatElse:
    case Atom::FormatEndif:
    case Atom::FormatIf:
        break;
    case Atom::FormattingLeft:
        out() << formattingLeftMap()[atom->string()];
        if ( atom->string() == ATOM_FORMATTING_PARAMETER ) {
            if ( atom->next() != 0 && atom->next()->type() == Atom::String ) {
                QRegExp subscriptRegExp( "([a-z]+)_([0-9n])" );
                if ( subscriptRegExp.exactMatch(atom->next()->string()) ) {
                    out() << subscriptRegExp.cap( 1 ) << "<sub>"
                          << subscriptRegExp.cap( 2 ) << "</sub>";
                    skipAhead = 1;
                }
            }
        }
        break;
    case Atom::FormattingRight:
        if ( atom->string() == ATOM_FORMATTING_LINK ) {
            endLink();
        } else {
            out() << formattingRightMap()[atom->string()];
        }
        break;
    case Atom::GeneratedList:
        if (atom->string() == "annotatedclasses") {
            generateAnnotatedList(relative, marker, nonCompatClasses);
        } else if (atom->string() == "classes") {
            generateCompactList(relative, marker, nonCompatClasses);
        } else if (atom->string().contains("classesbymodule")) {
            QString arg = atom->string().trimmed();
            QString moduleName = atom->string().mid(atom->string().indexOf(
                "classesbymodule") + 15).trimmed();
            if (moduleClassMap.contains(moduleName))
                generateAnnotatedList(relative, marker, moduleClassMap[moduleName]);
        } else if (atom->string().contains("classesbyedition")) {

            QString arg = atom->string().trimmed();
            QString editionName = atom->string().mid(atom->string().indexOf(
                "classesbyedition") + 16).trimmed();

            if (editionModuleMap.contains(editionName)) {

                // Add all classes in the modules listed for that edition.
                QMap<QString, const Node *> editionClasses;
                foreach (const QString &moduleName, editionModuleMap[editionName]) {
                    if (moduleClassMap.contains(moduleName))
                        editionClasses.unite(moduleClassMap[moduleName]);
                }

                // Add additional groups and remove groups of classes that
                // should be excluded from the edition.

                QMultiMap <QString, Node *> groups = tre->groups();
                foreach (const QString &groupName, editionGroupMap[editionName]) {
                    QList<Node *> groupClasses;
                    if (groupName.startsWith("-")) {
                        groupClasses = groups.values(groupName.mid(1));
                        foreach (const Node *node, groupClasses)
                            editionClasses.remove(node->name());
                    } else {
                        groupClasses = groups.values(groupName);
                        foreach (const Node *node, groupClasses)
                            editionClasses.insert(node->name(), node);
                    }
                }
                generateAnnotatedList(relative, marker, editionClasses);
            }
        } else if (atom->string() == "classhierarchy") {
            generateClassHierarchy(relative, marker, nonCompatClasses);
        } else if (atom->string() == "compatclasses") {
            generateCompactList(relative, marker, compatClasses);
        } else if (atom->string() == "functionindex") {
            generateFunctionIndex(relative, marker);
        } else if (atom->string() == "legalese") {
            generateLegaleseList(relative, marker);
        } else if (atom->string() == "mainclasses") {
            generateCompactList(relative, marker, mainClasses);
        } else if (atom->string() == "services") {
            generateCompactList(relative, marker, serviceClasses);
        } else if (atom->string() == "overviews") {
            generateOverviewList(relative, marker);
        } else if (atom->string() == "namespaces") {
            generateAnnotatedList(relative, marker, namespaceIndex);
        } else if (atom->string() == "related") {
            const FakeNode *fake = static_cast<const FakeNode *>(relative);
            if (fake && !fake->groupMembers().isEmpty()) {
                QMap<QString, const Node *> groupMembersMap;
                foreach (const Node *node, fake->groupMembers()) {
                    if (node->type() == Node::Fake)
                        groupMembersMap[fullName(node, relative, marker)] = node;
                }
                generateAnnotatedList(fake, marker, groupMembersMap);
            }
        } else if (atom->string() == "relatedinline") {
            const FakeNode *fake = static_cast<const FakeNode *>(relative);
            if (fake && !fake->groupMembers().isEmpty()) {
                // Reverse the list into the original scan order.
                // Should be sorted.  But on what?  It may not be a
                // regular class or page definition.
                QList<const Node *> list;
                foreach (const Node *node, fake->groupMembers())
                    list.prepend(node);
                foreach (const Node *node, list)
                    generateBody(node, marker );
            }
        }
        break;
    case Atom::Image:
    case Atom::InlineImage:
        {
            QString fileName = imageFileName(relative, atom->string());
            QString text;
            if ( atom->next() != 0 )
                text = atom->next()->string();
            if (atom->type() == Atom::Image)
                out() << "<p align=\"center\">";
            if ( fileName.isEmpty() ) {
                out() << "<font color=\"red\">[Missing image "
                      << protect( atom->string() ) << "]</font>";
            } else {
                out() << "<img src=\"" << protect( fileName ) << "\"";
                if ( !text.isEmpty() )
                    out() << " alt=\"" << protect( text ) << "\"";
                out() << " />";
                helpProjectWriter->addExtraFile(fileName);
            }
            if (atom->type() == Atom::Image)
                out() << "</p>";
        }
        break;
    case Atom::ImageText:
        break;
    case Atom::LegaleseLeft:
        out() << "<div style=\"padding: 0.5em; background: #e0e0e0; color: black\">";
        break;
    case Atom::LegaleseRight:
        out() << "</div>";
        break;
    case Atom::LineBreak:
        out() << "<br />";
        break;
    case Atom::Link:
        {
            const Node *node = 0;
            QString myLink = getLink(atom, relative, marker, node);
            if (myLink.isEmpty())
                relative->doc().location().warning(tr("Cannot link to '%1' in %2")
                        .arg(atom->string())
                        .arg(marker->plainFullName(relative)));
            beginLink(myLink, node, relative, marker);
            skipAhead = 1;
        }
        break;
    case Atom::LinkNode:
        {
            const Node *node = CodeMarker::nodeForString(atom->string());
            beginLink(linkForNode(node, relative), node, relative, marker);
            skipAhead = 1;
        }
        break;
    case Atom::ListLeft:
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        if ( atom->string() == ATOM_LIST_BULLET ) {
            out() << "<ul>\n";
        } else if ( atom->string() == ATOM_LIST_TAG ) {
            out() << "<dl>\n";
        } else if ( atom->string() == ATOM_LIST_VALUE ) {
            threeColumnEnumValueTable = isThreeColumnEnumValueTable(atom);
            if (threeColumnEnumValueTable) {
                out() << "<p><table border=\"1\" cellpadding=\"2\" cellspacing=\"1\" width=\"100%\">\n"
                         "<tr><th width=\"25%\">Constant</th><th width=\"15%\">Value</th>"
                         "<th width=\"60%\">Description</th></tr>\n";
            } else {
                out() << "<p><table border=\"1\" cellpadding=\"2\" cellspacing=\"1\" width=\"40%\">\n"
                      << "<tr><th width=\"60%\">Constant</th><th width=\"40%\">Value</th></tr>\n";
            }
        } else {
            out() << "<ol type=";
            if ( atom->string() == ATOM_LIST_UPPERALPHA ) {
                out() << "\"A\"";
            } else if ( atom->string() == ATOM_LIST_LOWERALPHA ) {
                out() << "\"a\"";
            } else if ( atom->string() == ATOM_LIST_UPPERROMAN ) {
                out() << "\"I\"";
            } else if ( atom->string() == ATOM_LIST_LOWERROMAN ) {
                out() << "\"i\"";
            } else { // ( atom->string() == ATOM_LIST_NUMERIC )
                out() << "\"1\"";
            }
            if ( atom->next() != 0 && atom->next()->string().toInt() != 1 )
                out() << " start=\"" << atom->next()->string() << "\"";
            out() << ">\n";
        }
        break;
    case Atom::ListItemNumber:
        break;
    case Atom::ListTagLeft:
        if ( atom->string() == ATOM_LIST_TAG ) {
            out() << "<dt>";
        } else { // ( atom->string() == ATOM_LIST_VALUE )
            // ### Trenton

            out() << "<tr><td valign=\"top\"><tt>"
                  << protect(plainCode(marker->markedUpEnumValue(atom->next()->string(),
                                                                 relative)))
                  << "</tt></td><td align=\"center\" valign=\"top\">";

            QString itemValue;
            if (relative->type() == Node::Enum) {
                const EnumNode *enume = static_cast<const EnumNode *>(relative);
                itemValue = enume->itemValue(atom->next()->string());
            }

            if (itemValue.isEmpty())
                out() << "?";
            else
                out() << "<tt>" << protect(itemValue) << "</tt>";

            skipAhead = 1;
        }
        break;
    case Atom::ListTagRight:
        if ( atom->string() == ATOM_LIST_TAG )
            out() << "</dt>\n";
        break;
    case Atom::ListItemLeft:
        if ( atom->string() == ATOM_LIST_TAG ) {
            out() << "<dd>";
        } else if ( atom->string() == ATOM_LIST_VALUE ) {
            if (threeColumnEnumValueTable) {
                out() << "</td><td valign=\"top\">";
                if ( matchAhead(atom, Atom::ListItemRight) )
                    out() << "&nbsp;";
            }
        } else {
            out() << "<li>";
        }
        if ( matchAhead(atom, Atom::ParaLeft) )
            skipAhead = 1;
        break;
    case Atom::ListItemRight:
        if ( atom->string() == ATOM_LIST_TAG ) {
            out() << "</dd>\n";
        } else if ( atom->string() == ATOM_LIST_VALUE ) {
            out() << "</td></tr>\n";
        } else {
            out() << "</li>\n";
        }
        break;
    case Atom::ListRight:
        if ( atom->string() == ATOM_LIST_BULLET ) {
            out() << "</ul>\n";
        } else if ( atom->string() == ATOM_LIST_TAG ) {
            out() << "</dl>\n";
        } else if ( atom->string() == ATOM_LIST_VALUE ) {
            out() << "</table></p>\n";
        } else {
            out() << "</ol>\n";
        }
        break;
    case Atom::Nop:
        break;
    case Atom::ParaLeft:
        out() << "<p>";
        in_para = true;
        break;
    case Atom::ParaRight:
        endLink();
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        //if (!matchAhead(atom, Atom::ListItemRight) && !matchAhead(atom, Atom::TableItemRight))
        //    out() << "</p>\n";
        break;
    case Atom::QuotationLeft:
        out() << "<blockquote>";
        break;
    case Atom::QuotationRight:
        out() << "</blockquote>\n";
        break;
    case Atom::RawString:
        out() << atom->string();
        break;
    case Atom::SectionLeft:
#if 0
        {
            int nextLevel = atom->string().toInt();
            if (sectionNumber.size() < nextLevel) {
                do {
                    sectionNumber.append("1");
                } while (sectionNumber.size() < nextLevel);
            } else {
                while (sectionNumber.size() > nextLevel) {
                    sectionNumber.removeLast();
                }
                sectionNumber.last() = QString::number(sectionNumber.last().toInt() + 1);
            }
            out() << "<a name=\"sec-" << sectionNumber.join("-") << "\"></a>\n";
        }
#else
        out() << "<a name=\"" << Doc::canonicalTitle(Text::sectionHeading(atom).toString())
              << "\"></a>\n";
#endif
        break;
    case Atom::SectionRight:
        break;
    case Atom::SectionHeadingLeft:
        out() << "<h" + QString::number(atom->string().toInt() + hOffset(relative)) + ">";
        inSectionHeading = true;
        break;
    case Atom::SectionHeadingRight:
        out() << "</h" + QString::number(atom->string().toInt() + hOffset(relative)) + ">\n";
        inSectionHeading = false;
        break;
    case Atom::SidebarLeft:
        break;
    case Atom::SidebarRight:
        break;
    case Atom::String:
        if (inLink && !inContents && !inSectionHeading) {
            generateLink(atom, relative, marker);
        } else {
            out() << protect( atom->string() );
        }
        break;
    case Atom::TableLeft:
        if (in_para) {
            out() << "</p>\n";
            in_para = false;
        }
        if (!atom->string().isEmpty()) {
            if (atom->string().contains("%"))
                out() << "<p><table width=\"" << atom->string() << "\" "
                      << "align=\"center\" cellpadding=\"2\" "
                      << "cellspacing=\"1\" border=\"0\">\n";
            else
                out() << "<p><table align=\"center\" cellpadding=\"2\" cellspacing=\"1\" border=\"0\">\n";
        } else {
            out() << "<p><table align=\"center\" cellpadding=\"2\" cellspacing=\"1\" border=\"0\">\n";
        }
        numTableRows = 0;
        break;
    case Atom::TableRight:
        out() << "</table></p>\n";
        break;
    case Atom::TableHeaderLeft:
        out() << "<thead><tr valign=\"top\" class=\"qt-style\">";
        inTableHeader = true;
        break;
    case Atom::TableHeaderRight:
        out() << "</tr>";
        if (matchAhead(atom, Atom::TableHeaderLeft)) {
            skipAhead = 1;
            out() << "\n<tr valign=\"top\" class=\"qt-style\">";
        } else {
            out() << "</thead>\n";
            inTableHeader = false;
        }
        break;
    case Atom::TableRowLeft:
        if (++numTableRows % 2 == 1)
            out() << "<tr valign=\"top\" class=\"odd\">";
        else
            out() << "<tr valign=\"top\" class=\"even\">";
        break;
    case Atom::TableRowRight:
        out() << "</tr>\n";
        break;
    case Atom::TableItemLeft:
        {
            if (inTableHeader)
                out() << "<th";
            else
                out() << "<td";

            QStringList spans = atom->string().split(",");
            if (spans.size() == 2) {
                if (spans.at(0) != "1")
                    out() << " colspan=\"" << spans.at(0) << "\"";
                if (spans.at(1) != "1")
                    out() << " rowspan=\"" << spans.at(1) << "\"";
                out() << ">";
            }
            if ( matchAhead(atom, Atom::ParaLeft) )
                skipAhead = 1;
        }
        break;
    case Atom::TableItemRight:
        if (inTableHeader)
            out() << "</th>";
        else
            out() << "</td>";
        if ( matchAhead(atom, Atom::ParaLeft) )
            skipAhead = 1;
        break;
    case Atom::TableOfContents:
        {
            int numColumns = 1;
            const Node *node = relative;

            Doc::SectioningUnit sectioningUnit = Doc::Section4;
            QStringList params = atom->string().split(",");
            QString columnText = params.at(0);
            QStringList pieces = columnText.split(" ", QString::SkipEmptyParts);
            if (pieces.size() >= 2) {
                columnText = pieces.at(0);
                pieces.pop_front();
                QString path = pieces.join(" ").trimmed();
                node = findNodeForTarget(path, relative, marker, atom);
            }

            if (params.size() == 2) {
                numColumns = qMax(columnText.toInt(), numColumns);
                sectioningUnit = (Doc::SectioningUnit)params.at(1).toInt();
            }

            if (node)
                generateTableOfContents(node, marker, sectioningUnit, numColumns,
                                        relative);
        }
        break;
    case Atom::Target:
        out() << "<a name=\"" << Doc::canonicalTitle(atom->string()) << "\"></a>";
        break;
    case Atom::UnhandledFormat:
        out() << "<font color=\"red\"><b>&lt;Missing HTML&gt;</b></font>";
        break;
    case Atom::UnknownCommand:
        out() << "<font color=\"red\"><b><code>\\" << protect( atom->string() )
              << "</code></b></font>";
        break;
    default:
        unknownAtom( atom );
    }
    return skipAhead;
}

void HtmlGenerator::generateClassLikeNode(const InnerNode *inner, CodeMarker *marker)
{
    QList<Section> sections;
    QList<Section>::ConstIterator s;

    const ClassNode *classe = 0;
    const NamespaceNode *namespasse = 0;

    QString title;
    QString rawTitle;
    QString fullTitle;
    if (inner->type() == Node::Namespace) {
        namespasse = static_cast<const NamespaceNode *>(inner);
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Namespace Reference";
    } else if (inner->type() == Node::Class) {
        classe = static_cast<const ClassNode *>(inner);
        rawTitle = marker->plainName(inner);
        fullTitle = marker->plainFullName(inner);
        title = rawTitle + " Class Reference";
    }

    DcfSection classSection;
    classSection.title = title;
    classSection.ref = linkForNode(inner, 0);
    classSection.keywords += qMakePair(inner->name(), classSection.ref);

    Text subtitleText;
    if (rawTitle != fullTitle)
        subtitleText << "(" << Atom(Atom::AutoLink, fullTitle) << ")"
                     << Atom(Atom::LineBreak);

    QString fixedModule = inner->moduleName();
    if (fixedModule == "Qt3SupportLight")
        fixedModule = "Qt3Support";
    if (!fixedModule.isEmpty())
        subtitleText << "[" << Atom(Atom::AutoLink, fixedModule) << " module]";

    if (fixedModule.isEmpty()) {
        QMultiMap<QString, QString> publicGroups = tre->publicGroups();
        QList<QString> groupNames = publicGroups.values(inner->name());
        if (!groupNames.isEmpty()) {
            qSort(groupNames.begin(), groupNames.end());
            subtitleText << "[";
            for (int j=0; j<groupNames.count(); j++) {
                subtitleText <<  Atom(Atom::AutoLink, groupNames[j]);
                if (j<groupNames.count()-1)
                    subtitleText <<", ";
            }
            subtitleText << "]";
        }
    }

    generateHeader(title, inner, marker, true);
    generateTitle(title, subtitleText, SmallSubTitle, inner, marker);

    generateBrief(inner, marker);
    generateIncludes(inner, marker);
    generateStatus(inner, marker);
    if (classe) {
        generateModuleWarning(classe, marker);
        generateInherits(classe, marker);
        generateInheritedBy(classe, marker);
    }
    generateThreadSafeness(inner, marker);
    generateSince(inner, marker);

    out() << "<ul>\n";

    QString membersLink = generateListOfAllMemberFile(inner, marker);
    if (!membersLink.isEmpty())
        out() << "<li><a href=\"" << membersLink << "\">"
              << "List of all members, including inherited members</a></li>\n";

    QString obsoleteLink = generateLowStatusMemberFile(inner, marker, CodeMarker::Obsolete);
    if (!obsoleteLink.isEmpty())
        out() << "<li><a href=\"" << obsoleteLink << "\">"
              << "Obsolete members</a></li>\n";

    QString compatLink = generateLowStatusMemberFile(inner, marker, CodeMarker::Compat);
    if (!compatLink.isEmpty())
        out() << "<li><a href=\"" << compatLink << "\">"
              << "Qt 3 support members</a></li>\n";

    out() << "</ul>\n";

    bool needOtherSection = false;

    sections = marker->sections(inner, CodeMarker::Summary, CodeMarker::Okay);
    s = sections.begin();
    while ( s != sections.end() ) {
        if (s->members.isEmpty()) {
            if (!s->inherited.isEmpty())
                needOtherSection = true;
        } else {
            out() << "<a name=\"" << registerRef((*s).name.toLower()) << "\"></a>\n";
            out() << "<h3>" << protect((*s).name) << "</h3>\n";

            generateSectionList(*s, inner, marker, CodeMarker::Summary);
        }
        ++s;
    }

    if (needOtherSection) {
        out() << "<h3>Additional Inherited Members</h3>\n"
                 "<ul>\n";

        s = sections.begin();
        while ( s != sections.end() ) {
            if (s->members.isEmpty() && !s->inherited.isEmpty())
                generateSectionInheritedList(*s, inner, marker);
            ++s;
        }
        out() << "</ul>\n";
    }

    out() << "<a name=\"" << registerRef( "details" ) << "\"></a>\n";

    if ( !inner->doc().isEmpty() ) {
        out() << "<hr />\n"
              << "<h2>" << "Detailed Description" << "</h2>\n";
        generateBody( inner, marker );
        generateAlsoList( inner, marker );
    }

    sections = marker->sections(inner, CodeMarker::Detailed, CodeMarker::Okay);
    s = sections.begin();
    while ( s != sections.end() ) {
        out() << "<hr />\n";
        out() << "<h2>" << protect( (*s).name ) << "</h2>\n";

        NodeList::ConstIterator m = (*s).members.begin();
        while ( m != (*s).members.end() ) {
            if ( (*m)->access() != Node::Private ) { // ### check necessary?
                if ((*m)->type() != Node::Class)
                    generateDetailedMember(*m, inner, marker);
                else {
                    out() << "<h3> class ";
                    generateFullName(*m, inner, marker);
                    out() << "</h3>";
                    generateBrief(*m, marker, inner);
                }

                QStringList names;
                names << (*m)->name();
                if ((*m)->type() == Node::Function) {
                    const FunctionNode *func = reinterpret_cast<const FunctionNode *>(*m);
                    if (func->metaness() == FunctionNode::Ctor || func->metaness() == FunctionNode::Dtor
                            || func->overloadNumber() != 1)
                        names.clear();
                } else if ((*m)->type() == Node::Property) {
                    const PropertyNode *prop = reinterpret_cast<const PropertyNode *>(*m);
                    if (!prop->getters().isEmpty() && !names.contains(prop->getters().first()->name()))
                        names << prop->getters().first()->name();
                    if (!prop->setters().isEmpty())
                        names << prop->setters().first()->name();
                    if (!prop->resetters().isEmpty())
                        names << prop->resetters().first()->name();
                } else if ((*m)->type() == Node::Enum) {
                    const EnumNode *enume = reinterpret_cast<const EnumNode *>(*m);
                    if (enume->flagsType())
                        names << enume->flagsType()->name();

                    foreach (const QString &enumName,
                             enume->doc().enumItemNames().toSet()
                             - enume->doc().omitEnumItemNames().toSet())
                        names << plainCode(marker->markedUpEnumValue(enumName, enume));
                }
                foreach (const QString &name, names)
                    classSection.keywords += qMakePair(name, linkForNode(*m, 0));
            }
            ++m;
        }
        ++s;
    }
    generateFooter( inner );

    if (!membersLink.isEmpty()) {
        DcfSection membersSection;
        membersSection.title = "List of all members";
        membersSection.ref = membersLink;
        appendDcfSubSection(&classSection, membersSection);
    }
    if (!obsoleteLink.isEmpty()) {
        DcfSection obsoleteSection;
        obsoleteSection.title = "Obsolete members";
        obsoleteSection.ref = obsoleteLink;
        appendDcfSubSection(&classSection, obsoleteSection);
    }
    if (!compatLink.isEmpty()) {
        DcfSection compatSection;
        compatSection.title = "Qt 3 support members";
        compatSection.ref = compatLink;
        appendDcfSubSection(&classSection, compatSection);
    }

    appendDcfSubSection(&dcfClassesRoot, classSection);
}

void HtmlGenerator::generateFakeNode( const FakeNode *fake, CodeMarker *marker )
{
    SubTitleSize subTitleSize = LargeSubTitle;
    DcfSection fakeSection;
    fakeSection.title = fake->fullTitle();
    fakeSection.ref = linkForNode(fake, 0);

    QList<Section> sections;
    QList<Section>::const_iterator s;

    QString htmlTitle = fake->fullTitle();
    if (fake->subType() == FakeNode::File && !fake->subTitle().isEmpty()) {
        subTitleSize = SmallSubTitle;
        htmlTitle += " (" + fake->subTitle() + ")";
    }

    generateHeader(htmlTitle, fake, marker, true);
    generateTitle(fake->fullTitle(), Text() << fake->subTitle(), subTitleSize,
                  fake, marker);

    if (fake->subType() == FakeNode::Module) {
        // Generate brief text and status for modules.
        generateBrief(fake, marker);
        generateStatus(fake, marker);

        if (moduleNamespaceMap.contains(fake->name())) {
            out() << "<h2>Namespaces</h2>\n";
            generateAnnotatedList(fake, marker, moduleNamespaceMap[fake->name()]);
        }
        if (moduleClassMap.contains(fake->name())) {
            out() << "<h2>Classes</h2>\n";
            generateAnnotatedList(fake, marker, moduleClassMap[fake->name()]);
        }
    } else if (fake->subType() == FakeNode::HeaderFile) {
        // Generate brief text and status for modules.
        generateBrief(fake, marker);
        generateStatus(fake, marker);

        out() << "<ul>\n";

        QString membersLink = generateListOfAllMemberFile(fake, marker);
        if (!membersLink.isEmpty())
            out() << "<li><a href=\"" << membersLink << "\">"
                  << "List of all members, including inherited members</a></li>\n";

        QString obsoleteLink = generateLowStatusMemberFile(fake, marker, CodeMarker::Obsolete);
        if (!obsoleteLink.isEmpty())
            out() << "<li><a href=\"" << obsoleteLink << "\">"
                  << "Obsolete members</a></li>\n";

        QString compatLink = generateLowStatusMemberFile(fake, marker, CodeMarker::Compat);
        if (!compatLink.isEmpty())
            out() << "<li><a href=\"" << compatLink << "\">"
                  << "Qt 3 support members</a></li>\n";

        out() << "</ul>\n";

        if (!membersLink.isEmpty()) {
            DcfSection membersSection;
            membersSection.title = "List of all members";
            membersSection.ref = membersLink;
            appendDcfSubSection(&fakeSection, membersSection);
        }
        if (!obsoleteLink.isEmpty()) {
            DcfSection obsoleteSection;
            obsoleteSection.title = "Obsolete members";
            obsoleteSection.ref = obsoleteLink;
            appendDcfSubSection(&fakeSection, obsoleteSection);
        }
        if (!compatLink.isEmpty()) {
            DcfSection compatSection;
            compatSection.title = "Qt 3 support members";
            compatSection.ref = compatLink;
            appendDcfSubSection(&fakeSection, compatSection);
        }
    }

    sections = marker->sections(fake, CodeMarker::Summary, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        out() << "<a name=\"" << registerRef((*s).name) << "\"></a>\n";
        out() << "<h3>" << protect((*s).name) << "</h3>\n";
        generateSectionList(*s, fake, marker, CodeMarker::Summary);
        ++s;
    }

    Text brief = fake->doc().briefText();
    if (fake->subType() == FakeNode::Module && !brief.isEmpty()) {
        out() << "<a name=\"" << registerRef("details") << "\"></a>\n";
        out() << "<h2>" << "Detailed Description" << "</h2>\n";
    }

    generateBody(fake, marker);
    generateAlsoList(fake, marker);

    if (!fake->groupMembers().isEmpty()) {
        QMap<QString, const Node *> groupMembersMap;
        foreach (const Node *node, fake->groupMembers()) {
            if (node->type() == Node::Class || node->type() == Node::Namespace)
                groupMembersMap[node->name()] = node;
        }
        generateAnnotatedList(fake, marker, groupMembersMap);
    }

    fakeSection.keywords += qMakePair(fakeSection.title, fakeSection.ref);

    sections = marker->sections(fake, CodeMarker::Detailed, CodeMarker::Okay);
    s = sections.begin();
    while (s != sections.end()) {
        out() << "<hr />\n";
        out() << "<h2>" << protect((*s).name) << "</h2>\n";

        NodeList::ConstIterator m = (*s).members.begin();
        while ( m != (*s).members.end() ) {
            generateDetailedMember(*m, fake, marker);
            fakeSection.keywords += qMakePair((*m)->name(), linkForNode(*m, 0));
            ++m;
        }
        ++s;
    }
    generateFooter(fake);

    if (fake->subType() == FakeNode::Example) {
        appendDcfSubSection(&dcfExamplesRoot, fakeSection);
    } else if (fake->subType() != FakeNode::File) {
        QString contentsPage = fake->links().value(Node::ContentsLink).first;

        if (contentsPage == "Qt Designer Manual") {
            appendDcfSubSection(&dcfDesignerRoot, fakeSection);
        } else if (contentsPage == "Qt Linguist Manual") {
            appendDcfSubSection(&dcfLinguistRoot, fakeSection);
        } else if (contentsPage == "Qt Assistant Manual") {
            appendDcfSubSection(&dcfAssistantRoot, fakeSection);
        } else if (contentsPage == "qmake Manual") {
            appendDcfSubSection(&dcfQmakeRoot, fakeSection);
        } else {
            appendDcfSubSection(&dcfOverviewsRoot, fakeSection);
        }
    }
}

QString HtmlGenerator::fileExtension(const Node * /* node */)
{
    return "html";
}

void HtmlGenerator::generateHeader(const QString& title, const Node *node,
                                   CodeMarker *marker, bool mainPage)
{
    out() << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";

    out() << "<!DOCTYPE html\n"
             "    PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"DTD/xhtml1-strict.dtd\">\n"
             "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n";

    QString shortVersion;
    if ( (project != "Qtopia") && (project != "Qt Extended") ) {
        shortVersion = project + " " + shortVersion + ": ";
        if (node && !node->doc().location().isEmpty())
            out() << "<!-- " << node->doc().location().filePath() << " -->\n";

        shortVersion = tre->version();
        if (shortVersion.count(QChar('.')) == 2)
            shortVersion.truncate(shortVersion.lastIndexOf(QChar('.')));
        if (!shortVersion.isEmpty()) {
            if (project == "QSA")
                shortVersion = "QSA " + shortVersion + ": ";
            else
                shortVersion = "Qt " + shortVersion + ": ";
        }
    }

    out() << "<head>\n"
             "  <title>" << shortVersion << protect( title ) << "</title>\n";
    if (!style.isEmpty())
        out() << "    <style type=\"text/css\">" << style << "</style>\n";

    const QMap<QString, QString> &metaMap = node->doc().metaTagMap();
    if (!metaMap.isEmpty()) {
        QMapIterator<QString, QString> i(metaMap);
        while (i.hasNext()) {
            i.next();
            out() << "    <meta name=\"" << protect(i.key()) << "\" contents=\""
                  << protect(i.value()) << "\" />\n";
        }
    }

    navigationLinks.clear();

    if (node && !node->links().empty()) {
        QPair<QString,QString> linkPair;
        QPair<QString,QString> anchorPair;
        const Node *linkNode;

        if (node->links().contains(Node::PreviousLink)) {
            linkPair = node->links()[Node::PreviousLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);

            out() << "  <link rel=\"prev\" href=\""
                  << anchorPair.first << "\" />\n";

            navigationLinks += "[Previous: <a href=\"" + anchorPair.first + "\">";
            if (linkPair.first == linkPair.second && !anchorPair.second.isEmpty())
                navigationLinks += protect(anchorPair.second);
            else
                navigationLinks += protect(linkPair.second);
            navigationLinks += "</a>]\n";
        }
        if (node->links().contains(Node::ContentsLink)) {
            linkPair = node->links()[Node::ContentsLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);

            out() << "  <link rel=\"contents\" href=\""
                  << anchorPair.first << "\" />\n";

            navigationLinks += "[<a href=\"" + anchorPair.first + "\">";
            if (linkPair.first == linkPair.second && !anchorPair.second.isEmpty())
                navigationLinks += protect(anchorPair.second);
            else
                navigationLinks += protect(linkPair.second);
            navigationLinks += "</a>]\n";
        }
        if (node->links().contains(Node::NextLink)) {
            linkPair = node->links()[Node::NextLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);

            out() << "  <link rel=\"next\" href=\""
                  << anchorPair.first << "\" />\n";

            navigationLinks += "[Next: <a href=\"" + anchorPair.first + "\">";
            if (linkPair.first == linkPair.second && !anchorPair.second.isEmpty())
                navigationLinks += protect(anchorPair.second);
            else
                navigationLinks += protect(linkPair.second);
            navigationLinks += "</a>]\n";
        }
        if (node->links().contains(Node::IndexLink)) {
            linkPair = node->links()[Node::IndexLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);
            out() << "  <link rel=\"index\" href=\""
                  << anchorPair.first << "\" />\n";
        }
        if (node->links().contains(Node::StartLink)) {
            linkPair = node->links()[Node::StartLink];
            linkNode = findNodeForTarget(linkPair.first, node, marker);
            if (!linkNode || linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);
            out() << "  <link rel=\"start\" href=\""
                  << anchorPair.first << "\" />\n";
        }
    }

    foreach (const QString &stylesheet, stylesheets) {
        out() << "  <link href=\"" << stylesheet << "\" rel=\"stylesheet\" "
              << "type=\"text/css\" />\n";
    }

    foreach (const QString &customHeadElement, customHeadElements) {
        out() << "  " << customHeadElement << "\n";
    }

    out() << "</head>\n"
             "<body>\n";
    if (mainPage)
        generateMacRef(node, marker);
    out() << QString(postHeader).replace("\\" + COMMAND_VERSION, tre->version());


    if (node && !node->links().empty())
        out() << "<p>\n" << navigationLinks << "</p>\n";
}

void HtmlGenerator::generateTitle(const QString& title, const Text &subTitle,
                                  SubTitleSize subTitleSize,
                                  const Node *relative, CodeMarker *marker)
{
    out() << "<h1 class=\"title\">" << protect( title );
    if (!subTitle.isEmpty()) {
        out() << "<br />";
        if (subTitleSize == SmallSubTitle)
            out() << "<span class=\"small-subtitle\">";
        else
            out() << "<span class=\"subtitle\">";
        generateText(subTitle, relative, marker);
        out() << "</span>\n";
    }
    out() << "</h1>\n";
}

void HtmlGenerator::generateFooter( const Node *node )
{
    if (node && !node->links().empty())
        out() << "<p>\n" << navigationLinks << "</p>\n";

    out() << QString(footer).replace("\\" + COMMAND_VERSION, tre->version())
          << QString(address).replace("\\" + COMMAND_VERSION, tre->version())
          << "</body>\n"
             "</html>\n";
}

void HtmlGenerator::generateBrief(const Node *node, CodeMarker *marker,
                                  const Node *relative)
{
    Text brief = node->doc().briefText();
    if (!brief.isEmpty()) {
        out() << "<p>";
        generateText(brief, node, marker);
        if (!relative || node == relative)
            out() << " <a href=\"#";
        else
            out() << " <a href=\"" << linkForNode(node, relative) << "#";
        out() << registerRef("details") << "\">More...</a></p>\n";
    }
}

void HtmlGenerator::generateIncludes(const InnerNode *inner, CodeMarker *marker)
{
    if (!inner->includes().isEmpty()) {
        out() << "<pre>" << trimmedTrailing(highlightedCode(indent(codeIndent,
                                                                   marker->markedUpIncludes(
                                                                        inner->includes())),
                                                                        marker, inner))
              << "</pre>";
    }
}

void HtmlGenerator::generateTableOfContents(const Node *node, CodeMarker *marker,
                                            Doc::SectioningUnit sectioningUnit,
                                            int numColumns, const Node *relative)

{
    if (!node->doc().hasTableOfContents())
        return;
    QList<Atom *> toc = node->doc().tableOfContents();
    if (toc.isEmpty())
        return;

    QString nodeName = "";
    if (node != relative)
        nodeName = node->name();

    QStringList sectionNumber;
    int columnSize = 0;

    QString tdTag;
    if (numColumns > 1) {
        tdTag = "<td width=\"" + QString::number((100 + numColumns - 1) / numColumns) + "%\">";
        out() << "<p><table width=\"100%\">\n<tr valign=\"top\">" << tdTag << "\n";
    }

    // disable nested links in table of contents
    inContents = true;
    inLink = true;

    for (int i = 0; i < toc.size(); ++i) {
        Atom *atom = toc.at(i);

        int nextLevel = atom->string().toInt();
        if (nextLevel > (int)sectioningUnit)
            continue;

        if (sectionNumber.size() < nextLevel) {
            do {
                out() << "<ul>";
                sectionNumber.append("1");
            } while (sectionNumber.size() < nextLevel);
        } else {
            while (sectionNumber.size() > nextLevel) {
                out() << "</ul>\n";
                sectionNumber.removeLast();
            }
            sectionNumber.last() = QString::number(sectionNumber.last().toInt() + 1);
        }
        int numAtoms;
        Text headingText = Text::sectionHeading(atom);

        if (sectionNumber.size() == 1 && columnSize > toc.size() / numColumns) {
            out() << "</ul></td>" << tdTag << "<ul>\n";
            columnSize = 0;
        }
        out() << "<li>";
        out() << "<a href=\"" << nodeName << "#" << Doc::canonicalTitle(headingText.toString())
              << "\">";
        generateAtomList(headingText.firstAtom(), node, marker, true, numAtoms);
        out() << "</a></li>\n";

        ++columnSize;
    }
    while (!sectionNumber.isEmpty()) {
        out() << "</ul>\n";
        sectionNumber.removeLast();
    }

    if (numColumns > 1)
        out() << "</td></tr></table></p>\n";

    inContents = false;
    inLink = false;
}

#if 0
void HtmlGenerator::generateNavigationBar( const NavigationBar& bar,
                                           const Node *node,
                                           CodeMarker *marker )
{
    if ( bar.prev.begin() != 0 || bar.current.begin() != 0 ||
         bar.next.begin() != 0 ) {
        out() << "<p align=\"right\">";
        if ( bar.prev.begin() != 0 ) {
#if 0
            out() << "[<a href=\"" << section.previousBaseName()
                  << ".html\">Prev: ";
            generateText( section.previousHeading(), node, marker );
            out() << "</a>]\n";
#endif
        }
        if ( bar.current.begin() != 0 ) {
            out() << "[<a href=\"" << "home"
                  << ".html\">Home</a>]\n";
        }
        if ( bar.next.begin() != 0 ) {
            out() << "[<a href=\"" << fileBase( node, bar.next )
                  << ".html\">Next: ";
            generateText( Text::sectionHeading(bar.next.begin()), node, marker );
            out() << "</a>]\n";
        }
        out() << "</p>\n";
    }
}
#endif

QString HtmlGenerator::generateListOfAllMemberFile(const InnerNode *inner, CodeMarker *marker)
{
    QList<Section> sections;
    QList<Section>::ConstIterator s;

    sections = marker->sections(inner, CodeMarker::SeparateList, CodeMarker::Okay);
    if (sections.isEmpty())
        return QString();

    QString fileName = fileBase(inner) + "-members." + fileExtension(inner);
    beginSubPage(inner->location(), fileName);
    QString title = "List of All Members for " + inner->name();
    generateHeader(title, inner, marker, false);
    generateTitle(title, Text(), SmallSubTitle, inner, marker);
    out() << "<p>This is the complete list of members for ";
    generateFullName(inner, 0, marker);
    out() << ", including inherited members.</p>\n";

    Section section = sections.first();
    generateSectionList(section, 0, marker, CodeMarker::SeparateList);

    generateFooter();
    endSubPage();
    return fileName;
}

QString HtmlGenerator::generateLowStatusMemberFile(const InnerNode *inner, CodeMarker *marker,
                                                   CodeMarker::Status status)
{
    QList<Section> sections = marker->sections(inner, CodeMarker::Summary, status);
    QMutableListIterator<Section> j(sections);
    while (j.hasNext()) {
        if (j.next().members.size() == 0)
            j.remove();
    }
    if (sections.isEmpty())
        return QString();

    int i;

    QString title;
    QString fileName;

    if (status == CodeMarker::Compat) {
        title = "Qt 3 Support Members for " + inner->name();
        fileName = fileBase(inner) + "-qt3." + fileExtension(inner);
    } else {
        title = "Obsolete Members for " + inner->name();
        fileName = fileBase(inner) + "-obsolete." + fileExtension(inner);
    }

    beginSubPage(inner->location(), fileName);
    generateHeader(title, inner, marker, false);
    generateTitle(title, Text(), SmallSubTitle, inner, marker);

    if (status == CodeMarker::Compat) {
        out() << "<p><b>The following class members are part of the "
                 "<a href=\"qt3support.html\">Qt 3 support layer</a>.</b> "
                 "They are provided to help you port old code to Qt 4. We advise against "
                 "using them in new code.</p>\n";
    } else {
        out() << "<p><b>The following class members are obsolete.</b> They are provided to keep "
                 "old source code working. We strongly advise against using them in new "
                 "code.</p>\n";
    }

    out() << "<p><ul><li><a href=\"" << linkForNode(inner, 0) << "\">" << protect(inner->name())
          << " class reference</a></li></ul></p>\n";

    for (i = 0; i < sections.size(); ++i) {
        out() << "<h3>" << protect(sections.at(i).name) << "</h3>\n";

        generateSectionList(sections.at(i), inner, marker, CodeMarker::Summary);
    }

    sections = marker->sections(inner, CodeMarker::Detailed, status);
    for (i = 0; i < sections.size(); ++i) {
        out() << "<hr />\n";
        out() << "<h2>" << protect( sections.at(i).name ) << "</h2>\n";

        NodeList::ConstIterator m = sections.at(i).members.begin();
        while ( m != sections.at(i).members.end() ) {
            if ( (*m)->access() != Node::Private )
                generateDetailedMember(*m, inner, marker);
            ++m;
        }
    }

    generateFooter();
    endSubPage();
    return fileName;
}

void HtmlGenerator::generateClassHierarchy(const Node *relative, CodeMarker *marker,
                                           const QMap<QString, const Node *> &classMap)
{
    if (classMap.isEmpty())
        return;

    QMap<QString, const Node *> topLevel;
    QMap<QString, const Node *>::ConstIterator c = classMap.begin();
    while (c != classMap.end()) {
        const ClassNode *classe = static_cast<const ClassNode *>(*c);
        if (classe->baseClasses().isEmpty())
            topLevel.insert(classe->name(), classe);
        ++c;
    }

    QStack<QMap<QString, const Node *> > stack;
    stack.push(topLevel);

    out() << "<ul>\n";
    while (!stack.isEmpty()) {
        if (stack.top().isEmpty()) {
            stack.pop();
            out() << "</ul>\n";
        } else {
            const ClassNode *child = static_cast<const ClassNode *>(*stack.top().begin());
            out() << "<li>";
            generateFullName(child, relative, marker);
            out() << "</li>\n";
            stack.top().erase(stack.top().begin());

            QMap<QString, const Node *> newTop;
            foreach (const RelatedClass &d, child->derivedClasses()) {
                if (d.access != Node::Private)
                    newTop.insert(d.node->name(), d.node);
            }
            if (!newTop.isEmpty()) {
                stack.push(newTop);
                out() << "<ul>\n";
            }
        }
    }
}

void HtmlGenerator::generateAnnotatedList(const Node *relative, CodeMarker *marker,
                    const QMap<QString, const Node *> &nodeMap)
{
    out() << "<p><table width=\"100%\" class=\"annotated\" cellpadding=\"2\" cellspacing=\"1\" border=\"0\">\n";

    int row = 0;
    foreach (const QString &name, nodeMap.keys()) {
        const Node *node = nodeMap[name];

        if (++row % 2 == 1)
            out() << "<tr valign=\"top\" class=\"odd\">";
        else
            out() << "<tr valign=\"top\" class=\"even\">";
        out() << "<th>";
        generateFullName(node, relative, marker);
        out() << "</th>";

        if (!(node->type() == Node::Fake)) {
            Text brief = node->doc().trimmedBriefText(name);
            if (!brief.isEmpty()) {
                out() << "<td>";
                generateText(brief, node, marker);
                out() << "</td>";
            }
        } else {
            out() << "<td>";
            out() << protect(node->doc().briefText().toString());
            out() << "</td>";
        }
        out() << "</tr>\n";
    }
    out() << "</table></p>\n";
}

void HtmlGenerator::generateCompactList(const Node *relative, CodeMarker *marker,
                                        const QMap<QString, const Node *> &classMap)
{
    const int NumParagraphs = 37; // '0' to '9', 'A' to 'Z', '_'
    const int NumColumns = 4; // number of columns in the result

    if (classMap.isEmpty())
        return;

    /*
      First, find out the common prefix of all non-namespaced classes.
      For Qt, the prefix is Q. It can easily be derived from the first
      and last classes in alphabetical order (QAccel and QXtWidget in Qt 2.1).
    */
    int commonPrefixLen = 0;
    QString commonPrefix;
    QString first;
    QString last;

    QMap<QString, const Node *>::const_iterator iter = classMap.begin();
    while (iter != classMap.end()) {
        if (!iter.key().contains("::")) {
            first = iter.key();
            break;
        }
        ++iter;
    }

    if (first.isEmpty())
        first = classMap.begin().key();

    iter = classMap.end();
    while (iter != classMap.begin()) {
        --iter;
        if (!iter.key().contains("::")) {
            last = iter.key();
            break;
        }
    }

    if (last.isEmpty())
        last = classMap.begin().key();

    if (classMap.size() > 1) {
        while (commonPrefixLen < first.length() + 1 && commonPrefixLen < last.length() + 1
               && first[commonPrefixLen] == last[commonPrefixLen])
            ++commonPrefixLen;
    }

    commonPrefix = first.left(commonPrefixLen);

    /*
      Divide the data into 37 paragraphs: 0, ..., 9, A, ..., Z,
      underscore (_). QAccel will fall in paragraph 10 (A) and
      QXtWidget in paragraph 33 (X). This is the only place where we
      assume that NumParagraphs is 37. Each paragraph is a
      QMap<QString, const Node *>.
    */
    QMap<QString, const Node *> paragraph[NumParagraphs];
    QString paragraphName[NumParagraphs];

    QMap<QString, const Node *>::ConstIterator c = classMap.begin();
    while (c != classMap.end()) {
        QStringList pieces = c.key().split("::");
        QString key;
        if (pieces.size() == 1)
            key = pieces.last().mid( commonPrefixLen ).toLower();
        else
            key = pieces.last().toLower();

        int paragraphNo = NumParagraphs - 1;

        if (key[0].digitValue() != -1) {
            paragraphNo = key[0].digitValue();
        } else if (key[0] >= QLatin1Char('a') && key[0] <= QLatin1Char('z')) {
            paragraphNo = 10 + key[0].unicode() - 'a';
        }

        paragraphName[paragraphNo] = key[0].toUpper();
        paragraph[paragraphNo].insert(key, c.value());
        ++c;
    }

    /*
      Each paragraph j has a size: paragraph[j].count(). In the
      discussion, we will assume paragraphs 0 to 5 will have sizes
      3, 1, 4, 1, 5, 9.

      We now want to compute the paragraph offset. Paragraphs 0 to 6
      start at offsets 0, 3, 4, 8, 9, 14, 23.
    */
    int paragraphOffset[NumParagraphs + 1];
    int i, j, k;

    paragraphOffset[0] = 0;
    for ( j = 0; j < NumParagraphs; j++ )
        paragraphOffset[j + 1] = paragraphOffset[j] + paragraph[j].count();

    int firstOffset[NumColumns + 1];
    int currentOffset[NumColumns];
    int currentParagraphNo[NumColumns];
    int currentOffsetInParagraph[NumColumns];

    int numRows = ( classMap.count() + NumColumns - 1 ) / NumColumns;
    int curParagNo = 0;

    for ( i = 0; i < NumColumns; i++ ) {
        firstOffset[i] = qMin(i * numRows, classMap.size());
        currentOffset[i] = firstOffset[i];

        for ( j = curParagNo; j < NumParagraphs; j++ ) {
            if ( paragraphOffset[j] > firstOffset[i] )
                break;
            if ( paragraphOffset[j] <= firstOffset[i] )
                curParagNo = j;
        }
        currentParagraphNo[i] = curParagNo;
        currentOffsetInParagraph[i] = firstOffset[i] -
                                      paragraphOffset[curParagNo];
    }
    firstOffset[NumColumns] = classMap.count();

    out() << "<p><table width=\"100%\">\n";
    for ( k = 0; k < numRows; k++ ) {
        out() << "<tr>\n";
        for ( i = 0; i < NumColumns; i++ ) {
            if ( currentOffset[i] >= firstOffset[i + 1] ) {
                // this column is finished
                out() << "<td>\n</td>\n";
            } else {
                while (currentOffsetInParagraph[i] == paragraph[currentParagraphNo[i]].count()) {
                    ++currentParagraphNo[i];
                    currentOffsetInParagraph[i] = 0;
                }

                out() << "<td align=\"right\">";
                if ( currentOffsetInParagraph[i] == 0 ) {
                    // start a new paragraph
                    out() << "<b>" << paragraphName[currentParagraphNo[i]] << "&nbsp;</b>";
                }
                out() << "</td>\n";

                // bad loop
                QMap<QString, const Node *>::Iterator it;
                it = paragraph[currentParagraphNo[i]].begin();
                for ( j = 0; j < currentOffsetInParagraph[i]; j++ )
                    ++it;

                out() << "<td>";
                // Previously, we used generateFullName() for this, but we
                // require some special formatting.
                out() << "<a href=\"" << linkForNode(it.value(), relative) << "\">";
                QStringList pieces = fullName(it.value(), relative, marker).split("::");
                out() << protect(pieces.last());
                out() << "</a>";
                if (pieces.size() > 1) {
                    out() << " (";
                    generateFullName(it.value()->parent(), relative, marker);
                    out() << ")";
                }
                out() << "</td>\n";

                currentOffset[i]++;
                currentOffsetInParagraph[i]++;
            }
        }
        out() << "</tr>\n";
    }
    out() << "</table></p>\n";
}

void HtmlGenerator::generateFunctionIndex(const Node *relative, CodeMarker *marker)
{
    out() << "<p align=\"center\"><font size=\"+1\"><b>";
    for ( int i = 0; i < 26; i++ ) {
        QChar ch( 'a' + i );
        out() << QString("<a href=\"#%1\">%2</a>&nbsp;").arg(ch).arg(ch.toUpper());
    }
    out() << "</b></font></p>\n";

    char nextLetter = 'a';
    char currentLetter;

#if 1
    out() << "<ul>\n";
#endif
    QMap<QString, QMap<QString, const Node *> >::ConstIterator f = funcIndex.begin();
    while (f != funcIndex.end()) {
#if 1
        out() << "<li>";
#else
        out() << "<p>";
#endif
        out() << protect(f.key()) << ":";

        currentLetter = f.key()[0].unicode();
        while (islower(currentLetter) && currentLetter >= nextLetter) {
            out() << QString("<a name=\"%1\"></a>").arg(nextLetter);
            nextLetter++;
        }

        QMap<QString, const Node *>::ConstIterator s = (*f).begin();
        while ( s != (*f).end() ) {
            out() << " ";
            generateFullName((*s)->parent(), relative, marker, *s);
            ++s;
        }
#if 1
        out() << "</li>";
#else
        out() << "</p>";
#endif
        out() << "\n";
        ++f;
    }
#if 1
    out() << "</ul>\n";
#endif
}

void HtmlGenerator::generateLegaleseList(const Node *relative, CodeMarker *marker)
{
    QMap<Text, const Node *>::ConstIterator it = legaleseTexts.begin();
    while (it != legaleseTexts.end()) {
        Text text = it.key();
        out() << "<hr />\n";
        generateText(text, relative, marker);
        out() << "<ul>\n";
        do {
            out() << "<li>";
            generateFullName(it.value(), relative, marker);
            out() << "</li>\n";
            ++it;
        } while (it != legaleseTexts.end() && it.key() == text);
        out() << "</ul>\n";
    }
}

void HtmlGenerator::generateSynopsis(const Node *node, const Node *relative,
                                     CodeMarker *marker, CodeMarker::SynopsisStyle style)
{
    QString marked = marker->markedUpSynopsis( node, relative, style );
    QRegExp templateTag("(<[^@>]*>)");
    if (marked.indexOf(templateTag) != -1) {
        QString contents = protect(marked.mid(templateTag.pos(1),
                                              templateTag.cap(1).length()));
        marked.replace( templateTag.pos(1), templateTag.cap(1).length(),
                        contents );
    }
    marked.replace(QRegExp("<@param>([a-z]+)_([1-9n])</@param>"), "<i>\\1<sub>\\2</sub></i>");
    marked.replace("<@param>", "<i>");
    marked.replace("</@param>", "</i>");

    if (style == CodeMarker::Summary)
        marked.replace("@name>", "b>");

    if (style == CodeMarker::SeparateList) {
        QRegExp extraRegExp("<@extra>.*</@extra>");
        extraRegExp.setMinimal(true);
        marked.replace(extraRegExp, "");
    } else {
        marked.replace( "<@extra>", "&nbsp;&nbsp;<tt>" );
        marked.replace( "</@extra>", "</tt>" );
    }

    if (style != CodeMarker::Detailed) {
        marked.replace("<@type>", "");
        marked.replace("</@type>", "");
    }
    out() << highlightedCode(marked, marker, relative);
}

void HtmlGenerator::generateOverviewList(const Node *relative, CodeMarker * /* marker */)
{
    QMap<const FakeNode *, QMap<QString, FakeNode *> > fakeNodeMap;
    QMap<QString, const FakeNode *> groupTitlesMap;
    QMap<QString, FakeNode *> uncategorizedNodeMap;
    QRegExp singleDigit("\\b([0-9])\\b");

    const NodeList children = tre->root()->childNodes();
    foreach (Node *child, children) {
        if (child->type() == Node::Fake && child != relative) {
            FakeNode *fakeNode = static_cast<FakeNode *>(child);

            // Check whether the page is part of a group or is the group
            // definition page.
            QString group;
            bool isGroupPage = false;
            if (fakeNode->doc().metaCommandsUsed().contains("group")) {
                group = fakeNode->doc().metaCommandArgs("group")[0];
                isGroupPage = true;
            }

            // there are too many examples; they would clutter the list
            if (fakeNode->subType() == FakeNode::Example)
                continue;

            // not interested either in individual (Qt Designer etc.) manual chapters
            if (fakeNode->links().contains(Node::ContentsLink))
                continue;

            // Discard external nodes.
            if (fakeNode->subType() == FakeNode::ExternalPage)
                continue;

            QString sortKey = fakeNode->fullTitle().toLower();
            if (sortKey.startsWith("the "))
                sortKey.remove(0, 4);
            sortKey.replace(singleDigit, "0\\1");

            if (!group.isEmpty()) {
                if (isGroupPage) {
                    // If we encounter a group definition page, we add all
                    // the pages in that group to the list for that group.
                    foreach (Node *member, fakeNode->groupMembers()) {
                        if (member->type() != Node::Fake)
                            continue;
                        FakeNode *page = static_cast<FakeNode *>(member);
                        if (page) {
                            QString sortKey = page->fullTitle().toLower();
                            if (sortKey.startsWith("the "))
                                sortKey.remove(0, 4);
                            sortKey.replace(singleDigit, "0\\1");
                            fakeNodeMap[const_cast<const FakeNode *>(fakeNode)].insert(sortKey, page);
                            groupTitlesMap[fakeNode->fullTitle()] = const_cast<const FakeNode *>(fakeNode);
                        }
                    }
                } else if (!isGroupPage) {
                    // If we encounter a page that belongs to a group then
                    // we add that page to the list for that group.
                    const FakeNode *groupNode = static_cast<const FakeNode *>(tre->root()->findNode(group, Node::Fake));
                    if (groupNode)
                        fakeNodeMap[groupNode].insert(sortKey, fakeNode);
                    //else
                    //    uncategorizedNodeMap.insert(sortKey, fakeNode);
                }// else
                //    uncategorizedNodeMap.insert(sortKey, fakeNode);
            }// else
            //    uncategorizedNodeMap.insert(sortKey, fakeNode);
        }
    }

    // We now list all the pages found that belong to groups.
    // If only certain pages were found for a group, but the definition page
    // for that group wasn't listed, the list of pages will be intentionally
    // incomplete. However, if the group definition page was listed, all the
    // pages in that group are listed for completeness.

    if (!fakeNodeMap.isEmpty()) {
        foreach (const QString &groupTitle, groupTitlesMap.keys()) {
            const FakeNode *groupNode = groupTitlesMap[groupTitle];
            out() << QString("<h3><a href=\"%1\">%2</a></h3>\n").arg(
                        linkForNode(groupNode, relative)).arg(
                        protect(groupNode->fullTitle()));

            if (fakeNodeMap[groupNode].count() == 0)
                continue;

            out() << "<ul>\n";

            foreach (const FakeNode *fakeNode, fakeNodeMap[groupNode]) {
                QString title = fakeNode->fullTitle();
                if (title.startsWith("The "))
                    title.remove(0, 4);
                out() << "<li><a href=\"" << linkForNode(fakeNode, relative) << "\">"
                      << protect(title) << "</a></li>\n";
            }
            out() << "</ul>\n";
        }
    }

    if (!uncategorizedNodeMap.isEmpty()) {
        out() << QString("<h3>Miscellaneous</h3>\n");
        out() << "<ul>\n";
        foreach (const FakeNode *fakeNode, uncategorizedNodeMap) {
            QString title = fakeNode->fullTitle();
            if (title.startsWith("The "))
                title.remove(0, 4);
            out() << "<li><a href=\"" << linkForNode(fakeNode, relative) << "\">"
                  << protect(title) << "</a></li>\n";
        }
        out() << "</ul>\n";
    }
}

void HtmlGenerator::generateSectionList(const Section& section, const Node *relative,
                                        CodeMarker *marker, CodeMarker::SynopsisStyle style)
{
    if ( !section.members.isEmpty() ) {
        bool twoColumn = false;
        if ( style == CodeMarker::SeparateList ) {
            twoColumn = ( section.members.count() >= 16 );
        } else if ( section.members.first()->type() == Node::Property ) {
            twoColumn = ( section.members.count() >= 5 );
        }
        if ( twoColumn )
            out() << "<p><table width=\"100%\" border=\"0\" cellpadding=\"0\""
                     " cellspacing=\"0\">\n"
                  << "<tr><td width=\"45%\" valign=\"top\">";
        out() << "<ul>\n";

        int i = 0;
        NodeList::ConstIterator m = section.members.begin();
        while ( m != section.members.end() ) {
            if ((*m)->access() == Node::Private) {
                ++m;
                continue;
            }

            if ( twoColumn && i == (int) (section.members.count() + 1) / 2 )
                out() << "</ul></td><td valign=\"top\"><ul>\n";

            out() << "<li><div class=\"fn\"></div>";
            if (style == CodeMarker::Accessors)
                out() << "<b>";
            generateSynopsis( *m, relative, marker, style );
            if (style == CodeMarker::Accessors)
                out() << "</b>";
            out() << "</li>\n";
            i++;
            ++m;
        }
        out() << "</ul>\n";
        if ( twoColumn )
            out() << "</td></tr>\n</table></p>\n";
    }

    if (style == CodeMarker::Summary && !section.inherited.isEmpty()) {
        out() << "<ul>\n";
        generateSectionInheritedList(section, relative, marker);
        out() << "</ul>\n";
    }
}

void HtmlGenerator::generateSectionInheritedList(const Section& section, const Node *relative,
                                                 CodeMarker *marker)
{
    QList<QPair<ClassNode *, int> >::ConstIterator p = section.inherited.begin();
    while ( p != section.inherited.end() ) {
        out() << "<li><div class=\"fn\"></div>";
        out() << (*p).second << " ";
        if ( (*p).second == 1 ) {
            out() << section.singularMember;
        } else {
            out() << section.pluralMember;
        }
        out() << " inherited from <a href=\"" << fileName((*p).first)
              << "#" << HtmlGenerator::cleanRef(section.name.toLower()) << "\">"
              << protect(marker->plainFullName((*p).first, relative))
              << "</a></li>\n";
        ++p;
    }
}

void HtmlGenerator::generateLink(const Atom *atom, const Node * /* relative */, CodeMarker *marker)
{
    static QRegExp camelCase("[A-Z][A-Z][a-z]|[a-z][A-Z0-9]|_");

    if (funcLeftParen.indexIn(atom->string()) != -1 && marker->recognizeLanguage("Cpp")) {
        // hack for C++: move () outside of link
        int k = funcLeftParen.pos( 1 );
        out() << protect( atom->string().left(k) );
        if ( link.isEmpty() ) {
            if (showBrokenLinks)
                out() << "</i>";
        } else {
            out() << "</a>";
        }
        inLink = false;
        out() << protect(atom->string().mid(k));
    } else if (marker->recognizeLanguage("Java")) {
	// hack for Java: remove () and use <tt> when appropriate
        bool func = atom->string().endsWith("()");
        bool tt = (func || atom->string().contains(camelCase));
        if (tt)
            out() << "<tt>";
        if (func) {
            out() << protect(atom->string().left(atom->string().length() - 2));
        } else {
            out() << protect(atom->string());
        }
        out() << "</tt>";
    } else {
        out() << protect(atom->string());
    }
}

QString HtmlGenerator::cleanRef( const QString& ref )
{
    QString clean;

    if ( ref.isEmpty() )
        return clean;

    clean.reserve(ref.size() + 20);
    const QChar c = ref[0];
    const uint u = c.unicode();

    if ( (u >= 'a' && u <= 'z') ||
         (u >= 'A' && u <= 'Z') ||
         (u >= '0' && u <= '9') ) {
        clean += c;
    } else if ( u == '~' ) {
        clean += "dtor.";
    } else if ( u == '_' ) {
        clean += "underscore.";
    } else {
        clean += "A";
    }

    for ( int i = 1; i < (int) ref.length(); i++ ) {
        const QChar c = ref[i];
        const uint u = c.unicode();
        if ( (u >= 'a' && u <= 'z') ||
             (u >= 'A' && u <= 'Z') ||
             (u >= '0' && u <= '9') || u == '-' ||
             u == '_' || u == ':' || u == '.' ) {
            clean += c;
        } else if ( c.isSpace() ) {
            clean += "-";
        } else if ( u == '!' ) {
            clean += "-not";
        } else if ( u == '&' ) {
            clean += "-and";
        } else if ( u == '<' ) {
            clean += "-lt";
        } else if ( u == '=' ) {
            clean += "-eq";
        } else if ( u == '>' ) {
            clean += "-gt";
        } else if ( u == '#' ) {
            clean += "#";
        } else {
            clean += "-";
            clean += QString::number((int)u, 16);
        }
    }
    return clean;
}

QString HtmlGenerator::registerRef( const QString& ref )
{
    QString clean = HtmlGenerator::cleanRef( ref );

    for ( ;; ) {
        QString& prevRef = refMap[clean.toLower()];
        if ( prevRef.isEmpty() ) {
            prevRef = ref;
            break;
        } else if ( prevRef == ref ) {
            break;
        }
        clean += "x";
    }
    return clean;
}

QString HtmlGenerator::protect( const QString& string )
{
#define APPEND(x) \
    if (html.isEmpty()) { \
        html = string; \
        html.truncate(i); \
    } \
    html += (x);

    QString html;
    int n = string.length();

    for (int i = 0; i < n; ++i) {
        QChar ch = string.at(i);

        if (ch == QLatin1Char('&')) {
            APPEND("&amp;");
        } else if (ch == QLatin1Char('<')) {
            APPEND("&lt;");
        } else if (ch == QLatin1Char('>')) {
            APPEND("&gt;");
        } else if (ch == QLatin1Char('"')) {
            APPEND("&quot;");
        } else if (ch.unicode() > 0x00FF
                   || (ch == QLatin1Char('*') && i + 1 < n && string.at(i) == QLatin1Char('/'))
                   || (ch == QLatin1Char('.') && i > 2 && string.at(i - 2) == QLatin1Char('.'))) {
            // we escape '*/' and the last dot in 'e.g.' and 'i.e.' for the Javadoc generator
            APPEND("&#x");
            html += QString::number(ch.unicode(), 16);
            html += QLatin1Char(';');
        } else {
            if (!html.isEmpty())
                html += ch;
        }
    }

    if (!html.isEmpty())
        return html;
    return string;

#undef APPEND
}

static QRegExp linkTag("(<@link node=\"([^\"]+)\">).*(</@link>)");
static QRegExp funcTag("(<@func target=\"([^\"]*)\">)(.*)(</@func>)");
static QRegExp typeTag("(<@(type|headerfile|func)(?: +[^>]*)?>)(.*)(</@\\2>)");
static QRegExp spanTag("</@(?:comment|preprocessor|string|char)>");
static QRegExp unknownTag("</?@[^>]*>");


bool parseArg(const QString &src, const QString &tag, int *pos, int n,
    QStringRef *contents, QStringRef *par1 = 0, bool debug = false)
{
#define SKIP_CHAR(c) \
    if (debug) \
        qDebug() << "looking for " << c << " at " << QString(src.data() + i, n - i); \
    if (i >= n || src[i] != c) { \
        if (debug) \
            qDebug() << " char '" << c << "' not found"; \
        return false; \
    } \
    ++i;


#define SKIP_SPACE \
    while (i < n && src[i] == ' ') \
        ++i;

    int i = *pos;
    int j = i;

    // assume "<@" has been parsed outside
    //SKIP_CHAR('<');
    //SKIP_CHAR('@');

    if (tag != QStringRef(&src, i, tag.length())) {
        if (0 && debug)
            qDebug() << "tag " << tag << " not found at " << i;
        return false;
    }

    if (debug)
        qDebug() << "haystack:" << src << "needle:" << tag << "i:" <<i;

    // skip tag
    i += tag.length();

    // parse stuff like:  linkTag("(<@link node=\"([^\"]+)\">).*(</@link>)");
    if (par1) {
        SKIP_SPACE;
        // read parameter name
        j = i;
        while (i < n && src[i].isLetter())
            ++i;
        if (src[i] == '=') {
            if (debug)
                qDebug() << "read parameter" << QString(src.data() + j, i - j);
            SKIP_CHAR('=');
            SKIP_CHAR('"');
            // skip parameter name
            j = i;
            while (i < n && src[i] != '"')
                ++i;
            *par1 = QStringRef(&src, j, i - j);
            SKIP_CHAR('"');
            SKIP_SPACE;
        } else {
            if (debug)
                qDebug() << "no optional parameter found";
        }
    }
    SKIP_SPACE;
    SKIP_CHAR('>');

    // find contents up to closing "</@tag>
    j = i;
    for (; true; ++i) {
        if (i + 4 + tag.length() > n)
            return false;
        if (src[i] != '<')
            continue;
        if (src[i + 1] != '/')
            continue;
        if (src[i + 2] != '@')
            continue;
        if (tag != QStringRef(&src, i + 3, tag.length()))
            continue;
        if (src[i + 3 + tag.length()] != '>')
            continue;
        break;
    }

    *contents = QStringRef(&src, j, i - j);

    i += tag.length() + 4;

    *pos = i;
    if (debug)
        qDebug() << " tag " << tag << " found: pos now: " << i;
    return true;
#undef SKIP_CHAR
}

static void addLink(const QString &linkTarget, const QStringRef &nestedStuff,
    QString *res)
{
    if (!linkTarget.isEmpty()) {
        *res += "<a href=\"";
        *res += linkTarget;
        *res += "\">";
        *res += nestedStuff;
        *res += "</a>";
    } else {
        *res += nestedStuff;
    }
}

QString HtmlGenerator::highlightedCode(const QString& markedCode, CodeMarker *marker,
                                       const Node *relative)
{
    QString src = markedCode;
    QString html;
    QStringRef arg;
    QStringRef par1;

    const QChar charLangle = '<';
    const QChar charAt = '@';

    // replace all <@link> tags: "(<@link node=\"([^\"]+)\">).*(</@link>)"
    static const QString linkTag("link");
    for (int i = 0, n = src.size(); i < n; ) {
        if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
            i += 2;
            if (parseArg(src, linkTag, &i, n, &arg, &par1)) {
                QString link = linkForNode(
                    CodeMarker::nodeForString(par1.toString()), relative);
                addLink(link, arg, &html);
            } else {
                html += charLangle;
                html += charAt;
            }
        } else {
            html += src.at(i++);
        }
    }


    if (slow) {
        // is this block ever used at all?
        // replace all <@func> tags: "(<@func target=\"([^\"]*)\">)(.*)(</@func>)"
        src = html;
        html = QString();
        static const QString funcTag("func");
        for (int i = 0, n = src.size(); i < n; ) {
            if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
                i += 2;
                if (parseArg(src, funcTag, &i, n, &arg, &par1)) {
                    QString link = linkForNode(
                            marker->resolveTarget(par1.toString(), tre, relative),
                            relative);
                    addLink(link, arg, &html);
                    par1 = QStringRef();
                } else {
                    html += charLangle;
                    html += charAt;
                }
            } else {
                html += src.at(i++);
            }
        }
    }

    // replace all "(<@(type|headerfile|func)(?: +[^>]*)?>)(.*)(</@\\2>)" tags
    src = html;
    html = QString();
    static const QString typeTags[] = { "type", "headerfile", "func" };
    for (int i = 0, n = src.size(); i < n; ) {
        if (src.at(i) == charLangle && src.at(i + 1) == charAt) {
            i += 2;
            bool handled = false;
            for (int k = 0; k != 3; ++k) {
                if (parseArg(src, typeTags[k], &i, n, &arg, &par1)) {
                    par1 = QStringRef();
                    QString link = linkForNode(
                            marker->resolveTarget(arg.toString(), tre, relative),
                            relative);
                    addLink(link, arg, &html);
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                html += charLangle;
                html += charAt;
            }
        } else {
            html += src.at(i++);
        }
    }

    // replace all
    // "<@comment>" -> "<span class=\"comment\">";
    // "<@preprocessor>" -> "<span class=\"preprocessor\">";
    // "<@string>" -> "<span class=\"string\">";
    // "<@char>" -> "<span class=\"char\">";
    // "</@(?:comment|preprocessor|string|char)>" -> "</span>"
    src = html;
    html = QString();
    static const QString spanTags[] = {
        "<@comment>",      "<span class=\"comment\">",
        "<@preprocessor>", "<span class=\"preprocessor\">",
        "<@string>",       "<span class=\"string\">",
        "<@char>",         "<span class=\"char\">",
        "</@comment>",     "</span>",
        "</@preprocessor>","</span>",
        "</@string>",      "</span>",
        "</@char>",        "</span>"
        // "<@char>",      "<font color=blue>",
        // "</@char>",     "</font>",
        // "<@func>",      "<font color=green>",
        // "</@func>",     "</font>",
        // "<@id>",        "<i>",
        // "</@id>",       "</i>",
        // "<@keyword>",   "<b>",
        // "</@keyword>",  "</b>",
        // "<@number>",    "<font color=yellow>",
        // "</@number>",   "</font>",
        // "<@op>",        "<b>",
        // "</@op>",       "</b>",
        // "<@param>",     "<i>",
        // "</@param>",    "</i>",
        // "<@string>",    "<font color=green>",
        // "</@string>",  "</font>",
    };
    for (int i = 0, n = src.size(); i < n; ) {
        if (src.at(i) == charLangle) {
            bool handled = false;
            for (int k = 0; k != 8; ++k) {
                const QString & tag = spanTags[2 * k];
                if (tag == QStringRef(&src, i, tag.length())) {
                    html += spanTags[2 * k + 1];
                    i += tag.length();
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                ++i;
                if (src.at(i) == charAt ||
                    (src.at(i) == QLatin1Char('/') && src.at(i + 1) == charAt)) {
                    // drop 'our' unknown tags (the ones still containing '@')
                    while (i < n && src.at(i) != QLatin1Char('>'))
                        ++i;
                    ++i;
                } else {
                    // retain all others
                    html += charLangle;
                }
            }
        } else {
            html += src.at(i);
            ++i;
        }
    }

    return html;
}

QString HtmlGenerator::fileBase(const Node *node)
{
    QString result;

    result = PageGenerator::fileBase(node);

    if (!node->isInnerNode()) {
        switch (node->status()) {
        case Node::Compat:
            result += "-qt3";
            break;
        case Node::Obsolete:
            result += "-obsolete";
            break;
        default:
            ;
        }
    }
    return result;
}

#if 0
QString HtmlGenerator::fileBase( const Node *node,
                                 const SectionIterator& section )
{
    QStringList::ConstIterator s = section.sectionNumber().end();
    QStringList::ConstIterator b = section.baseNameStack().end();

    QString suffix;
    QString base = fileBase( node );

    while ( s != section.sectionNumber().begin() ) {
        --s;
        --b;
        if ( !(*b).isEmpty() ) {
            base = *b;
            break;
        }
        suffix.prepend( "-" + *s );
    }
    return base + suffix;
}
#endif

QString HtmlGenerator::fileName( const Node *node )
{
    if (node->type() == Node::Fake) {
        if (static_cast<const FakeNode *>(node)->subType() == FakeNode::ExternalPage)
            return node->name();
    }

    return PageGenerator::fileName(node);
}

QString HtmlGenerator::refForNode(const Node *node)
{
    const FunctionNode *func;
    const TypedefNode *typedeffe;
    QString ref;

    switch ( node->type() ) {
    case Node::Namespace:
    case Node::Class:
    default:
        break;
    case Node::Enum:
        ref = node->name() + "-enum";
        break;
    case Node::Typedef:
        typedeffe = static_cast<const TypedefNode *>(node);
        if (typedeffe->associatedEnum()) {
            return refForNode(typedeffe->associatedEnum());
        } else {
            ref = node->name() + "-typedef";
        }
        break;
    case Node::Function:
        func = static_cast<const FunctionNode *>(node);
        if (func->associatedProperty()) {
            return refForNode(func->associatedProperty());
        } else {
            ref = func->name();
            if (func->overloadNumber() != 1)
                ref += "-" + QString::number(func->overloadNumber());
        }
        break;
    case Node::Property:
        ref = node->name() + "-prop";
        break;
    case Node::Variable:
        ref = node->name() + "-var";
        break;
    case Node::Target:
        return protect(node->name());
    }
    return registerRef( ref );
}

QString HtmlGenerator::linkForNode(const Node *node, const Node *relative)
{
    QString link;
    QString fn;
    QString ref;

    if (node == 0 || node == relative)
        return QString();
    if (!node->url().isEmpty())
        return node->url();
    if (fileBase(node).isEmpty())
        return QString();
    if (node->access() == Node::Private)
        return QString();

    fn = fileName(node);
/*    if (!node->url().isEmpty())
        return fn;*/
#if 0
    // ### reintroduce this test, without breaking .dcf files
    if (fn != outFileName())
#endif
        link += fn;

    if (!node->isInnerNode()) {
        ref = refForNode(node);
        if (relative && fn == fileName(relative) && ref == refForNode(relative))
            return QString();

        link += "#";
        link += ref;
    }
    return link;
}

QString HtmlGenerator::refForAtom(Atom *atom, const Node * /* node */)
{
    if (atom->type() == Atom::SectionLeft) {
        return Doc::canonicalTitle(Text::sectionHeading(atom).toString());
    } else if (atom->type() == Atom::Target) {
        return Doc::canonicalTitle(atom->string());
    } else {
        return QString();
    }
}

void HtmlGenerator::generateFullName(const Node *apparentNode, const Node *relative,
                                     CodeMarker *marker, const Node *actualNode)
{
    if ( actualNode == 0 )
        actualNode = apparentNode;
    out() << "<a href=\"" << linkForNode(actualNode, relative);
    if (true || relative == 0 || relative->status() != actualNode->status()) {
        switch (actualNode->status()) {
        case Node::Obsolete:
            out() << "\" class=\"obsolete";
            break;
        case Node::Compat:
            out() << "\" class=\"compat";
            break;
        default:
            ;
        }
    }
    out() << "\">";
    out() << protect(fullName(apparentNode, relative, marker));
    out() << "</a>";
}

void HtmlGenerator::generateDetailedMember(const Node *node, const InnerNode *relative,
                                           CodeMarker *marker)
{
    const EnumNode *enume;

    generateMacRef(node, marker);
    if (node->type() == Node::Enum
            && (enume = static_cast<const EnumNode *>(node))->flagsType()) {
        generateMacRef(enume->flagsType(), marker);
        out() << "<h3 class=\"flags\">";
        out() << "<a name=\"" + refForNode( node ) + "\"></a>";
        generateSynopsis(enume, relative, marker, CodeMarker::Detailed);
        out() << "<br />";
        generateSynopsis(enume->flagsType(), relative, marker, CodeMarker::Detailed);
        out() << "</h3>\n";
    } else {
        out() << "<h3 class=\"fn\">";
        out() << "<a name=\"" + refForNode( node ) + "\"></a>";
        generateSynopsis( node, relative, marker, CodeMarker::Detailed );
        out() << "</h3>\n";
    }

    generateStatus(node, marker);
    generateBody(node, marker);
    generateThreadSafeness(node, marker);
    generateSince(node, marker);

    if (node->type() == Node::Property) {
        const PropertyNode *property = static_cast<const PropertyNode *>(node);
        Section section;

        section.members += property->getters();
        section.members += property->setters();
        section.members += property->resetters();

        if (!section.members.isEmpty()) {
            out() << "<p>Access functions:</p>\n";
            generateSectionList(section, node, marker, CodeMarker::Accessors);
        }
    } else if (node->type() == Node::Enum) {
        const EnumNode *enume = static_cast<const EnumNode *>(node);
        if (enume->flagsType()) {
            out() << "<p>The " << protect(enume->flagsType()->name())
                  << " type is a typedef for "
                  << "<a href=\"qflags.html\">QFlags</a>&lt;"
                  << protect(enume->name())
                  << "&gt;. It stores an OR combination of " << protect(enume->name())
                  << " values.</p>\n";
        }
    }
    generateAlsoList( node, marker );
}

void HtmlGenerator::findAllClasses(const InnerNode *node)
{
    NodeList::const_iterator c = node->childNodes().constBegin();
    while (c != node->childNodes().constEnd()) {
        if ((*c)->access() != Node::Private && (*c)->url().isEmpty()) {
            if ((*c)->type() == Node::Class && !(*c)->doc().isEmpty()) {
                QString className = (*c)->name();
                if ((*c)->parent() && (*c)->parent()->type() == Node::Namespace &&
                    !(*c)->parent()->name().isEmpty())
                    className = (*c)->parent()->name()+"::"+className;

                if (!(static_cast<const ClassNode *>(*c))->hideFromMainList()) {
                    if ((*c)->status() == Node::Compat) {
                        compatClasses.insert(className, *c);
                    } else {
                        nonCompatClasses.insert(className, *c);
                        if ((*c)->status() == Node::Main)
                            mainClasses.insert(className, *c);
                    }
                }

                QString moduleName = (*c)->moduleName();
                if (moduleName == "Qt3SupportLight") {
                    moduleClassMap[moduleName].insert((*c)->name(), *c);
                    moduleName = "Qt3Support";
                }
                if (!moduleName.isEmpty())
                    moduleClassMap[moduleName].insert((*c)->name(), *c);

                QString serviceName =
                    (static_cast<const ClassNode *>(*c))->serviceName();
                if (!serviceName.isEmpty())
                    serviceClasses.insert(serviceName, *c);
            } else if ((*c)->isInnerNode()) {
                findAllClasses(static_cast<InnerNode *>(*c));
            }
        }
        ++c;
    }
}

void HtmlGenerator::findAllFunctions(const InnerNode *node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if ((*c)->isInnerNode() && (*c)->url().isEmpty()) {
                findAllFunctions(static_cast<const InnerNode *>(*c));
            } else if ((*c)->type() == Node::Function) {
                const FunctionNode *func = static_cast<const FunctionNode *>(*c);
                if (func->status() > Node::Obsolete && func->metaness() != FunctionNode::Ctor
                        && func->metaness() != FunctionNode::Dtor) {
                    funcIndex[(*c)->name()].insert((*c)->parent()->name(), *c);
                }
            }
        }
        ++c;
    }
}

void HtmlGenerator::findAllLegaleseTexts(const InnerNode *node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if (!(*c)->doc().legaleseText().isEmpty())
                legaleseTexts.insertMulti((*c)->doc().legaleseText(), *c);
            if ((*c)->isInnerNode())
                findAllLegaleseTexts(static_cast<const InnerNode *>(*c));
        }
        ++c;
    }
}

void HtmlGenerator::findAllNamespaces(const InnerNode *node)
{
    NodeList::ConstIterator c = node->childNodes().begin();
    while (c != node->childNodes().end()) {
        if ((*c)->access() != Node::Private) {
            if ((*c)->isInnerNode() && (*c)->url().isEmpty()) {
                findAllNamespaces(static_cast<const InnerNode *>(*c));
                if ((*c)->type() == Node::Namespace) {
                    const NamespaceNode *nspace = static_cast<const NamespaceNode *>(*c);
                    // Ensure that the namespace's name is not empty (the root
                    // namespace has no name).
                    if (!nspace->name().isEmpty()) {
                        namespaceIndex.insert(nspace->name(), *c);
                        QString moduleName = (*c)->moduleName();
                        if (moduleName == "Qt3SupportLight") {
                            moduleNamespaceMap[moduleName].insert((*c)->name(), *c);
                            moduleName = "Qt3Support";
                        }
                        if (!moduleName.isEmpty())
                            moduleNamespaceMap[moduleName].insert((*c)->name(), *c);
                    }
                }
            }
        }
        ++c;
    }
}

int HtmlGenerator::hOffset(const Node *node)
{
    switch (node->type()) {
    case Node::Namespace:
    case Node::Class:
        return 2;
    case Node::Fake:
        if (node->doc().briefText().isEmpty())
            return 1;
        else
            return 2;
    case Node::Enum:
    case Node::Typedef:
    case Node::Function:
    case Node::Property:
    default:
        return 3;
    }
}

bool HtmlGenerator::isThreeColumnEnumValueTable(const Atom *atom)
{
    while (atom != 0 && !(atom->type() == Atom::ListRight && atom->string() == ATOM_LIST_VALUE)) {
        if (atom->type() == Atom::ListItemLeft && !matchAhead(atom, Atom::ListItemRight))
            return true;
        atom = atom->next();
    }
    return false;
}

const Node *HtmlGenerator::findNodeForTarget(const QString &target,
    const Node *relative, CodeMarker *marker, const Atom *atom)
{
    const Node *node = 0;

    if (target.isEmpty()) {
        node = relative;
    } else if (target.endsWith(".html")) {
        node = tre->root()->findNode(target, Node::Fake);
    } else if (marker) {
        node = marker->resolveTarget(target, tre, relative);
        if (!node)
            node = tre->findFakeNodeByTitle(target);
        if (!node && atom) {
            node = tre->findUnambiguousTarget(target,
                *const_cast<Atom**>(&atom));
        }
    }

    if (!node)
        relative->doc().location().warning(tr("Cannot link to '%1'").arg(target));

    return node;
}

const QPair<QString,QString> HtmlGenerator::anchorForNode(const Node *node)
{
    QPair<QString,QString> anchorPair;

    anchorPair.first = PageGenerator::fileName(node);
    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode*>(node);
        anchorPair.second = fakeNode->title();
    }

    return anchorPair;
}

QString HtmlGenerator::getLink(const Atom *atom, const Node *relative, CodeMarker *marker, const Node *node)
{
    QString link;
    node = 0;

    if (atom->string().contains(":") &&
            (atom->string().startsWith("file:")
             || atom->string().startsWith("http:")
             || atom->string().startsWith("https:")
             || atom->string().startsWith("ftp:")
             || atom->string().startsWith("mailto:"))) {

        link = atom->string();
    } else {
        QStringList path;
        if (atom->string().contains('#')) {
            path = atom->string().split('#');
        } else {
            path.append(atom->string());
        }

        Atom *targetAtom = 0;

        QString first = path.first().trimmed();
        if (first.isEmpty()) {
            node = relative;
        } else if (first.endsWith(".html")) {
            node = tre->root()->findNode(first, Node::Fake);
        } else {
            node = marker->resolveTarget(first, tre, relative);
            if (!node)
                node = tre->findFakeNodeByTitle(first);
            if (!node)
                node = tre->findUnambiguousTarget(first, targetAtom);
        }

        if (node) {
            if (!node->url().isEmpty())
                return node->url();
            else
                path.removeFirst();
        } else {
            node = relative;
        }

        while (!path.isEmpty()) {
            targetAtom = tre->findTarget(path.first(), node);
            if (targetAtom == 0)
                break;
            path.removeFirst();
        }

        if (path.isEmpty()) {
            link = linkForNode(node, relative);
            if (targetAtom)
                link += "#" + refForAtom(targetAtom, node);
        }
    }
    return link;
}

void HtmlGenerator::generateDcf(const QString &fileBase, const QString &startPage,
                                const QString &title, DcfSection &dcfRoot)
{
    dcfRoot.ref = startPage;
    dcfRoot.title = title;
    generateDcfSections(dcfRoot, outputDir() + "/" + fileBase + ".dcf", fileBase + "/reference");
}

void HtmlGenerator::generateIndex(const QString &fileBase, const QString &url,
                                  const QString &title)
{
    tre->generateIndex(outputDir() + "/" + fileBase + ".index", url, title);
}

void HtmlGenerator::generateStatus( const Node *node, CodeMarker *marker )
{
    Text text;

    switch ( node->status() ) {
    case Node::Obsolete:
        if (node->isInnerNode())
	    Generator::generateStatus(node, marker);
        break;
    case Node::Compat:
        if (node->isInnerNode()) {
            text << Atom::ParaLeft << Atom( Atom::FormattingLeft, ATOM_FORMATTING_BOLD ) << "This "
                 << typeString( node ) << " is part of the Qt 3 support library."
                 << Atom( Atom::FormattingRight, ATOM_FORMATTING_BOLD )
                 << " It is provided to keep old source code working. We strongly advise against "
                 << "using it in new code. See ";

            const FakeNode *fakeNode = tre->findFakeNodeByTitle("Porting To Qt 4");
            Atom *targetAtom = 0;
            if (fakeNode && node->type() == Node::Class) {
                QString oldName(node->name());
                targetAtom = tre->findTarget(oldName.replace("3", ""),
                                             fakeNode);
            }

            if (targetAtom) {
                text << Atom(Atom::Link, linkForNode(fakeNode, node) + "#" +
                                         refForAtom(targetAtom, fakeNode));
            } else
                text << Atom(Atom::Link, "Porting to Qt 4");

            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
                 << Atom(Atom::String, "Porting to Qt 4")
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK)
                 << " for more information."
                 << Atom::ParaRight;
        }
        generateText(text, node, marker);
        break;
    default:
        Generator::generateStatus(node, marker);
    }
}

void HtmlGenerator::generateMacRef(const Node *node, CodeMarker *marker)
{
    if (!pleaseGenerateMacRef || marker == 0)
        return;

    QStringList macRefs = marker->macRefsForNode(node);
    foreach (const QString &macRef, macRefs)
        out() << "<a name=\"" << "//apple_ref/" << macRef << "\" />\n";
}

void HtmlGenerator::beginLink(const QString &link, const Node *node, const Node *relative, CodeMarker *marker)
{
    Q_UNUSED(marker)
    Q_UNUSED(relative)

    this->link = link;
    if (link.isEmpty()) {
        if (showBrokenLinks)
            out() << "<i>";
    } else if (node == 0 || (relative != 0 && node->status() == relative->status())) {
        out() << "<a href=\"" << link << "\">";
    } else {
        switch (node->status()) {
        case Node::Obsolete:
            out() << "<a href=\"" << link << "\" class=\"obsolete\">";
            break;
        case Node::Compat:
            out() << "<a href=\"" << link << "\" class=\"compat\">";
            break;
        default:
            out() << "<a href=\"" << link << "\">";
        }
    }
    inLink = true;
}

void HtmlGenerator::endLink()
{
    if (inLink) {
        if (link.isEmpty()) {
            if (showBrokenLinks)
                out() << "</i>";
        } else {
            out() << "</a>";
        }
    }
    inLink = false;
}

QT_END_NAMESPACE
