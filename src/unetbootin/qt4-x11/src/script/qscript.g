----------------------------------------------------------------------------
--
-- Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
-- Contact: Qt Software Information (qt-info@nokia.com)
--
-- This file is part of the QtScript module of the Qt Toolkit.
--
-- $QT_BEGIN_LICENSE:LGPL$
-- Commercial Usage
-- Licensees holding valid Qt Commercial licenses may use this file in
-- accordance with the Qt Commercial License Agreement provided with the
-- Software or, alternatively, in accordance with the terms contained in
-- a written agreement between you and Nokia.
--
-- GNU Lesser General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU Lesser
-- General Public License version 2.1 as published by the Free Software
-- Foundation and appearing in the file LICENSE.LGPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU Lesser General Public License version 2.1 requirements
-- will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
--
-- In addition, as a special exception, Nokia gives you certain
-- additional rights. These rights are described in the Nokia Qt LGPL
-- Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
-- package.
--
-- GNU General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU
-- General Public License version 3.0 as published by the Free Software
-- Foundation and appearing in the file LICENSE.GPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU General Public License version 3.0 requirements will be
-- met: http://www.gnu.org/copyleft/gpl.html.
--
-- If you are unsure which license is appropriate for your use, please
-- contact the sales department at qt-sales@nokia.com.
-- $QT_END_LICENSE$
--
-- This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
-- WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
--
----------------------------------------------------------------------------

%parser         QScriptGrammar
%decl           qscriptparser_p.h
%impl           qscriptparser.cpp
%expect         3
%expect-rr      1

%token T_AND "&"                T_AND_AND "&&"              T_AND_EQ "&="
%token T_BREAK "break"          T_CASE "case"               T_CATCH "catch"
%token T_COLON ":"              T_COMMA ";"                 T_CONTINUE "continue"
%token T_DEFAULT "default"      T_DELETE "delete"           T_DIVIDE_ "/"
%token T_DIVIDE_EQ "/="         T_DO "do"                   T_DOT "."
%token T_ELSE "else"            T_EQ "="                    T_EQ_EQ "=="
%token T_EQ_EQ_EQ "==="         T_FINALLY "finally"         T_FOR "for"
%token T_FUNCTION "function"    T_GE ">="                   T_GT ">"
%token T_GT_GT ">>"             T_GT_GT_EQ ">>="            T_GT_GT_GT ">>>"
%token T_GT_GT_GT_EQ ">>>="     T_IDENTIFIER "identifier"   T_IF "if"
%token T_IN "in"                T_INSTANCEOF "instanceof"   T_LBRACE "{"
%token T_LBRACKET "["           T_LE "<="                   T_LPAREN "("
%token T_LT "<"                 T_LT_LT "<<"                T_LT_LT_EQ "<<="
%token T_MINUS "-"              T_MINUS_EQ "-="             T_MINUS_MINUS "--"
%token T_NEW "new"              T_NOT "!"                   T_NOT_EQ "!="
%token T_NOT_EQ_EQ "!=="        T_NUMERIC_LITERAL "numeric literal"     T_OR "|"
%token T_OR_EQ "|="             T_OR_OR "||"                T_PLUS "+"
%token T_PLUS_EQ "+="           T_PLUS_PLUS "++"            T_QUESTION "?"
%token T_RBRACE "}"             T_RBRACKET "]"              T_REMAINDER "%"
%token T_REMAINDER_EQ "%="      T_RETURN "return"           T_RPAREN ")"
%token T_SEMICOLON ";"          T_AUTOMATIC_SEMICOLON       T_STAR "*"
%token T_STAR_EQ "*="           T_STRING_LITERAL "string literal"
%token T_SWITCH "switch"        T_THIS "this"               T_THROW "throw"
%token T_TILDE "~"              T_TRY "try"                 T_TYPEOF "typeof"
%token T_VAR "var"              T_VOID "void"               T_WHILE "while"
%token T_WITH "with"            T_XOR "^"                   T_XOR_EQ "^="
%token T_NULL "null"            T_TRUE "true"               T_FALSE "false"
%token T_CONST "const"
%token T_DEBUGGER "debugger"
%token T_RESERVED_WORD "reserved word"

%start Program

/.
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include <QtCore/QtDebug>

#ifndef QT_NO_SCRIPT

#include <string.h>

#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptlexer_p.h"
#include "qscriptast_p.h"
#include "qscriptnodepool_p.h"

#define Q_SCRIPT_UPDATE_POSITION(node, startloc, endloc) do { \
    node->startLine = startloc.startLine; \
    node->startColumn = startloc.startColumn; \
    node->endLine = endloc.endLine; \
    node->endColumn = endloc.endColumn; \
} while (0)

