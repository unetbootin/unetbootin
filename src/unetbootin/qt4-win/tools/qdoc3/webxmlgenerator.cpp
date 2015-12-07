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
    PageGenerator::generateTree(tree, marker);

    if (generateIndex)
        tre->generateIndex(outputDir() + "/" + project.toLower() + ".index",
                           projectUrl, projectDescription, true);
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
            foreach (Node *child, inner->childNodes())
                generateIndexSections(writer, child, marker);

            writer.writeStartElement("related");
            if (inner->relatedNodes().size() > 0) {
                foreach (Node *child, inner->relatedNodes())
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
        if (!inLink) {
            writer.writeStartElement("link");
            writer.writeAttribute("raw", atom->string());
            const Node *node = findNode(atom, relative, marker);
            if (node) {
                writer.writeAttribute("href", tre->fullDocumentLocation(node));
                QString type = targetType(node);
                writer.writeAttribute("type", type);
                switch (node->type()) {
                case Node::Enum:
                    writer.writeAttribute("enum", tre->fullDocumentName(node));
                    break;
                case Node::Fake:
                    writer.writeAttribute("page", tre->fullDocumentName(node));
                    break;
                default:
                    ;
                }
            } else
                writer.writeAttribute("href", "");
        }
        writer.writeCharacters(atom->string());
        if (!inLink)
            writer.writeEndElement(); // link
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
                out() << " holds ";
            else
                out() << " ";
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
        writer.writeCharacters(atom->string());
        writer.writeEndElement(); // code
        break;

    case Atom::CodeQuoteCommand:
        writer.writeStartElement(atom->string());
        break;

    case Atom::FootnoteLeft:
        writer.writeStartElement("footnote");
        break;

    case Atom::FootnoteRight:
        writer.writeEndElement(); // footnote
        break;

    case Atom::FormatElse:
        writer.writeStartElement("else");
        writer.writeEndElement(); // else
        break;
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
                foreach (QString moduleName, editionModuleMap[editionName]) {
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
            writer.writeStartElement("link");
            writer.writeAttribute("raw", atom->string());
            const Node *node = findNode(atom, relative, marker);
            if (node) {
                writer.writeAttribute("href", tre->fullDocumentLocation(node));
                QString type = targetType(node);
                writer.writeAttribute("type", type);
                switch (node->type()) {
                case Node::Enum:
                    writer.writeAttribute("enum", tre->fullDocumentName(node));
                    break;
                case Node::Fake:
                    writer.writeAttribute("page", tre->fullDocumentName(node));
                    break;
                default:
                    ;
                }
            } else
                writer.writeAttribute("href", "");
            inLink = true;
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
        writer.writeStartElement("item");
        break;

    case Atom::TableItemRight:
        writer.writeEndElement(); // item
        break;

    case Atom::TableOfContents:
        writer.writeStartElement("tableofcontents");
        writer.writeAttribute("details", atom->string());
/*        {
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
        }*/
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
    QString link;
    if (atom->string().contains(":") &&
            (atom->string().startsWith("file:")
             || atom->string().startsWith("http:")
             || atom->string().startsWith("https:")
             || atom->string().startsWith("ftp:")
             || atom->string().startsWith("mailto:"))) {

        return 0;
    } else if (atom->string().count('@') == 1) {
        return 0;
    } else {
        QStringList path;
        if (atom->string().contains('#')) {
            path = atom->string().split('#');
        } else {
            path.append(atom->string());
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
                return 0;
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

        return node;
    }
    return 0;
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

QT_END_NAMESPACE
