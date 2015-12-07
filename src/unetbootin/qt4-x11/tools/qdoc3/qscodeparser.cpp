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
  qscodeparser.cpp
*/

#include <qfile.h>
#include <qregexp.h>

#include "config.h"
#include "qscodeparser.h"
#include "text.h"
#include "tokenizer.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

#define CONFIG_QUICK                    "quick"
#define CONFIG_REPLACES                 "replaces"

#define COMMAND_BRIEF                   Doc::alias( "brief" )
#define COMMAND_CODE                    Doc::alias( "code" )
#define COMMAND_ENDCODE                 Doc::alias( "endcode" )
#define COMMAND_ENDQUICKCODE            Doc::alias( "endquickcode" )
#define COMMAND_FILE                    Doc::alias( "file" )
#define COMMAND_GROUP                   Doc::alias( "group" )
#define COMMAND_MODULE                  Doc::alias( "module" )
#define COMMAND_PAGE                    Doc::alias( "page" )
#define COMMAND_QUICKCLASS              Doc::alias( "quickclass" )
#define COMMAND_QUICKCODE               Doc::alias( "quickcode" )
#define COMMAND_QUICKENUM               Doc::alias( "quickenum" )
#define COMMAND_QUICKFN                 Doc::alias( "quickfn" )
#define COMMAND_QUICKIFY                Doc::alias( "quickify" )
#define COMMAND_QUICKPROPERTY           Doc::alias( "quickproperty" )
#define COMMAND_PROTECTED               Doc::alias( "protected" )
#define COMMAND_REPLACE                 Doc::alias( "replace" )

static QString balancedParens = "(?:[^()]+|\\([^()]*\\))*";

QsCodeParser::QsCodeParser( Tree *cppTree )
    : cppTre( cppTree ), qsTre( 0 ), replaceRegExp( "/(.+)/([^/]*)/" )
{
}

QsCodeParser::~QsCodeParser()
{
}

void QsCodeParser::initializeParser( const Config& config )
{
    CppCodeParser::initializeParser( config );

    nodeTypeMap.insert( COMMAND_QUICKCLASS, Node::Class );
    nodeTypeMap.insert( COMMAND_QUICKENUM, Node::Enum );
    nodeTypeMap.insert( COMMAND_QUICKPROPERTY, Node::Property );
    nodeTypeMap.insert( COMMAND_QUICKFN, Node::Function );

    QString quickDotReplaces = CONFIG_QUICK + Config::dot + CONFIG_REPLACES;
    QStringList replaces = config.getStringList( quickDotReplaces );
    QStringList::ConstIterator r = replaces.begin();
    while ( r != replaces.end() ) {
	if ( replaceRegExp.exactMatch(*r) ) {
	    QRegExp before( replaceRegExp.cap(1) );
	    before.setMinimal( true );
	    QString after = replaceRegExp.cap( 2 );

	    if ( before.isValid() ) {
		replaceBefores << before;
		replaceAfters << after;
	    } else {
		config.lastLocation().warning(
			tr("Invalid regular expression '%1'")
			.arg(before.pattern()) );
	    }
	} else {
	    config.lastLocation().warning( tr("Bad syntax in '%1'")
					   .arg(quickDotReplaces) );
	}
	++r;
    }
}

void QsCodeParser::terminateParser()
{
    nodeTypeMap.clear();
    classesWithNoQuickDoc.clear();
    replaceBefores.clear();
    replaceAfters.clear();
    CppCodeParser::terminateParser();
}

QString QsCodeParser::language()
{
    return "Qt Script";
}

QString QsCodeParser::headerFileNameFilter()
{
    return "*";
}

QString QsCodeParser::sourceFileNameFilter()
{
    return "*.qs *.qsd";
}

