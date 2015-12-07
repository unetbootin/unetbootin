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
  webxmlgenerator.cpp
*/

#include <QtXml>

#include "codemarker.h"
#include "pagegenerator.h"
#include "webxmlgenerator.h"
#include "node.h"
#include "separator.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

#define COMMAND_VERSION                 Doc::alias("version")

WebXMLGenerator::WebXMLGenerator()
    : PageGenerator()
{
}

WebXMLGenerator::~WebXMLGenerator()
{
}

void WebXMLGenerator::initializeGenerator(const Config &config)
{
    Generator::initializeGenerator(config);

    project = config.getString(CONFIG_PROJECT);

    projectDescription = config.getString(CONFIG_DESCRIPTION);
    if (projectDescription.isEmpty() && !project.isEmpty())
        projectDescription = project + " Reference Documentation";

    projectUrl = config.getString(CONFIG_URL);

    generateIndex = config.getBool(CONFIG_GENERATEINDEX);
}

void WebXMLGenerator::terminateGenerator()
{
    PageGenerator::terminateGenerator();
}

QString WebXMLGenerator::format()
{
    return "WebXML";
}

QString WebXMLGenerator::fileExtension(const Node * /* node */)
{
    return "xml";
}

void WebXMLGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    tre = tree;
    moduleClassMap.clear();
    moduleNamespaceMap.clear();
    serviceClasses.clear();
    findAllClasses(tree->root());
    findAllNamespaces(tree->root());

    PageGenerator::generateTree(tree, marker);

    if (generateIndex)
        tre->generateIndex(outputDir() + "/" + project.toLower() + ".index",
                           projectUrl, projectDescription, false);
}

void WebXMLGenerator::startText(const Node *relative, CodeMarker *marker)
{
    inLink = false;
    inContents = false;
    inSectionHeading = false;
    numTableRows = 0;
    sectionNumber.clear();
    PageGenerator::startText(relative, marker);
}

int WebXMLGenerator::generateAtom(QXmlStreamWriter &writer, const Atom *atom,
                                  const Node *relative, CodeMarker *marker)
{
    Q_UNUSED(writer);

    int skipAhead = 0;

    switch (atom->type()) {
    default:
        PageGenerator::generateAtom(atom, relative, marker);
    }
    return skipAhead;
}

void WebXMLGenerator::generateClassLikeNode(const InnerNode *inner,
                                            CodeMarker *marker)
{
    QByteArray data;
    QXmlStreamWriter writer(&data);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("WebXML");
    writer.writeStartElement("document");

    generateIndexSections(writer, inner, marker);

    writer.writeEndElement(); // document
    writer.writeEndElement(); // WebXML
    writer.writeEndDocument();

    out() << data;
    out().flush();
}

void WebXMLGenerator::generateFakeNode(const FakeNode *fake, CodeMarker *marker)
{
    QByteArray data;
    QXmlStreamWriter writer(&data);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("WebXML");
    writer.writeStartElement("document");

    generateIndexSections(writer, fake, marker);

    writer.writeEndElement(); // document
    writer.writeEndElement(); // WebXML
    writer.writeEndDocument();

    out() << data;
    out().flush();
}

