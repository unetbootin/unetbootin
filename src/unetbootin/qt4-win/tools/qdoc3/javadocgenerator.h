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

#ifndef JAVADOCGENERATOR_H
#define JAVADOCGENERATOR_H

#include "htmlgenerator.h"

QT_BEGIN_NAMESPACE

class JavadocGenerator : public HtmlGenerator
{
public:
    JavadocGenerator();
    ~JavadocGenerator();

    void initializeGenerator(const Config &config);
    void terminateGenerator();
    QString format();
    bool canHandleFormat(const QString &format) { return format == "HTML" || format == "javadoc"; }
    void generateTree(const Tree *tree, CodeMarker *marker);
    QString typeString(const Node *node);
    QString imageFileName(const Node *relative, const QString &fileBase);

protected:
    QString fileExtension(const Node *node);
    void startText( const Node *relative, CodeMarker *marker );
    void endText( const Node *relative, CodeMarker *marker );
    int generateAtom( const Atom *atom, const Node *relative, CodeMarker *marker );
    void generateClassLikeNode(const InnerNode *inner, CodeMarker *marker);
    void generateFakeNode( const FakeNode *fake, CodeMarker *marker );

    void generateText( const Text& text, const Node *relative, CodeMarker *marker );
    void generateBody( const Node *node, CodeMarker *marker );
    void generateAlsoList( const Node *node, CodeMarker *marker );

    QString refForNode( const Node *node );
    QString linkForNode( const Node *node, const Node *relative );
    QString refForAtom(Atom *atom, const Node *node);

private:
    void generateDcf(const QString &fileBase, const QString &startPage,
                     const QString &title, DcfSection &dcfRoot);
    void generateIndex(const QString &fileBase, const QString &url,
                       const QString &title);
    void generateIndent();
    void generateDoc(const Node *node, CodeMarker *marker);
    void generateEnumItemDoc(const Text &text, const Node *node, CodeMarker *marker);

    QString buffer;
    QIODevice *oldDevice;
    int currentDepth;
};

QT_END_NAMESPACE

#endif
