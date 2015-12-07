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

#include <QtXml>
#include <QHash>
#include <QMap>

#include "atom.h"
#include "helpprojectwriter.h"
#include "config.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

HelpProjectWriter::HelpProjectWriter(const Config &config, const QString &defaultFileName)
{
    // The output directory should already have been checked by the calling
    // generator.
    outputDir = config.getString(CONFIG_OUTPUTDIR);

    QStringList names = config.getStringList(CONFIG_QHP + Config::dot + "projects");

    foreach (QString projectName, names) {
        HelpProject project;
        project.name = projectName;

        QString prefix = CONFIG_QHP + Config::dot + projectName + Config::dot;
        project.helpNamespace = config.getString(prefix + "namespace");
        project.virtualFolder = config.getString(prefix + "virtualFolder");
        project.fileName = config.getString(prefix + "file");
        if (project.fileName.isEmpty())
            project.fileName = defaultFileName;
        project.extraFiles = config.getStringSet(prefix + "extraFiles");
        project.indexTitle = config.getString(prefix + "indexTitle");
        project.indexRoot = config.getString(prefix + "indexRoot");
        project.filterAttributes = config.getStringList(prefix + "filterAttributes").toSet();
        QSet<QString> customFilterNames = config.subVars(prefix + "customFilters");
        foreach (QString filterName, customFilterNames) {
            QString name = config.getString(prefix + "customFilters" + Config::dot + filterName + Config::dot + "name");
            QSet<QString> filters = config.getStringList(prefix + "customFilters" + Config::dot + filterName + Config::dot + "filterAttributes").toSet();
            project.customFilters[name] = filters;
        }
        //customFilters = config.defs.
        
        foreach (QString name, config.getStringSet(prefix + "excluded"))
            project.excluded.insert(name.replace("\\", "/"));
        
        foreach (QString name, config.getStringList(prefix + "subprojects")) {
            SubProject subproject;
            QString subprefix = prefix + "subprojects" + Config::dot + name + Config::dot;
            subproject.title = config.getString(subprefix + "title");
            subproject.indexTitle = config.getString(subprefix + "indexTitle");
            readSelectors(subproject, config.getStringList(subprefix + "selectors"));
            project.subprojects[name] = subproject;
        }

        if (project.subprojects.isEmpty()) {
            SubProject subproject;
            readSelectors(subproject, config.getStringList(prefix + "selectors"));
            project.subprojects[""] = subproject;
        }

        projects.append(project);
    }
}

void HelpProjectWriter::readSelectors(SubProject &subproject, const QStringList &selectors)
{
    QHash<QString, Node::Type> typeHash;
    typeHash["namespace"] = Node::Namespace;
    typeHash["class"] = Node::Class;
    typeHash["fake"] = Node::Fake;
    typeHash["enum"] = Node::Enum;
    typeHash["typedef"] = Node::Typedef;
    typeHash["function"] = Node::Function;
    typeHash["property"] = Node::Property;
    typeHash["variable"] = Node::Variable;
    typeHash["target"] = Node::Target;
    
    QHash<QString, FakeNode::SubType> subTypeHash;
    subTypeHash["example"] = FakeNode::Example;
    subTypeHash["headerfile"] = FakeNode::HeaderFile;
    subTypeHash["file"] = FakeNode::File;
    subTypeHash["group"] = FakeNode::Group;
    subTypeHash["module"] = FakeNode::Module;
    subTypeHash["page"] = FakeNode::Page;
    subTypeHash["externalpage"] = FakeNode::ExternalPage;
    
    QSet<FakeNode::SubType> allSubTypes = QSet<FakeNode::SubType>::fromList(subTypeHash.values());
    
    foreach (QString selector, selectors) {
        QStringList pieces = selector.split(":");
        if (pieces.size() == 1) {
            QString lower = selector.toLower();
            if (typeHash.contains(lower))
                subproject.selectors[typeHash[lower]] = allSubTypes;
        } else if (pieces.size() >= 2) {
            QString lower = pieces[0].toLower();
            pieces = pieces[1].split(",");
            if (typeHash.contains(lower)) {
                QSet<FakeNode::SubType> subTypes;
                for (int i = 0; i < pieces.size(); ++i) {
                    QString lower = pieces[i].toLower();
                    if (subTypeHash.contains(lower))
                        subTypes.insert(subTypeHash[lower]);
                }
                subproject.selectors[typeHash[lower]] = subTypes;
            }
        }
    }
}

void HelpProjectWriter::addExtraFile(const QString &file)
{
    foreach (HelpProject project, projects)
        project.extraFiles.insert(file);
}

