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
    jambiapiparser.cpp
*/

#include <QtXml>

#include "cppcodeparser.h"
#include "jambiapiparser.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

static const char USED_INTERNALLY[] = "";

static Text textWithFixedBrief(const Text &text, const Text &beforeBrief,
                               const Text &afterBrief)
{
    Text result;

    const Atom *atom = text.firstAtom();
    while (atom) {
        if (atom->type() == Atom::BriefLeft) {
            result << Atom::ParaLeft << beforeBrief;
        } else if (atom->type() == Atom::BriefRight) {
            result << afterBrief << Atom::ParaRight;
        } else {
            result << *atom;
        }
        atom = atom->next();
    }

    return result;
}

static void setPass1JambifiedDoc(Node *javaNode, const Node *cppNode, const QString &qName = "")
{
    Doc newDoc(cppNode->doc());

    if (javaNode->type() == Node::Function) {
        const FunctionNode *javaFunc = static_cast<const FunctionNode *>(javaNode);
        if (cppNode->type() == Node::Function) {
            const FunctionNode *cppFunc = static_cast<const FunctionNode *>(cppNode);
            if (const PropertyNode *property = cppFunc->associatedProperty()) {
                newDoc = property->doc();
                Text text(newDoc.body());

                Node *mutableCppNode = const_cast<Node *>(cppNode);
                if (property->getters().contains(mutableCppNode)) {
                    text = textWithFixedBrief(text, Text("Returns "), Text("."));
                } else if (property->setters().contains(mutableCppNode)) {
                    Text afterBrief;
                    if (javaFunc->parameterNames().count() == 1
                            && !javaFunc->parameterNames().first().isEmpty()) {
                        afterBrief << " to "
                                   << Atom(Atom::FormattingLeft, ATOM_FORMATTING_PARAMETER)
                                   << javaFunc->parameterNames().first()
                                   << Atom(Atom::FormattingRight, ATOM_FORMATTING_PARAMETER);
                    }
                    afterBrief << ".";
                    text = textWithFixedBrief(text, Text("Sets "), afterBrief);
                } else if (property->resetters().contains(mutableCppNode)) {
                    text = textWithFixedBrief(text, Text("Resets "), Text("."));
                }

                newDoc.setBody(text);
            } else {
                QStringList javaParams = javaFunc->parameterNames();
                QStringList cppParams = cppFunc->parameterNames();
                newDoc.renameParameters(cppParams, javaParams);

                if (cppNode->access() == Node::Private) {
                    Text text;
                    text << Atom::ParaLeft;
                    if (cppFunc->reimplementedFrom()) {
                        text << "This function is reimplemented for internal reasons.";
                    } else {
                        text << USED_INTERNALLY;
                    }
                    text << Atom::ParaRight;
                    newDoc.setBody(text);
                }
            }
        } else if (cppNode->type() == Node::Variable) {
            Text text(newDoc.body());

            if (qName == "variablegetter") {
                text = textWithFixedBrief(text, Text("Returns "), Text("."));
            } else if (qName == "variablesetter") {
                Text afterBrief;
                if (javaFunc->parameterNames().count() == 1
                        && !javaFunc->parameterNames().first().isEmpty()) {
                    afterBrief << " to "
                               << Atom(Atom::FormattingLeft, ATOM_FORMATTING_PARAMETER)
                               << javaFunc->parameterNames().first()
                               << Atom(Atom::FormattingRight, ATOM_FORMATTING_PARAMETER);
                }
                afterBrief << ".";
                text = textWithFixedBrief(text, Text("Sets "), afterBrief);
            }

            newDoc.setBody(text);
        }
    } else {    // ### enum value names?

    }

    javaNode->setDoc(newDoc, true);
}

static void setStatus(Node *javaNode, const Node *cppNode)
{
    if (cppNode->status() == Node::Compat) {
        javaNode->setStatus(Node::Obsolete);
    } else {
        javaNode->setStatus(cppNode->status());
    }
}

