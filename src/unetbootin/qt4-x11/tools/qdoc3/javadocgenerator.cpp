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

#include "javadocgenerator.h"

QT_BEGIN_NAMESPACE

enum JavaSignatureSyntax {
    GeneratedJdocFile,
    JavadocRef,
    SlotSignature
};

static QString javaSignature(const FunctionNode *func, JavaSignatureSyntax syntax,
                             int maxParams = 65535)
{
    maxParams = qMin(maxParams, func->parameters().count());

    QString result;

    if (syntax == GeneratedJdocFile) {
        if (func->access() == Node::Public) {
            result += "public ";
        } else if (func->access() == Node::Protected) {
            result += "protected ";
        } else {
            result += "private ";
        }

        if (func->metaness() == FunctionNode::Native)
            result += "native ";

        if (func->isConst())
            result += "final ";

        // ### func->metaness() == FunctionNode::Abstract

        if (func->isStatic())
            result += "static ";

        if (!func->returnType().isEmpty()) {
            result += func->returnType();
            result += ' ';
        }
    }

    if (syntax == SlotSignature) {
        result += "void mySlot";
    } else {
        result += func->name();
    }
    result += '(';
    for (int i = 0; i < maxParams; ++i) {
        if (i != 0)
            result += ", ";
        result += func->parameters().at(i).leftType();
        if (syntax != JavadocRef) {
            result += ' ';
            result += func->parameters().at(i).name();
        }
    }
    result += ')';

    return result;
}

static QString packageName(const Node *node)
{
    while (node && node->type() != Node::Class && node->type() != Node::Fake)
        node = node->parent();
    if (!node)
        return QString();
    return node->moduleName();
}

JavadocGenerator::JavadocGenerator()
    : oldDevice(0), currentDepth(0)
{
}

JavadocGenerator::~JavadocGenerator()
{
}

void JavadocGenerator::initializeGenerator(const Config &config)
{
    HtmlGenerator::initializeGenerator(config);

    formattingLeftMap().insert(ATOM_FORMATTING_PARAMETER,
                               formattingLeftMap().value(ATOM_FORMATTING_TELETYPE));
    formattingRightMap().insert(ATOM_FORMATTING_PARAMETER,
                                formattingRightMap().value(ATOM_FORMATTING_TELETYPE));
}

void JavadocGenerator::terminateGenerator()
{
    HtmlGenerator::terminateGenerator();
}

QString JavadocGenerator::format()
{
    return "javadoc";
}

void JavadocGenerator::generateTree(const Tree *tree, CodeMarker *marker)
{
    HtmlGenerator::generateTree(tree, marker);
}

QString JavadocGenerator::fileExtension(const Node *node)
{
    if (node->type() == Node::Fake) {
        return "html";
    } else {
        return "jdoc";
    }
}

QString JavadocGenerator::typeString(const Node *node)
{
    if (node->type() == Node::Function) {
        const FunctionNode *func = static_cast<const FunctionNode *>(node);
        return func->metaness() == FunctionNode::Signal ? "signal" : "method";
    } else {
        return HtmlGenerator::typeString(node);
    }
}

QString JavadocGenerator::imageFileName(const Node *relative, const QString& fileBase)
{
    QString result = HtmlGenerator::imageFileName(relative, fileBase);
    if (!result.isEmpty()) {
        QString package = packageName(relative);
        int numSubPackages = package.count('.') - 2;
        while (numSubPackages > 0) {
            result.prepend("%2E%2E/");  // javadoc 1.5.0_06 chokes on '../'
            --numSubPackages;
        }
    }
    return result;
}

static int textDepth = 0;

void JavadocGenerator::startText(const Node *relative, CodeMarker *marker)
{
    if (textDepth++ == 0 && relative->type() != Node::Fake) {
        Q_ASSERT(!oldDevice);
        oldDevice = out().device();
        Q_ASSERT(oldDevice);
        out().setString(&buffer);
    }
    HtmlGenerator::startText(relative, marker);
}

