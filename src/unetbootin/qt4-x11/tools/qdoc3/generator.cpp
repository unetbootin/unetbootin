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
  generator.cpp
*/

#include <qdir.h>

#include "codemarker.h"
#include "config.h"
#include "doc.h"
#include "editdistance.h"
#include "generator.h"
#include "node.h"
#include "openedlist.h"
#include "quoter.h"
#include "separator.h"
#include "tokenizer.h"

QT_BEGIN_NAMESPACE

QList<Generator *> Generator::generators;
QMap<QString, QMap<QString, QString> > Generator::fmtLeftMaps;
QMap<QString, QMap<QString, QString> > Generator::fmtRightMaps;
QMap<QString, QStringList> Generator::imgFileExts;
QSet<QString> Generator::outputFormats;
QStringList Generator::imageFiles;
QStringList Generator::imageDirs;
QString Generator::outDir;
QString Generator::project;

static Text stockLink(const QString &target)
{
    return Text() << Atom(Atom::Link, target) << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
                  << target << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
}

Generator::Generator()
    : amp("&amp;"), lt("&lt;"), gt("&gt;"), quot("&quot;"), tag("</?@[^>]*>")
{
    generators.prepend(this);
}

Generator::~Generator()
{
    generators.removeAll(this);
}

void Generator::initializeGenerator(const Config & /* config */)
{
}

void Generator::terminateGenerator()
{
}

void Generator::initialize(const Config &config)
{
    outputFormats = config.getStringSet(CONFIG_OUTPUTFORMATS);
    if ( !outputFormats.isEmpty() ) {
        outDir = config.getString(CONFIG_OUTPUTDIR);
        if ( outDir.isEmpty() )
            config.lastLocation().fatal(tr("No output directory specified in configuration file"));

        QDir dirInfo;
        if ( dirInfo.exists(outDir) ) {
            if ( !Config::removeDirContents(outDir) )
                config.lastLocation().error(tr("Cannot empty output directory '%1'").arg(outDir));
        } else {
            if ( !dirInfo.mkpath(outDir) )
                config.lastLocation().fatal(tr("Cannot create output directory '%1'").arg(outDir));
        }

        if ( !dirInfo.mkdir(outDir + "/images") )
            config.lastLocation().fatal(tr("Cannot create output directory '%1'")
                                        .arg(outDir + "/images"));
    }

    imageFiles = config.getStringList(CONFIG_IMAGES);
    imageDirs = config.getStringList(CONFIG_IMAGEDIRS);

    QString imagesDotFileExtensions = CONFIG_IMAGES + Config::dot + CONFIG_FILEEXTENSIONS;
    QSet<QString> formats = config.subVars( imagesDotFileExtensions );
    QSet<QString>::ConstIterator f = formats.begin();
    while ( f != formats.end() ) {
        imgFileExts[*f] = config.getStringList(imagesDotFileExtensions + Config::dot + *f);
        ++f;
    }

    QList<Generator *>::ConstIterator g = generators.begin();
    while (g != generators.end()) {
        if (outputFormats.contains((*g)->format())) {
            (*g)->initializeGenerator(config);
            QStringList extraImages = config.getStringList(CONFIG_EXTRAIMAGES + Config::dot
                                                           + (*g)->format());
            QStringList::ConstIterator e = extraImages.begin();
            while (e != extraImages.end()) {
                QString userFriendlyFilePath;
                QString filePath = Config::findFile(config.lastLocation(), imageFiles, imageDirs, *e,
                                                    imgFileExts[(*g)->format()], userFriendlyFilePath);
                if (!filePath.isEmpty())
                    Config::copyFile(config.lastLocation(), filePath, userFriendlyFilePath,
                                         (*g)->outputDir() + "/images");
                ++e;
            }
        }
        ++g;
    }

    QRegExp secondParamAndAbove( "[\2-\7]" );
    QSet<QString> formattingNames = config.subVars( CONFIG_FORMATTING );
    QSet<QString>::ConstIterator n = formattingNames.begin();
    while ( n != formattingNames.end() ) {
        QString formattingDotName = CONFIG_FORMATTING + Config::dot + *n;

        QSet<QString> formats = config.subVars( formattingDotName );
        QSet<QString>::ConstIterator f = formats.begin();
        while ( f != formats.end() ) {
            QString def = config.getString( formattingDotName + Config::dot +
                                            *f );
            if ( !def.isEmpty() ) {
                int numParams = Config::numParams( def );
                int numOccs = def.count("\1");

                if ( numParams != 1 ) {
                    config.lastLocation().warning(tr("Formatting '%1' must have exactly one"
                                                     " parameter (found %2)")
                                                 .arg(*n).arg(numParams));
                } else if ( numOccs > 1 ) {
                    config.lastLocation().fatal(tr("Formatting '%1' must contain exactly one"
                                                    " occurrence of '\\1' (found %2)")
                                                .arg(*n).arg(numOccs));
                } else {
                    int paramPos = def.indexOf( "\1" );
                    fmtLeftMaps[*f].insert( *n, def.left(paramPos) );
                    fmtRightMaps[*f].insert( *n, def.mid(paramPos + 1) );
                }
            }
            ++f;
        }
        ++n;
    }

    project = config.getString(CONFIG_PROJECT);
}