static Text findEnumText(Node *javaEnum, const QString &enumItemName)
{
    const Text &body = javaEnum->doc().body();
    const Atom *atom = body.firstAtom();
    while (atom) {
        if (atom->type() == Atom::ListTagLeft && atom->string() == ATOM_LIST_VALUE) {
            atom = atom->next();
            if (atom) {
                // ### paras?
                if (atom->string() == enumItemName)
                    return body.subText(Atom::ListItemLeft, Atom::ListItemRight, atom);
            }
        } else {
            atom = atom->next();
        }
    }
    return Text();
}

JambiApiParser::JambiApiParser(Tree *cppTree)
    : cppTre(cppTree), javaTre(0), metJapiTag(false)
{
}

JambiApiParser::~JambiApiParser()
{
}

void JambiApiParser::initializeParser(const Config &config)
{
    CodeParser::initializeParser(config);
}

void JambiApiParser::terminateParser()
{
    CodeParser::terminateParser();
}

QString JambiApiParser::language()
{
    return "Java";
}

QString JambiApiParser::sourceFileNameFilter()
{
    return "*.japi";
}

void JambiApiParser::parseSourceFile(const Location &location, const QString &filePath, Tree *tree)
{
    javaTre = tree;
    metJapiTag = false;

    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        location.warning(tr("Cannot open JAPI file '%1'").arg(filePath));
        return;
    }

    japiLocation = Location(filePath);
    QXmlInputSource xmlSource(&file);
    reader.parse(xmlSource);
}

void JambiApiParser::doneParsingSourceFiles(Tree * /* tree */)
{
    /*
        Also import the overview documents.
    */
    foreach (Node *cppNode, cppTre->root()->childNodes()) {
        if (cppNode->type() == Node::Fake) {
            FakeNode *cppFake = static_cast<FakeNode *>(cppNode);
            if (cppFake->subType() == FakeNode::Page) {
                FakeNode *javaFake = new FakeNode(javaTre->root(), cppFake->name(),
                                                  cppFake->subType());
                javaFake->setModuleName("com.trolltech.qt");    // ### hard-coded
                javaFake->setTitle(cppFake->title());
                javaFake->setSubTitle(cppFake->subTitle());
                setStatus(javaFake, cppFake);
                setPass1JambifiedDoc(javaFake, cppFake);
            }
        }
    }

    /*
        Fix the docs.
    */
    if (javaTre) {
        javaTre->resolveInheritance();
        jambifyDocsPass2(javaTre->root());
        javaTre = 0;
    }
}

