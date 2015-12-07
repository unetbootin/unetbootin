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

#include "qscriptxmlgenerator_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptast_p.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptlexer_p.h"
#include "qscriptparser_p.h"

#include <QtCore/qstring.h>
#include <QtCore/qtextstream.h>

QT_BEGIN_NAMESPACE

namespace QScript {

extern QString numberToString(qsreal value);

// copy of Qt::escape() (it's in QtGui :-( )

static QString escape(const QString& plain)
{
    QString rich;
    rich.reserve(int(plain.length() * 1.1));
    for (int i = 0; i < plain.length(); ++i) {
        if (plain.at(i) == QLatin1Char('<'))
            rich += QLatin1String("&lt;");
        else if (plain.at(i) == QLatin1Char('>'))
            rich += QLatin1String("&gt;");
        else if (plain.at(i) == QLatin1Char('&'))
            rich += QLatin1String("&amp;");
        else
            rich += plain.at(i);
    }
    return rich;
}

XmlGenerator::XmlGenerator(QTextStream &o):
    out(o), m_indentLevel(-1), m_formatOutput(false)
{
}

XmlGenerator::~XmlGenerator()
{
}

QTextStream &XmlGenerator::operator()(const QString &program, int lineNumber)
{
    QScriptEnginePrivate priv;
    NodePool *pool = new NodePool(/*fileName=*/QString(), &priv);
    priv.setNodePool(pool);

    Lexer lex(&priv);
    priv.setLexer(&lex);
    lex.setCode(program, lineNumber);

    QScriptParser parser;
    if (parser.parse(&priv)) {
        accept(priv.abstractSyntaxTree());
    }

    delete pool;

    return out;
}

QTextStream &XmlGenerator::newlineAndIndent()
{
    enum { IND = 2 };
    if (m_formatOutput)
        out << endl << QString().fill(QLatin1Char(' '), m_indentLevel * IND);
    return out;
}

QTextStream &XmlGenerator::startTag(const QString &name, AST::Node *locationNode)
{
    pushIndentLevel();
    newlineAndIndent();
    out << QLatin1String("<") << name;
    if (locationNode)
        out << QLatin1String(" line=\"") << locationNode->startLine << QLatin1String("\"");
    out << QLatin1String(">");
    return out;
}

QTextStream &XmlGenerator::endTag(const QString &name)
{
    newlineAndIndent();
    popIndentLevel();
    out << QLatin1String("</") << escape(name) << QLatin1String(">");
    return out;
}

void XmlGenerator::accept(AST::Node *node)
{
    AST::Node::acceptChild(node, this);
}

bool XmlGenerator::visit(AST::ThisExpression *)
{
    pushIndentLevel();
    newlineAndIndent();
    out << QLatin1String("<this/>");
    popIndentLevel();
    return true;
}

void XmlGenerator::endVisit(AST::ThisExpression *)
{
}

bool XmlGenerator::visit(AST::IdentifierExpression *node)
{
    startTag(QLatin1String("identifier"));
    out << escape(QScriptEnginePrivate::toString(node->name));
    out << QLatin1String("</identifier>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::IdentifierExpression *)
{
}

bool XmlGenerator::visit(AST::NullExpression *)
{
    pushIndentLevel();
    newlineAndIndent();
    out << QLatin1String("<null/>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::NullExpression *)
{
}

bool XmlGenerator::visit(AST::TrueLiteral *)
{
    pushIndentLevel();
    newlineAndIndent();
    out << QLatin1String("<true/>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::TrueLiteral *)
{
}

bool XmlGenerator::visit(AST::FalseLiteral *)
{
    pushIndentLevel();
    newlineAndIndent();
    out << QLatin1String("<false/>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::FalseLiteral *)
{
}

bool XmlGenerator::visit(AST::StringLiteral *node)
{
    startTag(QLatin1String("string"));
    out << escape(QScriptEnginePrivate::toString(node->value)) << QLatin1String("</string>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::StringLiteral *)
{
}

bool XmlGenerator::visit(AST::NumericLiteral *node)
{
    startTag(QLatin1String("number"));
    out << QString::number(node->value) << QLatin1String("</number>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::NumericLiteral *)
{
}

bool XmlGenerator::visit(AST::RegExpLiteral *node)
{
    startTag(QLatin1String("regexp"));
    out << QLatin1String("/") << escape(QScriptEnginePrivate::toString(node->pattern)) << QLatin1String("/");
    if (node->flags)
        out << QScript::Ecma::RegExp::flagsToString(node->flags);
    out << QLatin1String("</regexp>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::RegExpLiteral *)
{
}

bool XmlGenerator::visit(AST::ArrayLiteral *)
{
    startTag(QLatin1String("array-literal"));
    return true;
}

void XmlGenerator::endVisit(AST::ArrayLiteral *)
{
    endTag(QLatin1String("array-literal"));
}

bool XmlGenerator::visit(AST::ObjectLiteral *)
{
    startTag(QLatin1String("object-literal"));
    return true;
}

void XmlGenerator::endVisit(AST::ObjectLiteral *)
{
    endTag(QLatin1String("object-literal"));
}

bool XmlGenerator::visit(AST::ElementList *)
{
    startTag(QLatin1String("element-list"));
    return true;
}

void XmlGenerator::endVisit(AST::ElementList *)
{
    endTag(QLatin1String("element-list"));
}

bool XmlGenerator::visit(AST::Elision *)
{
    startTag(QLatin1String("elision")); // ### count
    return true;
}

void XmlGenerator::endVisit(AST::Elision *)
{
    endTag(QLatin1String("elision"));
}

bool XmlGenerator::visit(AST::PropertyNameAndValueList *)
{
    startTag(QLatin1String("property-name-and-value-list"));
    return true;
}

void XmlGenerator::endVisit(AST::PropertyNameAndValueList *)
{
    endTag(QLatin1String("property-name-and-value-list"));
}

bool XmlGenerator::visit(AST::IdentifierPropertyName *node)
{
    startTag(QLatin1String("identifier"));
    out << escape(QScriptEnginePrivate::toString(node->id)) << QLatin1String("</identifier>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::IdentifierPropertyName *)
{
}

bool XmlGenerator::visit(AST::StringLiteralPropertyName *node)
{
    startTag(QLatin1String("string"));
    out << escape(QScriptEnginePrivate::toString(node->id)) << QLatin1String("</string>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::StringLiteralPropertyName *)
{
}

bool XmlGenerator::visit(AST::NumericLiteralPropertyName *node)
{
    startTag(QLatin1String("number"));
    out << escape(QScript::numberToString(node->id)) << QLatin1String("</number>");
    popIndentLevel();
    return false;
}

void XmlGenerator::endVisit(AST::NumericLiteralPropertyName *)
{
}

bool XmlGenerator::visit(AST::ArrayMemberExpression *)
{
    startTag(QLatin1String("array-member-expression"));
    return true;
}

void XmlGenerator::endVisit(AST::ArrayMemberExpression *)
{
    endTag(QLatin1String("array-member-expression"));
}

bool XmlGenerator::visit(AST::FieldMemberExpression *)
{
    startTag(QLatin1String("field-member-expression"));
    return true;
}

void XmlGenerator::endVisit(AST::FieldMemberExpression *node)
{
    startTag(QLatin1String("identifier"));
    out << escape(QScriptEnginePrivate::toString(node->name));
    out << QLatin1String("</identifier>");
    popIndentLevel();
    endTag(QLatin1String("field-member-expression"));
}

bool XmlGenerator::visit(AST::NewMemberExpression *)
{
    startTag(QLatin1String("new-member-expression"));
    return true;
}

void XmlGenerator::endVisit(AST::NewMemberExpression *)
{
    endTag(QLatin1String("new-member-expression"));
}

bool XmlGenerator::visit(AST::NewExpression *)
{
    startTag(QLatin1String("new"));
    return true;
}

void XmlGenerator::endVisit(AST::NewExpression *)
{
    endTag(QLatin1String("new"));
}

bool XmlGenerator::visit(AST::CallExpression *)
{
    startTag(QLatin1String("call"));
    return true;
}

void XmlGenerator::endVisit(AST::CallExpression *)
{
    endTag(QLatin1String("call"));
}

bool XmlGenerator::visit(AST::ArgumentList *)
{
    startTag(QLatin1String("argument-list"));
    return true;
}

void XmlGenerator::endVisit(AST::ArgumentList *)
{
    endTag(QLatin1String("argument-list"));
}

bool XmlGenerator::visit(AST::PostIncrementExpression *)
{
    startTag(QLatin1String("post-increment"));
    return true;
}

void XmlGenerator::endVisit(AST::PostIncrementExpression *)
{
    endTag(QLatin1String("post-increment"));
}

bool XmlGenerator::visit(AST::PostDecrementExpression *)
{
    startTag(QLatin1String("post-decrement"));
    return true;
}

void XmlGenerator::endVisit(AST::PostDecrementExpression *)
{
    endTag(QLatin1String("post-decrement"));
}

bool XmlGenerator::visit(AST::DeleteExpression *)
{
    startTag(QLatin1String("delete"));
    return true;
}

void XmlGenerator::endVisit(AST::DeleteExpression *)
{
    endTag(QLatin1String("delete"));
}

bool XmlGenerator::visit(AST::VoidExpression *)
{
    startTag(QLatin1String("void"));
    return true;
}

void XmlGenerator::endVisit(AST::VoidExpression *)
{
    endTag(QLatin1String("void"));
}

bool XmlGenerator::visit(AST::TypeOfExpression *)
{
    startTag(QLatin1String("typeof"));
    return true;
}

void XmlGenerator::endVisit(AST::TypeOfExpression *)
{
    endTag(QLatin1String("typeof"));
}

bool XmlGenerator::visit(AST::PreIncrementExpression *)
{
    startTag(QLatin1String("pre-increment"));
    return true;
}

void XmlGenerator::endVisit(AST::PreIncrementExpression *)
{
    endTag(QLatin1String("pre-increment"));
}

bool XmlGenerator::visit(AST::PreDecrementExpression *)
{
    startTag(QLatin1String("pre-decrement"));
    return true;
}

void XmlGenerator::endVisit(AST::PreDecrementExpression *)
{
    endTag(QLatin1String("pre-decrement"));
}

bool XmlGenerator::visit(AST::UnaryPlusExpression *)
{
    startTag(QLatin1String("unary-plus"));
    return true;
}

void XmlGenerator::endVisit(AST::UnaryPlusExpression *)
{
    endTag(QLatin1String("unary-plus"));
}

bool XmlGenerator::visit(AST::UnaryMinusExpression *)
{
    startTag(QLatin1String("unary-minus"));
    return true;
}

void XmlGenerator::endVisit(AST::UnaryMinusExpression *)
{
    endTag(QLatin1String("unary-minus"));
}

bool XmlGenerator::visit(AST::TildeExpression *)
{
    startTag(QLatin1String("bitwise-not"));
    return true;
}

void XmlGenerator::endVisit(AST::TildeExpression *)
{
    endTag(QLatin1String("bitwise-not"));
}

bool XmlGenerator::visit(AST::NotExpression *)
{
    startTag(QLatin1String("logical-not"));
    return true;
}

void XmlGenerator::endVisit(AST::NotExpression *)
{
    endTag(QLatin1String("logical-not"));
}

bool XmlGenerator::visit(AST::BinaryExpression *node)
{
    QString s;
    switch (node->op) {
        case QSOperator::Add:
            s = QLatin1String("+"); break;
        case QSOperator::And:
            s = QLatin1String("&amp;&amp;"); break;
        case QSOperator::InplaceAnd:
            s = QLatin1String("&amp;="); break;
        case QSOperator::Assign:
            s = QLatin1String("="); break;
        case QSOperator::BitAnd:
            s = QLatin1String("&amp;"); break;
        case QSOperator::BitOr:
            s = QLatin1String("|"); break;
        case QSOperator::BitXor:
            s = QLatin1String("^"); break;
        case QSOperator::InplaceSub:
            s = QLatin1String("-="); break;
        case QSOperator::Div:
            s = QLatin1String("/"); break;
        case QSOperator::InplaceDiv:
            s = QLatin1String("/="); break;
        case QSOperator::Equal:
            s = QLatin1String("=="); break;
        case QSOperator::Ge:
            s = QLatin1String(">="); break;
        case QSOperator::Gt:
            s = QLatin1String("&gt;"); break;
        case QSOperator::In:
            s = QLatin1String("in"); break;
        case QSOperator::InplaceAdd:
            s = QLatin1String("+="); break;
        case QSOperator::InstanceOf:
            s = QLatin1String("instanceof"); break;
        case QSOperator::Le:
            s = QLatin1String("&lt;="); break;
        case QSOperator::LShift:
            s = QLatin1String("&lt;&lt;"); break;
        case QSOperator::InplaceLeftShift:
            s = QLatin1String("&lt;&lt;="); break;
        case QSOperator::Lt:
            s = QLatin1String("&lt;"); break;
        case QSOperator::Mod:
            s = QLatin1String("%"); break;
        case QSOperator::InplaceMod:
            s = QLatin1String("%="); break;
        case QSOperator::Mul:
            s = QLatin1String("*"); break;
        case QSOperator::InplaceMul:
            s = QLatin1String("*="); break;
        case QSOperator::NotEqual:
            s = QLatin1String("!="); break;
        case QSOperator::Or:
            s = QLatin1String("||"); break;
        case QSOperator::InplaceOr:
            s = QLatin1String("|="); break;
        case QSOperator::RShift:
            s = QLatin1String("&gt;&gt;"); break;
        case QSOperator::InplaceRightShift:
            s = QLatin1String("&gt;&gt;="); break;
        case QSOperator::StrictEqual:
            s = QLatin1String("==="); break;
        case QSOperator::StrictNotEqual:
            s = QLatin1String("!=="); break;
        case QSOperator::Sub:
            s = QLatin1String("-"); break;
        case QSOperator::URShift:
            s = QLatin1String("&gt;&gt;&gt;"); break;
        case QSOperator::InplaceURightShift:
            s = QLatin1String("&gt;&gt;&gt;="); break;
        case QSOperator::InplaceXor:
            s = QLatin1String("^="); break;
        default:
            Q_ASSERT (0);
    }
    pushIndentLevel();
    newlineAndIndent();
    out << QLatin1String("<binary-expression op=\"") << s << QLatin1String("\">");
    return true;
}

void XmlGenerator::endVisit(AST::BinaryExpression *)
{
    endTag(QLatin1String("binary-expression"));
}

bool XmlGenerator::visit(AST::ConditionalExpression *)
{
    startTag(QLatin1String("conditional"));
    return true;
}

void XmlGenerator::endVisit(AST::ConditionalExpression *)
{
    endTag(QLatin1String("conditional"));
}

bool XmlGenerator::visit(AST::Expression *)
{
    startTag(QLatin1String("comma-expression"));
    return true;
}

void XmlGenerator::endVisit(AST::Expression *)
{
    endTag(QLatin1String("comma-expression"));
}

bool XmlGenerator::visit(AST::Block *)
{
    startTag(QLatin1String("block"));
    return true;
}

void XmlGenerator::endVisit(AST::Block *)
{
    endTag(QLatin1String("block"));
}

bool XmlGenerator::visit(AST::StatementList *)
{
    startTag(QLatin1String("statement-list"));
    return true;
}

void XmlGenerator::endVisit(AST::StatementList *)
{
    endTag(QLatin1String("statement-list"));
}

bool XmlGenerator::visit(AST::VariableDeclarationList *)
{
    startTag(QLatin1String("variable-declaration-list"));
    return true;
}

void XmlGenerator::endVisit(AST::VariableDeclarationList *)
{
    endTag(QLatin1String("variable-declaration-list"));
}

bool XmlGenerator::visit(AST::VariableStatement *node)
{
    startTag(QLatin1String("variable-statement"), node);
    return true;
}

void XmlGenerator::endVisit(AST::VariableStatement *)
{
    endTag(QLatin1String("variable-statement"));
}

bool XmlGenerator::visit(AST::VariableDeclaration *node)
{
    startTag(QLatin1String("variable-declaration"), node);
    startTag(QLatin1String("name"));
    out << escape(QScriptEnginePrivate::toString(node->name));
    out << QLatin1String("</name>");
    popIndentLevel();
    return true;
}

void XmlGenerator::endVisit(AST::VariableDeclaration *)
{
    endTag(QLatin1String("variable-declaration"));
}

bool XmlGenerator::visit(AST::EmptyStatement *node)
{
    startTag(QLatin1String("empty-statement"), node);
    return true;
}

void XmlGenerator::endVisit(AST::EmptyStatement *)
{
    endTag(QLatin1String("empty-statement"));
}

bool XmlGenerator::visit(AST::ExpressionStatement *node)
{
    startTag(QLatin1String("expression-statement"), node);
    return true;
}

void XmlGenerator::endVisit(AST::ExpressionStatement *)
{
    endTag(QLatin1String("expression-statement"));
}

bool XmlGenerator::visit(AST::IfStatement *node)
{
    startTag(QLatin1String("if"), node);
    return true;
}

void XmlGenerator::endVisit(AST::IfStatement *)
{
    endTag(QLatin1String("if"));
}

bool XmlGenerator::visit(AST::DoWhileStatement *node)
{
    startTag(QLatin1String("do-while"), node);
    return true;
}

void XmlGenerator::endVisit(AST::DoWhileStatement *)
{
    endTag(QLatin1String("do-while"));
}

bool XmlGenerator::visit(AST::WhileStatement *node)
{
    startTag(QLatin1String("while"), node);
    return true;
}

void XmlGenerator::endVisit(AST::WhileStatement *)
{
    endTag(QLatin1String("while"));
}

bool XmlGenerator::visit(AST::ForStatement *node)
{
    startTag(QLatin1String("for"), node);
    return true;
}

void XmlGenerator::endVisit(AST::ForStatement *)
{
    endTag(QLatin1String("for"));
}

bool XmlGenerator::visit(AST::LocalForStatement *node)
{
    startTag(QLatin1String("for"), node);
    return true;
}

void XmlGenerator::endVisit(AST::LocalForStatement *)
{
    endTag(QLatin1String("for"));
}

bool XmlGenerator::visit(AST::ForEachStatement *node)
{
    startTag(QLatin1String("for-in"), node);
    return false;
}

void XmlGenerator::endVisit(AST::ForEachStatement *)
{
    endTag(QLatin1String("for-in"));
}

bool XmlGenerator::visit(AST::LocalForEachStatement *node)
{
    startTag(QLatin1String("for-in"), node);
    return true;
}

void XmlGenerator::endVisit(AST::LocalForEachStatement *)
{
    endTag(QLatin1String("for-in"));
}

bool XmlGenerator::visit(AST::ContinueStatement *node)
{
    startTag(QLatin1String("continue"), node);
    if (node->label) {
        startTag(QLatin1String("label"));
        out << escape(QScriptEnginePrivate::toString(node->label));
        out << QLatin1String("</label>");
        popIndentLevel();
    }
    return true;
}

void XmlGenerator::endVisit(AST::ContinueStatement *)
{
    endTag(QLatin1String("continue"));
}

bool XmlGenerator::visit(AST::BreakStatement *node)
{
    startTag(QLatin1String("break"), node);
    if (node->label) {
        startTag(QLatin1String("label"));
        out << escape(QScriptEnginePrivate::toString(node->label));
        out << QLatin1String("</label>");
        popIndentLevel();
    }
    return true;
}

void XmlGenerator::endVisit(AST::BreakStatement *)
{
    endTag(QLatin1String("break"));
}

bool XmlGenerator::visit(AST::ReturnStatement *node)
{
    startTag(QLatin1String("return"), node);
    return true;
}

void XmlGenerator::endVisit(AST::ReturnStatement *)
{
    endTag(QLatin1String("return"));
}

bool XmlGenerator::visit(AST::WithStatement *node)
{
    startTag(QLatin1String("with"), node);
    return true;
}

void XmlGenerator::endVisit(AST::WithStatement *)
{
    endTag(QLatin1String("with"));
}

bool XmlGenerator::visit(AST::SwitchStatement *node)
{
    startTag(QLatin1String("switch"), node);
    return true;
}

void XmlGenerator::endVisit(AST::SwitchStatement *)
{
    endTag(QLatin1String("switch"));
}

bool XmlGenerator::visit(AST::CaseBlock *)
{
    startTag(QLatin1String("case-block"));
    return true;
}

void XmlGenerator::endVisit(AST::CaseBlock *)
{
    endTag(QLatin1String("case-block"));
}

bool XmlGenerator::visit(AST::CaseClauses *)
{
    startTag(QLatin1String("case-clauses"));
    return true;
}

void XmlGenerator::endVisit(AST::CaseClauses *)
{
    endTag(QLatin1String("case-clauses"));
}

bool XmlGenerator::visit(AST::CaseClause *)
{
    startTag(QLatin1String("case-clause"));
    return true;
}

void XmlGenerator::endVisit(AST::CaseClause *)
{
    endTag(QLatin1String("case-clause"));
}

bool XmlGenerator::visit(AST::DefaultClause *)
{
    startTag(QLatin1String("default-clause"));
    return true;
}

void XmlGenerator::endVisit(AST::DefaultClause *)
{
    endTag(QLatin1String("default-clause"));
}

bool XmlGenerator::visit(AST::LabelledStatement *node)
{
    startTag(QLatin1String("labelled-statement"), node);
    startTag(QLatin1String("label"));
    out << escape(QScriptEnginePrivate::toString(node->label));
    out << QLatin1String("</label>");
    popIndentLevel();
    return true;
}

void XmlGenerator::endVisit(AST::LabelledStatement *)
{
    endTag(QLatin1String("labelled-statement"));
}

bool XmlGenerator::visit(AST::ThrowStatement *node)
{
    startTag(QLatin1String("throw"), node);
    return true;
}

void XmlGenerator::endVisit(AST::ThrowStatement *)
{
    endTag(QLatin1String("throw"));
}

bool XmlGenerator::visit(AST::TryStatement *node)
{
    startTag(QLatin1String("try"), node);
    return true;
}

void XmlGenerator::endVisit(AST::TryStatement *)
{
    endTag(QLatin1String("try"));
}

bool XmlGenerator::visit(AST::Catch *node)
{
    startTag(QLatin1String("catch"));
    startTag(QLatin1String("identifier"));
    out << escape(QScriptEnginePrivate::toString(node->name));
    out << QLatin1String("</identifier>");
    popIndentLevel();
    return true;
}

void XmlGenerator::endVisit(AST::Catch *)
{
    endTag(QLatin1String("catch"));
}

bool XmlGenerator::visit(AST::Finally *)
{
    startTag(QLatin1String("finally"));
    return true;
}

void XmlGenerator::endVisit(AST::Finally *)
{
    endTag(QLatin1String("finally"));
}

bool XmlGenerator::visit(AST::FunctionDeclaration *node)
{
    startTag(QLatin1String("function-declaration"), node);
    startTag(QLatin1String("name"));
    if (node->name)
        out << escape(QScriptEnginePrivate::toString(node->name));
    out << QLatin1String("</name>");
    popIndentLevel();
    if (!node->formals) {
        startTag(QLatin1String("formal-parameter-list"));
        endTag(QLatin1String("formal-parameter-list"));
    }
    if (!node->body) {
        startTag(QLatin1String("function-body"));
        endTag(QLatin1String("function-body"));
    }
    return true;
}

void XmlGenerator::endVisit(AST::FunctionDeclaration *)
{
    endTag(QLatin1String("function-declaration"));
}

bool XmlGenerator::visit(AST::FunctionExpression *node)
{
    startTag(QLatin1String("function-expression"), node);
    startTag(QLatin1String("name"));
    if (node->name)
        out << escape(QScriptEnginePrivate::toString(node->name));
    out << QLatin1String("</name>");
    if (!node->formals) {
        startTag(QLatin1String("formal-parameter-list"));
        endTag(QLatin1String("formal-parameter-list"));
    }
    if (!node->body) {
        startTag(QLatin1String("function-body"));
        endTag(QLatin1String("function-body"));
    }
    return true;
}

void XmlGenerator::endVisit(AST::FunctionExpression *)
{
    endTag(QLatin1String("function-expression"));
}

bool XmlGenerator::visit(AST::FormalParameterList *node)
{
    Q_UNUSED(node);
    startTag(QLatin1String("formal-parameter-list"));
    for (AST::FormalParameterList *it = node; it; it = it->next) {
        startTag(QLatin1String("identifier"));
        out << escape(QScriptEnginePrivate::toString(it->name));
        out << QLatin1String("</identifier>");
        popIndentLevel();
    }
    return true;
}

void XmlGenerator::endVisit(AST::FormalParameterList *)
{
    endTag(QLatin1String("formal-parameter-list"));
}

bool XmlGenerator::visit(AST::FunctionBody *)
{
    startTag(QLatin1String("function-body"));
    return true;
}

void XmlGenerator::endVisit(AST::FunctionBody *)
{
    endTag(QLatin1String("function-body"));
}

bool XmlGenerator::visit(AST::Program *)
{
    startTag(QLatin1String("program"));
    return true;
}

void XmlGenerator::endVisit(AST::Program *)
{
    endTag(QLatin1String("program"));
}

bool XmlGenerator::visit(AST::SourceElements *)
{
    startTag(QLatin1String("source-elements"));
    return true;
}

void XmlGenerator::endVisit(AST::SourceElements *)
{
    endTag(QLatin1String("source-elements"));
}

bool XmlGenerator::visit(AST::FunctionSourceElement *)
{
    return true;
}

void XmlGenerator::endVisit(AST::FunctionSourceElement *)
{
}

bool XmlGenerator::visit(AST::StatementSourceElement *)
{
    return true;
}

void XmlGenerator::endVisit(AST::StatementSourceElement *)
{
}

bool XmlGenerator::visit(AST::DebuggerStatement *node)
{
    startTag(QLatin1String("debugger-statement"), node);
    return true;
}

void XmlGenerator::endVisit(AST::DebuggerStatement *)
{
    endTag(QLatin1String("debugger-statement"));
}

bool XmlGenerator::preVisit(AST::Node *)
{
    return true;
}

} // namespace QScript

Q_SCRIPT_EXPORT QString qt_scriptToXml(const QString &program, int lineNumber = 1)
{
    QString result;
    QTextStream out(&result, QIODevice::WriteOnly);
    QScript::XmlGenerator gen(out);
    gen(program, lineNumber);
    out.flush();
    return result;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