void Generator::terminate()
{
    QList<Generator *>::ConstIterator g = generators.begin();
    while ( g != generators.end() ) {
        if (outputFormats.contains((*g)->format()))
            (*g)->terminateGenerator();
        ++g;
    }

    fmtLeftMaps.clear();
    fmtRightMaps.clear();
    imgFileExts.clear();
    imageFiles.clear();
    imageDirs.clear();
    outDir = "";
}

Generator *Generator::generatorForFormat( const QString& format )
{
    QList<Generator *>::ConstIterator g = generators.begin();
    while ( g != generators.end() ) {
        if ( (*g)->format() == format )
            return *g;
        ++g;
    }
    return 0;
}

void Generator::startText( const Node * /* relative */,
                           CodeMarker * /* marker */ )
{
}

void Generator::endText( const Node * /* relative */,
                         CodeMarker * /* marker */ )
{
}

int Generator::generateAtom( const Atom * /* atom */,
                             const Node * /* relative */,
                             CodeMarker * /* marker */ )
{
    return 0;
}

void Generator::generateClassLikeNode(const InnerNode * /* classe */, CodeMarker * /* marker */)
{
}

void Generator::generateFakeNode( const FakeNode * /* fake */,
                                  CodeMarker * /* marker */ )
{
}

void Generator::generateText( const Text& text, const Node *relative,
                              CodeMarker *marker )
{
    if ( text.firstAtom() != 0 ) {
        int numAtoms = 0;
        startText( relative, marker );
        generateAtomList( text.firstAtom(), relative, marker, true, numAtoms );
        endText( relative, marker );
    }
}