bool JambiApiParser::startElement(const QString & /* namespaceURI */,
                                  const QString & /* localName */,
                                  const QString &qName,
                                  const QXmlAttributes &attributes)
{
    if (!metJapiTag && qName != "japi") {
        // ### The file is not a JAPI file.
        return true;
    }
    metJapiTag = true;

    EnumNode *javaEnum = 0;
    EnumNode *cppEnum = 0;
    InnerNode *javaParent = javaTre->root();
    InnerNode *cppParent = cppTre->root();

    for (int i = 0; i < classAndEnumStack.count(); ++i) {
        const ClassOrEnumInfo &info = classAndEnumStack.at(i);
        if (info.cppNode) {
            if (info.cppNode->type() == Node::Enum) {
                Q_ASSERT(info.javaNode->type() == Node::Enum);
                javaEnum = static_cast<EnumNode *>(info.javaNode);
                cppEnum = static_cast<EnumNode *>(info.cppNode);
            } else {
                Q_ASSERT(info.javaNode->type() == Node::Class
                         || info.javaNode->type() == Node::Namespace);
                javaParent = static_cast<InnerNode *>(info.javaNode);
                cppParent = static_cast<InnerNode *>(info.cppNode);
            }
        }
    }

    if (qName == "class" || qName == "enum") {
        Node::Type type = (qName == "class") ? Node::Class : Node::Enum;

        QString javaExtends = attributes.value("java-extends");
        QString javaImplements = attributes.value("javaimplements");

        ClassOrEnumInfo info;
        info.tag = qName;
        info.javaName = attributes.value("java");
        info.cppName = attributes.value("cpp");
        info.cppNode = cppTre->findNode(info.cppName.split("::"), type, cppParent);
        if (!info.cppNode && type == Node::Class) {
            type = Node::Namespace;
            info.cppNode = cppTre->findNode(info.cppName.split("::"), type, cppParent);
        }

        if (!info.cppNode) {
            japiLocation.warning(tr("Cannot find C++ class or enum '%1'").arg(info.cppName));
        } else {
            if (qName == "class") {
                ClassNode *javaClass = new ClassNode(javaParent, info.javaName);
                javaClass->setModuleName(attributes.value("package"));
                if (!javaExtends.isEmpty())
                    javaTre->addBaseClass(javaClass, Node::Public, javaExtends.split('.'),
                                          javaExtends);
                if (!javaImplements.isEmpty())
                    javaTre->addBaseClass(javaClass, Node::Public, javaImplements.split('.'),
                                          javaExtends);

                info.javaNode = javaClass;
            } else {
                info.javaNode = new EnumNode(javaParent, info.javaName);
            }
            info.javaNode->setLocation(japiLocation);
            setStatus(info.javaNode, info.cppNode);

            setPass1JambifiedDoc(info.javaNode, info.cppNode);
        }
        classAndEnumStack.push(info);
    } else if (qName == "method" || qName == "signal") {
        QString javaSignature = attributes.value("java");
        if (javaSignature.startsWith("private"))
            return true;

        QString cppSignature = attributes.value("cpp");

        CppCodeParser cppParser;
        const FunctionNode *cppNode = cppParser.findFunctionNode(cppSignature, cppTre,
                                                                 cppParent,
                                                                 true /* fuzzy */);
        if (!cppNode) {
            bool quiet = false;

            /*
                Default constructors sometimes don't exist in C++.
            */
            if (!quiet && javaSignature == "public " + javaParent->name() + "()")
                quiet = true;

            if (!quiet)
                japiLocation.warning(tr("Cannot find C++ function '%1' ('%2')")
                                     .arg(cppSignature).arg(cppParent->name()));
        }

        FunctionNode *javaNode;
        if (makeFunctionNode(javaParent, javaSignature, &javaNode)) {
            javaNode->setLocation(japiLocation);
            if (qName == "signal")
                javaNode->setMetaness(FunctionNode::Signal);

            if (cppNode) {
                setStatus(javaNode, cppNode);

                int overloadNo = cppNode->parameters().count() - javaNode->parameters().count() + 1;
                if (overloadNo == 1) {
                    setPass1JambifiedDoc(javaNode, cppNode);
                } else {
                    Text text;

                    text << Atom::ParaLeft << "Equivalent to "
                         << Atom(Atom::Link, javaNode->name() + "()")
                         << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
                         << javaNode->name()
                         << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK)
                         << "(";

                    for (int i = 0; i < cppNode->parameters().count(); ++i) {
                        if (i > 0)
                            text << ", ";
                        if (i < javaNode->parameters().count()) {
                            text << Atom(Atom::FormattingLeft, ATOM_FORMATTING_PARAMETER)
                                 << javaNode->parameters().at(i).name()
                                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_PARAMETER);
                        } else {
                            // ### convert to Java
                            text << cppNode->parameters().at(i).defaultValue();
                        }
                    }

                    text << ").";

                    Doc doc;
                    doc.setBody(text);
                    javaNode->setDoc(doc, true);
                }
                javaNode->setOverload(overloadNo > 1);
            }
        }
    } else if (qName == "variablesetter" || qName == "variablegetter") {
        QString javaSignature = attributes.value("java");
        if (javaSignature.startsWith("private"))
            return true;

        QString cppVariable = attributes.value("cpp");

        VariableNode *cppNode = static_cast<VariableNode *>(cppParent->findNode(cppVariable,
                                                                                Node::Variable));
        FunctionNode *javaNode;
        if (makeFunctionNode(javaParent, javaSignature, &javaNode)) {
            javaNode->setLocation(japiLocation);

            if (!cppNode) {
#if 0
                japiLocation.warning(tr("Cannot find C++ variable '%1' ('%2')")
                                     .arg(cppVariable).arg(cppParent->name()));
#endif
                javaNode->setDoc(Doc(japiLocation, japiLocation,
                                     USED_INTERNALLY,
                                     QSet<QString>()), true);
            } else {
                setPass1JambifiedDoc(javaNode, cppNode, qName);
                setStatus(javaNode, cppNode);
            }
        }
    } else if (qName == "enum-value") {
        QString javaName = attributes.value("java");
        QString cppName = attributes.value("cpp");
        QString value = attributes.value("value");

        if (javaEnum) {
            EnumItem item(javaName, value, findEnumText(javaEnum, javaName));
            javaEnum->addItem(item);
        }
    }

    return true;
}