void QsCodeParser::parseHeaderFile( const Location& location,
				    const QString& filePath, Tree *tree )
{
    qsTre = tree;

    FILE *in = fopen( QFile::encodeName(filePath), "r" );
    if ( in == 0 ) {
	location.error( tr("Cannot open Qt Script class list '%1'")
			.arg(filePath) );
	return;
    }

    Location fileLocation( filePath );
    Tokenizer fileTokenizer( fileLocation, in );
    int tok = fileTokenizer.getToken();
    while ( tok != Tok_Eoi ) {
	if ( tok == Tok_Ident ) {
	    ClassNode *quickClass = new ClassNode( qsTre->root(),
						   fileTokenizer.lexeme() );
	    quickClass->setLocation( fileTokenizer.location() );
	} else {
	    fileTokenizer.location().error( tr("Unexpected token '%1' in Qt"
					       " Script class list")
					    .arg(fileTokenizer.lexeme()) );
	    break;
	}
	tok = fileTokenizer.getToken();
    }
    fclose( in );
}

void QsCodeParser::parseSourceFile( const Location& location,
				    const QString& filePath, Tree *tree )
{
    qsTre = tree;
    CppCodeParser::parseSourceFile( location, filePath, tree );
}

void QsCodeParser::doneParsingHeaderFiles( Tree *tree )
{
    NodeList::ConstIterator c = tree->root()->childNodes().begin();
    while ( c != tree->root()->childNodes().end() ) {
	if ( (*c)->type() == Node::Class )
	    quickifyClass( (ClassNode *) *c );
	++c;
    }
    cppTre->root()->deleteChildren(); // save memory
    tree->resolveInheritance();
    tree->resolveProperties();
}

void QsCodeParser::doneParsingSourceFiles( Tree *tree )
{
    tree->root()->normalizeOverloads();

    NodeList::ConstIterator c = tree->root()->childNodes().begin();
    while ( c != tree->root()->childNodes().end() ) {
	if ( (*c)->type() == Node::Class ) {
	    QMap<QString, Node *>::ConstIterator cwnqd =
		    classesWithNoQuickDoc.find( (*c)->name() );
	    if ( cwnqd != classesWithNoQuickDoc.end() ) {
		(*cwnqd)->location().warning( tr("No '\\%1' documentation for"
						 " class '%2'")
					      .arg(COMMAND_QUICKCLASS)
					      .arg(cwnqd.key()) );
		(*cwnqd)->setDoc( Doc(), true );
	    }
	}
	++c;
    }

    // ### check which enum types are used
}

FunctionNode *QsCodeParser::findFunctionNode( const QString& synopsis,
					      Tree *tree )
{
    QStringList parentPath;
    FunctionNode *clone;
    FunctionNode *func = 0;

    if ( makeFunctionNode(synopsis, &parentPath, &clone) ) {
	func = tree->findFunctionNode( parentPath, clone );
	delete clone;
    }
    return func;
}

QSet<QString> QsCodeParser::topicCommands()
{
    return QSet<QString>() << COMMAND_FILE << COMMAND_GROUP << COMMAND_MODULE
			   << COMMAND_PAGE << COMMAND_QUICKCLASS
			   << COMMAND_QUICKENUM << COMMAND_QUICKFN
			   << COMMAND_QUICKPROPERTY;
}