void Generator::generateBody( const Node *node, CodeMarker *marker )
{
    bool quiet = false;

    if ( node->type() == Node::Function ) {
#if 0        
        const FunctionNode *func = (const FunctionNode *) node;
        if ( func->isOverload() && func->metaness() != FunctionNode::Ctor )
            generateOverload( node, marker );
#endif        
    } else if (node->type() == Node::Fake) {
        const FakeNode *fake = static_cast<const FakeNode *>(node);
        if (fake->subType() == FakeNode::Example)
            generateExampleFiles(fake, marker);
        else if (fake->subType() == FakeNode::File)
            quiet = true;
    }

    if (node->doc().isEmpty()) {
        if (!quiet) // ### might be unnecessary
            node->location().warning(tr("No documentation for '%1'")
                            .arg(marker->plainFullName(node)));
    } else {
        generateText(node->doc().body(), node, marker);

        if ( node->type() == Node::Enum ) {
            const EnumNode *enume = (const EnumNode *) node;

            QSet<QString> definedItems;
            QList<EnumItem>::ConstIterator it = enume->items().begin();
            while ( it != enume->items().end() ) {
                definedItems.insert( (*it).name() );
                ++it;
            }

            QSet<QString> documentedItems = enume->doc().enumItemNames().toSet();
            QSet<QString> allItems = definedItems + documentedItems;
            if ( allItems.count() > definedItems.count() ||
                 allItems.count() > documentedItems.count() ) {
                QSet<QString>::ConstIterator a = allItems.begin();
                while ( a != allItems.end() ) {
                    if ( !definedItems.contains(*a) ) {
                        QString details;
                        QString best = nearestName( *a, definedItems );
                        if ( !best.isEmpty() && !documentedItems.contains(best) )
                            details = tr( "Maybe you meant '%1'?" ).arg( best );

                        node->doc().location().warning(
                            tr("No such enum item '%1' in %2").arg(*a).arg(marker->plainFullName(node)),
                            details);
                    } else if ( !documentedItems.contains(*a) ) {
                        node->doc().location().warning(
                            tr("Undocumented enum item '%1' in %2").arg(*a).arg(marker->plainFullName(node)));
                    }
                    ++a;
                }
            }
        } else if ( node->type() == Node::Function ) {
            const FunctionNode *func = static_cast<const FunctionNode *>(node);

            QSet<QString> definedParams;
            QList<Parameter>::ConstIterator p = func->parameters().begin();
            while (p != func->parameters().end()) {
                if ((*p).name().isEmpty() && (*p).leftType() != QLatin1String("...")
                        && func->name() != QLatin1String("operator++")
                        && func->name() != QLatin1String("operator--")) {
                    node->doc().location().warning(tr("Missing parameter name"));
                } else {
                    definedParams.insert( (*p).name() );
                }
                ++p;
            }

            QSet<QString> documentedParams = func->doc().parameterNames();
            QSet<QString> allParams = definedParams + documentedParams;
            if (allParams.count() > definedParams.count()
                    || allParams.count() > documentedParams.count()) {
                QSet<QString>::ConstIterator a = allParams.begin();
                while (a != allParams.end()) {
                    if (!definedParams.contains(*a)) {
                        QString details;
                        QString best = nearestName(*a, definedParams);
                        if ( !best.isEmpty() )
                            details = tr("Maybe you meant '%1'?").arg(best);

                        node->doc().location().warning(
                            tr("No such parameter '%1' in %2").arg(*a).arg(marker->plainFullName(node)),
                            details);
                    } else if ( !(*a).isEmpty() && !documentedParams.contains(*a) ) {
                        bool needWarning = (func->status() > Node::Obsolete);
                        if (func->overloadNumber() > 1) {
                            FunctionNode *primaryFunc =
                                    func->parent()->findFunctionNode(func->name());
                            if (primaryFunc) {
                                foreach (const Parameter &param, primaryFunc->parameters()) {
                                    if (param.name() == *a) {
                                        needWarning = false;
                                        break;
                                    }
                                }
                            }
                        }
                        if (needWarning)
                            node->doc().location().warning(
                                tr("Undocumented parameter '%1' in %2").arg(*a).arg(marker->plainFullName(node)));
                    }
                    ++a;
                }
            }
/* Something like this return value check should be implemented at some point. */
            if (func->status() > Node::Obsolete && func->returnType() == "bool"
                    && func->reimplementedFrom() == 0 && !func->isOverload()) {
                QString body = func->doc().body().toString();
                if (!body.contains("return", Qt::CaseInsensitive))
                    node->doc().location().warning(tr("Undocumented return value"));
            }

            if ( func->reimplementedFrom() != 0 )
                generateReimplementedFrom( func, marker );
        }
    }

    if (node->type() == Node::Fake) {
        const FakeNode *fake = static_cast<const FakeNode *>(node);
        if (fake->subType() == FakeNode::File) {
            Text text;
            Quoter quoter;
            Doc::quoteFromFile(fake->doc().location(), quoter, fake->name());
            QString code = quoter.quoteTo(fake->location(), "", "");
            text << Atom(Atom::Code, code);
            generateText(text, fake, marker);
        }
    }
}

void Generator::generateAlsoList( const Node *node, CodeMarker *marker )
{
    QList<Text> alsoList = node->doc().alsoList();
    supplementAlsoList(node, alsoList);

    if (!alsoList.isEmpty()) {
        Text text;
        text << Atom::ParaLeft << "See also ";

        for (int i = 0; i < alsoList.size(); ++i)
            text << alsoList.at(i) << separator(i, alsoList.size());

        text << Atom::ParaRight;
        generateText( text, node, marker );
    }
}

