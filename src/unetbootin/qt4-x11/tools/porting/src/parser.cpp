/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "parser.h"
#include "tokens.h"
#include "errors.h"

#include <QString>
#include <QStringList>
#include <QDateTime>

QT_BEGIN_NAMESPACE

#define TT (tokenStream->currentTokenText().data())

#define ADVANCE(tk, descr) \
{ \
  if (tokenStream->lookAhead() != tk) { \
    reportError(QString::fromLatin1("'%1' expected found '%2'").arg(QLatin1String(descr)).arg(QString::fromLatin1((tokenStream->currentTokenText().constData())))); \
      return false; \
  } \
  advance(); \
}

#define ADVANCE_NR(tk, descr) \
  do { \
    if (tokenStream->lookAhead() != tk) { \
        reportError(i18n("'%1' expected found '%2'").arg(QLatin1String(descr)).arg(QString::fromLatin1(tokenStream->currentTokenText().constData()))); \
    } \
    else \
        advance(); \
  } while (0)

#define CHECK(tk, descr) \
  do { \
    if (tokenStream->lookAhead() != tk) { \
        return false; \
    } \
    advance(); \
  } while (0)

#define MATCH(tk, descr) \
  do { \
    if (tokenStream->lookAhead() != tk) { \
        reportError(Errors::SyntaxError); \
        return false; \
    } \
  } while (0)

#define UPDATE_POS(_node, start, end) \
  do { \
      (_node)->setPosition(start, end); \
  } while (0)

#define AST_FROM_TOKEN(node, tk) \
    AST *node = CreateNode<AST>(m_pool); \
    UPDATE_POS(node, (tk), (tk)+1);

#define DUMP_AST(node) \
  do { \
    fprintf(stderr, "\n=================================================\n"); \
    for (int i=node->startToken(); i<node->endToken(); ++i) \
       fprintf(stderr, "%s", tokenStream->tokenText(i).constData()); \
    fprintf(stderr, "\n=================================================\n"); \
  } while (0)

#define RXX_NO_ERROR

QString i18n(const char *arg)
{
    return QLatin1String(arg);
}


//@todo remove me
enum
{
    OBJC_CLASS,
    OBJC_PROTOCOL,
    OBJC_ALIAS
};

Parser::Parser()
{
    m_maxProblems = 5;
    objcp = false;
}

Parser::~Parser()
{
}

TranslationUnitAST *Parser::parse(TokenStreamAdapter::TokenStream *p_tokenStream, pool *p)
{
   //tokenStream->rewind(0);
    m_pool = p;
    tokenStream = p_tokenStream;
    TranslationUnitAST *ast = 0;
    parseTranslationUnit(ast);
    return ast;
}

/*
    Parses a part of the translation unit given by tokenStream. When the number
    of nodes in the AST exeeds targetMaxASTnodes, this function will return as
    soon as possible. The progress is stored by updating the cursor inside
    tokenStream. done is set to true if the parser finished parsing the
    tokenStream, and to false otherwise.
*/
TranslationUnitAST *Parser::parse(TokenStreamAdapter::TokenStream *p_tokenStream, pool *p, int targetMaxASTNodes, bool &done)
{
    m_pool = p;
    tokenStream = p_tokenStream;
    TranslationUnitAST *ast = 0;
    // we always create one node, so target max nodes cannot be < 2.
    if (targetMaxASTNodes < 2)
        targetMaxASTNodes = 2;

    // Advance past whitespace and comment tokens at the start.
    while (tokenStream->isHidden(tokenStream->cursor())) {
        tokenStream->nextToken();
    }
    int start = tokenStream->cursor();

    AST::N = 0;
    m_problems = 0;
    ast = CreateNode<TranslationUnitAST>(m_pool);
    while (tokenStream->lookAhead() && AST::N < targetMaxASTNodes) {
        DeclarationAST *def = 0;
        int startDecl = tokenStream->cursor();
        if (!parseDeclaration(def)) {
            // error recovery
            if (startDecl == tokenStream->cursor())
                advance(); // skip at least one token
            skipUntilDeclaration();
        }
        ast->addDeclaration(def);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());

    done = tokenStream->tokenAtEnd();
    return ast;
}


bool Parser::reportError(const Error& err)
{
Q_UNUSED(err);
#ifndef RXX_NO_ERROR
    if (m_problems < m_maxProblems) {
        ++m_problems;
        int line=0, col=0;
        QByteArray fileName;
        tokenStream->getTokenStartPosition(tokenStream->cursor(), &line, &col, &fileName);

        QString s = tokenStream->currentTokenText();
        s = s.left(30).trimmed();
        if (s.isEmpty())
            s = i18n("<eof>");

        if (fileName.isEmpty())
            //fileName = m_file->fileName;
            fileName = "implement me";

        //        m_driver->addProblem(m_driver->currentFileName(), Problem(err.text.arg(s), line, col));
        fprintf(stderr, "%s: error %s at line %d column %d\n",
                fileName.constData(),
                err.text.arg(s).toLatin1().constData(), line, col);
    }
#endif // RXX_NO_ERROR
    return true;
}

bool Parser::reportError(const QString& msg)
{
Q_UNUSED(msg);
#ifndef RXX_NO_ERROR
    if (m_problems < m_maxProblems) {
        ++m_problems;
        int line=0, col=0;
        QByteArray fileName;
        tokenStream->getTokenStartPosition(tokenStream->cursor(), &line, &col, &fileName);

        if (fileName.isEmpty())
            //fileName = m_file->fileName;
            fileName = "implement me";

        //        m_driver->addProblem(m_driver->currentFileName(), Problem(msg, line, col));
        fprintf(stderr, "%s: error %s at line %d column %d\n",
                fileName.constData(),
                msg.toLatin1().constData(), line, col);
    }
#endif // RXX_NO_ERROR
    return true;
}

void Parser::syntaxError()
{
    (void) reportError(Errors::SyntaxError);
}

bool Parser::skipUntil(int token)
{
    while (tokenStream->lookAhead()) {
        if (tokenStream->lookAhead() == token)
            return true;

        advance();
    }

    return false;
}

bool Parser::skipUntilDeclaration()
{
    while (tokenStream->lookAhead()) {

        switch(tokenStream->lookAhead()) {
        case ';':
        case '~':
        case Token_scope:
        case Token_identifier:
        case Token_operator:
        case Token_char:
        case Token_wchar_t:
        case Token_bool:
        case Token_short:
        case Token_int:
        case Token_long:
        case Token_signed:
        case Token_unsigned:
        case Token_float:
        case Token_double:
        case Token_void:
        case Token_extern:
        case Token_namespace:
        case Token_using:
        case Token_typedef:
        case Token_asm:
        case Token_template:
        case Token_export:

        case Token_const:       // cv
        case Token_volatile:    // cv

        case Token_public:
        case Token_protected:
        case Token_private:
        case Token_signals:      // Qt
        case Token_slots:        // Qt
              return true;

        default:
            advance();
        }
    }

    return false;
}

bool Parser::skipUntilStatement()
{
    while (tokenStream->lookAhead()) {
        switch(tokenStream->lookAhead()) {
                case ';':
                case '{':
                case '}':
                case Token_const:
                case Token_volatile:
                case Token_identifier:
                case Token_case:
                case Token_default:
                case Token_if:
                case Token_switch:
                case Token_while:
                case Token_do:
                case Token_for:
                case Token_break:
                case Token_continue:
                case Token_return:
                case Token_goto:
                case Token_try:
                case Token_catch:
                case Token_throw:
                case Token_char:
                case Token_wchar_t:
                case Token_bool:
                case Token_short:
                case Token_int:
                case Token_long:
                case Token_signed:
                case Token_unsigned:
                case Token_float:
                case Token_double:
                case Token_void:
                case Token_class:
                case Token_struct:
                case Token_union:
                case Token_enum:
                case Token_scope:
                case Token_template:
                case Token_using:
                    return true;

            default:
                  advance();
        }
    }

    return false;
}

bool Parser::skip(int l, int r)
{
    int count = 0;
    while (tokenStream->lookAhead()) {
        int tk = tokenStream->lookAhead();

        if (tk == l)
            ++count;
        else if (tk == r)
            --count;
        else if (l != '{' && (tk == '{' || tk == '}' || tk == ';'))
            return false;

        if (count == 0)
            return true;

        advance();
    }

    return false;
}

bool Parser::skipCommaExpression(AbstractExpressionAST *&node)
{
#ifndef RXX_NO_PARSE_EXPRESSION
    return parseCommaExpression(node);
#else
    int start = tokenStream->cursor();

    AbstractExpressionAST *expr = 0;
    if (!skipExpression(expr))
        return false;

    while (tokenStream->lookAhead() == ',') {
        advance();

        if (!skipExpression(expr)) {
            reportError(i18n("expression expected"));
            return false;
        }
    }

    AbstractExpressionAST *ast = CreateNode<AbstractExpressionAST>(m_pool);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
#endif // RXX_NO_PARSE_EXPRESSION
}

bool Parser::skipExpression(AbstractExpressionAST *&node)
{
#ifndef RXX_NO_PARSE_EXPRESSION
    return parseExpression(node);
#else
    int start = tokenStream->cursor();
    int count = 0;

    while (tokenStream->lookAhead()) {
        int tk = tokenStream->lookAhead();

        switch(tk) {
        case '(':
        case '[':
        case '{':
            ++count;
            advance();
            break;

        case ']':
        case ')':
        case '}':
            if (count == 0) {
                AbstractExpressionAST *ast = CreateNode<AbstractExpressionAST>(m_pool);
                UPDATE_POS(ast, start, tokenStream->cursor());
                node = ast;
                return true;
            }
            --count;
            advance();
            break;

        case Token_struct:
        case Token_union:
        case Token_class: {
            int c = tokenStream->cursor();
            TypeSpecifierAST *spec = 0;
            if (!parseClassSpecifier(spec))
                tokenStream->rewind(c + 1);
        }
        break;

        case ',':
        case ';':
        case Token_case:
        case Token_default:
        case Token_if:
        case Token_while:
        case Token_do:
        case Token_for:
        case Token_break:
        case Token_continue:
        case Token_return:
        case Token_goto:
        {
            if ((tk == ',' || tk == ';') && count > 0) {
                advance();
                break;
            }

            AbstractExpressionAST *ast = CreateNode<AbstractExpressionAST>(m_pool);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
        }
        return true;

        default:
            advance();
        }
    }

    return false;
#endif // RXX_NO_PARSE_EXPRESSION
}