void WebXMLGenerator::generateIndexSections(QXmlStreamWriter &writer,
                                 const Node *node, CodeMarker *marker)
{
    if (tre->generateIndexSection(writer, node, true)) {

        // Add documentation to this node if it exists.
        writer.writeStartElement("description");
        writer.writeAttribute("path", node->doc().location().filePath());
        writer.writeAttribute("line", QString::number(node->doc().location().lineNo()));
        writer.writeAttribute("column", QString::number(node->doc().location().columnNo()));

        if (node->type() == Node::Fake) {

            const FakeNode *fake = static_cast<const FakeNode *>(node);

            generateRelations(writer, node, marker);

            if (fake->subType() == FakeNode::Module) {
                writer.writeStartElement("generatedlist");
                writer.writeAttribute("contents", "classesbymodule");

                if (moduleNamespaceMap.contains(fake->name())) {
                    writer.writeStartElement("section");
                    writer.writeStartElement("heading");
                    writer.writeAttribute("level", "1");
                    writer.writeCharacters("Namespaces");
                    writer.writeEndElement(); // heading
                    generateAnnotatedList(writer, fake, marker, moduleNamespaceMap[fake->name()]);
                    writer.writeEndElement(); // section
                }
                if (moduleClassMap.contains(fake->name())) {
                    writer.writeStartElement("section");
                    writer.writeStartElement("heading");
                    writer.writeAttribute("level", "1");
                    writer.writeCharacters("Classes");
                    writer.writeEndElement(); // heading
                    generateAnnotatedList(writer, fake, marker, moduleClassMap[fake->name()]);
                    writer.writeEndElement(); // section
                }

                writer.writeEndElement(); // generatedlist
            }
        }

        startText(node, marker);

        const Atom *atom = node->doc().body().firstAtom();
        while (atom)
            atom = addAtomElements(writer, atom, node, marker);

        QList<Text> alsoList = node->doc().alsoList();
        supplementAlsoList(node, alsoList);

        if (!alsoList.isEmpty()) {
            writer.writeStartElement("see-also");
            for (int i = 0; i < alsoList.size(); ++i) {
                const Atom *atom = alsoList.at(i).firstAtom();
                while (atom)
                    atom = addAtomElements(writer, atom, node, marker);
            }
            writer.writeEndElement(); // see-also
        }

        writer.writeEndElement(); // description

        if (node->isInnerNode()) {
            const InnerNode *inner = static_cast<const InnerNode *>(node);

            // Recurse to generate an element for this child node and all its children.
            foreach (const Node *child, inner->childNodes())
                generateIndexSections(writer, child, marker);

            writer.writeStartElement("related");
            if (inner->relatedNodes().size() > 0) {
                foreach (const Node *child, inner->relatedNodes())
                    generateIndexSections(writer, child, marker);
            }
            writer.writeEndElement(); // related
        }
        writer.writeEndElement();
    }
}

void WebXMLGenerator::generateInnerNode(const InnerNode *node, CodeMarker *marker)
{
    if (!node->url().isNull())
        return;

    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode *>(node);
        if (fakeNode->subType() == FakeNode::ExternalPage)
            return;
    }

    if ( node->parent() != 0 ) {
	beginSubPage( node->location(), fileName(node) );
	if ( node->type() == Node::Namespace || node->type() == Node::Class) {
	    generateClassLikeNode(node, marker);
	} else if ( node->type() == Node::Fake ) {
	    generateFakeNode(static_cast<const FakeNode *>(node), marker);
	}
	endSubPage();
    }

    NodeList::ConstIterator c = node->childNodes().begin();
    while ( c != node->childNodes().end() ) {
	if ((*c)->isInnerNode() && (
            (*c)->access() != Node::Private || (*c)->status() == Node::Internal))
	    generateInnerNode( (const InnerNode *) *c, marker );
	++c;
    }
}