./

/:
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//
// This file is automatically generated from qscript.g.
// Changes will be lost.
//

#ifndef QSCRIPTPARSER_P_H
#define QSCRIPTPARSER_P_H

#include "qscriptgrammar_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptastfwd_p.h"

QT_BEGIN_NAMESPACE

class QString;
class QScriptEnginePrivate;
class QScriptNameIdImpl;

class QScriptParser: protected $table
{
public:
    union Value {
      int ival;
      double dval;
      QScriptNameIdImpl *sval;
      QScript::AST::ArgumentList *ArgumentList;
      QScript::AST::CaseBlock *CaseBlock;
      QScript::AST::CaseClause *CaseClause;
      QScript::AST::CaseClauses *CaseClauses;
      QScript::AST::Catch *Catch;
      QScript::AST::DefaultClause *DefaultClause;
      QScript::AST::ElementList *ElementList;
      QScript::AST::Elision *Elision;
      QScript::AST::ExpressionNode *Expression;
      QScript::AST::Finally *Finally;
      QScript::AST::FormalParameterList *FormalParameterList;
      QScript::AST::FunctionBody *FunctionBody;
      QScript::AST::FunctionDeclaration *FunctionDeclaration;
      QScript::AST::Node *Node;
      QScript::AST::PropertyName *PropertyName;
      QScript::AST::PropertyNameAndValueList *PropertyNameAndValueList;
      QScript::AST::SourceElement *SourceElement;
      QScript::AST::SourceElements *SourceElements;
      QScript::AST::Statement *Statement;
      QScript::AST::StatementList *StatementList;
      QScript::AST::VariableDeclaration *VariableDeclaration;
      QScript::AST::VariableDeclarationList *VariableDeclarationList;
    };

    struct Location {
      int startLine;
      int startColumn;
      int endLine;
      int endColumn;
    };

public:
    QScriptParser();
    ~QScriptParser();

    bool parse(QScriptEnginePrivate *driver);

    inline QString errorMessage() const
    { return error_message; }
    inline int errorLineNumber() const
    { return error_lineno; }
    inline int errorColumnNumber() const
    { return error_column; }

protected:
    inline void reallocateStack();

    inline Value &sym(int index)
    { return sym_stack [tos + index - 1]; }

    inline Location &loc(int index)
    { return location_stack [tos + index - 2]; }

protected:
    int tos;
    int stack_size;
    Value *sym_stack;
    int *state_stack;
    Location *location_stack;
    QString error_message;
    int error_lineno;
    int error_column;
};

inline void QScriptParser::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;

    sym_stack = reinterpret_cast<Value*> (qRealloc(sym_stack, stack_size * sizeof(Value)));
    state_stack = reinterpret_cast<int*> (qRealloc(state_stack, stack_size * sizeof(int)));
    location_stack = reinterpret_cast<Location*> (qRealloc(location_stack, stack_size * sizeof(Location)));
}

:/


/.

#include "qscriptparser_p.h"

//
// This file is automatically generated from qscript.g.
// Changes will be lost.
//

QT_BEGIN_NAMESPACE

inline static bool automatic(QScriptEnginePrivate *driver, int token)
{
    return token == $table::T_RBRACE
        || token == 0
        || driver->lexer()->prevTerminator();
}


QScriptParser::QScriptParser():
    tos(0),
    stack_size(0),
    sym_stack(0),
    state_stack(0),
    location_stack(0)
{
}

QScriptParser::~QScriptParser()
{
    if (stack_size) {
        qFree(sym_stack);
        qFree(state_stack);
        qFree(location_stack);
    }
}

static inline QScriptParser::Location location(QScript::Lexer *lexer)
{
    QScriptParser::Location loc;
    loc.startLine = lexer->startLineNo();
    loc.startColumn = lexer->startColumnNo();
    loc.endLine = lexer->endLineNo();
    loc.endColumn = lexer->endColumnNo();
    return loc;
}

