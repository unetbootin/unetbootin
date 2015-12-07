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
  htmlgenerator.h
*/

#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include <qmap.h>
#include <qregexp.h>

#include "codemarker.h"
#include "config.h"
#include "dcfsection.h"
#include "pagegenerator.h"

QT_BEGIN_NAMESPACE

#if 0
struct NavigationBar
{
    SectionIterator prev;
    SectionIterator current;
    SectionIterator next;
};
#endif

class HelpProjectWriter;

class HtmlGenerator : public PageGenerator
{
public:
    HtmlGenerator();
    ~HtmlGenerator();

    virtual void initializeGenerator( const Config& config );
    virtual void terminateGenerator();
    virtual QString format();
    virtual void generateTree(const Tree *tree, CodeMarker *marker);

    static QString protect( const QString& string );
    static QString cleanRef( const QString& ref );

protected:
    virtual void startText( const Node *relative, CodeMarker *marker );
    virtual int generateAtom( const Atom *atom, const Node *relative,
			      CodeMarker *marker );
    virtual void generateClassLikeNode(const InnerNode *inner, CodeMarker *marker);
    virtual void generateFakeNode( const FakeNode *fake, CodeMarker *marker );
    virtual QString fileExtension(const Node *node);
    virtual QString refForNode( const Node *node );
    virtual QString linkForNode( const Node *node, const Node *relative );
    virtual QString refForAtom(Atom *atom, const Node *node);

private:
    enum SubTitleSize { SmallSubTitle, LargeSubTitle };

    const QPair<QString,QString> anchorForNode(const Node *node);
    const Node *findNodeForTarget(const QString &target, const Node *relative,
                                  CodeMarker *marker, const Atom *atom = 0);
    void generateHeader(const QString& title, const Node *node = 0,
                        CodeMarker *marker = 0, bool mainPage = true);
    void generateTitle(const QString& title, const Text &subTitle, SubTitleSize subTitleSize,
                       const Node *relative, CodeMarker *marker);
    void generateFooter( const Node *node = 0 );
    void generateBrief(const Node *node, CodeMarker *marker,
                       const Node *relative = 0);
    void generateIncludes(const InnerNode *inner, CodeMarker *marker);
#if 0
    void generateNavigationBar( const NavigationBar& bar, const Node *node,
    				CodeMarker *marker );
#endif
    void generateTableOfContents(const Node *node, CodeMarker *marker,
                                 Doc::SectioningUnit sectioningUnit,
                                 int numColumns, const Node *relative = 0);
    QString generateListOfAllMemberFile(const InnerNode *inner, CodeMarker *marker);
    QString generateLowStatusMemberFile(const InnerNode *inner, CodeMarker *marker,
                                        CodeMarker::Status status);
    void generateClassHierarchy(const Node *relative, CodeMarker *marker,
				const QMap<QString, const Node *> &classMap);
    void generateAnnotatedList(const Node *relative, CodeMarker *marker,
			       const QMap<QString, const Node *> &nodeMap);
    void generateCompactList(const Node *relative, CodeMarker *marker,
			     const QMap<QString, const Node *> &classMap);
    void generateFunctionIndex(const Node *relative, CodeMarker *marker);
    void generateLegaleseList(const Node *relative, CodeMarker *marker);
    void generateOverviewList(const Node *relative, CodeMarker *marker);
    void generateSynopsis(const Node *node, const Node *relative, CodeMarker *marker,
			  CodeMarker::SynopsisStyle style);
    void generateSectionList(const Section& section, const Node *relative,
			     CodeMarker *marker, CodeMarker::SynopsisStyle style);
    void generateSectionInheritedList(const Section& section, const Node *relative,
                                      CodeMarker *marker);
    void generateFullName(const Node *apparentNode, const Node *relative, CodeMarker *marker,
			  const Node *actualNode = 0);
    void generateDetailedMember(const Node *node, const InnerNode *relative, CodeMarker *marker);
    void generateLink(const Atom *atom, const Node *relative, CodeMarker *marker);
    void generateStatus( const Node *node, CodeMarker *marker );
    
    QString registerRef( const QString& ref );
    QString highlightedCode( const QString& markedCode, CodeMarker *marker, const Node *relative );
    QString fileBase(const Node *node);
#if 0
    QString fileBase( const Node *node, const SectionIterator& section );
#endif
    QString fileName( const Node *node );
    void findAllClasses(const InnerNode *node);
    void findAllFunctions(const InnerNode *node);
    void findAllLegaleseTexts(const InnerNode *node);
    void findAllNamespaces(const InnerNode *node);
    static int hOffset(const Node *node);
    static bool isThreeColumnEnumValueTable(const Atom *atom);
    virtual QString getLink(const Atom *atom, const Node *relative, CodeMarker *marker, const Node *node = 0);
    virtual void generateDcf(const QString &fileBase, const QString &startPage,
                             const QString &title, DcfSection &dcfRoot);
    virtual void generateIndex(const QString &fileBase, const QString &url,
                               const QString &title);
    void generateMacRef(const Node *node, CodeMarker *marker);
    void beginLink(const QString &link, const Node *node, const Node *relative, CodeMarker *marker);
    void endLink();

#if 0
    NavigationBar currentNavigationBar;
#endif
    QMap<QString, QString> refMap;
    int codeIndent;
    DcfSection dcfClassesRoot;
    DcfSection dcfOverviewsRoot;
    DcfSection dcfExamplesRoot;
    DcfSection dcfDesignerRoot;
    DcfSection dcfLinguistRoot;
    DcfSection dcfAssistantRoot;
    DcfSection dcfQmakeRoot;
    HelpProjectWriter *helpProjectWriter;
    bool inLink;
    bool inContents;
    bool inSectionHeading;
    bool inTableHeader;
    int numTableRows;
    bool threeColumnEnumValueTable;
    QString link;
    QStringList sectionNumber;
    QRegExp funcLeftParen;
    QString style;
    QString postHeader;
    QString footer;
    QString address;
    bool pleaseGenerateMacRef;
    QString project;
    QString projectDescription;
    QString projectUrl;
    QString navigationLinks;
    QStringList stylesheets;
    QStringList customHeadElements;
    const Tree *tre;
    bool slow;
    QMap<QString, QMap<QString, const Node *> > moduleClassMap;
    QMap<QString, QMap<QString, const Node *> > moduleNamespaceMap;
    QMap<QString, const Node *> nonCompatClasses;
    QMap<QString, const Node *> mainClasses;
    QMap<QString, const Node *> compatClasses;
    QMap<QString, const Node *> namespaceIndex;
    QMap<QString, const Node *> serviceClasses;
    QMap<QString, QMap<QString, const Node *> > funcIndex;
    QMap<Text, const Node *> legaleseTexts;
};

#define HTMLGENERATOR_ADDRESS           "address"
#define HTMLGENERATOR_FOOTER            "footer"
#define HTMLGENERATOR_GENERATEMACREFS    "generatemacrefs" // ### document me
#define HTMLGENERATOR_POSTHEADER        "postheader"
#define HTMLGENERATOR_STYLE             "style"
#define HTMLGENERATOR_STYLESHEETS       "stylesheets"
#define HTMLGENERATOR_CUSTOMHEADELEMENTS "customheadelements"

QT_END_NAMESPACE

#endif