const Atom *WebXMLGenerator::addAtomElements(QXmlStreamWriter &writer,
     const Atom *atom, const Node *relative, CodeMarker *marker)
{
    switch (atom->type()) {
    case Atom::AbstractLeft:
    case Atom::AbstractRight:
        break;
    case Atom::AutoLink:
        if (!inLink && !inSectionHeading) {
            const Node *node = findNode(atom, relative, marker);
            if (node) {
                startLink(writer, atom, node, relative);
                if (inLink) {
                    writer.writeCharacters(atom->string());
                    writer.writeEndElement(); // link
                    inLink = false;
                }
            } else
                writer.writeCharacters(atom->string());
        } else
            writer.writeCharacters(atom->string());
        break;
    case Atom::BaseName:
        break;
    case Atom::BriefLeft:

        writer.writeStartElement("brief");
        switch (relative->type()) {
        case Node::Property:
            writer.writeCharacters("This property");
            break;
        case Node::Variable:
            writer.writeCharacters("This variable");
            break;
        default:
            break;
        }
        if (relative->type() == Node::Property || relative->type() == Node::Variable) {
            QString str;
            const Atom *a = atom->next();
            while (a != 0 && a->type() != Atom::BriefRight) {
                if (a->type() == Atom::String || a->type() == Atom::AutoLink)
                    str += a->string();
                a = a->next();
            }
            str[0] = str[0].toLower();
            if (str.right(1) == ".")
                str.chop(1);

            QStringList words = str.split(" ");
            if (!(words.first() == "contains" || words.first() == "specifies"
                || words.first() == "describes" || words.first() == "defines"
                || words.first() == "holds" || words.first() == "determines"))
                writer.writeCharacters(" holds ");
            else
                writer.writeCharacters(" ");
        }
        break;

    case Atom::BriefRight:
        if (relative->type() == Node::Property || relative->type() == Node::Variable)
            writer.writeCharacters(".");

        writer.writeEndElement(); // brief
        break;

    case Atom::C:
        writer.writeStartElement("teletype");
        if (inLink)
            writer.writeAttribute("type", "normal");
        else
            writer.writeAttribute("type", "highlighted");

        writer.writeCharacters(plainCode(atom->string()));
        writer.writeEndElement(); // teletype
        break;

    case Atom::Code:
        writer.writeTextElement("code", trimmedTrailing(plainCode(atom->string())));
        break;

    case Atom::CodeBad:
        writer.writeTextElement("badcode", trimmedTrailing(plainCode(atom->string())));
        break;

    case Atom::CodeNew:
        writer.writeTextElement("para", "you can rewrite it as");
        writer.writeTextElement("newcode", trimmedTrailing(plainCode(atom->string())));
        break;

    case Atom::CodeOld:
        writer.writeTextElement("para", "For example, if you have code like");
        writer.writeTextElement("oldcode", trimmedTrailing(plainCode(atom->string())));
        break;

    case Atom::CodeQuoteArgument:
        if (quoteCommand == "dots") {
            writer.writeAttribute("indent", atom->string());
            writer.writeCharacters("...");
        } else
            writer.writeCharacters(atom->string());
        writer.writeEndElement(); // code
        break;

    case Atom::CodeQuoteCommand:
        quoteCommand = atom->string();
        writer.writeStartElement(quoteCommand);
        break;

    case Atom::FootnoteLeft:
        writer.writeStartElement("footnote");
        break;

    case Atom::FootnoteRight:
        writer.writeEndElement(); // footnote
        break;
/*
    case Atom::FormatElse:
        writer.writeStartElement("else");
        writer.writeEndElement(); // else
        break;
*/
    case Atom::FormatEndif:
        writer.writeEndElement(); // raw
        break;
    case Atom::FormatIf:
        writer.writeStartElement("raw");
        writer.writeAttribute("format", atom->string());
        break;
    case Atom::FormattingLeft:
	{
            if (atom->string() == ATOM_FORMATTING_BOLD)
                writer.writeStartElement("bold");
	    else if (atom->string() == ATOM_FORMATTING_ITALIC)
                writer.writeStartElement("italic");
	    else if (atom->string() == ATOM_FORMATTING_UNDERLINE)
                writer.writeStartElement("underline");
	    else if (atom->string() == ATOM_FORMATTING_SUBSCRIPT)
                writer.writeStartElement("subscript");
	    else if (atom->string() == ATOM_FORMATTING_SUPERSCRIPT)
                writer.writeStartElement("superscript");
	    else if (atom->string() == ATOM_FORMATTING_TELETYPE)
                writer.writeStartElement("teletype");
	    else if (atom->string() == ATOM_FORMATTING_PARAMETER)
                writer.writeStartElement("argument");
	    else if (atom->string() == ATOM_FORMATTING_INDEX)
                writer.writeStartElement("index");
        }
        break;
/*        out() << formattingLeftMap()[atom->string()];
        if ( atom->string() == ATOM_FORMATTING_PARAMETER ) {
            if ( atom->next() != 0 && atom->next()->type() == Atom::String ) {
                QRegExp subscriptRegExp( "([a-z]+)_([0-9n])" );
                if ( subscriptRegExp.exactMatch(atom->next()->string()) ) {
                    out() << subscriptRegExp.cap( 1 ) << "<sub>"
                          << subscriptRegExp.cap( 2 ) << "</sub>";
                    skipAhead = 1;
                }
            }
        }*/
    case Atom::FormattingRight:
	{
            if (atom->string() == ATOM_FORMATTING_BOLD)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_ITALIC)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_UNDERLINE)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_SUBSCRIPT)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_SUPERSCRIPT)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_TELETYPE)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_PARAMETER)
                writer.writeEndElement();
	    else if (atom->string() == ATOM_FORMATTING_INDEX)
                writer.writeEndElement();
        }
        if (inLink) {
            writer.writeEndElement(); // link
            inLink = false;
        }
	break;