bool QScriptParser::parse(QScriptEnginePrivate *driver)
{
  const int INITIAL_STATE = 0;
  QScript::Lexer *lexer = driver->lexer();

  int yytoken = -1;
  int saved_yytoken = -1;

  reallocateStack();

  tos = 0;
  state_stack[++tos] = INITIAL_STATE;

  while (true)
    {
      const int state = state_stack [tos];
      if (yytoken == -1 && - TERMINAL_COUNT != action_index [state])
        {
          if (saved_yytoken == -1)
            {
              yytoken = lexer->lex();
              location_stack [tos] = location(lexer);
            }
          else
            {
              yytoken = saved_yytoken;
              saved_yytoken = -1;
            }
        }

      int act = t_action (state, yytoken);

      if (act == ACCEPT_STATE)
        return true;

      else if (act > 0)
        {
          if (++tos == stack_size)
            reallocateStack();

          sym_stack [tos].dval = lexer->dval ();
          state_stack [tos] = act;
          location_stack [tos] = location(lexer);
          yytoken = -1;
        }

      else if (act < 0)
        {
          int r = - act - 1;

          tos -= rhs [r];
          act = state_stack [tos++];

          switch (r) {
./

PrimaryExpression: T_THIS ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ThisExpression> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_IDENTIFIER ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::IdentifierExpression> (driver->nodePool(), sym(1).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_NULL ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::NullExpression> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_TRUE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::TrueLiteral> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_FALSE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FalseLiteral> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_NUMERIC_LITERAL ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::NumericLiteral> (driver->nodePool(), sym(1).dval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_STRING_LITERAL ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::StringLiteral> (driver->nodePool(), sym(1).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_DIVIDE_ ;
/:
#define Q_SCRIPT_REGEXPLITERAL_RULE1 $rule_number
:/
/.
case $rule_number: {
  bool rx = lexer->scanRegExp(QScript::Lexer::NoPrefix);
  if (!rx) {
      error_message = lexer->errorMessage();
      error_lineno = lexer->startLineNo();
      error_column = lexer->startColumnNo();
      return false;
  }
  sym(1).Node = QScript::makeAstNode<QScript::AST::RegExpLiteral> (driver->nodePool(), lexer->pattern, lexer->flags);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_DIVIDE_EQ ;
/:
#define Q_SCRIPT_REGEXPLITERAL_RULE2 $rule_number
:/
/.
case $rule_number: {
  bool rx = lexer->scanRegExp(QScript::Lexer::EqualPrefix);
  if (!rx) {
      error_message = lexer->errorMessage();
      error_lineno = lexer->startLineNo();
      error_column = lexer->startColumnNo();
      return false;
  }
  sym(1).Node = QScript::makeAstNode<QScript::AST::RegExpLiteral> (driver->nodePool(), lexer->pattern, lexer->flags);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PrimaryExpression: T_LBRACKET ElisionOpt T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArrayLiteral> (driver->nodePool(), sym(2).Elision);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

PrimaryExpression: T_LBRACKET ElementList T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArrayLiteral> (driver->nodePool(), sym(2).ElementList->finish ());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

PrimaryExpression: T_LBRACKET ElementList T_COMMA ElisionOpt T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArrayLiteral> (driver->nodePool(), sym(2).ElementList->finish (), sym(4).Elision);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

-- PrimaryExpression: T_LBRACE T_RBRACE ;
-- /.
-- case $rule_number: {
--   sym(1).Node = QScript::makeAstNode<QScript::AST::ObjectLiteral> (driver->nodePool());
--   Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
-- } break;
-- ./

PrimaryExpression: T_LBRACE PropertyNameAndValueListOpt T_RBRACE ;
/.
case $rule_number: {
  if (sym(2).Node)
    sym(1).Node = QScript::makeAstNode<QScript::AST::ObjectLiteral> (driver->nodePool(), sym(2).PropertyNameAndValueList->finish ());
  else
    sym(1).Node = QScript::makeAstNode<QScript::AST::ObjectLiteral> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

PrimaryExpression: T_LBRACE PropertyNameAndValueList T_COMMA T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ObjectLiteral> (driver->nodePool(), sym(2).PropertyNameAndValueList->finish ());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

PrimaryExpression: T_LPAREN Expression T_RPAREN ;
/.
case $rule_number: {
  sym(1) = sym(2);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ElementList: ElisionOpt AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ElementList> (driver->nodePool(), sym(1).Elision, sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

ElementList: ElementList T_COMMA ElisionOpt AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ElementList> (driver->nodePool(), sym(1).ElementList, sym(3).Elision, sym(4).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

Elision: T_COMMA ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Elision> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

Elision: Elision T_COMMA ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Elision> (driver->nodePool(), sym(1).Elision);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

ElisionOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ElisionOpt: Elision ;
/.
case $rule_number: {
  sym(1).Elision = sym(1).Elision->finish ();
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PropertyNameAndValueList: PropertyName T_COLON AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::PropertyNameAndValueList> (driver->nodePool(), sym(1).PropertyName, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

PropertyNameAndValueList: PropertyNameAndValueList T_COMMA PropertyName T_COLON AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::PropertyNameAndValueList> (driver->nodePool(), sym(1).PropertyNameAndValueList, sym(3).PropertyName, sym(5).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

PropertyName: T_IDENTIFIER ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::IdentifierPropertyName> (driver->nodePool(), sym(1).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PropertyName: T_STRING_LITERAL ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::StringLiteralPropertyName> (driver->nodePool(), sym(1).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PropertyName: T_NUMERIC_LITERAL ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::NumericLiteralPropertyName> (driver->nodePool(), sym(1).dval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

PropertyName: ReservedIdentifier ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::IdentifierPropertyName> (driver->nodePool(), sym(1).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

ReservedIdentifier: T_BREAK ;
/.
case $rule_number:
./
ReservedIdentifier: T_CASE ;
/.
case $rule_number:
./
ReservedIdentifier: T_CATCH ;
/.
case $rule_number:
./
ReservedIdentifier: T_CONTINUE ;
/.
case $rule_number:
./
ReservedIdentifier: T_DEFAULT ;
/.
case $rule_number:
./
ReservedIdentifier: T_DELETE ;
/.
case $rule_number:
./
ReservedIdentifier: T_DO ;
/.
case $rule_number:
./
ReservedIdentifier: T_ELSE ;
/.
case $rule_number:
./
ReservedIdentifier: T_FALSE ;
/.
case $rule_number:
./
ReservedIdentifier: T_FINALLY ;
/.
case $rule_number:
./
ReservedIdentifier: T_FOR ;
/.
case $rule_number:
./
ReservedIdentifier: T_FUNCTION ;
/.
case $rule_number:
./
ReservedIdentifier: T_IF ;
/.
case $rule_number:
./
ReservedIdentifier: T_IN ;
/.
case $rule_number:
./
ReservedIdentifier: T_INSTANCEOF ;
/.
case $rule_number:
./
ReservedIdentifier: T_NEW ;
/.
case $rule_number:
./
ReservedIdentifier: T_NULL ;
/.
case $rule_number:
./
ReservedIdentifier: T_RETURN ;
/.
case $rule_number:
./
ReservedIdentifier: T_SWITCH ;
/.
case $rule_number:
./
ReservedIdentifier: T_THIS ;
/.
case $rule_number:
./
ReservedIdentifier: T_THROW ;
/.
case $rule_number:
./
ReservedIdentifier: T_TRUE ;
/.
case $rule_number:
./
ReservedIdentifier: T_TRY ;
/.
case $rule_number:
./
ReservedIdentifier: T_TYPEOF ;
/.
case $rule_number:
./
ReservedIdentifier: T_VAR ;
/.
case $rule_number:
./
ReservedIdentifier: T_VOID ;
/.
case $rule_number:
./
ReservedIdentifier: T_WHILE ;
/.
case $rule_number:
./
ReservedIdentifier: T_CONST ;
/.
case $rule_number:
./
ReservedIdentifier: T_DEBUGGER ;
/.
case $rule_number:
./
ReservedIdentifier: T_RESERVED_WORD ;
/.
case $rule_number:
./
ReservedIdentifier: T_WITH ;
/.
case $rule_number:
{
  sym(1).sval = driver->intern(lexer->characterBuffer(), lexer->characterCount());
} break;
./

PropertyIdentifier: T_IDENTIFIER ;
PropertyIdentifier: ReservedIdentifier ;

MemberExpression: PrimaryExpression ;
MemberExpression: FunctionExpression ;

MemberExpression: MemberExpression T_LBRACKET Expression T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArrayMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

MemberExpression: MemberExpression T_DOT PropertyIdentifier ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FieldMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

MemberExpression: T_NEW MemberExpression Arguments ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::NewMemberExpression> (driver->nodePool(), sym(2).Expression, sym(3).ArgumentList);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

NewExpression: MemberExpression ;

NewExpression: T_NEW NewExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::NewExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

CallExpression: MemberExpression Arguments ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CallExpression> (driver->nodePool(), sym(1).Expression, sym(2).ArgumentList);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

CallExpression: CallExpression Arguments ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CallExpression> (driver->nodePool(), sym(1).Expression, sym(2).ArgumentList);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

CallExpression: CallExpression T_LBRACKET Expression T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArrayMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

CallExpression: CallExpression T_DOT PropertyIdentifier ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FieldMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

Arguments: T_LPAREN T_RPAREN ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

Arguments: T_LPAREN ArgumentList T_RPAREN ;
/.
case $rule_number: {
  sym(1).Node = sym(2).ArgumentList->finish ();
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ArgumentList: AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArgumentList> (driver->nodePool(), sym(1).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

ArgumentList: ArgumentList T_COMMA AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ArgumentList> (driver->nodePool(), sym(1).ArgumentList, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

LeftHandSideExpression: NewExpression ;
LeftHandSideExpression: CallExpression ;
PostfixExpression: LeftHandSideExpression ;

PostfixExpression: LeftHandSideExpression T_PLUS_PLUS ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::PostIncrementExpression> (driver->nodePool(), sym(1).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

PostfixExpression: LeftHandSideExpression T_MINUS_MINUS ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::PostDecrementExpression> (driver->nodePool(), sym(1).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: PostfixExpression ;

UnaryExpression: T_DELETE UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::DeleteExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_VOID UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VoidExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_TYPEOF UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::TypeOfExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_PLUS_PLUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::PreIncrementExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_MINUS_MINUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::PreDecrementExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_PLUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::UnaryPlusExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_MINUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::UnaryMinusExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_TILDE UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::TildeExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

UnaryExpression: T_NOT UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::NotExpression> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

MultiplicativeExpression: UnaryExpression ;

MultiplicativeExpression: MultiplicativeExpression T_STAR UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Mul, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

MultiplicativeExpression: MultiplicativeExpression T_DIVIDE_ UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Div, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

MultiplicativeExpression: MultiplicativeExpression T_REMAINDER UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Mod, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

AdditiveExpression: MultiplicativeExpression ;

AdditiveExpression: AdditiveExpression T_PLUS MultiplicativeExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Add, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

AdditiveExpression: AdditiveExpression T_MINUS MultiplicativeExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Sub, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ShiftExpression: AdditiveExpression ;

ShiftExpression: ShiftExpression T_LT_LT AdditiveExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::LShift, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ShiftExpression: ShiftExpression T_GT_GT AdditiveExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::RShift, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ShiftExpression: ShiftExpression T_GT_GT_GT AdditiveExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::URShift, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpression: ShiftExpression ;

RelationalExpression: RelationalExpression T_LT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Lt, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpression: RelationalExpression T_GT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Gt, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpression: RelationalExpression T_LE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Le, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpression: RelationalExpression T_GE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Ge, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpression: RelationalExpression T_INSTANCEOF ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::InstanceOf, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpression: RelationalExpression T_IN ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::In, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpressionNotIn: ShiftExpression ;

RelationalExpressionNotIn: RelationalExpressionNotIn T_LT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Lt, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_GT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Gt, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_LE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Le, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_GE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Ge, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_INSTANCEOF ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::InstanceOf, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpression: RelationalExpression ;

EqualityExpression: EqualityExpression T_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Equal, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpression: EqualityExpression T_NOT_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::NotEqual, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpression: EqualityExpression T_EQ_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictEqual, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpression: EqualityExpression T_NOT_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictNotEqual, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpressionNotIn: RelationalExpressionNotIn ;

EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Equal, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ RelationalExpressionNotIn;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::NotEqual, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictEqual, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictNotEqual, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BitwiseANDExpression: EqualityExpression ;

BitwiseANDExpression: BitwiseANDExpression T_AND EqualityExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitAnd, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BitwiseANDExpressionNotIn: EqualityExpressionNotIn ;

BitwiseANDExpressionNotIn: BitwiseANDExpressionNotIn T_AND EqualityExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitAnd, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BitwiseXORExpression: BitwiseANDExpression ;

BitwiseXORExpression: BitwiseXORExpression T_XOR BitwiseANDExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitXor, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BitwiseXORExpressionNotIn: BitwiseANDExpressionNotIn ;

BitwiseXORExpressionNotIn: BitwiseXORExpressionNotIn T_XOR BitwiseANDExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitXor, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BitwiseORExpression: BitwiseXORExpression ;

BitwiseORExpression: BitwiseORExpression T_OR BitwiseXORExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitOr, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BitwiseORExpressionNotIn: BitwiseXORExpressionNotIn ;

BitwiseORExpressionNotIn: BitwiseORExpressionNotIn T_OR BitwiseXORExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitOr, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

LogicalANDExpression: BitwiseORExpression ;

LogicalANDExpression: LogicalANDExpression T_AND_AND BitwiseORExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::And, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

LogicalANDExpressionNotIn: BitwiseORExpressionNotIn ;

LogicalANDExpressionNotIn: LogicalANDExpressionNotIn T_AND_AND BitwiseORExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::And, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

LogicalORExpression: LogicalANDExpression ;

LogicalORExpression: LogicalORExpression T_OR_OR LogicalANDExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Or, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

LogicalORExpressionNotIn: LogicalANDExpressionNotIn ;

LogicalORExpressionNotIn: LogicalORExpressionNotIn T_OR_OR LogicalANDExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Or, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ConditionalExpression: LogicalORExpression ;

ConditionalExpression: LogicalORExpression T_QUESTION AssignmentExpression T_COLON AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ConditionalExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression, sym(5).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ConditionalExpressionNotIn: LogicalORExpressionNotIn ;

ConditionalExpressionNotIn: LogicalORExpressionNotIn T_QUESTION AssignmentExpressionNotIn T_COLON AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ConditionalExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression, sym(5).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

AssignmentExpression: ConditionalExpression ;

AssignmentExpression: LeftHandSideExpression AssignmentOperator AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, sym(2).ival, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

AssignmentExpressionNotIn: ConditionalExpressionNotIn ;

AssignmentExpressionNotIn: LeftHandSideExpression AssignmentOperator AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, sym(2).ival, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

AssignmentOperator: T_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::Assign;
} break;
./

AssignmentOperator: T_STAR_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceMul;
} break;
./

AssignmentOperator: T_DIVIDE_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceDiv;
} break;
./

AssignmentOperator: T_REMAINDER_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceMod;
} break;
./

AssignmentOperator: T_PLUS_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceAdd;
} break;
./

AssignmentOperator: T_MINUS_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceSub;
} break;
./

AssignmentOperator: T_LT_LT_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceLeftShift;
} break;
./

AssignmentOperator: T_GT_GT_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceRightShift;
} break;
./

AssignmentOperator: T_GT_GT_GT_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceURightShift;
} break;
./

AssignmentOperator: T_AND_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceAnd;
} break;
./

AssignmentOperator: T_XOR_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceXor;
} break;
./

AssignmentOperator: T_OR_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceOr;
} break;
./

Expression: AssignmentExpression ;

Expression: Expression T_COMMA AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Expression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ExpressionOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ExpressionOpt: Expression ;

ExpressionNotIn: AssignmentExpressionNotIn ;

ExpressionNotIn: ExpressionNotIn T_COMMA AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Expression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ExpressionNotInOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ExpressionNotInOpt: ExpressionNotIn ;

Statement: Block ;
Statement: VariableStatement ;
Statement: EmptyStatement ;
Statement: ExpressionStatement ;
Statement: IfStatement ;
Statement: IterationStatement ;
Statement: ContinueStatement ;
Statement: BreakStatement ;
Statement: ReturnStatement ;
Statement: WithStatement ;
Statement: LabelledStatement ;
Statement: SwitchStatement ;
Statement: ThrowStatement ;
Statement: TryStatement ;
Statement: DebuggerStatement ;


Block: T_LBRACE StatementListOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Block> (driver->nodePool(), sym(2).StatementList);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

StatementList: Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::StatementList> (driver->nodePool(), sym(1).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

StatementList: StatementList Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::StatementList> (driver->nodePool(), sym(1).StatementList, sym(2).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

StatementListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

StatementListOpt: StatementList ;
/.
case $rule_number: {
  sym(1).Node = sym(1).StatementList->finish ();
} break;
./

VariableStatement: VariableDeclarationKind VariableDeclarationList T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
VariableStatement: VariableDeclarationKind VariableDeclarationList T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableStatement> (driver->nodePool(), sym(2).VariableDeclarationList->finish (/*readOnly=*/sym(1).ival == T_CONST));
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

VariableDeclarationKind: T_CONST ;
/.
case $rule_number: {
  sym(1).ival = T_CONST;
} break;
./

VariableDeclarationKind: T_VAR ;
/.
case $rule_number: {
  sym(1).ival = T_VAR;
} break;
./

VariableDeclarationList: VariableDeclaration ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclaration);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

VariableDeclarationList: VariableDeclarationList T_COMMA VariableDeclaration ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclarationList, sym(3).VariableDeclaration);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

VariableDeclarationListNotIn: VariableDeclarationNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclaration);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

VariableDeclarationListNotIn: VariableDeclarationListNotIn T_COMMA VariableDeclarationNotIn ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclarationList, sym(3).VariableDeclaration);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

VariableDeclaration: T_IDENTIFIER InitialiserOpt ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableDeclaration> (driver->nodePool(), sym(1).sval, sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

VariableDeclarationNotIn: T_IDENTIFIER InitialiserNotInOpt ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::VariableDeclaration> (driver->nodePool(), sym(1).sval, sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

Initialiser: T_EQ AssignmentExpression ;
/.
case $rule_number: {
  sym(1) = sym(2);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

InitialiserOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

InitialiserOpt: Initialiser ;

InitialiserNotIn: T_EQ AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1) = sym(2);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

InitialiserNotInOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

InitialiserNotInOpt: InitialiserNotIn ;

EmptyStatement: T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::EmptyStatement> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

ExpressionStatement: Expression T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ExpressionStatement: Expression T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ExpressionStatement> (driver->nodePool(), sym(1).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement T_ELSE Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::IfStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement, sym(7).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(7));
} break;
./

IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::IfStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./


IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::DoWhileStatement> (driver->nodePool(), sym(2).Statement, sym(5).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(7));
} break;
./

IterationStatement: T_WHILE T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::WhileStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

IterationStatement: T_FOR T_LPAREN ExpressionNotInOpt T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ForStatement> (driver->nodePool(), sym(3).Expression, sym(5).Expression, sym(7).Expression, sym(9).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(9));
} break;
./

IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationListNotIn T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::LocalForStatement> (driver->nodePool(), sym(4).VariableDeclarationList->finish (/*readOnly=*/false), sym(6).Expression, sym(8).Expression, sym(10).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(10));
} break;
./

IterationStatement: T_FOR T_LPAREN LeftHandSideExpression T_IN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ForEachStatement> (driver->nodePool(), sym(3).Expression, sym(5).Expression, sym(7).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(7));
} break;
./

IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationNotIn T_IN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::LocalForEachStatement> (driver->nodePool(), sym(4).VariableDeclaration, sym(6).Expression, sym(8).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(8));
} break;
./

ContinueStatement: T_CONTINUE T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ContinueStatement: T_CONTINUE T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ContinueStatement> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

ContinueStatement: T_CONTINUE T_IDENTIFIER T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ContinueStatement: T_CONTINUE T_IDENTIFIER T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ContinueStatement> (driver->nodePool(), sym(2).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

BreakStatement: T_BREAK T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
BreakStatement: T_BREAK T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BreakStatement> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

BreakStatement: T_BREAK T_IDENTIFIER T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
BreakStatement: T_BREAK T_IDENTIFIER T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::BreakStatement> (driver->nodePool(), sym(2).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ReturnStatement: T_RETURN ExpressionOpt T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ReturnStatement: T_RETURN ExpressionOpt T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ReturnStatement> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

WithStatement: T_WITH T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::WithStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

SwitchStatement: T_SWITCH T_LPAREN Expression T_RPAREN CaseBlock ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::SwitchStatement> (driver->nodePool(), sym(3).Expression, sym(5).CaseBlock);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

CaseBlock: T_LBRACE CaseClausesOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CaseBlock> (driver->nodePool(), sym(2).CaseClauses);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

CaseBlock: T_LBRACE CaseClausesOpt DefaultClause CaseClausesOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CaseBlock> (driver->nodePool(), sym(2).CaseClauses, sym(3).DefaultClause, sym(4).CaseClauses);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

CaseClauses: CaseClause ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CaseClauses> (driver->nodePool(), sym(1).CaseClause);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

CaseClauses: CaseClauses CaseClause ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CaseClauses> (driver->nodePool(), sym(1).CaseClauses, sym(2).CaseClause);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

CaseClausesOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

CaseClausesOpt: CaseClauses ;
/.
case $rule_number: {
  sym(1).Node = sym(1).CaseClauses->finish ();
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

CaseClause: T_CASE Expression T_COLON StatementListOpt ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::CaseClause> (driver->nodePool(), sym(2).Expression, sym(4).StatementList);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

DefaultClause: T_DEFAULT T_COLON StatementListOpt ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::DefaultClause> (driver->nodePool(), sym(3).StatementList);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

LabelledStatement: T_IDENTIFIER T_COLON Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::LabelledStatement> (driver->nodePool(), sym(1).sval, sym(3).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

ThrowStatement: T_THROW Expression T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ThrowStatement: T_THROW Expression T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::ThrowStatement> (driver->nodePool(), sym(2).Expression);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

TryStatement: T_TRY Block Catch ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::TryStatement> (driver->nodePool(), sym(2).Statement, sym(3).Catch);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

TryStatement: T_TRY Block Finally ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::TryStatement> (driver->nodePool(), sym(2).Statement, sym(3).Finally);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

TryStatement: T_TRY Block Catch Finally ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::TryStatement> (driver->nodePool(), sym(2).Statement, sym(3).Catch, sym(4).Finally);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(4));
} break;
./

Catch: T_CATCH T_LPAREN T_IDENTIFIER T_RPAREN Block ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Catch> (driver->nodePool(), sym(3).sval, sym(5).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(5));
} break;
./

Finally: T_FINALLY Block ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Finally> (driver->nodePool(), sym(2).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

DebuggerStatement: T_DEBUGGER T_AUTOMATIC_SEMICOLON ; -- automatic semicolon
DebuggerStatement: T_DEBUGGER T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::DebuggerStatement> (driver->nodePool());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

FunctionDeclaration: T_FUNCTION T_IDENTIFIER T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FunctionDeclaration> (driver->nodePool(), sym(2).sval, sym(4).FormalParameterList, sym(7).FunctionBody);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(8));
} break;
./

FunctionExpression: T_FUNCTION IdentifierOpt T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FunctionExpression> (driver->nodePool(), sym(2).sval, sym(4).FormalParameterList, sym(7).FunctionBody);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(8));
} break;
./

FormalParameterList: T_IDENTIFIER ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FormalParameterList> (driver->nodePool(), sym(1).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

FormalParameterList: FormalParameterList T_COMMA T_IDENTIFIER ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FormalParameterList> (driver->nodePool(), sym(1).FormalParameterList, sym(3).sval);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(3));
} break;
./

FormalParameterListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

FormalParameterListOpt: FormalParameterList ;
/.
case $rule_number: {
  sym(1).Node = sym(1).FormalParameterList->finish ();
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

FunctionBodyOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

FunctionBodyOpt: FunctionBody ;

FunctionBody: SourceElements ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FunctionBody> (driver->nodePool(), sym(1).SourceElements->finish ());
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

Program: SourceElements ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::Program> (driver->nodePool(), sym(1).SourceElements->finish ());
  driver->changeAbstractSyntaxTree(sym(1).Node);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

SourceElements: SourceElement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::SourceElements> (driver->nodePool(), sym(1).SourceElement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

SourceElements: SourceElements SourceElement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::SourceElements> (driver->nodePool(), sym(1).SourceElements, sym(2).SourceElement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(2));
} break;
./

SourceElement: Statement ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::StatementSourceElement> (driver->nodePool(), sym(1).Statement);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

SourceElement: FunctionDeclaration ;
/.
case $rule_number: {
  sym(1).Node = QScript::makeAstNode<QScript::AST::FunctionSourceElement> (driver->nodePool(), sym(1).FunctionDeclaration);
  Q_SCRIPT_UPDATE_POSITION(sym(1).Node, loc(1), loc(1));
} break;
./

IdentifierOpt: ;
/.
case $rule_number: {
  sym(1).sval = 0;
} break;
./

IdentifierOpt: T_IDENTIFIER ;

PropertyNameAndValueListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

PropertyNameAndValueListOpt: PropertyNameAndValueList ;

/.
          } // switch

          state_stack [tos] = nt_action (act, lhs [r] - TERMINAL_COUNT);

          if (rhs[r] > 1) {
              location_stack[tos - 1].endLine = location_stack[tos + rhs[r] - 2].endLine;
              location_stack[tos - 1].endColumn = location_stack[tos + rhs[r] - 2].endColumn;
              location_stack[tos] = location_stack[tos + rhs[r] - 1];
          }
        }

      else
        {
          if (saved_yytoken == -1 && automatic (driver, yytoken) && t_action (state, T_AUTOMATIC_SEMICOLON) > 0)
            {
              saved_yytoken = yytoken;
              yytoken = T_SEMICOLON;
              continue;
            }

          else if ((state == INITIAL_STATE) && (yytoken == 0)) {
              // accept empty input
              yytoken = T_SEMICOLON;
              continue;
          }

          int ers = state;
          int shifts = 0;
          int reduces = 0;
          int expected_tokens [3];
          for (int tk = 0; tk < TERMINAL_COUNT; ++tk)
            {
              int k = t_action (ers, tk);

              if (! k)
                continue;
              else if (k < 0)
                ++reduces;
              else if (spell [tk])
                {
                  if (shifts < 3)
                    expected_tokens [shifts] = tk;
                  ++shifts;
                }
            }

          error_message.clear ();
          if (shifts && shifts < 3)
            {
              bool first = true;

              for (int s = 0; s < shifts; ++s)
                {
                  if (first)
                    error_message += QLatin1String ("Expected ");
                  else
                    error_message += QLatin1String (", ");

                  first = false;
                  error_message += QLatin1String("`");
                  error_message += QLatin1String (spell [expected_tokens [s]]);
                  error_message += QLatin1String("'");
                }
            }

          if (error_message.isEmpty())
              error_message = lexer->errorMessage();

          error_lineno = lexer->startLineNo();
          error_column = lexer->startColumnNo();

          return false;
        }
    }

    return false;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
./
/:
QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif // QSCRIPTPARSER_P_H
:/
