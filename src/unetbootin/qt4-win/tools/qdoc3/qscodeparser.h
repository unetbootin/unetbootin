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
  qscodeparser.h
*/

#ifndef QSCODEPARSER_H
#define QSCODEPARSER_H

#include "cppcodeparser.h"
#include "cpptoqsconverter.h"

QT_BEGIN_NAMESPACE

class QsCodeParser : public CppCodeParser
{
public:
    QsCodeParser( Tree *cppTree );
    ~QsCodeParser();

    virtual void initializeParser( const Config& config );
    virtual void terminateParser();
    virtual QString language();
    virtual QString headerFileNameFilter();
    virtual QString sourceFileNameFilter();
    virtual void parseHeaderFile( const Location& location,
				  const QString& filePath, Tree *tree );
    virtual void parseSourceFile( const Location& location,
				  const QString& filePath, Tree *tree );
    virtual void doneParsingHeaderFiles( Tree *tree );
    virtual void doneParsingSourceFiles( Tree *tree );

    FunctionNode *findFunctionNode( const QString& synopsis, Tree *tree );

protected:
    virtual QSet<QString> topicCommands();
    virtual Node *processTopicCommand( const Doc& doc, const QString& command,
				       const QString& arg );
    virtual QSet<QString> otherMetaCommands();
    virtual void processOtherMetaCommand( const Doc& doc,
					  const QString& command,
					  const QString& arg, Node *node );

private:
    ClassNode *tryClass( const QString& className );
    FunctionNode *findKernelFunction( const QStringList& parentPath,
				      const QString& name );
    void extractRegExp( const QRegExp& regExp, QString& source,
			const Doc& doc );
    void extractTarget( const QString& target, QString& source,
			const Doc& doc );
    void renameParameters( QString& source, const Doc& doc,
			   const QStringList& qtNames,
			   const QStringList& quickNames );
    void applyReplacementList( QString& source, const Doc& doc );
    void quickifyClass( ClassNode *quickClass );
    void quickifyEnum( ClassNode *quickClass, EnumNode *enume );
    void quickifyFunction( ClassNode *quickClass, ClassNode *qtClass,
			   FunctionNode *func, bool onBlackList );
    void quickifyProperty( ClassNode *quickClass, ClassNode *qtClass,
			   PropertyNode *property );
    QString quickifiedDoc( const QString& source );
    void setQtDoc( Node *quickNode, const Doc& doc );
    void setQuickDoc( Node *quickNode, const Doc& doc,
		      const QStringList& qtParams = QStringList(),
		      const QStringList& quickParams = QStringList() );
    bool makeFunctionNode( const QString& synopsis, QStringList *parentPathPtr,
			   FunctionNode **funcPtr );

    static bool isWord( QChar ch );
    static bool leftWordBoundary( const QString& str, int pos );
    static bool rightWordBoundary( const QString& str, int pos );

    QMap<QString, Node::Type> nodeTypeMap;
    QMap<QString, Node *> classesWithNoQuickDoc;
    QList<QRegExp> replaceBefores;
    QStringList replaceAfters;
    QSet<QString> classesWithNoQ;
    Tree *cppTre;
    Tree *qsTre;
    QRegExp replaceRegExp;
    CppToQsConverter cpp2qs;

    static int tabSize;
};

QT_END_NAMESPACE

#endif