Node *QsCodeParser::processTopicCommand( const Doc& doc, const QString& command,
					 const QString& arg )
{
    if ( command == COMMAND_QUICKFN ) {
	QStringList parentPath;
	FunctionNode *quickFunc = 0;
	FunctionNode *clone;

	if ( makeFunctionNode(arg, &parentPath, &clone) ) {
	    FunctionNode *kernelFunc = findKernelFunction( parentPath,
							   clone->name() );
	    if ( kernelFunc != 0 )
		kernelFunc->setAccess( Node::Private );

	    quickFunc = qsTre->findFunctionNode( parentPath, clone );
	    if ( quickFunc == 0 && kernelFunc != 0 ) {
		quickFunc = new FunctionNode( kernelFunc->parent(),
					      kernelFunc->name() );
		quickFunc->setLocation( kernelFunc->location() );
		quickFunc->setReturnType( clone->returnType() );
		quickFunc->setParameters( clone->parameters() );
	    }

	    if ( quickFunc == 0 ) {
		doc.location().warning( tr("Cannot find '%1' specified with '\\%2'")
					.arg(arg).arg(command) );
	    } else {
		quickFunc->setAccess( Node::Public );
		QStringList qtParams = quickFunc->parameterNames();
		quickFunc->borrowParameterNames( clone );
		QStringList quickParams = quickFunc->parameterNames();
		setQuickDoc( quickFunc, doc, qtParams, quickParams );
	    }
	    delete clone;
	} else {
	    doc.location().warning( tr("Cannot find '%1' specified with '\\%2'")
				    .arg(arg).arg(command) );
	}
	return 0;
    } else if ( nodeTypeMap.contains(command) ) {
	QStringList subArgs = arg.split(" ");
	QString dataType;

	if ( subArgs.count() == 3 && subArgs[1] == ":" ) {
	    dataType = subArgs[2];
	} else if ( subArgs.count() != 1 ) {
	    doc.location().warning( tr("Invalid syntax in '\\%1'")
				    .arg(command) );
	}

	QStringList path = subArgs[0].split(".");
	Node *quickNode = qsTre->findNode( path, nodeTypeMap[command] );
	if ( quickNode == 0 ) {
	    doc.location().warning( tr("Cannot find '%1' specified with '\\%2'")
				    .arg(arg).arg(command) );
	} else {
	    setQuickDoc( quickNode, doc );
	    if ( quickNode->type() == Node::Class ) {
		classesWithNoQuickDoc.remove( quickNode->name() );
		if ( doc.briefText().isEmpty() )
		    doc.location().warning( tr("Missing '\\%1' for class '%2'")
					    .arg(COMMAND_BRIEF)
					    .arg(quickNode->name()) );
	    } else if ( quickNode->type() == Node::Property ) {
		PropertyNode *quickProperty = (PropertyNode *) quickNode;
		if ( quickProperty->dataType() == "Object" ) {
		    if ( dataType.isEmpty() ) {
			doc.location().warning( tr("Missing data type in '\\%1'"
						   " (assuming 'Object')")
						.arg(command) );
		    } else {
			quickProperty->setDataType( dataType );
		    }
		} else if ( dataType != quickProperty->dataType() ) {
		    doc.location().warning( tr("Ignored contradictory data type"
					       " in '\\%1'")
					    .arg(command) );
		}
	    }
	}
	return 0;
    } else {
	return CppCodeParser::processTopicCommand( doc, command, arg );
    }
}

QSet<QString> QsCodeParser::otherMetaCommands()
{
    return commonMetaCommands() << COMMAND_ENDQUICKCODE << COMMAND_QUICKCODE
				<< COMMAND_QUICKIFY << COMMAND_REPLACE;
}

void QsCodeParser::processOtherMetaCommand( const Doc& doc,
			 		    const QString& command,
					    const QString& arg, Node *node )
{
    if ( command == COMMAND_PROTECTED ) {
	doc.location().warning( tr("Cannot use '\\%1' in %2")
				.arg(COMMAND_PROTECTED).arg(language()) );
    } else {
	CppCodeParser::processOtherMetaCommand( doc, command, arg, node );
    }
}

ClassNode *QsCodeParser::tryClass( const QString& className )
{
    return (ClassNode *) cppTre->findNode( QStringList(className), Node::Class );
}

FunctionNode *QsCodeParser::findKernelFunction( const QStringList& parentPath,
						const QString& name )
{
    FunctionNode clone( 0, name );
    clone.setReturnType( "Object" );
    clone.addParameter( Parameter("...") );
    return qsTre->findFunctionNode( parentPath, &clone );
}

void QsCodeParser::extractRegExp( const QRegExp& regExp, QString& source,
				  const Doc& doc )
{
    QRegExp blankLineRegExp(
	    "[ \t]*(?:\n(?:[ \t]*\n)+[ \t]*|[ \n\t]*\\\\code|"
	    "\\\\endcode[ \n\t]*)" );
    QStringList paras = source.trimmed().split(blankLineRegExp);
    paras = paras.filter( regExp );
    if ( paras.count() == 0 ) {
	doc.location().warning( tr("Cannot find regular expression '%1'")
				.arg(regExp.pattern()) );
    } else if ( paras.count() > 1 ) {
	doc.location().warning( tr("Regular rexpression '%1' matches multiple"
				   "times").arg(regExp.pattern()) );
    } else {
	source = paras.first() + "\n\n";
    }
}

