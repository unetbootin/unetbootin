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
  generator.h
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <qlist.h>
#include <qmap.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

#include "node.h"
#include "text.h"

QT_BEGIN_NAMESPACE

class ClassNode;
class Config;
class CodeMarker;
class FakeNode;
class FunctionNode;
class InnerNode;
class Location;
class NamespaceNode;
class Node;
class Tree;

class Generator
{
public:
    Generator();
    virtual ~Generator();

    virtual void initializeGenerator(const Config &config);
    virtual void terminateGenerator();
    virtual QString format() = 0;
    virtual bool canHandleFormat(const QString &format) { return format == this->format(); }
    virtual void generateTree(const Tree *tree, CodeMarker *marker) = 0;

    static void initialize( const Config& config );
    static void terminate();
    static Generator *generatorForFormat( const QString& format );

protected:
    virtual void startText( const Node *relative, CodeMarker *marker );
    virtual void endText( const Node *relative, CodeMarker *marker );
    virtual int generateAtom( const Atom *atom, const Node *relative,
			      CodeMarker *marker );
    virtual void generateClassLikeNode(const InnerNode *inner, CodeMarker *marker);
    virtual void generateFakeNode( const FakeNode *fake, CodeMarker *marker );

    virtual void generateText( const Text& text, const Node *relative,
			       CodeMarker *marker );
    virtual void generateBody( const Node *node, CodeMarker *marker );
    virtual void generateAlsoList( const Node *node, CodeMarker *marker );
    virtual void generateInherits( const ClassNode *classe,
				   CodeMarker *marker );
    virtual void generateInheritedBy( const ClassNode *classe,
				      CodeMarker *marker );

    void generateThreadSafeness( const Node *node, CodeMarker *marker );
    void generateSince(const Node *node, CodeMarker *marker);
    void generateStatus( const Node *node, CodeMarker *marker );
    const Atom *generateAtomList( const Atom *atom, const Node *relative,
				  CodeMarker *marker, bool generate,
				  int& numGeneratedAtoms );
    void generateExampleFiles(const FakeNode *fake, CodeMarker *marker);
    void generateModuleWarning( const ClassNode *classe, CodeMarker *marker);

    virtual int skipAtoms(const Atom *atom, Atom::Type type) const;
    virtual QString fullName(const Node *node, const Node *relative,
                             CodeMarker *marker) const;

    const QString& outputDir() { return outDir; }
    QString indent( int level, const QString& markedCode );
    QString plainCode( const QString& markedCode );
    virtual QString typeString( const Node *node );
    virtual QString imageFileName( const Node *relative, const QString& fileBase );
    void setImageFileExtensions( const QStringList& extensions );
    void unknownAtom( const Atom *atom );
    QMap<QString, QString> &formattingLeftMap();
    QMap<QString, QString> &formattingRightMap();

    QMap<QString, QStringList> editionModuleMap;

    static QString trimmedTrailing(const QString &string);
    static bool matchAhead( const Atom *atom, Atom::Type expectedAtomType );
    static void supplementAlsoList(const Node *node, QList<Text> &alsoList);

private:
    void generateOverload( const Node *node, CodeMarker *marker );
    void generateReimplementedFrom( const FunctionNode *func,
				    CodeMarker *marker );
    void appendFullName( Text& text, const Node *apparentNode,
			 const Node *relative, CodeMarker *marker,
			 const Node *actualNode = 0 );
    void appendSortedNames(Text& text, const ClassNode *classe,
                           const QList<RelatedClass> &classes,
                           CodeMarker *marker);

    QString amp;
    QString lt;
    QString gt;
    QString quot;
    QRegExp tag;

    static QList<Generator *> generators;
    static QMap<QString, QMap<QString, QString> > fmtLeftMaps;
    static QMap<QString, QMap<QString, QString> > fmtRightMaps;
    static QMap<QString, QStringList> imgFileExts;
    static QSet<QString> outputFormats;
    static QStringList imageFiles;
    static QStringList imageDirs;
    static QString outDir;
    static QString project;
};

QT_END_NAMESPACE

#endif