void HelpProjectWriter::addExtraFiles(const QSet<QString> &files)
{
    foreach (HelpProject project, projects)
        project.extraFiles.unite(files);
}

/*
    Returns a list of strings describing the keyword details for a given node.

    The first string is the human-readable name to be shown in Assistant.
    The second string is a unique identifier.
    The third string is the location of the documentation for the keyword.
*/
QStringList HelpProjectWriter::keywordDetails(const Node *node) const
{
    QStringList details;

    if (node->parent() && !node->parent()->name().isEmpty()) {
        // "name"
        if (node->type() == Node::Enum || node->type() == Node::Typedef)
            details << node->parent()->name()+"::"+node->name();
        else
            details << node->name();
        // "id"
        details << node->parent()->name()+"::"+node->name();
    } else if (node->type() == Node::Fake) {
        const FakeNode *fake = static_cast<const FakeNode *>(node);
        details << fake->fullTitle();
        details << fake->fullTitle();
    } else {
        details << node->name();
        details << node->name();
    }
    details << tree->fullDocumentLocation(node);

    return details;
}

bool HelpProjectWriter::generateSection(HelpProject &project,
                        QXmlStreamWriter & /* writer */, const Node *node)
{
    if (!node->url().isEmpty())
        return false;

    if (node->access() == Node::Private)
        return false;

    if (node->name().isEmpty())
        return true;

    QString docPath = node->doc().location().filePath();
    if (!docPath.isEmpty() && project.excluded.contains(docPath))
        return false;

    QString objName;
    if (node->type() == Node::Fake) {
        const FakeNode *fake = static_cast<const FakeNode *>(node);
        objName = fake->fullTitle();
    } else
        objName = tree->fullDocumentName(node);

    // Only add nodes to the set for each subproject if they match a selector.
    // Those that match will be listed in the table of contents.

    foreach (QString name, project.subprojects.keys()) {
        SubProject subproject = project.subprojects[name];
        // No selectors: accept all nodes.
        if (subproject.selectors.isEmpty())
            project.subprojects[name].nodes[objName] = node;
        else if (subproject.selectors.contains(node->type())) {
            // Accept only the node types in the selectors hash.
            if (node->type() != Node::Fake)
                project.subprojects[name].nodes[objName] = node;
            else {
                // Accept only fake nodes with subtypes contained in the selector's
                // mask.
                const FakeNode *fakeNode = static_cast<const FakeNode *>(node);
                if (subproject.selectors[node->type()].contains(fakeNode->subType()) &&
                    fakeNode->subType() != FakeNode::ExternalPage &&
                    !fakeNode->fullTitle().isEmpty())
                
                    project.subprojects[name].nodes[objName] = node;
            }
        }
    }

    switch (node->type()) {

        case Node::Class:
            project.keywords.append(keywordDetails(node));
            project.files.insert(tree->fullDocumentLocation(node));
            break;

        case Node::Namespace:
            project.keywords.append(keywordDetails(node));
            project.files.insert(tree->fullDocumentLocation(node));
            break;

        case Node::Enum:
            project.keywords.append(keywordDetails(node));
            {
                const EnumNode *enumNode = static_cast<const EnumNode*>(node);
                foreach (EnumItem item, enumNode->items()) {
                    QStringList details;
                    if (!node->parent()->name().isEmpty()) {
                        details << node->parent()->name()+"::"+item.name(); // "name"
                        details << node->parent()->name()+"::"+item.name(); // "id"
                    } else {
                        details << item.name(); // "name"
                        details << item.name(); // "id"
                    }
                    details << tree->fullDocumentLocation(node);
                    project.keywords.append(details);
                }
            }
            break;

        case Node::Property:
            project.keywords.append(keywordDetails(node));
            break;

        case Node::Function:
            {
                const FunctionNode *funcNode = static_cast<const FunctionNode *>(node);
                if (funcNode->metaness() != FunctionNode::Ctor)
                    project.keywords.append(keywordDetails(node));
                if (node->parent())
                    project.memberStatus[node->parent()].insert(node->status());
            }
            break;

        case Node::Typedef:
            {
                const TypedefNode *typedefNode = static_cast<const TypedefNode *>(node);
                QStringList typedefDetails = keywordDetails(node);
                const EnumNode *enumNode = typedefNode->associatedEnum();
                // Use the location of any associated enum node in preference
                // to that of the typedef.
                if (enumNode)
                    typedefDetails[2] = tree->fullDocumentLocation(enumNode);

                project.keywords.append(typedefDetails);
            }
            break;

        // Fake nodes (such as manual pages) contain subtypes, titles and other
        // attributes.
        case Node::Fake: {
            const FakeNode *fakeNode = static_cast<const FakeNode*>(node);
            if (fakeNode->subType() != FakeNode::ExternalPage &&
                !fakeNode->fullTitle().isEmpty()) {

                if (fakeNode->subType() != FakeNode::File) {
                    if (fakeNode->doc().hasKeywords()) {
                        foreach (Atom *keyword, fakeNode->doc().keywords()) {
                            if (!keyword->string().isEmpty()) {
                                QStringList details;
                                details << keyword->string()
                                        << keyword->string()
                                        << tree->fullDocumentLocation(node) + "#" + Doc::canonicalTitle(keyword->string());
                                project.keywords.append(details);
                            } else
                                fakeNode->doc().location().warning(
                                    tr("Bad keyword in %1").arg(tree->fullDocumentLocation(node))
                                    );
                        }
                    }
                    project.keywords.append(keywordDetails(node));
                }
/*
                if (fakeNode->doc().hasTableOfContents()) {
                    foreach (Atom *item, fakeNode->doc().tableOfContents()) {
                        QString title = Text::sectionHeading(item).toString();
                        if (!title.isEmpty()) {
                            QStringList details;
                            details << title
                                    << title
                                    << tree->fullDocumentLocation(node) + "#" + Doc::canonicalTitle(title);
                            project.keywords.append(details);
                        } else
                            fakeNode->doc().location().warning(
                                tr("Bad contents item in %1").arg(tree->fullDocumentLocation(node))
                                );
                    }
                }
*/
                project.files.insert(tree->fullDocumentLocation(node));
            }
            break;
            }
        default:
            ;
    }

    // Add all images referenced in the page to the set of files to include.
    const Atom *atom = node->doc().body().firstAtom();
    while (atom) {
        if (atom->type() == Atom::Image || atom->type() == Atom::InlineImage) {
            // Images are all placed within a single directory regardless of
            // whether the source images are in a nested directory structure.
            QStringList pieces = atom->string().split("/");
            project.files.insert("images/" + pieces.last());
        }
        atom = atom->next();
    }

    return true;
}