void QsCodeParser::extractTarget( const QString& target, QString& source,
				  const Doc& doc )
{
    QRegExp targetRegExp(
	    "(\\\\target\\s+(\\S+)[^\n]*\n"
	    "(?:(?!\\s*\\\\code)[^\n]+\n|\\s*\\\\code.*\\\\endcode\\s*\n)*)"
	    "(?:\\s*\n|[^\n]*$)" );
    targetRegExp.setMinimal( true );

    int pos = 0;
    while ( (pos = source.indexOf(targetRegExp, pos)) != -1 ) {
	if ( targetRegExp.cap(2) == target ) {
	    source = targetRegExp.cap( 1 ) + "\n\n";
	    return;
	}
	pos += targetRegExp.matchedLength();
    }
    doc.location().warning( tr("Cannot find target '%1'").arg(target) );
}

void QsCodeParser::renameParameters( QString& source, const Doc& /* doc */,
				     const QStringList& qtParams,
				     const QStringList& quickParams )
{
    QRegExp paramRegExp( "(\\\\a\\s*\\{?\\s*)([A-Za-z0-9_]+)" );

    int pos = 0;
    while ( (pos = paramRegExp.indexIn(source, pos)) != -1 ) {
	pos += paramRegExp.cap( 1 ).length();
	QString before = paramRegExp.cap( 2 );
	int index = qtParams.indexOf( before );
	if ( index != -1 ) {
	    QString after = quickParams[index];
	    source.replace( pos, before.size(), after );
	}
    }
}

void QsCodeParser::applyReplacementList( QString& source, const Doc& doc )
{
    QStringList args = doc.metaCommandArgs( COMMAND_REPLACE );
    QStringList::ConstIterator a = args.begin();
    while ( a != args.end() ) {
	if ( replaceRegExp.exactMatch(*a) ) {
	    QRegExp before( replaceRegExp.cap(1) );
	    before.setMinimal( true );
	    QString after = replaceRegExp.cap( 2 );

	    if ( before.isValid() ) {
		int oldLen = source.size();
		source.replace( before, after );

		// this condition is sufficient but not necessary
		if ( oldLen == source.size() && !source.contains(after) )
		    doc.location().warning(
			    tr("Regular expression '%1' did not match anything")
			    .arg(before.pattern()) );
	    } else {
		doc.location().warning(
			tr("Invalid regular expression '%1'")
			.arg(before.pattern()) );
	    }
	} else {
	    doc.location().warning( tr("Bad syntax in '\\%1'")
				    .arg(COMMAND_REPLACE) );
	}
	++a;
    }

    QRegExp codeRegExp( "\\\\" + COMMAND_CODE + "(.*)\\\\" + COMMAND_ENDCODE );
    codeRegExp.setMinimal( true );

    QRegExp quickcodeRegExp(
	    "\\\\" + COMMAND_QUICKCODE + "(.*)\\\\" + COMMAND_ENDQUICKCODE );
    quickcodeRegExp.setMinimal( true );

    int quickcodePos = doc.source().indexOf( quickcodeRegExp );
    if ( quickcodePos != -1 ) {
	int codePos = source.indexOf( codeRegExp );
	if ( codePos == -1 ) {
	    doc.location().warning(
		    tr("Cannot find any '\\%1' snippet corresponding to '\\%2'")
		    .arg(COMMAND_CODE).arg(COMMAND_QUICKCODE) );
	} else {
	    source.replace( codeRegExp.pos(1), codeRegExp.cap(1).length(),
			    quickcodeRegExp.cap(1) );
	    codePos = codeRegExp.pos( 1 ) + quickcodeRegExp.cap( 1 ).length();

	    if ( doc.source().indexOf(quickcodeRegExp, quickcodePos + 1) != -1 ) {
		doc.location().warning(
			tr("Cannot use '\\%1' twice in a row")
			.arg(COMMAND_QUICKCODE) );
	    } else if ( source.indexOf(codeRegExp, codePos + 1) != -1 ) {
		doc.location().warning( tr("Ambiguous '\\%1'")
					.arg(COMMAND_QUICKCODE) );
	    }
	}
    }
}

