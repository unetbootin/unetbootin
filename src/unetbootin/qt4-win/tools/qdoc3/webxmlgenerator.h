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
  WebXMLGenerator.h
*/

#ifndef WEBXMLGENERATOR_H
#define WEBXMLGENERATOR_H

#include "codemarker.h"
#include "config.h"
#include "pagegenerator.h"

QT_BEGIN_NAMESPACE

class WebXMLGenerator : public PageGenerator
{
public:
    WebXMLGenerator();
    ~WebXMLGenerator();

    virtual void initializeGenerator(const Config &config);
    virtual void terminateGenerator();
    virtual QString format();
    virtual void generateTree(const Tree *tree, CodeMarker *marker);

protected:
    virtual void startText( const Node *relative, CodeMarker *marker );
    virtual int generateAtom(QXmlStreamWriter &writer, const Atom *atom,
                             const Node *relative, CodeMarker *marker );
    virtual void generateClassLikeNode(const InnerNode *inner, CodeMarker *marker);
    virtual void generateFakeNode(const FakeNode *fake, CodeMarker *marker);
    virtual QString fileExtension(const Node *node);

    virtual const Atom *addAtomElements(QXmlStreamWriter &writer, const Atom *atom,
                                 const Node *relative, CodeMarker *marker);
    virtual void generateIndexSections(QXmlStreamWriter &writer, const Node *node,
                                       CodeMarker *marker);
    virtual void generateInnerNode( const InnerNode *node, CodeMarker *marker );

private:
    const Node *findNode(const Atom *atom, const Node *relative, CodeMarker *marker);
    QString targetType(const Node *node);

    const Tree *tre;
    bool generateIndex;
    bool inLink;
    bool inContents;
    bool inSectionHeading;
    bool inTableHeader;
    int numTableRows;
    bool threeColumnEnumValueTable;
    QString link;
    QString project;
    QString projectDescription;
    QString projectUrl;
    QStringList sectionNumber;
};

QT_END_NAMESPACE

#endif