void JavadocGenerator::endText(const Node *relative, CodeMarker *marker)
{
    HtmlGenerator::endText(relative, marker);
    if (--textDepth == 0 && relative->type() != Node::Fake) {
        Q_ASSERT(oldDevice);
        out().setDevice(oldDevice);
        oldDevice = 0;

        /*
            Need to escape XML metacharacters in .jdoc files.
        */
        buffer.replace("*/", "*&lt;!-- noop --&gt;/");
        buffer.replace("&", "&amp;");
        buffer.replace("\"", "&quot;");
        buffer.replace("<", "&lt;");
        buffer.replace(">", "&gt;");
        out() << buffer;
        buffer.clear();
    }
}

int JavadocGenerator::generateAtom(const Atom *atom, const Node *relative, CodeMarker *marker)
{
    return HtmlGenerator::generateAtom(atom, relative, marker);
}

void JavadocGenerator::generateClassLikeNode(const InnerNode *inner, CodeMarker *marker)
{
    generateIndent();
    out() << "<class name=\"" << protect(inner->name()) << "\"";
    generateDoc(inner, marker);
    out() << ">\n";

    ++currentDepth;
    foreach (Node *node, inner->childNodes()) {
        if (node->isInnerNode()) {
            generateClassLikeNode(static_cast<InnerNode *>(node), marker);
        } else {
            if (node->type() == Node::Enum) {
                EnumNode *enume = static_cast<EnumNode *>(node);

                generateIndent();
                out() << "<enum name=\"" << protect(node->name()) << "\"";
                generateDoc(node, marker);
                out() << ">\n";

                ++currentDepth;
                const QList<EnumItem> &items = enume->items();
                for (int i = 0; i < items.count(); ++i) {
                    const EnumItem &item = items.at(i);
                    generateIndent();
                    out() << "<enum-value name=\"" << protect(item.name()) << "\"";
                    generateEnumItemDoc(item.text(), enume, marker);
                    out() << "/>\n";
                }
                --currentDepth;

                out() << "</enum>\n";
            } else if (node->type() == Node::Function) {
                FunctionNode *func = static_cast<FunctionNode *>(node);
                generateIndent();
                out() << (func->metaness() == FunctionNode::Signal ? "<signal" : "<method")
                      << " name=\""
                      << protect(javaSignature(func, GeneratedJdocFile))
                      << "\"";
                generateDoc(node, marker);
                out() << "/>\n";
            }
        }
    }
    --currentDepth;

    generateIndent();
    out() << "</class>\n";
}

void JavadocGenerator::generateFakeNode(const FakeNode *fake, CodeMarker *marker)
{
    HtmlGenerator::generateFakeNode(fake, marker);
}

void JavadocGenerator::generateText(const Text& text, const Node *relative, CodeMarker *marker)
{
    HtmlGenerator::generateText(text, relative, marker);
}

void JavadocGenerator::generateBody(const Node *node, CodeMarker *marker)
{
    generateText(node->doc().body(), node, marker);
}

void JavadocGenerator::generateAlsoList( const Node *node, CodeMarker *marker )
{
    QList<Text> alsoList = node->doc().alsoList();
    supplementAlsoList(node, alsoList);

    if (node->type() == Node::Fake
            || (node->type() == Node::Function
                && static_cast<const FunctionNode *>(node)->metaness() == FunctionNode::Signal)) {
        Text text;

        if (!alsoList.isEmpty()) {
            text << Atom(Atom::ListLeft, ATOM_LIST_TAG)
                 << Atom(Atom::ListTagLeft, ATOM_LIST_TAG)
                 << Atom(Atom::FormattingLeft, ATOM_FORMATTING_BOLD)
                 << "See Also:"
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD)
                 << Atom(Atom::ListTagRight, ATOM_LIST_TAG)
                 << Atom(Atom::ListItemLeft, ATOM_LIST_TAG);

            for (int i = 0; i < alsoList.count(); ++i) {
                if (i != 0)
                    text << ", ";
                text << alsoList.at(i);
            }
            text << Atom(Atom::ListItemRight, ATOM_LIST_TAG)
                 << Atom(Atom::ListRight, ATOM_LIST_TAG);
        }

        generateText(text, node, marker);
    } else {
        foreach (const Text &text, alsoList) {
            out() << "\n@see ";
            generateText(text, node, marker);
        }
    }
}