bool Parser::parseName(NameAST *&node, bool parseTemplateId)
{
    AST *winDeclSpec = 0;
    parseWinDeclSpec(winDeclSpec);

    int start = tokenStream->cursor();

    NameAST *ast = CreateNode<NameAST>(m_pool);

    if (tokenStream->lookAhead() == Token_scope) {
        ast->setGlobal(true);
        advance();
    }

    int idx = tokenStream->cursor();

    while (true) {
        ClassOrNamespaceNameAST *n = 0;
        if (!parseUnqualifiedName(n))
            return false;

        if (tokenStream->lookAhead() == Token_scope) {
            advance();
            ast->addClassOrNamespaceName(n);
            if (tokenStream->lookAhead() == Token_template)
                advance(); /// skip optional template     #### @todo CHECK
        } else if (!parseTemplateId && n) {
            tokenStream->rewind(n->startToken());
            parseUnqualifiedName(n, parseTemplateId);
            ast->setUnqualifiedName(n);
            break;
        } else {
            ast->setUnqualifiedName(n);
            break;
        }
    }

    if (idx == tokenStream->cursor())
        return false;

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTranslationUnit(TranslationUnitAST *&node)
{
    QTime t;
    t.start();

    advance();
    int start = tokenStream->cursor();

    m_problems = 0;
    TranslationUnitAST *tun = CreateNode<TranslationUnitAST>(m_pool);
    node = tun;
    while (tokenStream->lookAhead()) {
        DeclarationAST *def = 0;
        int startDecl = tokenStream->cursor();
        if (!parseDeclaration(def)) {
            // error recovery
            if (startDecl == tokenStream->cursor())
                advance(); // skip at least one token
            skipUntilDeclaration();
        }
        node->addDeclaration(def);
    }

    UPDATE_POS(node, start, tokenStream->cursor());

    return m_problems == 0;
}

bool Parser::parseDeclaration(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    switch(tokenStream->lookAhead()) {

    case ';':
        advance();
        return true;

    case Token_extern:
        return parseLinkageSpecification(node);

    case Token_namespace:
        return parseNamespace(node);

    case Token_using:
        return parseUsing(node);

    case Token_typedef:
        return parseTypedef(node);

    case Token_asm:
        return parseAsmDefinition(node);

    case Token_template:
    case Token_export:
        return parseTemplateDeclaration(node);

    default:
        {
            // tokenStream->rewind(start);

            if (objcp && parseObjcDef(node))
                return true;

            tokenStream->rewind(start);

            AST *storageSpec = 0;
            parseStorageClassSpecifier(storageSpec);

            AST *cv = 0;
            parseCvQualify(cv);

            TypeSpecifierAST *spec = 0;
            if (parseEnumSpecifier(spec) || parseClassSpecifier(spec)) {
                spec->setCvQualify(cv);

                AST *cv2 = 0;
                parseCvQualify(cv2);
                spec->setCv2Qualify(cv2);

                InitDeclaratorListAST *declarators = 0;
                parseInitDeclaratorList(declarators);
                ADVANCE(';', ";");

                SimpleDeclarationAST *ast = CreateNode<SimpleDeclarationAST>(m_pool);
                ast->setStorageSpecifier(storageSpec);
                ast->setTypeSpec(spec);
                ast->setInitDeclaratorList(declarators);
                UPDATE_POS(ast, start, tokenStream->cursor());
                node = ast;

                return true;
            }

            tokenStream->rewind(start);
            return parseDeclarationInternal(node);
        }

    } // end switch
}

bool Parser::parseLinkageSpecification(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_extern) {
        return false;
    }
    advance();

    LinkageSpecificationAST *ast = CreateNode<LinkageSpecificationAST>(m_pool);

    int startExternType = tokenStream->cursor();
    if (tokenStream->lookAhead() == Token_string_literal) {
        advance();
        AST *externType = CreateNode<AST>(m_pool);
        UPDATE_POS(externType, startExternType, tokenStream->cursor());

        ast->setExternType(externType);
    }

    if (tokenStream->lookAhead() == '{') {
        LinkageBodyAST *linkageBody = 0;
        parseLinkageBody(linkageBody);
        ast->setLinkageBody(linkageBody);
    } else {
        DeclarationAST *decl = 0;
        if (!parseDeclaration(decl)) {
            reportError(i18n("Declaration syntax error"));
        }
        ast->setDeclaration(decl);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());

    node = ast;

    return true;
}

bool Parser::parseLinkageBody(LinkageBodyAST *&node)
{

    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != '{') {
        return false;
    }
    advance();

    LinkageBodyAST *lba = CreateNode<LinkageBodyAST>(m_pool);
    node = lba;

    while (tokenStream->lookAhead()) {
        int tk = tokenStream->lookAhead();

        if (tk == '}')
            break;

        DeclarationAST *def = 0;
        int startDecl = tokenStream->cursor();
        if (parseDeclaration(def)) {
            node->addDeclaration(def);
        } else {
            // error recovery
            if (startDecl == tokenStream->cursor())
                advance(); // skip at least one token
            skipUntilDeclaration();
        }
    }

    if (tokenStream->lookAhead() != '}') {
        reportError(i18n("} expected"));
    } else
        advance();

    UPDATE_POS(node, start, tokenStream->cursor());
    return true;
}

bool Parser::parseNamespace(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_namespace) {
        return false;
    }
    advance();

    int startNamespaceName = tokenStream->cursor();
    if (tokenStream->lookAhead() == Token_identifier) {
        advance();
    }
    AST *namespaceName = CreateNode<AST>(m_pool);
    UPDATE_POS(namespaceName, startNamespaceName, tokenStream->cursor());

    if (tokenStream->lookAhead() == '=') {
        // namespace alias
        advance();

        NameAST *name = 0;
        if (parseName(name)) {
            ADVANCE(';', ";");

            NamespaceAliasAST *ast = CreateNode<NamespaceAliasAST>(m_pool);
            ast->setNamespaceName(namespaceName);
            ast->setAliasName(name);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
            return true;
        } else {
            reportError(i18n("namespace expected"));
            return false;
        }
    } else if (tokenStream->lookAhead() != '{') {
        reportError(i18n("{ expected"));
        return false;
    }

    NamespaceAST *ast = CreateNode<NamespaceAST>(m_pool);
    ast->setNamespaceName(namespaceName);

    LinkageBodyAST *linkageBody = 0;
    parseLinkageBody(linkageBody);

    ast->setLinkageBody(linkageBody);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseUsing(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_using) {
        return false;
    }
    advance();

    if (tokenStream->lookAhead() == Token_namespace) {
        if (!parseUsingDirective(node)) {
            return false;
        }
        UPDATE_POS(node, start, tokenStream->cursor());
        return true;
    }

    UsingAST *ast = CreateNode<UsingAST>(m_pool);

    int startTypeName = tokenStream->cursor();
    if (tokenStream->lookAhead() == Token_typename) {
        advance();
        AST *tn = CreateNode<AST>(m_pool);
        UPDATE_POS(tn, startTypeName, tokenStream->cursor());
        ast->setTypeName(tn);
    }

    NameAST *name = 0;
    if (!parseName(name))
        return false;

    ast->setName(name);

    ADVANCE(';', ";");

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseUsingDirective(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_namespace) {
        return false;
    }
    advance();

    NameAST *name = 0;
    if (!parseName(name)) {
        reportError(i18n("Namespace name expected"));
        return false;
    }

    ADVANCE(';', ";");

    UsingDirectiveAST *ast = CreateNode<UsingDirectiveAST>(m_pool);
    ast->setName(name);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}


bool Parser::parseOperatorFunctionId(AST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_operator) {
        return false;
    }
    advance();

    AST *op = 0;
    if (parseOperator(op)) {
        AST *asn = CreateNode<AST>(m_pool);
        node = asn;
        UPDATE_POS(node, start, tokenStream->cursor());
        return true;
    } else {
        // parse cast operator
        AST *cv = 0;
        parseCvQualify(cv);

        TypeSpecifierAST *spec = 0;
        if (!parseSimpleTypeSpecifier(spec)) {
            syntaxError();
            return false;
        }
        spec->setCvQualify(cv);

        AST *cv2 = 0;
        parseCvQualify(cv2);
        spec->setCv2Qualify(cv2);

        AST *ptrOp = 0;
        while (parsePtrOperator(ptrOp))
              ;

        AST *asn = CreateNode<AST>(m_pool);
        node = asn;
        UPDATE_POS(node, start, tokenStream->cursor());
        return true;
    }
}