void QsCodeParser::quickifyClass( ClassNode *quickClass )
{
    QString qtClassName = quickClass->name();
    QString bare = quickClass->name();
    if ( bare != "Qt" && bare != "Object" ) {
	if ( bare.startsWith("Q") ) {
	    bare = bare.mid( 1 );
	} else {
	    qtClassName.prepend( "Q" );
	    classesWithNoQ.insert( bare );
	}
    }

    ClassNode *qtClass = 0;
    ClassNode *wrapperClass = 0;

    if ( (wrapperClass = tryClass("Quick" + bare)) != 0 ||
	 (wrapperClass = tryClass("QS" + bare + "Class")) != 0 ) {
	qtClass = tryClass( qtClassName );
	if ( qtClass == 0 ) {
	    qtClass = wrapperClass;
	    wrapperClass = 0;
	}
    } else if ( (wrapperClass = tryClass("Quick" + bare + "Ptr")) != 0 ) {
	QRegExp ptrToQtType( "(Q[A-Za-z0-9_]+)\\s*\\*" );
	FunctionNode *ctor =
		wrapperClass->findFunctionNode( wrapperClass->name() );
	if ( ctor != 0 && !ctor->parameters().isEmpty() &&
	     ptrToQtType.exactMatch(ctor->parameters().first().leftType()) )
	    qtClassName = ptrToQtType.cap( 1 );
	qtClass = tryClass( qtClassName );
    } else {
	wrapperClass = tryClass( "Q" + bare + "Ptr" );
	if ( wrapperClass == 0 )
	    wrapperClass = tryClass( "Quick" + bare + "Interface" );
	qtClass = tryClass( qtClassName );
    }

    if ( qtClass == 0 ) {
	if ( wrapperClass == 0 ) {
	    quickClass->location().warning( tr("Cannot find Qt class '%1'")
					    .arg(qtClassName) );
	} else {
	    quickClass->location().warning( tr("Cannot find Qt class '%1'"
					       " wrapped by '%2'")
					    .arg(qtClassName)
					    .arg(wrapperClass->name()) );
	}
	return;
    }

    QList<RelatedClass>::ConstIterator r = qtClass->baseClasses().begin();
    while ( r != qtClass->baseClasses().end() ) {
	ClassNode *quickBaseClass = cpp2qs.findClassNode( qsTre,
							  (*r).node->name() );
	if (quickBaseClass)
	    quickClass->addBaseClass((*r).access, quickBaseClass);
	++r;
    }
    if ( quickClass->baseClasses().isEmpty() && quickClass->name() != "Object" )
	quickClass->addBaseClass(Node::Public, cpp2qs.findClassNode(qsTre, "Object"));

    QSet<QString> funcBlackList;
    QSet<QString> propertyBlackList;

    NodeList children;
    if ( wrapperClass != 0 ) {
	children = wrapperClass->childNodes();

	funcBlackList.insert( wrapperClass->name() );
	funcBlackList.insert( "~" + wrapperClass->name() );
    }
    children += qtClass->childNodes();

    for ( int pass = 0; pass < 2; pass++ ) {
	NodeList::ConstIterator c = children.begin();
	while ( c != children.end() ) {
	    if ( (*c)->access() != Node::Private &&
		 (*c)->status() == Node::Commendable ) {
		if ( pass == 0 ) {
		    if ( (*c)->type() == Node::Enum ) {
			EnumNode *enume = (EnumNode *) *c;
			quickifyEnum( quickClass, enume );
		    } else if ( (*c)->type() == Node::Property ) {
			if ( !propertyBlackList.contains((*c)->name()) ) {
			    PropertyNode *property = (PropertyNode *) *c;
			    quickifyProperty( quickClass, qtClass, property );
			    if (!property->getters().isEmpty())
				funcBlackList.insert(property->getters().first()->name());
			    if (!property->setters().isEmpty())
				funcBlackList.insert(property->setters().first()->name());
			    if (!property->resetters().isEmpty())
				funcBlackList.insert(property->resetters().first()->name());
			    propertyBlackList.insert( property->name() );
			}
		    }
		} else if ( (*c)->type() == Node::Function )  {
		    FunctionNode *func = (FunctionNode *) *c;
		    quickifyFunction( quickClass, qtClass, func,
				      funcBlackList.contains((*c)->name()) &&
				      func->parameters().count() < 2 );
		}
	    }
	    ++c;
	}
    }
    setQtDoc( quickClass, qtClass->doc() );
    classesWithNoQuickDoc.insert( quickClass->name(), quickClass );
}