void HelpProjectWriter::generateSections(HelpProject &project,
                        QXmlStreamWriter &writer, const Node *node)
{
    if (!generateSection(project, writer, node))
        return;

    if (node->isInnerNode()) {
        const InnerNode *inner = static_cast<const InnerNode *>(node);

        // Ensure that we don't visit nodes more than once.
        QMap<QString, Node*> childMap;
        foreach (Node *node, inner->childNodes()) {
            if (node->access() == Node::Private)
                continue;
            if (node->type() == Node::Fake)
                childMap[static_cast<FakeNode *>(node)->fullTitle()] = node;
            else {
                if (node->type() == Node::Function) {
                    const FunctionNode *funcNode = static_cast<const FunctionNode *>(node);
                    if (funcNode->isOverload())
                        continue;
                }
                childMap[tree->fullDocumentName(node)] = node;
            }
        }

        foreach (Node *child, childMap)
            generateSections(project, writer, child);
    }
}

void HelpProjectWriter::generate(const Tree *tre)
{
    this->tree = tre;
    foreach (HelpProject project, projects)
        generateProject(project);
}

void HelpProjectWriter::writeNode(HelpProject &project, QXmlStreamWriter &writer,
                                  const Node *node)
{
    QString href = tree->fullDocumentLocation(node);
    QString objName = node->name();

    switch (node->type()) {

        case Node::Class:
            writer.writeStartElement("section");
            writer.writeAttribute("ref", href);
            if (node->parent() && !node->parent()->name().isEmpty())
                writer.writeAttribute("title", tr("%1::%2 Class Reference").arg(node->parent()->name()).arg(objName));
            else
                writer.writeAttribute("title", tr("%1 Class Reference").arg(objName));

            // Write subsections for all members, obsolete members and Qt 3
            // members.
            if (!project.memberStatus[node].isEmpty()) {
                QString membersPath = href.left(href.size()-5) + "-members.html";
                writer.writeStartElement("section");
                writer.writeAttribute("ref", membersPath);
                writer.writeAttribute("title", tr("List of all members"));
                writer.writeEndElement(); // section
                project.files.insert(membersPath);
            }
            if (project.memberStatus[node].contains(Node::Compat)) {
                QString compatPath = href.left(href.size()-5) + "-qt3.html";
                writer.writeStartElement("section");
                writer.writeAttribute("ref", compatPath);
                writer.writeAttribute("title", tr("Qt 3 support members"));
                writer.writeEndElement(); // section
                project.files.insert(compatPath);
            }
            if (project.memberStatus[node].contains(Node::Obsolete)) {
                QString obsoletePath = href.left(href.size()-5) + "-obsolete.html";
                writer.writeStartElement("section");
                writer.writeAttribute("ref", obsoletePath);
                writer.writeAttribute("title", tr("Obsolete members"));
                writer.writeEndElement(); // section
                project.files.insert(obsoletePath);
            }
            
            writer.writeEndElement(); // section
            break;

        case Node::Namespace:
            writer.writeStartElement("section");
            writer.writeAttribute("ref", href);
            writer.writeAttribute("title", objName);
            writer.writeEndElement(); // section
            break;

        case Node::Fake: {
            // Fake nodes (such as manual pages) contain subtypes, titles and other
            // attributes.
            const FakeNode *fakeNode = static_cast<const FakeNode*>(node);

            writer.writeStartElement("section");
            writer.writeAttribute("ref", href);
            writer.writeAttribute("title", fakeNode->fullTitle());
            writer.writeEndElement(); // section
            }
            break;
        default:
            ;
    }
}