bool Parser::parseTemplateArgumentList(TemplateArgumentListAST *&node, bool reportError)
{
    int start = tokenStream->cursor();

    TemplateArgumentListAST *ast = CreateNode<TemplateArgumentListAST>(m_pool);

    AST *templArg = 0;
    if (!parseTemplateArgument(templArg))
        return false;
    ast->addArgument(templArg);

    while (tokenStream->lookAhead() == ',') {
        advance();

        if (!parseTemplateArgument(templArg)) {
            if (reportError) {
               syntaxError();
               break;
            } else
               return false;
        }
        ast->addArgument(templArg);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTypedef(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_typedef) {
        return false;
    }
    advance();

    TypeSpecifierAST *spec = 0;
    if (!parseTypeSpecifierOrClassSpec(spec)) {
        reportError(i18n("Need a type specifier to declare"));
        return false;
    }

    InitDeclaratorListAST *declarators = 0;
    if (!parseInitDeclaratorList(declarators)) {
        //reportError(i18n("Need an identifier to declare"));
        //return false;
    }

    ADVANCE(';', ";");

    TypedefAST *ast = CreateNode<TypedefAST>(m_pool);
    ast->setTypeSpec(spec);
    ast->setInitDeclaratorList(declarators);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseAsmDefinition(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    ADVANCE(Token_asm, "asm");

    AST *cv = 0;
    parseCvQualify(cv);

    skip('(', ')');
    advance();
    ADVANCE(';', ";");

    DeclarationAST *ast = CreateNode<DeclarationAST>(m_pool);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTemplateDeclaration(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    AST *exp = 0;

    int startExport = tokenStream->cursor();
    if (tokenStream->lookAhead() == Token_export) {
        advance();
        AST *n = CreateNode<AST>(m_pool);
        UPDATE_POS(n, startExport, tokenStream->cursor());
        exp = n;
    }

    if (tokenStream->lookAhead() != Token_template) {
        return false;
    }
    advance();

    TemplateParameterListAST *params = 0;
    if (tokenStream->lookAhead() == '<') {
        advance();
        parseTemplateParameterList(params);

        ADVANCE('>', ">");
    }

    DeclarationAST *def = 0;
    if (!parseDeclaration(def)) {
        reportError(i18n("expected a declaration"));
    }

    TemplateDeclarationAST *ast = CreateNode<TemplateDeclarationAST>(m_pool);
    ast->setExported(exp);
    ast->setTemplateParameterList(params);
    ast->setDeclaration(def);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseOperator(AST *&/*node*/)
{
    QString text(QString::fromLatin1(tokenStream->currentTokenText().constData()));

    switch(tokenStream->lookAhead()) {
    case Token_new:
    case Token_delete:
        advance();
        if (tokenStream->lookAhead() == '[' && tokenStream->lookAhead(1) == ']') {
            advance();
            advance();
            text += QLatin1String("[]");
        }
        return true;

    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '^':
    case '&':
    case '|':
    case '~':
    case '!':
    case '=':
    case '<':
    case '>':
    case ',':
    case Token_assign:
    case Token_shift:
    case Token_eq:
    case Token_not_eq:
    case Token_leq:
    case Token_geq:
    case Token_and:
    case Token_or:
    case Token_incr:
    case Token_decr:
    case Token_ptrmem:
    case Token_arrow:
        advance();
        return true;

    default:
        if (tokenStream->lookAhead() == '(' && tokenStream->lookAhead(1) == ')') {
            advance();
            advance();
            return true;
        } else if (tokenStream->lookAhead() == '[' && tokenStream->lookAhead(1) == ']') {
            advance();
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::parseCvQualify(AST *&node)
{
    int start = tokenStream->cursor();

    AST *ast = CreateNode<AST>(m_pool);

    int n = 0;
    while (tokenStream->lookAhead()) {
        int tk = tokenStream->lookAhead();
        if (tk == Token_const || tk == Token_volatile) {
            ++n;
            int startWord = tokenStream->cursor();
            advance();
            AST *word = CreateNode<AST>(m_pool);
            UPDATE_POS(word, startWord, tokenStream->cursor());
            word->setParent(ast);
        } else
            break;
    }

    if (n == 0)
        return false;


    UPDATE_POS(ast, start, tokenStream->cursor());

    node = ast;
    return true;
}

bool Parser::parseSimpleTypeSpecifier(TypeSpecifierAST *&node, bool onlyIntegral)
{
    int start = tokenStream->cursor();
    bool isIntegral = false;
    bool done = false;

    while (!done) {
        switch(tokenStream->lookAhead()) {
            case Token_char:
            case Token_wchar_t:
            case Token_bool:
            case Token_short:
            case Token_int:
            case Token_long:
            case Token_signed:
            case Token_unsigned:
            case Token_float:
            case Token_double:
            case Token_void:
                isIntegral = true;
                advance();
                break;

            default:
                done = true;
        }
    }

    TypeSpecifierAST *ast = CreateNode<TypeSpecifierAST>(m_pool);
    if (isIntegral) {
        ClassOrNamespaceNameAST *cl = CreateNode<ClassOrNamespaceNameAST>(m_pool);

        AST *n = CreateNode<AST>(m_pool);
        UPDATE_POS(n, start, tokenStream->cursor());
        cl->setName(n);
        UPDATE_POS(cl, start, tokenStream->cursor());

        NameAST *name = CreateNode<NameAST>(m_pool);
        name->setUnqualifiedName(cl);
        UPDATE_POS(name, start, tokenStream->cursor());
        ast->setName(name);
    } else if (tokenStream->lookAhead() == Token___typeof) {
        advance();
        if (tokenStream->lookAhead() == '(') {
            advance();
            TypeIdAST *typeId = 0;
            parseTypeId(typeId);
            ADVANCE(')', ")");
        } else {
            AbstractExpressionAST *e = 0;
            parseUnaryExpression(e);
        }
    } else if (onlyIntegral) {
        tokenStream->rewind(start);
        return false;
    } else {
        NameAST *name = 0;
        if (!parseName(name)) {
            tokenStream->rewind(start);
            return false;
        }
        ast->setName(name);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parsePtrOperator(AST *&node)
{
    int start = tokenStream->cursor();
    int tok = tokenStream->lookAhead();
    AST *memPtr = 0;

    switch (tok) {
        case '&':
        case '*':
            advance();
            break;

        case Token_scope:
        case Token_identifier:
            if (!parsePtrToMember(memPtr)) {
                tokenStream->rewind(start);
                return false;
            }
            break;

        default:
            return false;
    }

    AST *cv = 0;
    parseCvQualify(cv);

    AST *ast = CreateNode<AST>(m_pool);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}


bool Parser::parseTemplateArgument(AST *&node)
{
    int start = tokenStream->cursor();

    TypeIdAST *typeId = 0;
    if (parseTypeId(typeId)) {
        if (tokenStream->lookAhead() == ',' || tokenStream->lookAhead() == '>') {
            node = typeId;
            return true;
        }
    }

    tokenStream->rewind(start);
    AbstractExpressionAST *expr = 0;
    if (!parseLogicalOrExpression(expr, true)) {
        return false;
    }
    node = expr;

    return true;
}

bool Parser::parseTypeSpecifier(TypeSpecifierAST *&spec)
{
    AST *cv = 0;
    parseCvQualify(cv);

    if (parseElaboratedTypeSpecifier(spec) || parseSimpleTypeSpecifier(spec)) {
        spec->setCvQualify(cv);

        AST *cv2 = 0;
        parseCvQualify(cv2);
        spec->setCv2Qualify(cv2);

        return true;
    }

    return false;
}

bool Parser::parseDeclarator(DeclaratorAST *&node)
{
    int start = tokenStream->cursor();

    DeclaratorAST *ast = CreateNode<DeclaratorAST>(m_pool);

    DeclaratorAST *decl = 0;
    NameAST *declId = 0;

    AST *ptrOp = 0;
    while (parsePtrOperator(ptrOp)) {
        ast->addPtrOp(ptrOp);
    }

    if (tokenStream->lookAhead() == '(') {
        advance();

        if (!parseDeclarator(decl)) {
            return false;
        }
        ast->setSubDeclarator(decl);

        if (tokenStream->lookAhead() != ')') {
            return false;
        }
        advance();
    } else {
        if (tokenStream->lookAhead() == ':') {
             // unnamed bitfield
        } else if (parseDeclaratorId(declId)) {
            ast->setDeclaratorId(declId);
        } else {
            tokenStream->rewind(start);
            return false;
        }

        if (tokenStream->lookAhead() == ':') {
            advance();
            AbstractExpressionAST *expr = 0;
            if (!parseConstantExpression(expr)) {
                reportError(i18n("Constant expression expected"));
            }
            goto update_pos;
        }
    }

    {
        bool isVector = false;

        while (tokenStream->lookAhead() == '[') {
            int startArray = tokenStream->cursor();
            advance();
            AbstractExpressionAST *expr = 0;
            parseCommaExpression(expr);

            ADVANCE(']', "]");
            AST *array = CreateNode<AST>(m_pool);
            UPDATE_POS(array, startArray, tokenStream->cursor());
            ast->addArrayDimension(array);
            isVector = true;
        }

        bool skipParen = false;
        if (tokenStream->lookAhead() == Token_identifier
                && tokenStream->lookAhead(1) == '('
                && tokenStream->lookAhead(2) == '(') {
            advance();
            advance();
            skipParen = true;
        }

        int tok = tokenStream->lookAhead();
        if (ast->subDeclarator() && !(isVector || tok == '(' || tok == ',' || tok == ';' || tok == '=')) {
            tokenStream->rewind(start);
            return false;
        }

        int index = tokenStream->cursor();
        if (tokenStream->lookAhead() == '(') {
            advance();

            ParameterDeclarationClauseAST *params = 0;
            if (!parseParameterDeclarationClause(params)) {
                tokenStream->rewind(index);
                goto update_pos;
            }
            ast->setParameterDeclarationClause(params);

            if (tokenStream->lookAhead() != ')') {
                tokenStream->rewind(index);
                goto update_pos;
            }

            advance();  // skip ')'

            AST *constant = 0;
            parseCvQualify(constant);
            ast->setConstant(constant);

            AST *except = 0;
            if (parseExceptionSpecification(except)) {
                ast->setExceptionSpecification(except);
            }
        }

        if (skipParen) {
            if (tokenStream->lookAhead() != ')') {
                reportError(i18n("')' expected"));
            } else
                advance();
        }
    }

update_pos:
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseAbstractDeclarator(DeclaratorAST *&node)
{
    int start = tokenStream->cursor();

    DeclaratorAST *ast = CreateNode<DeclaratorAST>(m_pool);
    DeclaratorAST *decl = 0;

    AST *ptrOp = 0;
    while (parsePtrOperator(ptrOp)) {
        ast->addPtrOp(ptrOp);
    }

    int index = tokenStream->cursor();
    if (tokenStream->lookAhead() == '(') {
        advance();

        if (!parseAbstractDeclarator(decl)) {
            tokenStream->rewind(index);
            goto label1;
        }

        ast->setSubDeclarator(decl);

        if (tokenStream->lookAhead() != ')'){
            tokenStream->rewind(start);
            return false;
        }
        advance();
    } else if (tokenStream->lookAhead() == ':') {
        advance();
        AbstractExpressionAST *expr = 0;
        if (!parseConstantExpression(expr)) {
            reportError(i18n("Constant expression expected"));
        }
        goto update_pos;
    }

label1:
    {
        bool isVector = false;

        while (tokenStream->lookAhead() == '[') {
            int startArray = tokenStream->cursor();
            advance();
            AbstractExpressionAST *expr = 0;
            parseCommaExpression(expr);

            ADVANCE(']', "]");
            AST *array = CreateNode<AST>(m_pool);
            UPDATE_POS(array, startArray, tokenStream->cursor());
            ast->addArrayDimension(array);
            isVector = true;
        }

        int tok = tokenStream->lookAhead();
        if (ast->subDeclarator() && !(isVector || tok == '(' || tok == ',' || tok == ';' || tok == '=')) {
            tokenStream->rewind(start);
            return false;
        }

        int index = tokenStream->cursor();
        if (tokenStream->lookAhead() == '(') {
            advance();

            ParameterDeclarationClauseAST *params = 0;
            if (!parseParameterDeclarationClause(params)) {
                tokenStream->rewind(index);
                goto update_pos;
            }
            ast->setParameterDeclarationClause(params);

            if (tokenStream->lookAhead() != ')') {
                tokenStream->rewind(index);
                goto update_pos;
            }

            advance();  // skip ')'

            AST *constant = 0;
            parseCvQualify(constant);
            ast->setConstant(constant);

            AST *except = 0;
            if (parseExceptionSpecification(except)) {
                ast->setExceptionSpecification(except);
            }
        }
    }

update_pos:
    if (tokenStream->cursor() == start)
        return false;

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseEnumSpecifier(TypeSpecifierAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_enum) {
        return false;
    }

    advance();

    NameAST *name = 0;
    parseName(name);

    if (tokenStream->lookAhead() != '{') {
        tokenStream->rewind(start);
        return false;
    }
    advance();

    EnumSpecifierAST *ast = CreateNode<EnumSpecifierAST>(m_pool);
    ast->setName(name);

    EnumeratorAST *enumerator = 0;
    if (parseEnumerator(enumerator)) {
        ast->addEnumerator(enumerator);

        while (tokenStream->lookAhead() == ',') {
            advance();

            if (!parseEnumerator(enumerator)) {
                //reportError(i18n("Enumerator expected"));
                break;
            }

            ast->addEnumerator(enumerator);
        }
    }

    if (tokenStream->lookAhead() != '}')
        reportError(i18n("} missing"));
    else
        advance();

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTemplateParameterList(TemplateParameterListAST *&node)
{
    int start = tokenStream->cursor();

    TemplateParameterListAST *ast = CreateNode<TemplateParameterListAST>(m_pool);

    TemplateParameterAST *param = 0;
    if (!parseTemplateParameter(param)) {
        return false;
    }
    ast->addTemplateParameter(param);

    while (tokenStream->lookAhead() == ',') {
        advance();

        if (!parseTemplateParameter(param)) {
            syntaxError();
            break;
        } else {
            ast->addTemplateParameter(param);
        }
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTemplateParameter(TemplateParameterAST *&node)
{
    int start = tokenStream->cursor();
    TemplateParameterAST *ast = CreateNode<TemplateParameterAST>(m_pool);

    TypeParameterAST *typeParameter = 0;
    ParameterDeclarationAST *param = 0;

    int tk = tokenStream->lookAhead();

    if ((tk == Token_class || tk == Token_typename || tk == Token_template) && parseTypeParameter(typeParameter)) {
        ast->setTypeParameter(typeParameter);
        goto ok;
    }

    if (!parseParameterDeclaration(param))
        return false;
    ast->setTypeValueParameter(param);

ok:
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTypeParameter(TypeParameterAST *&node)
{
    int start = tokenStream->cursor();
    TypeParameterAST *ast = CreateNode<TypeParameterAST>(m_pool);

    AST_FROM_TOKEN(kind, tokenStream->cursor());
    ast->setKind(kind);

    switch(tokenStream->lookAhead()) {

    case Token_class:
    case Token_typename:
        {
            advance(); // skip class

            // parse optional name
            NameAST *name = 0;
            if(parseName(name)){
                ast->setName(name);

                if (tokenStream->lookAhead() == '='){
                    advance();

                    TypeIdAST *typeId = 0;
                    if(!parseTypeId(typeId)){
                        //syntaxError();
                        tokenStream->rewind(start);
                        return false;
                    }
                    ast->setTypeId(typeId);
                } else if (!(tokenStream->lookAhead() == ',' || tokenStream->lookAhead() == '>')) {
                    tokenStream->rewind(start);
                    return false;
                }
            }
        }
        break;

    case Token_template:
        {
            advance(); // skip template
            ADVANCE('<', "<");

            TemplateParameterListAST *params = 0;
            if (!parseTemplateParameterList(params)) {
                return false;
            }
            ast->setTemplateParameterList(params);

            ADVANCE('>', ">");

            if (tokenStream->lookAhead() == Token_class)
                advance();

            // parse optional name
            NameAST *name = 0;
            if (parseName(name)) {
                ast->setName(name);
                if (tokenStream->lookAhead() == '=') {
                    advance();

                    TypeIdAST *typeId = 0;
                    if (!parseTypeId(typeId)) {
                        syntaxError();
                        return false;
                    }
                    ast->setTypeId(typeId);
                }
            }

            if (tokenStream->lookAhead() == '=') {
                advance();

                NameAST *templ_name = 0;
                parseName(templ_name);
            }
        }
        break;

    default:
        return false;

    } // end switch


    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseStorageClassSpecifier(AST *&node)
{
    int start = tokenStream->cursor();
    AST *ast = CreateNode<AST>(m_pool);

    while (tokenStream->lookAhead()) {
        int tk = tokenStream->lookAhead();
        if (tk == Token_friend || tk == Token_auto || tk == Token_register || tk == Token_static ||
                tk == Token_extern || tk == Token_mutable) {
            int startNode = tokenStream->cursor();
            advance();

            AST *n = CreateNode<AST>(m_pool);
            UPDATE_POS(n, startNode, tokenStream->cursor());
            n->setParent(ast);
        } else
            break;
    }

    if (length(ast->children()) == 0)
       return false;

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseFunctionSpecifier(AST *&node)
{
    int start = tokenStream->cursor();
    AST *ast = CreateNode<AST>(m_pool);

    while (tokenStream->lookAhead()) {
        int tk = tokenStream->lookAhead();
        if (tk == Token_inline || tk == Token_virtual || tk == Token_explicit) {
            int startNode = tokenStream->cursor();
            advance();

            AST *n = CreateNode<AST>(m_pool);
            UPDATE_POS(n, startNode, tokenStream->cursor());
            n->setParent(ast);
        } else {
            break;
    }
    }

    if (length(ast->children()) == 0)
       return false;

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseTypeId(TypeIdAST *&node)
{
    /// @todo implement the AST for typeId
    int start = tokenStream->cursor();

    TypeSpecifierAST *spec = 0;
    if (!parseTypeSpecifier(spec)) {
        tokenStream->rewind(start);
        return false;
    }

    DeclaratorAST *decl = 0;
    parseAbstractDeclarator(decl);

    TypeIdAST *ast = CreateNode<TypeIdAST>(m_pool);
    ast->setTypeSpecifier(spec);
    ast->setDeclarator(decl);

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseInitDeclaratorList(InitDeclaratorListAST *&node)
{
    int start = tokenStream->cursor();

    InitDeclaratorListAST *ast = CreateNode<InitDeclaratorListAST>(m_pool);
    InitDeclaratorAST *decl = 0;

    if (!parseInitDeclarator(decl)) {
        return false;
    }
    ast->addInitDeclarator(decl);

    while (tokenStream->lookAhead() == ',') {
        advance();

        if (!parseInitDeclarator(decl)) {
            syntaxError();
            break;
        }
        ast->addInitDeclarator(decl);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseParameterDeclarationClause(ParameterDeclarationClauseAST *&node)
{
    int start = tokenStream->cursor();

    ParameterDeclarationClauseAST *ast = CreateNode<ParameterDeclarationClauseAST>(m_pool);

    ParameterDeclarationListAST *params = 0;
    if (!parseParameterDeclarationList(params)) {

        if (tokenStream->lookAhead() == ')')
            goto good;

        if (tokenStream->lookAhead() == Token_ellipsis && tokenStream->lookAhead(1) == ')') {
            AST_FROM_TOKEN(ellipsis, tokenStream->cursor());
            ast->setEllipsis(ellipsis);
            advance();
            goto good;
        }
        return false;
    }

    if (tokenStream->lookAhead() == Token_ellipsis) {
        AST_FROM_TOKEN(ellipsis, tokenStream->cursor());
        ast->setEllipsis(ellipsis);
        advance();
    }

good:
    ast->setParameterDeclarationList(params);

    /// @todo add ellipsis
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseParameterDeclarationList(ParameterDeclarationListAST *&node)
{
    int start = tokenStream->cursor();

    ParameterDeclarationListAST *ast = CreateNode<ParameterDeclarationListAST>(m_pool);

    ParameterDeclarationAST *param = 0;
    if (!parseParameterDeclaration(param)) {
        tokenStream->rewind(start);
        return false;
    }
    ast->addParameter(param);

    while (tokenStream->lookAhead() == ',') {
        advance();

        if (tokenStream->lookAhead() == Token_ellipsis)
            break;

        if (!parseParameterDeclaration(param)) {
            tokenStream->rewind(start);
            return false;
        }
        ast->addParameter(param);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseParameterDeclaration(ParameterDeclarationAST *&node)
{
    int start = tokenStream->cursor();

    AST *storage = 0;
    parseStorageClassSpecifier(storage);

    // parse decl spec
    TypeSpecifierAST *spec = 0;
    if (!parseTypeSpecifier(spec)) {
        tokenStream->rewind(start);
        return false;
    }

    int index = tokenStream->cursor();

    DeclaratorAST *decl = 0;
    if (!parseDeclarator(decl)) {
        tokenStream->rewind(index);

        // try with abstract declarator
        parseAbstractDeclarator(decl);
    }

    AbstractExpressionAST *expr = 0;
    if (tokenStream->lookAhead() == '=') {
        advance();
        if (!parseLogicalOrExpression(expr,true)) {
            //reportError(i18n("Expression expected"));
        }
    }

    ParameterDeclarationAST *ast = CreateNode<ParameterDeclarationAST>(m_pool);
    ast->setTypeSpec(spec);
    ast->setDeclarator(decl);
    ast->setExpression(expr);

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseClassSpecifier(TypeSpecifierAST *&node)
{
    int start = tokenStream->cursor();

    AST *classKey = 0;
    int classKeyStart = tokenStream->cursor();

    int kind = tokenStream->lookAhead();
    if (kind == Token_class || kind == Token_struct || kind == Token_union) {
        AST *asn = CreateNode<AST>(m_pool);
        classKey = asn;
        advance();
        UPDATE_POS(classKey, classKeyStart, tokenStream->cursor());
    } else {
        return false;
    }

    AST *winDeclSpec = 0;
    parseWinDeclSpec(winDeclSpec);

    while (tokenStream->lookAhead() == Token_identifier && tokenStream->lookAhead(1) == Token_identifier)
            advance();

    NameAST *name = 0;
    parseName(name);

    BaseClauseAST *bases = 0;
    if (tokenStream->lookAhead() == ':') {
        if (!parseBaseClause(bases)) {
            skipUntil('{');
        }
    }

    if (tokenStream->lookAhead() != '{') {
        tokenStream->rewind(start);
        return false;
    }

    ADVANCE('{', "{");

    ClassSpecifierAST *ast = CreateNode<ClassSpecifierAST>(m_pool);
    ast->setWinDeclSpec(winDeclSpec);
    ast->setClassKey(classKey);
    ast->setName(name);
    ast->setBaseClause(bases);

    while (tokenStream->lookAhead()) {
        if (tokenStream->lookAhead() == '}')
            break;

        DeclarationAST *memSpec = 0;
        int startDecl = tokenStream->cursor();
        if (!parseMemberSpecification(memSpec)) {
            if (startDecl == tokenStream->cursor())
                advance(); // skip at least one token
            skipUntilDeclaration();
        } else
            ast->addDeclaration(memSpec);
    }

    if (tokenStream->lookAhead() != '}') {
        reportError(i18n("} missing"));
    } else
        advance();

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseAccessSpecifier(AST *&node)
{
    int start = tokenStream->cursor();

    switch(tokenStream->lookAhead()) {
    case Token_public:
    case Token_protected:
    case Token_private: {
        AST *asn = CreateNode<AST>(m_pool);
        node = asn;
        advance();
        UPDATE_POS(node, start, tokenStream->cursor());
        return true;
        }
    }

    return false;
}

bool Parser::parseMemberSpecification(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    AST *access = 0;

    if (tokenStream->lookAhead() == ';') {
        advance();
        return true;
    } else if (tokenStream->lookAhead() == Token_Q_OBJECT || tokenStream->lookAhead() == Token_K_DCOP) {
        advance();
        return true;
    } else if (tokenStream->lookAhead() == Token_signals
            || tokenStream->lookAhead() == Token_k_dcop
            || tokenStream->lookAhead() == Token_k_dcop_signals) {
        AccessDeclarationAST *ast = CreateNode<AccessDeclarationAST>(m_pool);
        advance();
        AST *n = CreateNode<AST>(m_pool);
        UPDATE_POS(n, start, tokenStream->cursor());
        ast->addAccess(n);
        ADVANCE(':', ":");
        UPDATE_POS(ast, start, tokenStream->cursor());
        node = ast;
        return true;
    } else if (parseTypedef(node)) {
        return true;
    } else if (parseUsing(node)) {
        return true;
    } else if (parseTemplateDeclaration(node)) {
        return true;
    } else if (parseAccessSpecifier(access)) {
        AccessDeclarationAST *ast = CreateNode<AccessDeclarationAST>(m_pool);
        ast->addAccess(access);

        int startSlot = tokenStream->cursor();
        if (tokenStream->lookAhead() == Token_slots) {
            advance();
            AST *sl = CreateNode<AST>(m_pool);
            UPDATE_POS(sl, startSlot, tokenStream->cursor());
            ast->addAccess(sl);
        }
        ADVANCE(':', ":");
        UPDATE_POS(ast, start, tokenStream->cursor());
        node = ast;
        return true;
    }

    tokenStream->rewind(start);

    AST *storageSpec = 0;
    parseStorageClassSpecifier(storageSpec);

    AST *cv = 0;
    parseCvQualify(cv);

    TypeSpecifierAST *spec = 0;
    if (parseEnumSpecifier(spec) || parseClassSpecifier(spec)) {
        spec->setCvQualify(cv);

        AST *cv2 = 0;
        parseCvQualify(cv2);
        spec->setCv2Qualify(cv2);

            InitDeclaratorListAST *declarators = 0;
        parseInitDeclaratorList(declarators);
        ADVANCE(';', ";");

        SimpleDeclarationAST *ast = CreateNode<SimpleDeclarationAST>(m_pool);
        ast->setTypeSpec(spec);
        ast->setInitDeclaratorList(declarators);
        UPDATE_POS(ast, start, tokenStream->cursor());
        node = ast;

        return true;
    }

    tokenStream->rewind(start);
    return parseDeclarationInternal(node);
}

bool Parser::parseCtorInitializer(AST *&/*node*/)
{
    if (tokenStream->lookAhead() != ':') {
        return false;
    }
    advance();

    AST *inits = 0;
    if (!parseMemInitializerList(inits)) {
        reportError(i18n("Member initializers expected"));
    }

    return true;
}

bool Parser::parseElaboratedTypeSpecifier(TypeSpecifierAST *&node)
{
    int start = tokenStream->cursor();

    int tk = tokenStream->lookAhead();
    if (tk == Token_class  ||
        tk == Token_struct ||
        tk == Token_union  ||
        tk == Token_enum   ||
        tk == Token_typename)
    {
        AST *kind = CreateNode<AST>(m_pool);
        advance();
        UPDATE_POS(kind, start, tokenStream->cursor());

        NameAST *name = 0;

        if (parseName(name)) {
            ElaboratedTypeSpecifierAST *ast = CreateNode<ElaboratedTypeSpecifierAST>(m_pool);
            ast->setKind(kind);
            ast->setName(name);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;

            return true;
        }
    }

    tokenStream->rewind(start);
    return false;
}

bool Parser::parseDeclaratorId(NameAST *&node)
{
    return parseName(node);
}

bool Parser::parseExceptionSpecification(AST *&node)
{
    if (tokenStream->lookAhead() != Token_throw) {
        return false;
    }
    advance();

    ADVANCE('(', "(");
    if (tokenStream->lookAhead() == Token_ellipsis) {
        // extension found in MSVC++ 7.x headers
        int start = tokenStream->cursor();
        AST *ast = CreateNode<AST>(m_pool);
        AST_FROM_TOKEN(ellipsis, tokenStream->cursor());
        ellipsis->setParent(ast);
        advance();
        UPDATE_POS(ast, start, tokenStream->cursor());
        node = ast;
    } else {
        parseTypeIdList(node);
    }
    ADVANCE(')', ")");

    return true;
}

bool Parser::parseEnumerator(EnumeratorAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_identifier) {
        return false;
    }
    advance();

    EnumeratorAST *ena = CreateNode<EnumeratorAST>(m_pool);
    node = ena;

    AST *id = CreateNode<AST>(m_pool);
    UPDATE_POS(id, start, tokenStream->cursor());
    node->setId(id);

    if (tokenStream->lookAhead() == '=') {
        advance();

        AbstractExpressionAST *expr = 0;
        if (!parseConstantExpression(expr)) {
            reportError(i18n("Constant expression expected"));
        }
        node->setExpression(expr);
    }

    UPDATE_POS(node, start, tokenStream->cursor());

    return true;
}

bool Parser::parseInitDeclarator(InitDeclaratorAST *&node)
{
    int start = tokenStream->cursor();

    DeclaratorAST *decl = 0;
    AST *init = 0;
    if (!parseDeclarator(decl)) {
        return false;
    }

    parseInitializer(init);

    InitDeclaratorAST *ast = CreateNode<InitDeclaratorAST>(m_pool);
    ast->setDeclarator(decl);
    ast->setInitializer(init);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}



bool Parser::parseBaseClause(BaseClauseAST *&node)
{
    int start = tokenStream->cursor();
    if (tokenStream->lookAhead() != ':') {
        return false;
    }
    advance();

    BaseClauseAST *bca = CreateNode<BaseClauseAST>(m_pool);

    BaseSpecifierAST *baseSpec = 0;
    if (parseBaseSpecifier(baseSpec)) {
        bca->addBaseSpecifier(baseSpec);

        while (tokenStream->lookAhead() == ',') {
            advance();

            if (!parseBaseSpecifier(baseSpec)) {
                reportError(i18n("Base class specifier expected"));
                return false;
            }
            bca->addBaseSpecifier(baseSpec);
        }
    } else
        return false;

    UPDATE_POS(bca, start, tokenStream->cursor());
    node = bca;

    return true;
}

bool Parser::parseInitializer(AST *&node)
{
    if (tokenStream->lookAhead() == '=') {
        advance();

        if (!parseInitializerClause(node)) {
            reportError(i18n("Initializer clause expected"));
            return false;
        }
        return true;
    } else if (tokenStream->lookAhead() == '(') {
        advance();
        AbstractExpressionAST *expr = 0;
        skipCommaExpression(expr);
        CHECK(')', ")");
        node = expr;
        return true;
    }

    return false;
}

bool Parser::parseMemInitializerList(AST *&/*node*/)
{
    AST *init = 0;
    if (!parseMemInitializer(init)) {
        return false;
    }

    while (tokenStream->lookAhead() == ',') {
        advance();

        if (!parseMemInitializer(init)) {
            break;
        }
    }

    return true;
}

bool Parser::parseMemInitializer(AST *&/*node*/)
{
    NameAST *initId = 0;
    if (!parseMemInitializerId(initId)) {
        reportError(i18n("Identifier expected"));
        return false;
    }
    ADVANCE('(', "(");
    AbstractExpressionAST *expr = 0;
    skipCommaExpression(expr);
    ADVANCE(')', ")");

    return true;
}

bool Parser::parseTypeIdList(AST *&node)
{

    int start = tokenStream->cursor();

    TypeIdAST *typeId = 0;
    if (!parseTypeId(typeId)) {
        return false;
    }

    AST *ast = CreateNode<AST>(m_pool);
    typeId->setParent(ast);

    while (tokenStream->lookAhead() == ',') {
        advance();
        if (parseTypeId(typeId)) {
            typeId->setParent(ast);
        } else {
            reportError(i18n("Type id expected"));
            break;
        }
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseBaseSpecifier(BaseSpecifierAST *&node)
{
    int start = tokenStream->cursor();
    BaseSpecifierAST *ast = CreateNode<BaseSpecifierAST>(m_pool);

    AST *access = 0;
    if (tokenStream->lookAhead() == Token_virtual) {
        AST_FROM_TOKEN(virt, tokenStream->cursor());
        ast->setIsVirtual(virt);

        advance();

        parseAccessSpecifier(access);
    } else {
        parseAccessSpecifier(access);

        if (tokenStream->lookAhead() == Token_virtual) {
            AST_FROM_TOKEN(virt, tokenStream->cursor());
            ast->setIsVirtual(virt);
            advance();
        }
    }

    NameAST *name = 0;
    if (!parseName(name)) {
        reportError(i18n("Class name expected"));
    }

    ast->setAccess(access);
    ast->setName(name);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}


bool Parser::parseInitializerClause(AST *&node)
{
    if (tokenStream->lookAhead() == '{') {
        if (!skip('{','}')) {
            reportError(i18n("} missing"));
        } else
            advance();
    } else {
        AbstractExpressionAST *expr = 0;
        if (!parseAssignmentExpression(expr)) {
            //reportError(i18n("Expression expected"));
        }
        node = expr;
    }

    return true;
}

bool Parser::parseMemInitializerId(NameAST *&node)
{

    return parseName(node);
}

bool Parser::parsePtrToMember(AST *&/*node*/)     /// ### create the AST node
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() == Token_scope)
        advance();

    ClassOrNamespaceNameAST *name = 0;
    while (tokenStream->lookAhead() == Token_identifier) {

        if (!parseUnqualifiedName(name))
            break;

        if (tokenStream->lookAhead() == Token_scope
                && tokenStream->lookAhead(1) == '*') {
            advance();
            advance();
            return true;
        }

        if (tokenStream->lookAhead() == Token_scope)
            advance();
    }

    tokenStream->rewind(start);
    return false;
}

bool Parser::parseUnqualifiedName(ClassOrNamespaceNameAST *&node, bool parseTemplateId)
{
    int start = tokenStream->cursor();
    bool isDestructor = false;

    ClassOrNamespaceNameAST *ast = CreateNode<ClassOrNamespaceNameAST>(m_pool);

    if (tokenStream->lookAhead() == Token_identifier) {
        int startName = tokenStream->cursor();
        AST *n = CreateNode<AST>(m_pool);
        advance();
        UPDATE_POS(n, startName, tokenStream->cursor());
        ast->setName(n);
    } else if (tokenStream->lookAhead() == '~' && tokenStream->lookAhead(1) == Token_identifier) {
        int startName = tokenStream->cursor();
        AST *n = CreateNode<AST>(m_pool);
        advance(); // skip ~
        advance(); // skip classname
        UPDATE_POS(n, startName, tokenStream->cursor());
        ast->setName(n);
        isDestructor = true;
    } else if (tokenStream->lookAhead() == Token_operator) {
        AST *n = 0;
        if (!parseOperatorFunctionId(n))
            return false;
        ast->setName(n);
    } else {
        return false;
    }

    if (parseTemplateId && !isDestructor) {

        int index = tokenStream->cursor();

        if (tokenStream->lookAhead() == '<') {
            advance();

            // optional template arguments
            TemplateArgumentListAST *args = 0;
            parseTemplateArgumentList(args);

            if (tokenStream->lookAhead() != '>') {
                tokenStream->rewind(index);
            } else {
                advance();
                ast->setTemplateArgumentList(args);
            }
        }
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseStringLiteral(AST *&node)
{
    int start = tokenStream->cursor();

    while (tokenStream->lookAhead()) {
        if (tokenStream->lookAhead() == Token_identifier &&
            tokenStream->currentTokenText() == "L" && tokenStream->lookAhead(1) == Token_string_literal) {

            advance();
            advance();
        } else if (tokenStream->lookAhead() == Token_string_literal) {
            advance();
        } else
            return false;
    }

    AST *ast = CreateNode<AST>(m_pool);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::skipExpressionStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *expr = 0;
    skipCommaExpression(expr);

    ADVANCE(';', ";");

    ExpressionStatementAST *ast = CreateNode<ExpressionStatementAST>(m_pool);
    ast->setExpression(expr);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    switch(tokenStream->lookAhead()) {

    case Token_while:
        return parseWhileStatement(node);

    case Token_do:
        return parseDoStatement(node);

    case Token_for:
        return parseForStatement(node);

    case Token_if:
        return parseIfStatement(node);

    case Token_switch:
        return parseSwitchStatement(node);

    case Token_try:
        return parseTryBlockStatement(node);

    case Token_case:
    case Token_default:
        return parseLabeledStatement(node);

    case Token_break:
    case Token_continue:
        advance();
        ADVANCE(';', ";");
        return true;

    case Token_goto:
        advance();
        ADVANCE(Token_identifier, "identifier");
        ADVANCE(';', ";");
        return true;

    case Token_return:
    {
        advance();
        AbstractExpressionAST *expr = 0;
        skipCommaExpression(expr);

        ADVANCE(';', ";");

        ReturnStatementAST *ast = CreateNode<ReturnStatementAST>(m_pool);
        ast->setExpression(expr);
        UPDATE_POS(ast, start, tokenStream->cursor());
        node = ast;
    }
    return true;

    case '{':
        return parseCompoundStatement(node);

    case Token_identifier:
        if (parseLabeledStatement(node))
            return true;
        break;
    }

    if (parseDeclarationStatement(node))
        return true;

    return skipExpressionStatement(node);
}

bool Parser::parseCondition(ConditionAST *&node)
{
    int start = tokenStream->cursor();

    ConditionAST *ast = CreateNode<ConditionAST>(m_pool);
    TypeSpecifierAST *spec = 0;

    if (parseTypeSpecifier(spec)) {
        DeclaratorAST *decl = 0;
        if (parseDeclarator(decl) && tokenStream->lookAhead() == '=') {
            advance();

            AbstractExpressionAST *expr = 0;
            if (parseExpression(expr)) {
                ast->setTypeSpec(spec);
                ast->setDeclarator(decl);
                ast->setExpression(expr);

                UPDATE_POS(ast, start, tokenStream->cursor());
                node = ast;

                return true;
            }
        }
    }

    tokenStream->rewind(start);

    AbstractExpressionAST *expr = 0;
    if (!skipCommaExpression(expr)) {
        return false;
    }

    ast->setExpression(expr);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}


bool Parser::parseWhileStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    ADVANCE(Token_while, "while");
    ADVANCE('(' , "(");

    ConditionAST *cond = 0;
    if (!parseCondition(cond)) {
        reportError(i18n("condition expected"));
        return false;
    }
    ADVANCE(')', ")");

    StatementAST *body = 0;
    if (!parseStatement(body)) {
        reportError(i18n("statement expected"));
        return false;
    }

    WhileStatementAST *ast = CreateNode<WhileStatementAST>(m_pool);
    ast->setCondition(cond);
    ast->setStatement(body);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseDoStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    ADVANCE(Token_do, "do");

    StatementAST *body = 0;
    if (!parseStatement(body)) {
        reportError(i18n("statement expected"));
        //return false;
    }

    ADVANCE_NR(Token_while, "while");
    ADVANCE_NR('(' , "(");

    AbstractExpressionAST *expr = 0;
    if (!skipCommaExpression(expr)) {
        reportError(i18n("expression expected"));
        //return false;
    }

    ADVANCE_NR(')', ")");
    ADVANCE_NR(';', ";");

    DoStatementAST *ast = CreateNode<DoStatementAST>(m_pool);
    ast->setStatement(body);
    //ast->setCondition(condition);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseForStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    ADVANCE(Token_for, "for");
    ADVANCE('(', "(");

    StatementAST *init = 0;
    if (!parseForInitStatement(init)) {
        reportError(i18n("for initialization expected"));
        return false;
    }

    ConditionAST *cond = 0;
    parseCondition(cond);
    ADVANCE(';', ";");

    AbstractExpressionAST *expr = 0;
    skipCommaExpression(expr);
    ADVANCE(')', ")");

    StatementAST *body = 0;
    if (!parseStatement(body))
        return false;

    ForStatementAST *ast = CreateNode<ForStatementAST>(m_pool);
    ast->setInitStatement(init);
    ast->setCondition(cond);
    // ast->setExpression(expression);
    ast->setStatement(body);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseForInitStatement(StatementAST *&node)
{
    if (parseDeclarationStatement(node))
        return true;

    return skipExpressionStatement(node);
}

bool Parser::parseCompoundStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != '{') {
        return false;
    }
    advance();

    StatementListAST *ast = CreateNode<StatementListAST>(m_pool);

    while (tokenStream->lookAhead()) {
        if (tokenStream->lookAhead() == '}')
            break;

        StatementAST *stmt = 0;
        int startStmt = tokenStream->cursor();
        if (!parseStatement(stmt)) {
            if (startStmt == tokenStream->cursor())
                advance();
            skipUntilStatement();
        } else {
            ast->addStatement(stmt);
        }
    }

    if (tokenStream->lookAhead() != '}') {
        reportError(i18n("} expected"));
    } else {
        advance();
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseIfStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    ADVANCE(Token_if, "if");

    ADVANCE('(' , "(");

    IfStatementAST *ast = CreateNode<IfStatementAST>(m_pool);

    ConditionAST *cond = 0;
    if (!parseCondition(cond)) {
        reportError(i18n("condition expected"));
        return false;
    }
    ADVANCE(')', ")");

    StatementAST *stmt = 0;
    if (!parseStatement(stmt)) {
        reportError(i18n("statement expected"));
        return false;
    }

    ast->setCondition(cond);
    ast->setStatement(stmt);

    if (tokenStream->lookAhead() == Token_else) {
        advance();
        StatementAST *elseStmt = 0;
        if (!parseStatement(elseStmt)) {
            reportError(i18n("statement expected"));
            return false;
        }
        ast->setElseStatement(elseStmt);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseSwitchStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();
    ADVANCE(Token_switch, "switch");

    ADVANCE('(' , "(");

    ConditionAST *cond = 0;
    if (!parseCondition(cond)) {
        reportError(i18n("condition expected"));
        return false;
    }
    ADVANCE(')', ")");

    StatementAST *stmt = 0;
    if (!parseStatement(stmt)) {
          syntaxError();
        return false;
    }

    SwitchStatementAST *ast = CreateNode<SwitchStatementAST>(m_pool);
    ast->setCondition(cond);
    ast->setStatement(stmt);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseLabeledStatement(StatementAST *&node)
{
    switch(tokenStream->lookAhead()) {
    case Token_identifier:
    case Token_default:
        if (tokenStream->lookAhead(1) == ':') {
            advance();
            advance();

            StatementAST *stmt = 0;
            LabeledStatementAST *ast = CreateNode<LabeledStatementAST>(m_pool);
            node = ast;
            if (parseStatement(stmt)) {
                ast->setStatement(stmt);
                return true;
            }
        }
        break;

    case Token_case:
    {
        advance();
        AbstractExpressionAST *expr = 0;
        if (!parseConstantExpression(expr)) {
            reportError(i18n("expression expected"));
        } else if (tokenStream->lookAhead() == Token_ellipsis) {
            advance();

            AbstractExpressionAST *expr2 = 0;
            if (!parseConstantExpression(expr2)) {
                reportError(i18n("expression expected"));
            }
        }
        ADVANCE(':', ":");

        StatementAST *stmt = 0;
        LabeledStatementAST *ast = CreateNode<LabeledStatementAST>(m_pool);
        node = ast;
        ast->setExpression(expr);

        if (parseStatement(stmt)) {
            ast->setStatement(stmt);
            return true;
        }
    }
    break;

    }
    return false;
}

bool Parser::parseBlockDeclaration(DeclarationAST *&node)
{
    switch(tokenStream->lookAhead()) {
    case Token_typedef:
        return parseTypedef(node);
    case Token_using:
        return parseUsing(node);
    case Token_asm:
        return parseAsmDefinition(node);
    case Token_namespace:
        return parseNamespaceAliasDefinition(node);
    }

    int start = tokenStream->cursor();

    AST *storageSpec = 0;
    parseStorageClassSpecifier(storageSpec);

    AST *cv = 0;
    parseCvQualify(cv);

    TypeSpecifierAST *spec = 0;
    if (!parseTypeSpecifierOrClassSpec(spec)) { // replace with simpleTypeSpecifier?!?!
        tokenStream->rewind(start);
        return false;
    }
    spec->setCvQualify(cv);

    AST *cv2 = 0;
    parseCvQualify(cv2);
    spec->setCv2Qualify(cv2);

    InitDeclaratorListAST *declarators = 0;
    parseInitDeclaratorList(declarators);

    if (tokenStream->lookAhead() != ';') {
        tokenStream->rewind(start);
        return false;
    }
    advance();

    SimpleDeclarationAST *ast = CreateNode<SimpleDeclarationAST>(m_pool);
    ast->setTypeSpec(spec);
    ast->setInitDeclaratorList(declarators);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseNamespaceAliasDefinition(DeclarationAST *&/*node*/)
{
    if (tokenStream->lookAhead() != Token_namespace) {
        return false;
    }
    advance();

    ADVANCE(Token_identifier,  "identifier");
    ADVANCE('=', "=");

    NameAST *name = 0;
    if (!parseName(name)) {
        reportError(i18n("Namespace name expected"));
    }

    ADVANCE(';', ";");

    return true;

}

bool Parser::parseDeclarationStatement(StatementAST *&node)
{
    int start = tokenStream->cursor();

    DeclarationAST *decl = 0;
    if (!parseBlockDeclaration(decl))
        return false;

    DeclarationStatementAST *ast = CreateNode<DeclarationStatementAST>(m_pool);
    ast->setDeclaration(decl);
    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseDeclarationInternal(DeclarationAST *&node)
{
    int start = tokenStream->cursor();

    // that is for the case '__declspec(dllexport) int ...' or
    // '__declspec(dllexport) inline int ...', etc.
    AST *winDeclSpec = 0;
    parseWinDeclSpec(winDeclSpec);

    AST *funSpec = 0;
    bool hasFunSpec = parseFunctionSpecifier(funSpec);

    AST *storageSpec = 0;
    bool hasStorageSpec = parseStorageClassSpecifier(storageSpec);

    if (hasStorageSpec && !hasFunSpec)
        hasFunSpec = parseFunctionSpecifier(funSpec);

    // that is for the case 'friend __declspec(dllexport) ....'
    AST *winDeclSpec2 = 0;
    parseWinDeclSpec(winDeclSpec2);

    AST *cv = 0;
    parseCvQualify(cv);

    int index = tokenStream->cursor();
    NameAST *name = 0;
    if (parseName(name) && tokenStream->lookAhead() == '(') {
        // no type specifier, maybe a constructor or a cast operator??

        tokenStream->rewind(index);

        InitDeclaratorAST *declarator = 0;
        if (parseInitDeclarator(declarator)) {
            switch(tokenStream->lookAhead()) {
            case ';':
                {
                    advance();

                    InitDeclaratorListAST *declarators = CreateNode<InitDeclaratorListAST>(m_pool);

                    // update declarators position
                    if (declarator)
                        declarators->setPosition(declarator->startToken(), declarator->endToken());
                    declarators->addInitDeclarator(declarator);

                    SimpleDeclarationAST *ast = CreateNode<SimpleDeclarationAST>(m_pool);
                    ast->setInitDeclaratorList(declarators);
                    node = ast;
                    UPDATE_POS(node, start, tokenStream->cursor());
                    return true;

                }
                break;

            case ':':
                {
                    AST *ctorInit = 0;
                    StatementListAST *funBody = 0;
                    if (parseCtorInitializer(ctorInit) && parseFunctionBody(funBody)) {
                        FunctionDefinitionAST *ast = CreateNode<FunctionDefinitionAST>(m_pool);
                        ast->setStorageSpecifier(storageSpec);
                        ast->setFunctionSpecifier(funSpec);
                        ast->setInitDeclarator(declarator);
                        ast->setFunctionBody(funBody);
                        node = ast;
                        UPDATE_POS(node, start, tokenStream->cursor());
                        return true;
                    }
                }
                break;

            case '{':
                {
                    StatementListAST *funBody = 0;
                    if (parseFunctionBody(funBody)) {
                        FunctionDefinitionAST *ast = CreateNode<FunctionDefinitionAST>(m_pool);
                        ast->setStorageSpecifier(storageSpec);
                        ast->setFunctionSpecifier(funSpec);
                        ast->setInitDeclarator(declarator);
                        ast->setFunctionBody(funBody);
                        node = ast;
                        UPDATE_POS(node, start, tokenStream->cursor());
                        return true;
                    }
                }
                break;

            case '(':
            case '[':
                // ops!! it seems a declarator
                goto start_decl;
                break;
            }

        }
    }

start_decl:
    tokenStream->rewind(index);

    if (tokenStream->lookAhead() == Token_const && tokenStream->lookAhead(1) == Token_identifier && tokenStream->lookAhead(2) == '=') {
        // constant definition
        advance();
        InitDeclaratorListAST *declarators = 0;
        if (parseInitDeclaratorList(declarators)) {
            ADVANCE(';', ";");
            DeclarationAST *ast = CreateNode<DeclarationAST>(m_pool);
            node = ast;
            UPDATE_POS(node, start, tokenStream->cursor());
            return true;
        }
        syntaxError();
        return false;
    }

    TypeSpecifierAST *spec = 0;
    if (parseTypeSpecifier(spec)) {
        if (!hasFunSpec)
            parseFunctionSpecifier(funSpec);         // e.g. "void inline"
        spec->setCvQualify(cv);

        InitDeclaratorListAST *declarators = 0;

        InitDeclaratorAST *decl = 0;
        int startDeclarator = tokenStream->cursor();
        bool maybeFunctionDefinition = false;

        if (tokenStream->lookAhead() != ';') {
            if (parseInitDeclarator(decl) && tokenStream->lookAhead() == '{') {
                // function definition
                maybeFunctionDefinition = true;
            } else {
                tokenStream->rewind(startDeclarator);
                if (!parseInitDeclaratorList(declarators)) {
                    syntaxError();
                    return false;
                }
            }
        }

        switch(tokenStream->lookAhead()) {
        case ';':
            {
                advance();
                SimpleDeclarationAST *ast = CreateNode<SimpleDeclarationAST>(m_pool);
                ast->setStorageSpecifier(storageSpec);
                ast->setFunctionSpecifier(funSpec);
                ast->setTypeSpec(spec);
                ast->setWinDeclSpec(winDeclSpec);
                ast->setInitDeclaratorList(declarators);
                node = ast;
                UPDATE_POS(node, start, tokenStream->cursor());
            }
            return true;

        case '{':
            {
                if (!maybeFunctionDefinition) {
                    syntaxError();
                    return false;
                }
                StatementListAST *funBody = 0;
                if (parseFunctionBody(funBody)) {
                    FunctionDefinitionAST *ast = CreateNode<FunctionDefinitionAST>(m_pool);
                    ast->setWinDeclSpec(winDeclSpec);
                    ast->setStorageSpecifier(storageSpec);
                    ast->setFunctionSpecifier(funSpec);
                    ast->setTypeSpec(spec);
                    ast->setInitDeclarator(decl);
                    ast->setFunctionBody(funBody);
                    node = ast;
                    UPDATE_POS(node, start, tokenStream->cursor());
                    return true;
                }
            }
            break;

        }
    }

    syntaxError();
    return false;
}

bool Parser::parseFunctionBody(StatementListAST *&node)
{
    int start = tokenStream->cursor();
    if (tokenStream->lookAhead() != '{') {
        return false;
    }
    advance();

    StatementListAST *ast = CreateNode<StatementListAST>(m_pool);

    while (tokenStream->lookAhead()) {
        if (tokenStream->lookAhead() == '}')
            break;

        StatementAST *stmt = 0;
        int startStmt = tokenStream->cursor();
        if (!parseStatement(stmt)) {
            syntaxError();
            if (startStmt == tokenStream->cursor())
                advance();
            skipUntilStatement();
        } else
            ast->addStatement(stmt);
    }

    if (tokenStream->lookAhead() != '}') {
        reportError(i18n("} expected"));
    } else
        advance();

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}

bool Parser::parseTypeSpecifierOrClassSpec(TypeSpecifierAST *&node)
{
    if (parseClassSpecifier(node))
        return true;
    else if (parseEnumSpecifier(node))
        return true;
    else if (parseTypeSpecifier(node))
        return true;

    return false;
}

bool Parser::parseTryBlockStatement(StatementAST *&node)
{
    if (tokenStream->lookAhead() != Token_try) {
        return false;
    }
    advance();

    StatementAST *stmt = 0;
    if (!parseCompoundStatement(stmt)) {
        syntaxError();
        return false;
    }

    if (tokenStream->lookAhead() != Token_catch) {
        reportError(i18n("catch expected"));
        return false;
    }

    while (tokenStream->lookAhead() == Token_catch) {
        advance();
        ADVANCE('(', "(");
        ConditionAST *cond = 0;
        if (tokenStream->lookAhead() == Token_ellipsis) {
            advance();
        } else if (!parseCondition(cond)) {
            reportError(i18n("condition expected"));
            return false;
        }
        ADVANCE(')', ")");

        StatementAST *body = 0;
        if (!parseCompoundStatement(body)) {
            syntaxError();
            return false;
        }
    }

    node = stmt;
    return true;
}

bool Parser::parsePrimaryExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = CreateExpression<NodeType_PrimaryExpression>(m_pool);

    switch(tokenStream->lookAhead()) {
        case Token_string_literal:
        {
            AST *lit = 0;
            parseStringLiteral(lit);
            if (lit)
                lit->setParent(ast);
        }
        break;

        case Token_number_literal:
        case Token_char_literal:
        case Token_true:
        case Token_false:
        case Token_this:
            {
                AST_FROM_TOKEN(opNode, tokenStream->cursor());
                opNode->setParent(ast);
                advance();
            }
            break;

        case '(':
            {
                advance();

                if (tokenStream->lookAhead() == '{') {
                    StatementAST *stmt = 0;
                    if (!parseCompoundStatement(stmt))
                        return false;
                    if (stmt)
                        stmt->setParent(ast);
                } else {
                    AbstractExpressionAST *expr = 0;
                    if (!parseExpression(expr)) {
                        return false;
                    }
                    if (expr)
                        expr->setParent(ast);
                }
                CHECK(')', ")");
            }
            break;

        default:
            {
/*  ### reenable me
                TypeSpecifierAST *typeSpec = 0;
                if (parseSimpleTypeSpecifier(typeSpec) && tokenStream->lookAhead() == '(') {
                    Q_ASSERT (0);
                    advance();
                    AbstractExpressionAST *expr = 0;
                    parseCommaExpression(expr);
                    CHECK(')', ")");
                    break;

                    if (typeSpec)
                        typeSpec->setParent(ast);

                    if (expr)
                        expr->setParent(ast);
                }

                tokenStream->rewind(start);
*/

                NameAST *name = 0;
                if (!parseName(name, false))
                    return false;

                if (name)
                    name->setParent(ast);

                break;
            }
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}


/*
   postfix-expression-internal:
     [ expression ]
     ( expression-list [opt] )
     (.|->) template [opt] id-expression
     (.|->) pseudo-destructor-name
     ++
     --
*/
bool Parser::parsePostfixExpressionInternal(AbstractExpressionAST *postfixExpr, AbstractExpressionAST *&node)
{
    Q_ASSERT (postfixExpr);

    int start = tokenStream->cursor();

    switch (tokenStream->lookAhead()) {
        case '[':
        {
            advance();
            AbstractExpressionAST *expr = 0;
            parseExpression(expr);
            CHECK(']', "]");

            SubscriptingAST *ast = CreateNode<SubscriptingAST>(m_pool);
            ast->setExpression(postfixExpr);
            ast->setSubscript(expr);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
        }
        return true;

        case '(':
        {
            advance();
            AbstractExpressionAST *expr = 0;
            parseExpression(expr);
            CHECK(')', ")");

            FunctionCallAST *ast = CreateNode<FunctionCallAST>(m_pool);
            ast->setExpression(postfixExpr);
            ast->setArguments(expr);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
        }
        return true;

        case '.':
        case Token_arrow:
        {
            AST_FROM_TOKEN(op, tokenStream->cursor());

            advance();
            if (tokenStream->lookAhead() == Token_template)
                advance();

            NameAST *name = 0;
            if (!parseName(name))
                return false;

            ClassMemberAccessAST *ast = CreateNode<ClassMemberAccessAST>(m_pool);
            ast->setOp(op);
            ast->setExpression(postfixExpr);
            ast->setName(name);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
        }
        return true;

        case Token_incr:
        case Token_decr:
        {
            AST_FROM_TOKEN(op, tokenStream->cursor());
            advance();

            IncrDecrAST *ast = CreateNode<IncrDecrAST>(m_pool);
            ast->setExpression(postfixExpr);
            ast->setOp(op);
            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
        }
        return true;

        default:
            return false;
    }
}

/*
   postfix-expression:
     simple-type-specifier ( expression-list [opt] )
     primary-expression postfix-expression-internal*
*/
bool Parser::parsePostfixExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    switch (tokenStream->lookAhead()) {
        case Token_dynamic_cast:
        case Token_static_cast:
        case Token_reinterpret_cast:
        case Token_const_cast:
        {
            AST_FROM_TOKEN(castOp, tokenStream->cursor());

            advance();
            CHECK('<', "<");
            TypeIdAST *typeId = 0;
            parseTypeId(typeId);
            CHECK('>', ">");

            CHECK('(', ")");
            AbstractExpressionAST *expr = 0;
            parseCommaExpression(expr);
            CHECK(')', ")");

            CppCastExpressionAST *tmp = CreateNode<CppCastExpressionAST>(m_pool);
            tmp->setCastOp(castOp);
            tmp->setTypeId(typeId);
            tmp->setExpression(expr);

            AbstractExpressionAST *ast = tmp;
            AbstractExpressionAST *e = 0;
            while (parsePostfixExpressionInternal(ast, e)) {
                ast = e;
            }

            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
        }
        return true;

        case Token_typename:
        {
            advance();

            NameAST* name = 0;
            if (!parseName(name))
                return false;

            CHECK('(', "(");
            AbstractExpressionAST *expr = 0;
            parseCommaExpression(expr);
            CHECK(')', ")");

            // ### AST
        }
        return true;

        case Token_typeid:
        {
            advance();

            CHECK('(', "(");
            AbstractExpressionAST *expr = 0;
            parseCommaExpression(expr);
            CHECK(')', ")");

            // ### AST
        }
        return true;

        default:
            break;
    }

    TypeSpecifierAST *typeSpec = 0;
    AbstractExpressionAST *expr = 0;

    if (parseSimpleTypeSpecifier(typeSpec/*, true*/) && tokenStream->lookAhead() == '(') {
        advance(); // skip '('
        parseCommaExpression(expr);
        CHECK(')', ")");
    } else {
        tokenStream->rewind(start);

        if (!parsePrimaryExpression(expr))
            return false;
    }

    AbstractExpressionAST *ast = CreateExpression<NodeType_PostfixExpression>(m_pool);
    if (typeSpec)
        typeSpec->setParent(ast);

    if (expr)
        expr->setParent(ast);

    AbstractExpressionAST *e = 0;
    while (parsePostfixExpressionInternal(ast, e)) {
        ast = e;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseUnaryExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    switch(tokenStream->lookAhead()) {
        case Token_incr:
        case Token_decr:
        case '*':
        case '&':
        case '+':
        case '-':
        case '!':
        case '~':
        {
            AST_FROM_TOKEN(opNode, tokenStream->cursor());

            advance();
            AbstractExpressionAST *expr = 0;
            if (!parseCastExpression(expr))
                return false;

            AbstractExpressionAST *ast = CreateExpression<NodeType_UnaryExpression>(m_pool);

            opNode->setParent(ast);
            if (expr)
                expr->setParent(ast);

            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
            return true;
        }

        case Token_sizeof:
        {
            AbstractExpressionAST *ast = CreateExpression<NodeType_UnaryExpression>(m_pool);

            AST_FROM_TOKEN(opNode, tokenStream->cursor());
            opNode->setParent(ast);

            advance();
            int index = tokenStream->cursor();
            if (tokenStream->lookAhead() == '(') {
                advance();
                TypeIdAST *typeId = 0;
                if (parseTypeId(typeId) && tokenStream->lookAhead() == ')') {
                    if (typeId)
                        typeId->setParent(ast);

                    advance();

                    UPDATE_POS(ast, start, tokenStream->cursor());
                    node = ast;
                    return true;
                }
                tokenStream->rewind(index);
            }
            AbstractExpressionAST *expr = 0;
            if (!parseUnaryExpression(expr))
                return false;

            UPDATE_POS(ast, start, tokenStream->cursor());
            node = ast;
            return true;
        }

        case Token_new:
            return parseNewExpression(node);

        case Token_delete:
            return parseDeleteExpression(node);
    }

    return parsePostfixExpression(node);
}

bool Parser::parseNewExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = CreateExpression<NodeType_NewExpression>(m_pool);

    if (tokenStream->lookAhead() == Token_scope && tokenStream->lookAhead(1) == Token_new) {
        AST_FROM_TOKEN(scopeNode, tokenStream->cursor());
        scopeNode->setParent(ast);
        advance();
    }

    AST_FROM_TOKEN(newNode, tokenStream->cursor());
    newNode->setParent(ast);

    CHECK(Token_new, "new");

    if (tokenStream->lookAhead() == '(') {
        advance();
        AbstractExpressionAST *expr = 0;
        parseCommaExpression(expr);
        if (expr)
            expr->setParent(ast);
        CHECK(')', ")");
    }

    if (tokenStream->lookAhead() == '(') {
        advance();
        TypeIdAST *typeId = 0;
        parseTypeId(typeId);
        if (typeId)
            typeId->setParent(ast);
        CHECK(')', ")");
    } else {
        AbstractExpressionAST *typeId = 0;
        parseNewTypeId(typeId);
        if (typeId)
            typeId->setParent(ast);
    }

    AbstractExpressionAST *init = 0;
    parseNewInitializer(init);
    if (init)
        init->setParent(ast);

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseNewTypeId(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    TypeSpecifierAST *typeSpec = 0;
    if (!parseTypeSpecifier(typeSpec))
        return false;

    AbstractExpressionAST *ast = CreateExpression<NodeType_NewTypeId>(m_pool);

    if (typeSpec)
        typeSpec->setParent(ast);

    AbstractExpressionAST *declarator = 0;
    parseNewDeclarator(declarator);
    if (declarator)
        declarator->setParent(ast);

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseNewDeclarator(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = CreateExpression<NodeType_NewInitializer>(m_pool);

    AST *ptrOp = 0;
    if (parsePtrOperator(ptrOp)) {
        if (ptrOp)
            ptrOp->setParent(ast);

        AbstractExpressionAST *declarator = 0;
        parseNewDeclarator(declarator);

        if (declarator)
            declarator->setParent(ast);
    }

    while (tokenStream->lookAhead() == '[') {
        advance();
        AbstractExpressionAST *expr = 0;
        parseExpression(expr);
        ADVANCE(']', "]");

        if (expr)
            expr->setParent(ast);
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseNewInitializer(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != '(')
        return false;

    AbstractExpressionAST *ast = CreateExpression<NodeType_NewInitializer>(m_pool);

    advance();
    AbstractExpressionAST *expr = 0;
    parseCommaExpression(expr);

    if (expr)
        expr->setParent(ast);

    CHECK(')', ")");

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseDeleteExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = CreateExpression<NodeType_DeleteExpression>(m_pool);

    if (tokenStream->lookAhead() == Token_scope && tokenStream->lookAhead(1) == Token_delete) {
        AST_FROM_TOKEN(scopeNode, tokenStream->cursor());
        scopeNode->setParent(ast);
        advance();
    }

    AST_FROM_TOKEN(deleteNode, tokenStream->cursor());
    deleteNode->setParent(ast);

    CHECK(Token_delete, "delete");

    if (tokenStream->lookAhead() == '[') {
        int beg = tokenStream->cursor();
        advance();
        CHECK(']', "]");

        AST *n = CreateNode<AST>(m_pool);
        UPDATE_POS(n, beg, tokenStream->cursor());
        n->setParent(ast);
    }

    AbstractExpressionAST *expr = 0;
    if (!parseCastExpression(expr))
        return false;

    if (expr)
        expr->setParent(ast);

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseCastExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() == '(') {
        AbstractExpressionAST *ast = CreateExpression<NodeType_CastExpression>(m_pool);

        advance();
        TypeIdAST *typeId = 0;
        if (parseTypeId(typeId)) {

            if (typeId)
                typeId->setParent(ast);

            if (tokenStream->lookAhead() == ')') {
                advance();

                AbstractExpressionAST *expr = 0;
                if (parseCastExpression(expr)) {
                    if (expr)
                        expr->setParent(ast);

                    UPDATE_POS(ast, start, tokenStream->cursor());
                    node = ast;
                    return true;
                }
            }
        }
    }

    tokenStream->rewind(start);
    return parseUnaryExpression(node);
}

bool Parser::parsePmExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseCastExpression(ast) || !ast) // ### fixme
        return false;

    while (tokenStream->lookAhead() == Token_ptrmem) {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseCastExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseMultiplicativeExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parsePmExpression(ast))
        return false;

    while (tokenStream->lookAhead() == '*' || tokenStream->lookAhead() == '/' || tokenStream->lookAhead() == '%') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parsePmExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}


bool Parser::parseAdditiveExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseMultiplicativeExpression(ast))
        return false;

    while (tokenStream->lookAhead() == '+' || tokenStream->lookAhead() == '-') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseMultiplicativeExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseShiftExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseAdditiveExpression(ast))
        return false;

    while (tokenStream->lookAhead() == Token_shift) {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseAdditiveExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseRelationalExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseShiftExpression(ast))
        return false;

    while (tokenStream->lookAhead() == '<' || (tokenStream->lookAhead() == '>' && !templArgs) ||
           tokenStream->lookAhead() == Token_leq || tokenStream->lookAhead() == Token_geq) {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseShiftExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseEqualityExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseRelationalExpression(ast, templArgs))
        return false;

    while (tokenStream->lookAhead() == Token_eq || tokenStream->lookAhead() == Token_not_eq) {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseRelationalExpression(rightExpr, templArgs))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseAndExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseEqualityExpression(ast, templArgs))
        return false;

    while (tokenStream->lookAhead() == '&') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseEqualityExpression(rightExpr, templArgs))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseExclusiveOrExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseAndExpression(ast, templArgs))
        return false;

    while (tokenStream->lookAhead() == '^') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseAndExpression(rightExpr, templArgs))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseInclusiveOrExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseExclusiveOrExpression(ast, templArgs))
        return false;

    while (tokenStream->lookAhead() == '|') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseExclusiveOrExpression(rightExpr, templArgs))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseLogicalAndExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseInclusiveOrExpression(ast, templArgs))
        return false;

    while (tokenStream->lookAhead() == Token_and) {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseInclusiveOrExpression(rightExpr, templArgs))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseLogicalOrExpression(AbstractExpressionAST *&node, bool templArgs)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseLogicalAndExpression(ast, templArgs))
        return false;

    while (tokenStream->lookAhead() == Token_or) {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseLogicalAndExpression(rightExpr, templArgs))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseConditionalExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();
    AbstractExpressionAST *ast = 0;
    if (!parseLogicalOrExpression(ast))
        return false;

    if (tokenStream->lookAhead() == '?') {
        advance();

        AbstractExpressionAST *leftExpr = 0;
        if (!parseExpression(leftExpr))
            return false;

        CHECK(':', ":");

        AbstractExpressionAST *rightExpr = 0;
        if (!parseAssignmentExpression(rightExpr))
            return false;

        ConditionalExpressionAST *tmp = CreateNode<ConditionalExpressionAST>(m_pool);
        tmp->setCondition(ast);
        tmp->setLeftExpression(leftExpr);
        tmp->setRightExpression(rightExpr);
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseAssignmentExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (tokenStream->lookAhead() == Token_throw && !parseThrowExpression(ast))
        return false;
    else if (!parseConditionalExpression(ast))
        return false;

    while (tokenStream->lookAhead() == Token_assign || tokenStream->lookAhead() == '=') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseConditionalExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseConstantExpression(AbstractExpressionAST *&node)
{
    return parseConditionalExpression(node);
}

bool Parser::parseExpression(AbstractExpressionAST *&node)
{
    return parseCommaExpression(node);
}

bool Parser::parseCommaExpression(AbstractExpressionAST *&node)
{
    int start = tokenStream->cursor();

    AbstractExpressionAST *ast = 0;
    if (!parseAssignmentExpression(ast))
        return false;

    while (tokenStream->lookAhead() == ',') {
        int startOp = tokenStream->cursor();
        AST_FROM_TOKEN(op, startOp);
        advance();

        AbstractExpressionAST *rightExpr = 0;
        if (!parseAssignmentExpression(rightExpr))
            return false;

        BinaryExpressionAST *tmp = CreateNode<BinaryExpressionAST>(m_pool);
        tmp->setOp(op);
        tmp->setLeftExpression(ast);
        tmp->setRightExpression(rightExpr);
        UPDATE_POS(tmp, startOp, tokenStream->cursor());
        ast = tmp;
    }

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;
    return true;
}

bool Parser::parseThrowExpression(AbstractExpressionAST *&node)
{
    if (tokenStream->lookAhead() != Token_throw)
        return false;

    int start = tokenStream->cursor();

    AST_FROM_TOKEN(throwNode, tokenStream->cursor());
    CHECK(Token_throw, "throw");
    AbstractExpressionAST *expr = 0;
    if (!parseAssignmentExpression(expr))
        return false;

    AbstractExpressionAST *ast = CreateExpression<NodeType_ThrowExpression>(m_pool);
    throwNode->setParent(ast);
    if (expr)
        expr->setParent(ast);

    UPDATE_POS(ast, start, tokenStream->cursor());
    node = ast;

    return true;
}


// ### Objective C++
bool Parser::parseIvarDeclList(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseIvarDecls(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseIvarDecl(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseIvars(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseIvarDeclarator(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseMethodDecl(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseUnarySelector(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordSelector(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseSelector(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordDecl(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseReceiver(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcMessageExpr(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseMessageArgs(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordExpr(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordArgList(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordArg(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseReservedWord(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseMyParms(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseMyParm(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseOptParmList(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcSelectorExpr(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseSelectorArg(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordNameList(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseKeywordName(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcEncodeExpr(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcString(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseProtocolRefs(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseIdentifierList(AST *& node)
{
    int start = tokenStream->cursor();

    if (tokenStream->lookAhead() != Token_identifier)
        return false;

    AST *ast = CreateNode<AST>(m_pool);

    AST_FROM_TOKEN(tk, tokenStream->cursor());
    tk->setParent(ast);
    advance();

    while (tokenStream->lookAhead() == ',') {
        advance();
        if (tokenStream->lookAhead() == Token_identifier) {
            AST_FROM_TOKEN(tk, tokenStream->cursor());
            tk->setParent(ast);
            advance();
        }
        ADVANCE(Token_identifier, "identifier");
    }

    node = ast;
    UPDATE_POS(node, start, tokenStream->cursor());
    return true;
}

bool Parser::parseIdentifierColon(AST *& node)
{
    Q_UNUSED(node);

    if (tokenStream->lookAhead() == Token_identifier && tokenStream->lookAhead(1) == ':') {
        advance();
        advance();
        return true;
    } // ### else if PTYPENAME -> return true ;

    return false;
}

bool Parser::parseObjcProtocolExpr(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcOpenBracketExpr(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcCloseBracket(AST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcDef(DeclarationAST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcClassDef(DeclarationAST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcClassDecl(DeclarationAST *& node)
{
    Q_UNUSED(node);

    ADVANCE(OBJC_CLASS, "@class");

    AST *idList = 0;
    parseIdentifierList(idList);
    ADVANCE(';', ";");

    return true;
}

bool Parser::parseObjcProtocolDecl(DeclarationAST *& node)
{
    Q_UNUSED(node);

    ADVANCE(OBJC_PROTOCOL, "@protocol");

    AST *idList = 0;
    parseIdentifierList(idList);
    ADVANCE(';', ";");

    return true;
}

bool Parser::parseObjcAliasDecl(DeclarationAST *& node)
{
    Q_UNUSED(node);

    ADVANCE(OBJC_ALIAS, "@alias");

    AST *idList = 0;
    parseIdentifierList(idList);
    ADVANCE(';', ";");

    return true;
}

bool Parser::parseObjcProtocolDef(DeclarationAST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseObjcMethodDef(DeclarationAST *& node)
{
    Q_UNUSED(node);
    return false;
}

bool Parser::parseWinDeclSpec(AST *& node)
{
    if (tokenStream->lookAhead() == Token_identifier
            && tokenStream->lookAhead(1) == '('
            && tokenStream->currentTokenText() == "__declspec") {
        int start = tokenStream->cursor();
        advance();
        advance(); // skip '('

        parseIdentifierList(node);
        ADVANCE(')', ")");

        UPDATE_POS(node, start, tokenStream->cursor());
        return true;
    }

    return false;
}

void Parser::advance()
{
    for (;;) {
        tokenStream->nextToken();
        if (!tokenStream->isHidden(tokenStream->cursor()))
            break;
    }
}

QT_END_NAMESPACE