void QsCodeParser::quickifyEnum( ClassNode *quickClass, EnumNode *enume )
{
    EnumNode *quickEnum = new EnumNode( quickClass, enume->name() );
    quickEnum->setLocation( enume->location() );
#if 0 // ### not yet
    quickEnum->setAccess( Node::Protected );
#endif

    QList<EnumItem>::ConstIterator it = enume->items().begin();
    while ( it != enume->items().end() ) {
	QString name = (*it).name();
	QString value = (*it).value();
	quickEnum->addItem( EnumItem(name, value) );
	++it;
    }
    setQtDoc( quickEnum, enume->doc() );
}

void QsCodeParser::quickifyFunction( ClassNode *quickClass, ClassNode *qtClass,
				     FunctionNode *func, bool onBlackList )
{
    if ( func->metaness() == FunctionNode::Dtor )
	return;

    FunctionNode *kernelFunc = findKernelFunction(
	    QStringList() << quickClass->name(), func->name() );

    QString quickName = func->name();
    if ( func->metaness() == FunctionNode::Ctor )
	quickName = quickClass->name();
    FunctionNode *quickFunc = new FunctionNode( quickClass, quickName );
    quickFunc->setLocation( func->location() );

    if ( onBlackList ) {
	quickFunc->setAccess( Node::Protected );
    } else {
	if ( kernelFunc != 0 && func->numOverloads() == 1 &&
	     (func->parameters().count() == 0 ||
	      func->parameters().last().defaultValue().isEmpty()) ) {
	    kernelFunc->setAccess( Node::Private );
	} else {
	    if ( func->metaness() == FunctionNode::Plain )
		quickFunc->setAccess( Node::Protected );
	}
    }

    quickFunc->setReturnType( cpp2qs.convertedDataType(qsTre,
						       func->returnType()) );
    if ( func->metaness() != FunctionNode::Slot )
	quickFunc->setMetaness( func->metaness() );
    quickFunc->setVirtualness( FunctionNode::ImpureVirtual );
    quickFunc->setOverload( func->isOverload() );

    QList<Parameter>::ConstIterator q = func->parameters().begin();
    while ( q != func->parameters().end() ) {
	QString dataType = cpp2qs.convertedDataType( qsTre, (*q).leftType(),
						     (*q).rightType() );
	if ( dataType.isEmpty() ) {
	    dataType = "UNKNOWN";
	    quickFunc->setAccess( Node::Private );
	}
	Parameter param( dataType, "", (*q).name(),
			 (*q).defaultValue().isEmpty() ? "" : "undefined" );
	quickFunc->addParameter( param );
	++q;
    }

    if ( func->doc().isEmpty() ) {
	if ( func->parent() != (InnerNode *) qtClass ) {
	    func = qtClass->findFunctionNode( func );
	    if ( func != 0 )
		setQtDoc( quickFunc, func->doc() );
	}
    } else {
	setQtDoc( quickFunc, func->doc() );
    }
}