void Generator::generateInherits(const ClassNode *classe, CodeMarker *marker)
{
    QList<RelatedClass>::ConstIterator r;
    int index;

    if ( !classe->baseClasses().isEmpty() ) {
        Text text;
        text << Atom::ParaLeft << "Inherits ";

        r = classe->baseClasses().begin();
        index = 0;
        while ( r != classe->baseClasses().end() ) {
            text << Atom(Atom::LinkNode, CodeMarker::stringForNode((*r).node))
                 << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
                 << Atom(Atom::String, (*r).dataTypeWithTemplateArgs)
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);

            if ( (*r).access == Node::Protected ) {
                text << " (protected)";
            } else if ( (*r).access == Node::Private ) {
                text << " (private)";
            }
            text << separator( index++, classe->baseClasses().count() );
            ++r;
        }
        text << Atom::ParaRight;
        generateText( text, classe, marker );
    }
}

void Generator::generateInheritedBy( const ClassNode *classe,
                                     CodeMarker *marker )
{
    if ( !classe->derivedClasses().isEmpty() ) {
        Text text;
        text << Atom::ParaLeft << "Inherited by ";

        appendSortedNames(text, classe, classe->derivedClasses(), marker);
        text << Atom::ParaRight;
        generateText( text, classe, marker );
    }
}

void Generator::generateExampleFiles(const FakeNode *fake, CodeMarker *marker)
{
    if (fake->childNodes().isEmpty())
        return;

    OpenedList openedList(OpenedList::Bullet);

    Text text;
    text << Atom::ParaLeft << "Files:" << Atom::ParaRight
         << Atom(Atom::ListLeft, openedList.styleString());
    foreach (const Node *child, fake->childNodes()) {
        QString exampleFile = child->name();
        openedList.next();
        text << Atom(Atom::ListItemNumber, openedList.numberString())
             << Atom(Atom::ListItemLeft, openedList.styleString()) << Atom::ParaLeft
             << Atom(Atom::Link, exampleFile)
             << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
             << exampleFile
             << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK)
             << Atom::ParaRight << Atom(Atom::ListItemRight, openedList.styleString());
    }
    text << Atom(Atom::ListRight, openedList.styleString());
    generateText(text, fake, marker);
}

void Generator::generateModuleWarning(const ClassNode *classe, CodeMarker *marker)
{
    QString module = classe->moduleName();
    if (!module.isEmpty()) {
        Text text;
        if (Tokenizer::isTrue("defined(consoleedition)")
                && !editionModuleMap["Console"].contains(module)) {
            text << Atom::ParaLeft
                 << Atom(Atom::FormattingLeft, ATOM_FORMATTING_BOLD)
                 << "This class is not part of the Qt Console Edition."
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD)
                 << Atom::ParaRight;
        } else if (Tokenizer::isTrue("defined(desktoplightedition)")
                && !editionModuleMap["DesktopLight"].contains(module)) {
            text << Atom::ParaLeft
                 << Atom(Atom::FormattingLeft, ATOM_FORMATTING_BOLD)
                 << "This class is not part of the Qt Desktop Light Edition."
                 << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD)
                 << Atom::ParaRight;
        } else if (module == "Qt3Support" && Tokenizer::isTrue("defined(opensourceedition)")) {
            text << Atom::ParaLeft << Atom( Atom::FormattingLeft, ATOM_FORMATTING_BOLD )
                 << "Note to Qt Desktop Light Edition users:"
                 << Atom( Atom::FormattingRight, ATOM_FORMATTING_BOLD )
                 << " This class is only available in the "
                 << Atom(Atom::AutoLink, "Qt Desktop Edition")
                 << "." << Atom::ParaRight;
        }

        generateText(text, classe, marker);
    }
}

QString Generator::indent( int level, const QString& markedCode )
{
    if ( level == 0 )
        return markedCode;

    QString t;
    int column = 0;

    int i = 0;
    while ( i < (int) markedCode.length() ) {
        if ( markedCode.at(i) == QLatin1Char('<') ) {
            while ( i < (int) markedCode.length() ) {
                t += markedCode.at(i++);
                if ( markedCode.at(i - 1) == QLatin1Char('>') )
                    break;
            }
        } else {
            if ( markedCode.at(i) == QLatin1Char('\n') ) {
                column = 0;
            } else {
                if ( column == 0 ) {
                    for ( int j = 0; j < level; j++ )
                        t += QLatin1Char(' ');
                }
                column++;
            }
            t += markedCode.at(i++);
        }
    }
    return t;
}

