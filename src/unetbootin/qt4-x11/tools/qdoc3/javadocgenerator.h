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