/*        if ( atom->string() == ATOM_FORMATTING_LINK ) {
            if (inLink) {
                if ( link.isEmpty() ) {
                    if (showBrokenLinks)
                        out() << "</i>";
                } else {
                    out() << "</a>";
                }
            }
            inLink = false;
        } else {
            out() << formattingRightMap()[atom->string()];
        }*/
    case Atom::GeneratedList:
        writer.writeStartElement("generatedlist");
        writer.writeAttribute("contents", atom->string());
        writer.writeEndElement(); // generatedlist
/*
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
                QMap<QString, const Node *> editionClasses;
                foreach (const QString &moduleName, editionModuleMap[editionName]) {
                    if (moduleClassMap.contains(moduleName))
                        editionClasses.unite(moduleClassMap[moduleName]);
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
                foreach (Node *node, fake->groupMembers()) {
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
*/
        break;
    case Atom::Image:
        writer.writeStartElement("image");
        writer.writeAttribute("href", imageFileName(relative, atom->string()));
        writer.writeEndElement(); // image
        break;

    case Atom::InlineImage:
        writer.writeStartElement("inlineimage");
        writer.writeAttribute("href", imageFileName(relative, atom->string()));
        writer.writeEndElement(); // inlineimage
        break;

    case Atom::ImageText:
        break;

    case Atom::LegaleseLeft:
        writer.writeStartElement("legalese");
        break;

    case Atom::LegaleseRight:
        writer.writeEndElement(); // legalese
        break;

    case Atom::Link:
    case Atom::LinkNode:
        if (!inLink) {
            const Node *node = findNode(atom, relative, marker);
            if (node)
                startLink(writer, atom, node, relative);
        }
        break;

    case Atom::ListLeft:
        writer.writeStartElement("list");

        if (atom->string() == ATOM_LIST_BULLET)
            writer.writeAttribute("type", "bullet");
        else if (atom->string() == ATOM_LIST_TAG)
            writer.writeAttribute("type", "definition");
        else if (atom->string() == ATOM_LIST_VALUE)
            writer.writeAttribute("type", "enum");
        else {
            writer.writeAttribute("type", "ordered");
            if (atom->string() == ATOM_LIST_UPPERALPHA)
                writer.writeAttribute("start", "A");
            else if (atom->string() == ATOM_LIST_LOWERALPHA)
                writer.writeAttribute("start", "a");
            else if (atom->string() == ATOM_LIST_UPPERROMAN)
                writer.writeAttribute("start", "I");
            else if (atom->string() == ATOM_LIST_LOWERROMAN)
                writer.writeAttribute("start", "i");
            else // (atom->string() == ATOM_LIST_NUMERIC)
                writer.writeAttribute("start", "1");
        }
        break;

    case Atom::ListItemNumber:
        break;

    case Atom::ListTagLeft:
        {
            writer.writeStartElement("definition");

            writer.writeTextElement("term", plainCode(
                marker->markedUpEnumValue(atom->next()->string(), relative)));
        }
        break;

    case Atom::ListTagRight:
        writer.writeEndElement(); // definition
        break;

    case Atom::ListItemLeft:
        writer.writeStartElement("item");
        break;

    case Atom::ListItemRight:
        writer.writeEndElement(); // item
        break;

    case Atom::ListRight:
        writer.writeEndElement(); // list
        break;

    case Atom::Nop:
        break;

    case Atom::ParaLeft:
        writer.writeStartElement("para");
        break;

    case Atom::ParaRight:
        writer.writeEndElement(); // para
        break;

    case Atom::QuotationLeft:
        writer.writeStartElement("quote");
        break;

    case Atom::QuotationRight:
        writer.writeEndElement(); // quote
        break;

    case Atom::RawString:
        writer.writeCharacters(atom->string());
        break;

    case Atom::SectionLeft:
        writer.writeStartElement("section");
        writer.writeAttribute("id", Doc::canonicalTitle(Text::sectionHeading(atom).toString()));
        break;

    case Atom::SectionRight:
        writer.writeEndElement(); // section
        break;

    case Atom::SectionHeadingLeft:
        writer.writeStartElement("heading");
        writer.writeAttribute("level", atom->string()); // + hOffset(relative)
        inSectionHeading = true;
        break;

    case Atom::SectionHeadingRight:
        writer.writeEndElement(); // heading
        inSectionHeading = false;
        break;

    case Atom::SidebarLeft:
    case Atom::SidebarRight:
        break;

    case Atom::SnippetCommand:
        writer.writeStartElement(atom->string());
        break;

    case Atom::SnippetIdentifier:
        writer.writeAttribute("identifier", atom->string());
        writer.writeEndElement(); // snippet
        break;

    case Atom::SnippetLocation:
        writer.writeAttribute("location", atom->string());
        break;

    case Atom::String:
        writer.writeCharacters(atom->string());
        break;

    case Atom::TableLeft:
        writer.writeStartElement("table");
        if (atom->string().contains("%"))
            writer.writeAttribute("width", atom->string());
        break;

    case Atom::TableRight:
        writer.writeEndElement(); // table
        break;

    case Atom::TableHeaderLeft:
        writer.writeStartElement("header");
        break;

    case Atom::TableHeaderRight:
        writer.writeEndElement(); // header
        break;

    case Atom::TableRowLeft:
        writer.writeStartElement("row");
        break;

    case Atom::TableRowRight:
        writer.writeEndElement(); // row
        break;

    case Atom::TableItemLeft:
        {
            writer.writeStartElement("item");
            QStringList spans = atom->string().split(",");
            if (spans.size() == 2) {
                if (spans.at(0) != "1")
                    writer.writeAttribute("colspan", spans.at(0).trimmed());
                if (spans.at(1) != "1")
                    writer.writeAttribute("rowspan", spans.at(1).trimmed());
            }
        }
        break;

    case Atom::TableItemRight:
        writer.writeEndElement(); // item
        break;

    case Atom::TableOfContents:
        writer.writeStartElement("tableofcontents");
        writer.writeAttribute("details", atom->string());
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
                node = findNode(path, relative, marker);
                if (node)
                    writer.writeAttribute("href", fileName(node));
            }

            if (params.size() == 2) {
                numColumns = qMax(columnText.toInt(), numColumns);
                sectioningUnit = (Doc::SectioningUnit)params.at(1).toInt();
                writer.writeAttribute("columns", QString::number(numColumns));
                writer.writeAttribute("unit", QString::number(sectioningUnit));
            }

            if (node)
                generateTableOfContents(writer, node, sectioningUnit, numColumns,
                                        relative);
        }
        writer.writeEndElement(); // tableofcontents
        break;

    case Atom::Target:
        writer.writeStartElement("target");
        writer.writeAttribute("name", Doc::canonicalTitle(atom->string()));
        writer.writeEndElement(); // target
        break;

    case Atom::UnhandledFormat:
    case Atom::UnknownCommand:
        writer.writeCharacters(atom->typeString());
        break;
    default:
        break;
    }

    if (atom)
        return atom->next();

    return 0;
}
/*
        QDomElement atomElement = document.createElement(atom->typeString().toLower());
        QDomText atomValue = document.createTextNode(atom->string());
        atomElement.appendChild(atomValue);
        descriptionElement.appendChild(atomElement);
*/