QString Generator::plainCode( const QString& markedCode )
{
    QString t = markedCode;
    t.replace( tag, QString() );
    t.replace( quot, QLatin1String("\"") );
    t.replace( gt, QLatin1String(">") );
    t.replace( lt, QLatin1String("<") );
    t.replace( amp, QLatin1String("&") );
    return t;
}

QString Generator::typeString( const Node *node )
{
    switch ( node->type() ) {
    case Node::Namespace:
        return "namespace";
    case Node::Class:
        return "class";
    case Node::Fake:
    default:
        return "documentation";
    case Node::Enum:
        return "enum";
    case Node::Typedef:
        return "typedef";
    case Node::Function:
        return "function";
    case Node::Property:
        return "property";
    }
}

QString Generator::imageFileName( const Node *relative, const QString& fileBase )
{
    QString userFriendlyFilePath;
    QString filePath = Config::findFile(
        relative->doc().location(), imageFiles, imageDirs, fileBase,
        imgFileExts[format()], userFriendlyFilePath);

    if (filePath.isEmpty())
        return QString();

    return QLatin1String("images/")
           + Config::copyFile(relative->doc().location(),
                              filePath, userFriendlyFilePath,
                              outputDir() + QLatin1String("/images"));
}

void Generator::setImageFileExtensions( const QStringList& extensions )
{
    imgFileExts[format()] = extensions;
}

void Generator::unknownAtom( const Atom *atom )
{
    Location::internalError( tr("unknown atom type '%1' in %2 generator")
                             .arg(atom->typeString()).arg(format()) );
}

bool Generator::matchAhead( const Atom *atom, Atom::Type expectedAtomType )
{
    return atom->next() != 0 && atom->next()->type() == expectedAtomType;
}

void Generator::supplementAlsoList(const Node *node, QList<Text> &alsoList)
{
    if (node->type() == Node::Function) {
        const FunctionNode *func = static_cast<const FunctionNode *>(node);
        if (func->overloadNumber() == 1) {
            QString alternateName;
            const FunctionNode *alternateFunc = 0;

            if (func->name().startsWith("set") && func->name().size() >= 4) {
                alternateName = func->name()[3].toLower();
                alternateName += func->name().mid(4);
                alternateFunc = func->parent()->findFunctionNode(alternateName);

                if (!alternateFunc) {
                    alternateName = "is" + func->name().mid(3);
                    alternateFunc = func->parent()->findFunctionNode(alternateName);
                    if (!alternateFunc) {
                        alternateName = "has" + func->name().mid(3);
                        alternateFunc = func->parent()->findFunctionNode(alternateName);
                    }
                }
            } else if (!func->name().isEmpty()) {
                alternateName = "set";
                alternateName += func->name()[0].toUpper();
                alternateName += func->name().mid(1);
                alternateFunc = func->parent()->findFunctionNode(alternateName);
            }

            if (alternateFunc && alternateFunc->access() != Node::Private) {
                int i;
                for (i = 0; i < alsoList.size(); ++i) {
                    if (alsoList.at(i).toString().contains(alternateName))
                        break;
                }

                if (i == alsoList.size()) {
                    alternateName += "()";

                    Text also;
                    also << Atom(Atom::Link, alternateName)
                         << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
                         << alternateName
                         << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                    alsoList.prepend(also);
                }
            }
        }
    }
}

QMap<QString, QString>& Generator::formattingLeftMap()
{
    return fmtLeftMaps[format()];
}

QMap<QString, QString>& Generator::formattingRightMap()
{
    return fmtRightMaps[format()];
}

QString Generator::trimmedTrailing(const QString &string)
{
    QString trimmed = string;
    while (trimmed.length() > 0 && trimmed[trimmed.length() - 1].isSpace())
        trimmed.truncate(trimmed.length() - 1);
    return trimmed;
}