bool JambiApiParser::endElement(const QString & /* namespaceURI */,
                                const QString & /* localName */,
                                const QString &qName)
{
    if (qName == "class" || qName == "enum")
        classAndEnumStack.pop();
    return true;
}

bool JambiApiParser::fatalError(const QXmlParseException &exception)
{
    japiLocation.setLineNo(exception.lineNumber());
    japiLocation.setColumnNo(exception.columnNumber());
    japiLocation.warning(tr("Syntax error in JAPI file (%1)").arg(exception.message()));
    return true;
}

void JambiApiParser::jambifyDocsPass2(Node *node)
{
    const Doc &doc = node->doc();
    if (!doc.isEmpty()) {
        if (node->type() == Node::Enum) {
            Doc newDoc(doc);
            newDoc.simplifyEnumDoc();
            node->setDoc(newDoc, true);
        }
    }

    if (node->isInnerNode()) {
        InnerNode *innerNode = static_cast<InnerNode *>(node);
        foreach (Node *child, innerNode->childNodes())
            jambifyDocsPass2(child);
    }
}

bool JambiApiParser::makeFunctionNode(InnerNode *parent, const QString &synopsis,
				      FunctionNode **funcPtr)
{
    Node::Access access = Node::Public;
    FunctionNode::Metaness metaness = FunctionNode::Plain;
    bool final = false;
    bool statique = false;

    QString mySynopsis = synopsis.simplified();
    int oldLen;
    do {
        oldLen = mySynopsis.length();

        if (mySynopsis.startsWith("public ")) {
            mySynopsis.remove(0, 7);
            access = Node::Public;
        }
        if (mySynopsis.startsWith("protected ")) {
            mySynopsis.remove(0, 10);
            access = Node::Protected;
        }
        if (mySynopsis.startsWith("private ")) {
            mySynopsis.remove(0, 8);
            access = Node::Private;
        }
        if (mySynopsis.startsWith("native ")) {
            mySynopsis.remove(0, 7);
            metaness = FunctionNode::Native;
        }
        if (mySynopsis.startsWith("final ")) {
            mySynopsis.remove(0, 6);
            final = true;
        }
        if (mySynopsis.startsWith("static ")) {
            mySynopsis.remove(0, 7);
            statique = true;
        }
    } while (oldLen != mySynopsis.length());

    // method or constructor
    QRegExp funcRegExp("(?:(.*) )?([A-Za-z_0-9]+)\\((.*)\\)");
    if (!funcRegExp.exactMatch(mySynopsis))
        return false;

    QString retType = funcRegExp.cap(1);
    QString funcName = funcRegExp.cap(2);
    QStringList params = funcRegExp.cap(3).split(",");

    FunctionNode *func = new FunctionNode(parent, funcName);
    func->setReturnType(retType);
    func->setAccess(access);
    func->setStatic(statique);
    func->setConst(final);
    func->setMetaness(metaness);

    QRegExp paramRegExp(" ?([^ ].*) ([A-Za-z_0-9]+) ?");

    foreach (const QString &param, params) {
        if (paramRegExp.exactMatch(param)) {
            func->addParameter(Parameter(paramRegExp.cap(1), "", paramRegExp.cap(2)));
        } else {
            // problem
        }
    }

    if (funcPtr) {
        *funcPtr = func;
    } else if (!parent) {
        delete func;
    }
    return true;
}

QT_END_NAMESPACE