/*
    ### Warning: findNode() is a modified version of HtmlGenerator::getLink().
*/
const Node *WebXMLGenerator::findNode(const Atom *atom, const Node *relative, CodeMarker *marker)
{
    return findNode(atom->string(), relative, marker);
}

const Node *WebXMLGenerator::findNode(const QString &title, const Node *relative, CodeMarker *marker)
{
    QString link;
    if (title.contains(":") &&
            (title.startsWith("file:")
             || title.startsWith("http:")
             || title.startsWith("https:")
             || title.startsWith("ftp:")
             || title.startsWith("mailto:"))) {

        return 0;
    } else if (title.count('@') == 1) {
        return 0;
    } else {
        QStringList path;
        if (title.contains('#')) {
            path = title.split('#');
        } else {
            path.append(title);
        }

        const Node *node = 0;
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
                return node;
            else
                path.removeFirst();
        } else {
            return 0;
        }

        while (!path.isEmpty()) {
            targetAtom = tre->findTarget(path.first(), node);
            if (targetAtom == 0)
                break;
            path.removeFirst();
        }
/* We would ideally treat targets as nodes to be consistent.
        if (targetAtom && node && node->isInnerNode()) {
            Node *parentNode = const_cast<Node *>(node);
            node = new TargetNode(static_cast<InnerNode*>(parentNode), first);
        }
*/
        return node;
    }
    return 0;
}