void Generator::generateStatus( const Node *node, CodeMarker *marker )
{
    Text text;

    switch ( node->status() ) {
    case Node::Commendable:
    case Node::Main:
        break;
    case Node::Preliminary:
        text << Atom::ParaLeft << Atom( Atom::FormattingLeft, ATOM_FORMATTING_BOLD ) << "This "
             << typeString( node ) << " is under development and is subject to change."
             << Atom( Atom::FormattingRight, ATOM_FORMATTING_BOLD ) << Atom::ParaRight;
        break;
    case Node::Deprecated:
        text << Atom::ParaLeft;
        if (node->isInnerNode())
            text << Atom( Atom::FormattingLeft, ATOM_FORMATTING_BOLD );
        text << "This " << typeString( node ) << " is deprecated.";
        if (node->isInnerNode())
            text << Atom( Atom::FormattingRight, ATOM_FORMATTING_BOLD );
        text << Atom::ParaRight;
        break;
    case Node::Obsolete:
        text << Atom::ParaLeft;
        if (node->isInnerNode())
            text << Atom( Atom::FormattingLeft, ATOM_FORMATTING_BOLD );
        text << "This " << typeString( node ) << " is obsolete.";
        if (node->isInnerNode())
            text << Atom( Atom::FormattingRight, ATOM_FORMATTING_BOLD );
        text << " It is provided to keep old source code working. We strongly advise against "
             << "using it in new code." << Atom::ParaRight;
        break;
    case Node::Compat:
        // reimplemented in HtmlGenerator subclass
        if (node->isInnerNode()) {
            text << Atom::ParaLeft << Atom( Atom::FormattingLeft, ATOM_FORMATTING_BOLD ) << "This "
                 << typeString( node ) << " is part of the Qt 3 compatibility layer."
                 << Atom( Atom::FormattingRight, ATOM_FORMATTING_BOLD )
                 << " It is provided to keep old source code working. We strongly advise against "
                 << "using it in new code. See "
                 << Atom(Atom::AutoLink, "Porting to Qt 4")
                 << " for more information."
                 << Atom::ParaRight;
        }
        break;
    case Node::Internal:
    default:
        break;
    }
    generateText(text, node, marker);
}

void Generator::generateThreadSafeness(const Node *node, CodeMarker *marker)
{
    Text text;
    Text theStockLink;
    Node::ThreadSafeness parent = node->parent()->inheritedThreadSafeness();

    switch (node->threadSafeness()) {
    case Node::UnspecifiedSafeness:
        break;
    case Node::NonReentrant:
        text << Atom::ParaLeft << Atom(Atom::FormattingLeft, ATOM_FORMATTING_BOLD) << "Warning:"
             << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD) << " This "
             << typeString(node) << " is not " << stockLink("reentrant") << "." << Atom::ParaRight;
        break;
    case Node::Reentrant:
    case Node::ThreadSafe:
        text << Atom::ParaLeft << Atom(Atom::FormattingLeft, ATOM_FORMATTING_BOLD);
        if (parent == Node::ThreadSafe) {
            text << "Warning:";
        } else {
            text << "Note:";
        }
        text << Atom(Atom::FormattingRight, ATOM_FORMATTING_BOLD) << " ";

        if (node->threadSafeness() == Node::ThreadSafe)
            theStockLink = stockLink("thread-safe");
        else
            theStockLink = stockLink("reentrant");

        if (node->isInnerNode()) {
            const InnerNode *innerNode = static_cast<const InnerNode *>(node);
            text << "All the functions in this " << typeString(node) << " are "
                 << theStockLink;

            NodeList except;
            NodeList::ConstIterator c = innerNode->childNodes().begin();
            while (c != innerNode->childNodes().end()) {
                if ((*c)->threadSafeness() != Node::UnspecifiedSafeness)
                    except.append(*c);
                ++c;
            }
            if (except.isEmpty()) {
                text << ".";
            } else {
                text << ", except ";

                NodeList::ConstIterator e = except.begin();
                int index = 0;
                while (e != except.end()) {
                    appendFullName(text, *e, innerNode, marker);
                    text << separator(index++, except.count());
                    ++e;
                }
            }
        } else {
            text << "This " << typeString(node) << " is " << theStockLink << ".";
        }
        text << Atom::ParaRight;
    }
    generateText(text, node, marker);
}

void Generator::generateSince(const Node *node, CodeMarker *marker)
{
    if (!node->since().isEmpty()) {
        Text text;
        text << Atom::ParaLeft << "This " << typeString(node)
             << " was introduced in ";
        if (project.isEmpty())
             text << "version";
        else
             text << project;
        text << " " << node->since() << "." << Atom::ParaRight;
        generateText(text, node, marker);
    }
}

QString Generator::getLink(const Atom *atom,
                           const Node *relative,
                           CodeMarker *marker,
                           const Node *node)
{
    return QString();
}