void QsCodeParser::quickifyProperty(ClassNode *quickClass, ClassNode * /* qtClass */,
				    PropertyNode *property)
{
    PropertyNode *quickProperty = new PropertyNode(quickClass, property->name());
    quickProperty->setLocation( property->location() );
    quickProperty->setDataType(cpp2qs.convertedDataType(qsTre, property->dataType()));
#if 0
    quickProperty->setGetter( property->getter() );
    quickProperty->setSetter( property->setter() );
    quickProperty->setResetter( property->resetter() );
#endif
    quickProperty->setStored( property->isStored() );
    quickProperty->setDesignable( property->isDesignable() );

    setQtDoc( quickProperty, property->doc() );
}

QString QsCodeParser::quickifiedDoc( const QString& source )
{
    QString result;
    int i = 0;

    while ( i < (int) source.length() ) {
	if ( leftWordBoundary(source, i) ) {
	    if ( source[i] == 'Q' ) {
		if ( source[i + 1] == 'C' && source.mid(i, 8) == "QCString" ) {
		    i += 2;
		} else {
		    int end = i + 1;
		    while ( isWord(source[end]) )
			++end;
		    if ( !classesWithNoQ.contains(
				  source.mid(i + 1, end - (i + 1))) )
			result += "Q";
		    i++;
		}
	    } else if ( source[i] == 'T' && source.mid(i, 4) == "TRUE" &&
			rightWordBoundary(source, i + 4) ) {
		result += "\\c{true}";
		i += 4;
	    } else if ( source[i] == 'F' && source.mid(i, 5) == "FALSE" &&
			rightWordBoundary(source, i + 5) ) {
		result += "\\c{false}";
		i += 5;
	    } else if ( source[i] == 'c' && source.mid(i, 6) == "const " ) {
		i += 6;
	    } else {
		result += source[i++];
	    }
	} else if ( (source[i] == ':' && source[i + 1] == ':') ||
		    (source[i] == '-' && source[i + 1] == '>') ) {
	    result += '.';
	    i += 2;
	} else if ( source[i] == '\\' ) {
	    // ### make independent of the command name
	    if ( source.mid(i, 5) == "\\code" ) {
		do {
		    result += source[i++];
		} while ( source[i - 1] != '\n' );

		int begin = i;
		int end = source.indexOf( "\\endcode", i );
		if ( end != -1 ) {
		    QString code = source.mid( begin, end - begin );
		    result += cpp2qs.convertedCode( qsTre, code,
						    classesWithNoQ );
		    i = end;
		}
	    } else {
		result += source[i++];
	    }
	} else {
	    result += source[i++];
	}
    }

    QList<QRegExp>::ConstIterator b = replaceBefores.begin();
    QStringList::ConstIterator a = replaceAfters.begin();
    while ( a != replaceAfters.end() ) {
	result.replace( *b, *a );
	++b;
	++a;
    }
    return result;
}

void QsCodeParser::setQtDoc( Node *quickNode, const Doc& doc )
{
    if ( !doc.isEmpty() ) {
	Doc quickDoc(doc.location(), doc.location(),
                     quickifiedDoc(doc.source()),
		     CppCodeParser::topicCommands() + CppCodeParser::otherMetaCommands());
	quickNode->setDoc( quickDoc, true );
    }
}