void WebXMLGenerator::startLink(QXmlStreamWriter &writer, const Atom *atom,
                                const Node *node, const Node *relative)
{
    QString location = tre->fullDocumentLocation(node);
    if (!location.isEmpty()) {
        writer.writeStartElement("link");
        writer.writeAttribute("raw", atom->string());
        if (atom->string().contains("#") || node == relative) {
            QString target = atom->string().split("#").last();
            Atom *targetAtom = tre->findTarget(target, node);
            if (targetAtom)
                location += "#" + Doc::canonicalTitle(target);
        }
        writer.writeAttribute("href", location);
        QString type = targetType(node);
        writer.writeAttribute("type", type);
        switch (node->type()) {
        case Node::Enum:
            writer.writeAttribute("enum", tre->fullDocumentName(node));
            break;
        case Node::Fake:
            writer.writeAttribute("page", tre->fullDocumentName(node));
            break;
        case Node::Property:
        {
            const PropertyNode *propertyNode = static_cast<const PropertyNode *>(node);
            if (propertyNode->getters().size() > 0)
                writer.writeAttribute("getter", tre->fullDocumentName(propertyNode->getters()[0]));
        }
        default:
            ;
        }
        inLink = true;
    }
}

QString WebXMLGenerator::targetType(const Node *node)
{
    switch (node->type()) {
        case Node::Namespace:
            return "namespace";
            break;
        case Node::Class:
            return "class";
            break;
        case Node::Fake:
            return "page";
            break;
        case Node::Enum:
            return "enum";
            break;
        case Node::Typedef:
            return "typedef";
            break;
        case Node::Property:
            return "property";
            break;
        case Node::Function:
            return "function";
            break;
        case Node::Variable:
            return "variable";
            break;
        case Node::Target:
            return "target";
            break;
        default:
            return "";
    }
    return "";
}

void WebXMLGenerator::generateRelations(QXmlStreamWriter &writer, const Node *node, CodeMarker *marker)
{
    if (node && !node->links().empty()) {
        QPair<QString,QString> linkPair;
        QPair<QString,QString> anchorPair;
        const Node *linkNode;

        foreach (Node::LinkType relation, node->links().keys()) {

            linkPair = node->links()[relation];
            linkNode = findNode(linkPair.first, node, marker);
            
            if (!linkNode)
                linkNode = node;

            if (linkNode == node)
                anchorPair = linkPair;
            else
                anchorPair = anchorForNode(linkNode);

            writer.writeStartElement("relation");
            writer.writeAttribute("href", anchorPair.first);
            writer.writeAttribute("type", targetType(linkNode));
            
            switch (relation) {
            case Node::StartLink:
                writer.writeAttribute("meta", "start");
                break;
            case Node::NextLink:
                writer.writeAttribute("meta", "next");
                break;
            case Node::PreviousLink:
                writer.writeAttribute("meta", "previous");
                break;
            case Node::ContentsLink: 
                writer.writeAttribute("meta", "contents");
                break;
            case Node::IndexLink:
                writer.writeAttribute("meta", "index");
                break;
            default:
                writer.writeAttribute("meta", "");
            }
            writer.writeAttribute("description", anchorPair.second);
            writer.writeEndElement(); // link
        }
    }
}

// Classes adapted from HtmlGenerator.