/*!
  No longer in use.
 */
void Generator::generateOverload( const Node *node, CodeMarker *marker )
{
    Text text;
    text << Atom::ParaLeft
         << "This function overloads ";
    QString t = node->name() + "()";
    text << Atom::AutoLink << t 
         << Atom::ParaRight;
    generateText( text, node, marker );
}

void Generator::generateReimplementedFrom( const FunctionNode *func,
                                           CodeMarker *marker )
{
    if ( func->reimplementedFrom() != 0 ) {
        const FunctionNode *from = func->reimplementedFrom();
        if (from->access() != Node::Private && from->parent()->access() != Node::Private) {
            Text text;
            text << Atom::ParaLeft << "Reimplemented from ";
            appendFullName( text, from->parent(), func, marker, from );
            text << "." << Atom::ParaRight;
            generateText( text, func, marker );
        }
    }
}

const Atom *Generator::generateAtomList(const Atom *atom, const Node *relative, CodeMarker *marker,
                                        bool generate, int &numAtoms)
{
    while (atom) {
        if (atom->type() == Atom::FormatIf) {
            int numAtoms0 = numAtoms;
            bool rightFormat = canHandleFormat(atom->string());
            atom = generateAtomList(atom->next(), relative, marker, generate && rightFormat,
                                    numAtoms);
            if (!atom)
                return 0;

            if (atom->type() == Atom::FormatElse) {
                ++numAtoms;
                atom = generateAtomList( atom->next(), relative, marker,
                                         generate && !rightFormat, numAtoms );
                if (!atom)
                    return 0;
            }

            if (atom->type() == Atom::FormatEndif) {
                if (generate && numAtoms0 == numAtoms) {
                    relative->location().warning(tr("Output format %1 not handled").arg(format()));
                    Atom unhandledFormatAtom(Atom::UnhandledFormat, format());
                    generateAtomList(&unhandledFormatAtom, relative, marker, generate, numAtoms);
                }
                atom = atom->next();
            }
        } else if (atom->type() == Atom::FormatElse || atom->type() == Atom::FormatEndif) {
            return atom;
        } else {
            int n = 1;
            if (generate) {
                n += generateAtom(atom, relative, marker);
                numAtoms += n;
            }
            while (n-- > 0)
                atom = atom->next();
        }
    }
    return 0;
}

void Generator::appendFullName( Text& text, const Node *apparentNode,
                                const Node *relative, CodeMarker *marker,
                                const Node *actualNode )
{
    if ( actualNode == 0 )
        actualNode = apparentNode;
    text << Atom(Atom::LinkNode, CodeMarker::stringForNode(actualNode))
         << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
         << Atom(Atom::String, marker->plainFullName(apparentNode, relative))
         << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
}

void Generator::appendSortedNames(Text& text, const ClassNode *classe,
                                  const QList<RelatedClass> &classes,
                                  CodeMarker *marker)
{
    QList<RelatedClass>::ConstIterator r;
    QMap<QString,Text> classMap;
    int index = 0;

    r = classes.begin();
    while ( r != classes.end() ) {
        if ((*r).node->access() == Node::Public && (*r).node->status() != Node::Internal
            && !(*r).node->doc().isEmpty()) {
            Text className;
            appendFullName( className, (*r).node, classe, marker );
            classMap[className.toString().toLower()] = className;
        }
        ++r;
    }

    QStringList classNames = classMap.keys();
    classNames.sort();

    foreach (const QString &className, classNames) {
        text << classMap[className];
        text << separator( index++, classNames.count() );
    }
}

int Generator::skipAtoms(const Atom *atom, Atom::Type type) const
{
    int skipAhead = 0;
    atom = atom->next();
    while ( atom != 0 && atom->type() != type ) {
        skipAhead++;
        atom = atom->next();
    }
    return skipAhead;
}

QString Generator::fullName(const Node *node, const Node *relative,
                            CodeMarker *marker) const
{
    if (node->type() == Node::Fake)
        return static_cast<const FakeNode *>(node)->title();
    else if (node->type() == Node::Class &&
        !(static_cast<const ClassNode *>(node))->serviceName().isEmpty())
        return (static_cast<const ClassNode *>(node))->serviceName();
    else
        return marker->plainFullName(node, relative);
}

QT_END_NAMESPACE