void HelpProjectWriter::generateProject(HelpProject &project)
{
    const Node *rootNode;
    if (!project.indexRoot.isEmpty())
        rootNode = tree->findFakeNodeByTitle(project.indexRoot);
    else
        rootNode = tree->root();

    if (!rootNode)
        return;

    project.files.clear();
    project.keywords.clear();

    QFile file(outputDir + QDir::separator() + project.fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("QtHelpProject");
    writer.writeAttribute("version", "1.0");

    // Write metaData, virtualFolder and namespace elements.
    writer.writeTextElement("namespace", project.helpNamespace);
    writer.writeTextElement("virtualFolder", project.virtualFolder);

    // Write customFilter elements.
    QHash<QString, QSet<QString> >::ConstIterator it;
    for (it = project.customFilters.begin(); it != project.customFilters.end(); ++it) {
        writer.writeStartElement("customFilter");
        writer.writeAttribute("name", it.key());
        foreach (QString filter, it.value())
            writer.writeTextElement("filterAttribute", filter);
        writer.writeEndElement(); // customFilter
    }

    // Start the filterSection.
    writer.writeStartElement("filterSection");

    // Write filterAttribute elements.
    foreach (QString filterName, project.filterAttributes)
        writer.writeTextElement("filterAttribute", filterName);

    writer.writeStartElement("toc");
    writer.writeStartElement("section");
    QString indexPath = tree->fullDocumentLocation(tree->findFakeNodeByTitle(project.indexTitle));
    if (indexPath.isEmpty())
        indexPath = "index.html";
    writer.writeAttribute("ref", indexPath);
    writer.writeAttribute("title", project.indexTitle);
    project.files.insert(tree->fullDocumentLocation(rootNode));

    generateSections(project, writer, rootNode);

    foreach (QString name, project.subprojects.keys()) {
        SubProject subproject = project.subprojects[name];

        if (!name.isEmpty()) {
            writer.writeStartElement("section");
            QString indexPath = tree->fullDocumentLocation(tree->findFakeNodeByTitle(subproject.indexTitle));
            writer.writeAttribute("ref", indexPath);
            writer.writeAttribute("title", subproject.title);
            project.files.insert(indexPath);
        }

        foreach (const Node *node, subproject.nodes)
            writeNode(project, writer, node);

        if (!name.isEmpty())
            writer.writeEndElement(); // section
    }

    writer.writeEndElement(); // section
    writer.writeEndElement(); // toc

    writer.writeStartElement("keywords");
    foreach (QStringList details, project.keywords) {
        writer.writeStartElement("keyword");
        writer.writeAttribute("name", details[0]);
        writer.writeAttribute("id", details[1]);
        writer.writeAttribute("ref", details[2]);
        writer.writeEndElement(); //keyword
    }
    writer.writeEndElement(); // keywords

    writer.writeStartElement("files");
    foreach (QString usedFile, project.files) {
        if (!usedFile.isEmpty())
            writer.writeTextElement("file", usedFile);
    }
    foreach (QString usedFile, project.extraFiles)
        writer.writeTextElement("file", usedFile);
    writer.writeEndElement(); // files

    writer.writeEndElement(); // filterSection
    writer.writeEndElement(); // QtHelpProject
    writer.writeEndDocument();
    file.close();
}

QT_END_NAMESPACE