void WebXMLGenerator::generateTableOfContents(QXmlStreamWriter &writer, const Node *node,
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

    if (numColumns > 1) {
        writer.writeStartElement("table");
        writer.writeAttribute("width", "100%");
        writer.writeStartElement("row");
        writer.writeStartElement("item");
        writer.writeAttribute("width", QString::number((100 + numColumns - 1) / numColumns) + "%");
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
                writer.writeStartElement("list");
                sectionNumber.append("1");
            } while (sectionNumber.size() < nextLevel);
        } else {
            while (sectionNumber.size() > nextLevel) {
                writer.writeEndElement();
                sectionNumber.removeLast();
            }
            sectionNumber.last() = QString::number(sectionNumber.last().toInt() + 1);
        }
        Text headingText = Text::sectionHeading(atom);

        if (sectionNumber.size() == 1 && columnSize > toc.size() / numColumns) {
            writer.writeEndElement(); // list
            writer.writeEndElement(); // item
            writer.writeStartElement("item");
            writer.writeAttribute("width", QString::number((100 + numColumns - 1) / numColumns) + "%");
            writer.writeStartElement("list");
            columnSize = 0;
        }

        writer.writeStartElement("item");
        writer.writeStartElement("para");
        writer.writeStartElement("link");
        writer.writeAttribute("href", nodeName + "#" + Doc::canonicalTitle(headingText.toString()));
        writer.writeAttribute("type", "page");
        writer.writeCharacters(headingText.toString());
        writer.writeEndElement(); // link
        writer.writeEndElement(); // para
        writer.writeEndElement(); // item

        ++columnSize;
    }
    while (!sectionNumber.isEmpty()) {
        writer.writeEndElement(); // list
        sectionNumber.removeLast();
    }

    if (numColumns > 1) {
        writer.writeEndElement(); // item
        writer.writeEndElement(); // row
        writer.writeEndElement(); // table
    }

    inContents = false;
    inLink = false;
}

void WebXMLGenerator::generateAnnotatedList(QXmlStreamWriter &writer,
    const Node *relative, CodeMarker *marker, const QMap<QString, const Node *> &nodeMap)
{
    writer.writeStartElement("table");
    writer.writeAttribute("width", "100%");

    foreach (QString name, nodeMap.keys()) {
        const Node *node = nodeMap[name];

        writer.writeStartElement("row");
        writer.writeStartElement("heading");
        generateFullName(writer, node, relative, marker);
        writer.writeEndElement(); // heading

        writer.writeStartElement("item");
        writer.writeCharacters(node->doc().briefText().toString());
        writer.writeEndElement(); // item
        writer.writeEndElement(); // row
    }
    writer.writeEndElement(); // table
}

void WebXMLGenerator::generateFullName(QXmlStreamWriter &writer,
    const Node *apparentNode, const Node *relative, CodeMarker *marker,
    const Node *actualNode)
{
    if ( actualNode == 0 )
        actualNode = apparentNode;
    writer.writeStartElement("link");
    writer.writeAttribute("href", tre->fullDocumentLocation(actualNode));
    writer.writeAttribute("type", targetType(actualNode));
    writer.writeCharacters(fullName(apparentNode, relative, marker));
    writer.writeEndElement(); // link
}

// Classes copied (and slightly adapted) from the HtmlGenerator. These need
// refactoring into a common ancestor class.

void WebXMLGenerator::findAllClasses(const InnerNode *node)
{
    NodeList::const_iterator c = node->childNodes().constBegin();
    while (c != node->childNodes().constEnd()) {
        if ((*c)->access() != Node::Private && (*c)->url().isEmpty()) {
            if ((*c)->type() == Node::Class && !(*c)->doc().isEmpty()) {
                QString className = (*c)->name();
                if ((*c)->parent() && (*c)->parent()->type() == Node::Namespace &&
                    !(*c)->parent()->name().isEmpty())
                    className = (*c)->parent()->name()+"::"+className;

                QString moduleName = (*c)->moduleName();
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

void WebXMLGenerator::findAllNamespaces(const InnerNode *node)
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
                        if (!moduleName.isEmpty())
                            moduleNamespaceMap[moduleName].insert((*c)->name(), *c);
                    }
                }
            }
        }
        ++c;
    }
}

const QPair<QString,QString> WebXMLGenerator::anchorForNode(const Node *node)
{
    QPair<QString,QString> anchorPair;

    anchorPair.first = PageGenerator::fileName(node);
    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode*>(node);
        anchorPair.second = fakeNode->title();
    }

    return anchorPair;
}

QT_END_NAMESPACE