QString JavadocGenerator::refForNode( const Node *node )
{
    if (node->type() == Node::Function)
        return javaSignature(static_cast<const FunctionNode *>(node), JavadocRef);

    return HtmlGenerator::refForNode(node);
}

QString JavadocGenerator::linkForNode( const Node *node, const Node *relative )
{
    // ### EVIL, relative should never be null
    if (!relative)
        relative = node;

    if (packageName(node).isEmpty()) {
        // ### jasmin: Fixme
        return QString();
    }

    QString result;
    if (node->type() == Node::Fake) {
        result = node->name();
    } else {
        if (!node->isInnerNode()) {
            result = linkForNode(node->parent(), relative) + "#" + refForNode(node);
        } else {
            result = node->name() + ".html";
        }
    }

    QStringList nodePackage = packageName(node).split(".");
    QStringList relativePackage = packageName(relative).split(".");
    if (nodePackage == QStringList(QString()) || relativePackage == QStringList(QString())) {
        qWarning("I'm in trouble [%s][%s]", qPrintable(node->name()), qPrintable(relative->name()));
        return QString();
    }

    int i = nodePackage.count() - 1;
    while (nodePackage.value(i) != relativePackage.value(i)) {
        result.prepend(nodePackage.at(i) + "/");
        --i;
    }

    ++i;
    while (i < relativePackage.count()) {
        result.prepend("%2E%2E/");      // javadoc 1.5.0_06 chokes on '../'
        ++i;
    }

    return result;
}

QString JavadocGenerator::refForAtom(Atom *atom, const Node *node)
{
    return HtmlGenerator::refForAtom(atom, node);
}

/*
    Neutralize dumb functions called from HtmlGenerator.
*/
void JavadocGenerator::generateDcf(const QString & /* fileBase */, const QString & /* startPage */,
                                   const QString & /* title */, DcfSection & /* dcfRoot */)
{
}

void JavadocGenerator::generateIndex(const QString & /* fileBase */, const QString & /* url */,
                                     const QString & /* title */)
{
}

void JavadocGenerator::generateIndent()
{
    for (int i = 0; i < currentDepth; ++i)
        out() << "    ";
}

void JavadocGenerator::generateDoc(const Node *node, CodeMarker *marker)
{
    const Text &text = node->doc().body();
    if (!text.isEmpty()) {
        out() << " doc=\"/**\n";
        Generator::generateStatus(node, marker);
        generateText(text, node, marker);
        if (node && node->type() == Node::Function) {
            const FunctionNode *func = static_cast<const FunctionNode *>(node);
            if (func->metaness() == FunctionNode::Signal) {
                QStringList slotSignatures;
                for (int i = func->parameters().count(); i >= 0; --i)
                    slotSignatures += javaSignature(func, SlotSignature, i);

                Text text;

                text << Atom(Atom::ListLeft, ATOM_LIST_TAG)
                     << Atom(Atom::ListTagLeft, ATOM_LIST_TAG)
                     << Atom(Atom::FormattingLeft, ATOM_FORMATTING_BOLD);

                if (slotSignatures.count() == 1) {
                    text << "Compatible Slot Signature:";
                } else {
                    text << "Compatible Slot Signatures:";
                }

                text << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD)
                     << Atom(Atom::ListTagRight, ATOM_LIST_TAG);

                for (int i = 0; i < slotSignatures.count(); ++i) {
                    text << Atom(Atom::ListItemLeft, ATOM_LIST_TAG)
                         << Atom(Atom::C, marker->markedUpCode(slotSignatures.at(i), 0, ""))
                         << Atom(Atom::ListItemRight, ATOM_LIST_TAG);
                }
                text << Atom(Atom::ListRight, ATOM_LIST_TAG);
                generateText(text, node, marker);
            }
        }
        if (node)
            generateAlsoList(node, marker);
        out() << " */\"";
    }
}

void JavadocGenerator::generateEnumItemDoc(const Text &text, const Node *node, CodeMarker *marker)
{
    out() << " doc=\"/**\n";
    if (text.isEmpty()) {
        out() << "Internal.";
    } else {
        generateText(text, node, marker);
    }
    out() << " */\"";
}

QT_END_NAMESPACE