void QsCodeParser::setQuickDoc( Node *quickNode, const Doc& doc,
				const QStringList& qtParams,
				const QStringList& quickParams )
{
    QRegExp quickifyCommand( "\\\\" + COMMAND_QUICKIFY + "([^\n]*)(?:\n|$)" );

    if ( quickNode->type() == Node::Function ) {
	FunctionNode *quickFunc = (FunctionNode *) quickNode;
	quickFunc->setOverload( false );
    }

    if (doc.metaCommandsUsed().contains(COMMAND_QUICKIFY)) {
	QString source = doc.source();
	int pos = source.indexOf( quickifyCommand );
	if ( pos != -1 ) {
	    QString quickifiedSource = quickNode->doc().source();
	    if ( !qtParams.isEmpty() && qtParams != quickParams )
		renameParameters( quickifiedSource, doc, qtParams,
				  quickParams );
	    applyReplacementList( quickifiedSource, doc );

	    do {
		QString extract = quickifiedSource;
		QString arg = quickifyCommand.cap( 1 ).simplified();
		if ( !arg.isEmpty() ) {
		    if ( arg.startsWith("/") && arg.endsWith("/") &&
			 arg.length() > 2 ) {
			QString pattern = arg.mid( 1, arg.length() - 2 );
			extractRegExp( QRegExp(pattern), extract, doc );
		    } else {
			extractTarget( arg, extract, doc );
		    }
		}
		source.replace( pos, quickifyCommand.matchedLength(), extract );
		pos += extract.length();
	    } while ( (pos = source.indexOf(quickifyCommand, pos)) != -1 );

	    QRegExp quickcodeRegExp(
		    "\\\\" + COMMAND_QUICKCODE + "(.*)\\\\" +
		    COMMAND_ENDQUICKCODE );
	    quickcodeRegExp.setMinimal( true );
	    source.replace( quickcodeRegExp, "" );
	}

	Doc quickDoc( doc.location(), doc.location(), source,
		      (CppCodeParser::topicCommands() + topicCommands() +
		       CppCodeParser::otherMetaCommands()) << COMMAND_REPLACE );
	quickNode->setDoc( quickDoc, true );
	processOtherMetaCommands( quickDoc, quickNode );
    } else {
	quickNode->setDoc( doc, true );
	processOtherMetaCommands( doc, quickNode );
    }
}

bool QsCodeParser::makeFunctionNode( const QString& synopsis,
				     QStringList *parentPathPtr,
				     FunctionNode **funcPtr )
{
    QRegExp funcRegExp(
	    "\\s*([A-Za-z0-9_]+)\\.([A-Za-z0-9_]+)\\s*\\((" + balancedParens +
	    ")\\)(?:\\s*:\\s*([A-Za-z0-9_]+))?\\s*" );
    QRegExp paramRegExp(
	    "\\s*(\\[)?\\s*(?:([A-Za-z0-9_]+)\\s*:\\s*)?"
	    "([A-Za-z0-9_]+|\\.\\.\\.)\\s*(\\[)?[\\s\\]]*" );

    if ( !funcRegExp.exactMatch(synopsis) )
	return false;

    ClassNode *classe = (ClassNode *) qsTre->findNode( QStringList(funcRegExp.cap(1)),
						       Node::Class );
    if ( classe == 0 )
	return false;

    FunctionNode *clone = new FunctionNode( 0, funcRegExp.cap(2) );
    bool optional = false;

    QString paramStr = funcRegExp.cap( 3 );
    QStringList params = paramStr.split(",");
    QStringList::ConstIterator p = params.begin();
    while ( p != params.end() ) {
	if ( paramRegExp.exactMatch(*p) ) {
	    if ( !paramRegExp.cap(1).isEmpty() )
		optional = true;
	    clone->addParameter( Parameter(paramRegExp.cap(3), "",
					   paramRegExp.cap(2),
					   optional ? "undefined" : "") );
	    if ( !paramRegExp.cap(4).isEmpty() )
		optional = true;
	} else {
	    delete clone;
	    return false;
	}
	++p;
    }
    QString returnType = funcRegExp.cap( 4 );
    if ( !returnType.isEmpty() )
	clone->setReturnType( returnType );
    if ( parentPathPtr != 0 )
	*parentPathPtr = QStringList() << classe->name();
    if ( funcPtr != 0 ) {
	*funcPtr = clone;
    } else {
	delete clone;
    }
    return true;
}

bool QsCodeParser::isWord( QChar ch )
{
    return ch.isLetterOrNumber() || ch == QChar( '_' );
}

bool QsCodeParser::leftWordBoundary( const QString& str, int pos )
{
    return !isWord( str[pos - 1] ) && isWord( str[pos] );
}

bool QsCodeParser::rightWordBoundary( const QString& str, int pos )
{
    return isWord( str[pos - 1] ) && !isWord( str[pos] );
}

QT_END_NAMESPACE
